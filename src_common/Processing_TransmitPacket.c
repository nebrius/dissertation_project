#include"System.h"

#pragma CODE_SECTION(SendControlPacket,"FlashCode");
Uint16 SendControlPacket(struct Packet* packet)
{
	// Local variables
	Uint16 flit[NUM_CHARACTERS_IN_FLIT];

	// Update the statistics
	globals.statistics.packet.numControlPacketsSent++;

	// Set the virtual channel
	packet->transmissionInfo.virtualChannelTransmittedOn = VIRTUAL_CHANNEL_CONTROL;

	// Finalize the the packet
	if(MarshallPacketHeader(packet,flit) == ERROR)
		return ERROR;

	// Set the link layer fields
	flit[0] = (packet->transmissionInfo.destination << 8) + FLIT_TYPE_HEADER + VIRTUAL_CHANNEL_CONTROL;

	// Transmit the packet
	return EnqueOutboundFlit(flit,packet->transmissionInfo.portTransmittedOn);
}

#pragma CODE_SECTION(SendDataPacket,"FlashCode");
Uint16 SendDataPacket(struct Packet* packet)
{
	// Local variables
	Uint16 flit[NUM_CHARACTERS_IN_FLIT];
	Uint32 crc;

	// Calculate the CRC of the data
	crc = CRC32(packet->dataBuffer,packet->dataBufferInfo.dataBufferLength);
	packet->f.commandSpecific1 = (crc >> 24);
	packet->g.commandSpecific2 = (crc >> 16) & 0x000000FF;
	packet->g.commandSpecific3 = (crc >> 8) & 0x000000FF;
	packet->h.commandSpecific4 = crc & 0x000000FF;

	// Find the path to take
	if(packet->path == 0 && packet->a.communicationType == COMM_TYPE_UNICAST)
	{
		if(globals.routing.operatingInLimpMode)
		{

#if defined(IS_ROOT)

			packet->path = globals.routing.limpModePaths[packet->transmissionInfo.destination];
			if(packet->path == 0)
				return ERROR;

#elif defined(IS_ROUTER)

			if(packet->transmissionInfo.destination == ROOT_ADDRESS)
				packet->path = globals.routing.limpModePath;
			else
				return ERROR;

#endif
		}
		else
			GenerateRoutingPath(globals.protocol.address, packet->transmissionInfo.destination, packet);
	}

	// Get the port to transmit on
	packet->transmissionInfo.portTransmittedOn = GetTransmitPort(packet);

	if(packet->transmissionInfo.portTransmittedOn != PORT_UNDEFINED && packet->transmissionInfo.portTransmittedOn != ERROR)
	{
		// Store the packet length, including the header
		packet->f.commandSpecific0 = packet->dataBufferInfo.dataBufferLength;

		// Get the virtual channel to transmit on
		packet->transmissionInfo.virtualChannelTransmittedOn = ReserveVirtualChannel(packet->transmissionInfo.portTransmittedOn,packet->a.packetID);

		if(packet->transmissionInfo.virtualChannelTransmittedOn < VIRTUAL_CHANNEL_CONTROL)
		{
			// Finalize the packet
			if(MarshallPacketHeader(packet,flit) == ERROR)
			{
				ReleaseVirtualChannel(packet->transmissionInfo.portTransmittedOn,packet->transmissionInfo.virtualChannelTransmittedOn);
				return ERROR;
			}

			// Set the link layer fields
			flit[0] = (packet->transmissionInfo.destination << 8) + FLIT_TYPE_HEADER + packet->transmissionInfo.virtualChannelTransmittedOn;

			// Store the packet for later lookup and transmit the packet header
			if(SetDataTransferLookup(packet) == SUCCESS)
			{
				// Send the data header
				return EnqueOutboundFlit(flit,packet->transmissionInfo.portTransmittedOn);
			}
			else
			{
				// Cleanup after the failed attempt to set the data transfer lookup
				ReleaseVirtualChannel(packet->transmissionInfo.portTransmittedOn,packet->transmissionInfo.virtualChannelTransmittedOn);
				return ERROR;
			}
		}
		else
			return ERROR;
	}
	else
		return ERROR;
}

#pragma CODE_SECTION(SendDataPacketPayload,"FlashCode");
Uint16 SendDataPacketPayload(struct Packet* packet)
{
	Uint16 flit[NUM_CHARACTERS_IN_FLIT], i = 0;

	if(packet->dataBufferInfo.dataBufferLength > 10)
		asm(" NOP");
	// Loop through each data item and marshall it into the output buffer
	while(packet->dataBufferInfo.dataBufferLength > i)
	{
		MarshallData(flit, packet->dataBuffer, packet->dataBufferInfo.dataBufferLength, &i, packet->transmissionInfo.virtualChannelTransmittedOn);

		// Set the flit type
		if(i >= packet->dataBufferInfo.dataBufferLength)
			flit[0] = (packet->transmissionInfo.destination << 8) + FLIT_TYPE_DATA_PAYLOAD_END + packet->transmissionInfo.virtualChannelTransmittedOn;
		else
			flit[0] = (packet->transmissionInfo.destination << 8) + FLIT_TYPE_DATA_PAYLOAD + packet->transmissionInfo.virtualChannelTransmittedOn;

		// Send the flit on its way
		while(globals.processing.outboundFlitQueFull[packet->transmissionInfo.portTransmittedOn]);
		if(EnqueOutboundFlit(flit,packet->transmissionInfo.portTransmittedOn) ==  ERROR)
			return ERROR;
	}

	return SUCCESS;
}

#pragma CODE_SECTION(SendErrorPacket,"FlashCode");
Uint16 SendErrorPacket(struct Packet* packet, Uint16 errorCode)
{
	struct Packet newPacket;

	// Initialize the packet
	InitializePacketFromPreviousSequence(&newPacket,packet);

	// Create the address denied packet
	newPacket.transmissionInfo.destination = packet->transmissionInfo.source;
	newPacket.b.packetSequenceStep = SEQUENCE_ERROR;
	newPacket.f.commandSpecific0 = errorCode;
	newPacket.f.commandSpecific1 = packet->b.packetSequenceStep;

	// Send the packet
	return SendControlPacket(&newPacket);
}


#pragma CODE_SECTION(ReserveVirtualChannel,"FlashCode");
Uint16 ReserveVirtualChannel(Uint16 port, Uint16 packetID)
{
	Uint16 i;

	if(packetID == PACKET_ID_UNDEFINED)
	{
		LogError(__FILE__,__LINE__);
		return ERROR;
	}

	// Find an open virtual channel
	for(i = 0; i < NUM_VIRTUAL_DATA_CHANNELS; i++)
	{
		if(globals.processing.virtualChannels[port][i].packetID == VIRTUAL_CHANNEL_AVAILABLE &&
			globals.processing.virtualChannels[port][i].virtualChannelBlockedCount == 0)
		{
			// Store the virtual channel settings
			globals.processing.virtualChannels[port][i].packetID = packetID;

			// Update the virtual channel statistics
			globals.statistics.virtualChannel.currentVirtualChannelLoad[port]++;
			if(globals.statistics.virtualChannel.currentVirtualChannelLoad[port] > globals.statistics.virtualChannel.maxVirtualChannelLoad[port])
				globals.statistics.virtualChannel.maxVirtualChannelLoad[port] = globals.statistics.virtualChannel.currentVirtualChannelLoad[port];

			// Return the virtual channel
			globals.processing.lastVirtualChannelAssigned = i;
			return i;
		}
	}

	// If it's here, it means it couldn't find a virtual channel, so return undefined
	LogWarning(__FILE__,__LINE__);
	return VIRTUAL_CHANNEL_UNDEFINED;
}

#pragma CODE_SECTION(ReleaseVirtualChannel,"FlashCode");
Uint16 ReleaseVirtualChannel(Uint16 port, Uint16 virtualChannel)
{
	// Release the virtual channel
	if(globals.processing.virtualChannels[port][virtualChannel].packetID != VIRTUAL_CHANNEL_AVAILABLE)
	{
		globals.processing.virtualChannels[port][virtualChannel].packetID = VIRTUAL_CHANNEL_AVAILABLE;
		globals.processing.virtualChannels[port][virtualChannel].virtualChannelBlockedCount = 0;

		// Update the virtual channel statistics
		globals.statistics.virtualChannel.currentVirtualChannelLoad[port]--;
	}
	else
	{
		LogError(__FILE__,__LINE__);
		return ERROR;
	}

	return SUCCESS;
}

#pragma CODE_SECTION(GetPortFromPacketID,"FlashCode");
Uint16 GetPortFromPacketID(Uint16 packetID)
{
	Uint16 i, j;

	for(i = 0; i < NUM_PORTS; i++)
	{
		for(j = 0; j < NUM_VIRTUAL_DATA_CHANNELS; j++)
		{
			if(globals.processing.virtualChannels[i][j].packetID == packetID)
				return i;
		}
	}

	LogError(__FILE__,__LINE__);
	return PORT_UNDEFINED;
}

#pragma CODE_SECTION(GetVirtualChannelFromPacketID,"FlashCode");
Uint16 GetVirtualChannelFromPacketID(Uint16 packetID)
{
	Uint16 i, j;

	for(i = 0; i < NUM_PORTS; i++)
	{
		for(j = 0; j < NUM_VIRTUAL_DATA_CHANNELS; j++)
		{
			if(globals.processing.virtualChannels[i][j].packetID == packetID)
				return j;
		}
	}

	LogError(__FILE__,__LINE__);
	return VIRTUAL_CHANNEL_UNDEFINED;
}
