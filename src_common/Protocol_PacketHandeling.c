#include"System.h"

double mincrctime = 10000, maxcrctime = 0, minunmarshall = 100000, maxunmarshall = 0;

// Initialize the packet
#pragma CODE_SECTION(InitializePacket,"FlashCode");
void InitializePacket(struct Packet* packet, Uint16 packetID)
{
	// Initialize all of the packet header fields
	packet->a.version = CURRENT_PACKET_VERSION;
	packet->a.communicationType = COMM_TYPE_UNDEFINED;
	packet->b.command = COMMAND_UNDEFINED;
	packet->b.packetSequenceStep = 0;
	packet->c.sourceServiceTag = 0;
	packet->c.destinationServiceTag = 0;
	packet->path = 0;
	packet->f.commandSpecific0 = 0;
	packet->f.commandSpecific1 = 0;
	packet->g.commandSpecific2 = 0;
	packet->g.commandSpecific3 = 0;
	packet->h.commandSpecific4 = 0;
	packet->h.CRC = 0;

	// Set the packet ID
	if(packetID == PACKET_ID_UNDEFINED)
	{
		packet->a.packetID = globals.protocol.transmitCounts++;
	
		// If the transmitCount rolled over, set it to 1 since 0 is reserved for other uses
		if(globals.protocol.transmitCounts == 256)
			globals.protocol.transmitCounts = 1;
	}
	else
		packet->a.packetID = packetID;

	// Initialize the packet data field entries
	packet->dataBuffer = NULL;
	packet->dataBufferInfo.dataBufferLength = 0;

	// Initialize the transmission variables
	packet->transmissionInfo.portTransmittedOn = PORT_UNDEFINED;
	packet->transmissionInfo.virtualChannelTransmittedOn = VIRTUAL_CHANNEL_UNDEFINED;
	packet->transmissionInfo.source = globals.protocol.address;
	packet->transmissionInfo.destination = 0;
}

#pragma CODE_SECTION(InitializePacketFromPreviousSequence,"FlashCode");
void InitializePacketFromPreviousSequence(struct Packet* newPacket,struct Packet* oldPacket)
{
	// Initialize all of the packet header fields
	newPacket->a.version = CURRENT_PACKET_VERSION;
	newPacket->a.communicationType = oldPacket->a.communicationType;
	newPacket->a.packetID = oldPacket->a.packetID;
	newPacket->b.command = oldPacket->b.command;
	newPacket->b.packetSequenceStep = SEQUENCE_UNDEFINED;
	newPacket->c.destinationServiceTag = oldPacket->c.sourceServiceTag;
	newPacket->path = 0;
	newPacket->f.commandSpecific0 = 0;
	newPacket->f.commandSpecific1 = 0;
	newPacket->g.commandSpecific2 = 0;
	newPacket->g.commandSpecific3 = 0;
	newPacket->h.commandSpecific4 = 0;
	newPacket->h.CRC = 0;

	// Initialize the packet data field entries
	newPacket->dataBuffer = NULL;
	newPacket->dataBufferInfo.dataBufferLength = 0;

	// Initialize the transmission variables
	newPacket->transmissionInfo.portTransmittedOn = PORT_UNDEFINED;
	newPacket->transmissionInfo.virtualChannelTransmittedOn = VIRTUAL_CHANNEL_UNDEFINED;
	newPacket->transmissionInfo.source = globals.protocol.address;
	newPacket->transmissionInfo.destination = 0;
}

#pragma CODE_SECTION(InitializePacketFromSelf,"FlashCode");
void InitializePacketFromSelf(struct Packet* packet)
{
	// Initialize the packet data field entries
	packet->dataBuffer = NULL;
	packet->dataBufferInfo.dataBufferLength = 0;

	// Initialize the transmission variables
	packet->transmissionInfo.portTransmittedOn = PORT_UNDEFINED;
	packet->transmissionInfo.virtualChannelTransmittedOn = VIRTUAL_CHANNEL_UNDEFINED;
}

// Unmarshall a packet into a packet header structure
#pragma CODE_SECTION(UnmarshallPacketHeader,"FlashCode");
Uint16 UnmarshallPacketHeader(Uint16* flit,struct Packet* packet)
{
	// Variable declarations
	Uint16 crc, i;
	Uint32 path;

	// Calculate the CRC of the complete flit (including CRC), but excluding the port and virtual channel in the first character of the flit
	packet->h.CRC = flit[8] & 0x00FF;
	flit[8] &= 0xFF00;
	crc = CRC8_INITIAL;
	for(i = 1; i < NUM_CHARACTERS_IN_FLIT; i++)
	{
		// Computer the upper half
		crc = (crc << 8) ^ CRC8Table[((flit[i] >> 8) ^ crc) & 0xFF];

		// Computer the lower half
		crc = (crc << 8) ^ CRC8Table[((flit[i] & 0xFF) ^ crc) & 0xFF];
	}
	crc &= 0x00FF;

	// Check if a transmission error occured
	if(crc != packet->h.CRC)
	{
		globals.statistics.packet.numControlCrcErrors++;
		LogWarning(__FILE__,__LINE__);
		return ERROR;
	}

	// Check which version of the protocol is being used and parse accordingly
	packet->a.version = flit[1] >> 12;
	if(packet->a.version != CURRENT_PACKET_VERSION)
	{
		LogWarning(__FILE__,__LINE__);
		return ERROR;
	}
		
	// Unmarshall the packet data
	packet->a.communicationType = (flit[1] & 0x0F00) >> 8;
	packet->a.packetID = flit[1] & 0x00FF;
	packet->b.command = flit[2] >> 8;
	packet->b.packetSequenceStep = flit[2] & 0x00FF;
	packet->c.sourceServiceTag = flit[3] >> 8;
	packet->c.destinationServiceTag = flit[3] & 0x00FF;
	packet->path = (((Uint32)flit[4]) << 16) + ((Uint32)flit[5]);
	packet->f.commandSpecific0 = flit[6] >> 8;
	packet->f.commandSpecific1 = flit[6] & 0x00FF;
	packet->g.commandSpecific2 = flit[7] >> 8;
	packet->g.commandSpecific3 = flit[7] & 0x00FF;
	packet->h.commandSpecific4 = flit[8] >> 8;

	// Set the data buffer length
	packet->dataBufferInfo.dataBufferLength = 0;

	// Copy the paths since they will be modified in the search
	path = packet->path;

	// Save the source and destination
	packet->transmissionInfo.source = path >> 28;
	packet->transmissionInfo.destination = (flit[0] & 0x0F00) >> 8;
	packet->transmissionInfo.portTransmittedOn = ADDRESS_UNDEFINED;

	return SUCCESS;
}


// Marshall a packet from a packet header structure
#pragma CODE_SECTION(MarshallPacketHeader,"FlashCode");
Uint16 MarshallPacketHeader(struct Packet* packet, Uint16* flit)
{
	// Variable declarations
	Uint16 crc, i;

	// Find the path to take
	if(packet->path == 0 && packet->a.communicationType == COMM_TYPE_UNICAST)
	{
		if(globals.routing.operatingInLimpMode)
		{

#if defined(IS_ROOT)

			packet->path = globals.routing.limpModePaths[packet->transmissionInfo.destination];
			if(packet->path == 0)
			{
				LogError(__FILE__,__LINE__);
				return ERROR;
			}

#elif defined(IS_ROUTER)

			if(packet->transmissionInfo.destination == ROOT_ADDRESS)
				packet->path = globals.routing.limpModePath;
			else
			{
				LogError(__FILE__,__LINE__);
				return ERROR;
			}

#endif
		}
		else
		{
			if(GenerateRoutingPath(globals.protocol.address, packet->transmissionInfo.destination, packet) == ERROR)
			{
				LogError(__FILE__,__LINE__);
				return ERROR;
			}
		}
	}

	// Find the port to transmit on
	if(packet->transmissionInfo.portTransmittedOn == PORT_UNDEFINED)
	{
		packet->transmissionInfo.portTransmittedOn = GetTransmitPort(packet);
		if(packet->transmissionInfo.portTransmittedOn >= NUM_PORTS)
			return ERROR;
	}

	// Marshall the packet data
	flit[1] =  packet->a.version << 12;
	flit[1] += packet->a.communicationType << 8;
	flit[1] += packet->a.packetID;
	flit[2] =  packet->b.command << 8;
	flit[2] += packet->b.packetSequenceStep;
	flit[3] =  packet->c.sourceServiceTag << 8;
	flit[3] += packet->c.destinationServiceTag;
	flit[4] =  packet->path >> 16;
	flit[5] =  packet->path & 0x0000FFFF;
	flit[6] =  packet->f.commandSpecific0 << 8;
	flit[6] += packet->f.commandSpecific1;
	flit[7] =  packet->g.commandSpecific2 << 8;
	flit[7] += packet->g.commandSpecific3;
	flit[8] =  packet->h.commandSpecific4 << 8;

	// Calculate the CRC and marshall it
	crc = CRC8_INITIAL;
	for(i = 1; i < NUM_CHARACTERS_IN_FLIT; i++)
	{
		// Computer the upper half
		crc = (crc << 8) ^ CRC8Table[((flit[i] >> 8) ^ crc) & 0xFF];

		// Computer the lower half
		crc = (crc << 8) ^ CRC8Table[((flit[i] & 0xFF) ^ crc) & 0xFF];
	}
	crc &= 0x00FF;
	
	// Store the CRC to the packet and the marshalled flit
	packet->h.CRC = crc;
	flit[8] += crc;

	return SUCCESS;
}

// Unmarshall data from a 10-bit array into 16-bit arrays
#pragma CODE_SECTION(UnmarshallData,"FlashCode");
void UnmarshallData(Uint16* flit, Uint16 portArrivedOn, Uint16 virtualChannelArrivedOn)
{
	Uint16 i;
	struct DirectBuffer* buffer = &globals.processing.directBuffer[portArrivedOn][virtualChannelArrivedOn];

	for(i = 1; i < NUM_CHARACTERS_IN_FLIT; i++)
	{
		if(buffer->bufferStatus.currentDataIndex < buffer->packet.dataBufferInfo.dataBufferLength)
		{
			buffer->packet.dataBuffer[buffer->bufferStatus.currentDataIndex++] = flit[i];
		}
	}
}

#pragma CODE_SECTION(MarshallData,"FlashCode");
void MarshallData(Uint16* flit, Uint16* dataBuffer, Uint16 dataBufferLength, Uint16* dataBufferIndex, Uint16 virtualChannelTransmittedOn)
{
	Uint16 i;
	
	// Initialize the flit
	for(i = 1; i < NUM_CHARACTERS_IN_FLIT; i++)
	{
		if(*dataBufferIndex < dataBufferLength)
		{
			flit[i] = dataBuffer[(*dataBufferIndex)++];
		}
	}
}

#pragma CODE_SECTION(CopyPacket,"FlashCode");
void CopyPacket(struct Packet* sourcePacket, struct Packet* destPacket)
{
	int i;

	// Packet header fields
	destPacket->a.version = sourcePacket->a.version;
	destPacket->a.communicationType = sourcePacket->a.communicationType;
	destPacket->a.packetID = sourcePacket->a.packetID;
	destPacket->b.command = sourcePacket->b.command;
	destPacket->b.packetSequenceStep = sourcePacket->b.packetSequenceStep;
	destPacket->c.sourceServiceTag = sourcePacket->c.sourceServiceTag;
	destPacket->c.destinationServiceTag = sourcePacket->c.destinationServiceTag;
	destPacket->path = sourcePacket->path;
	destPacket->f.commandSpecific0 = sourcePacket->f.commandSpecific0;
	destPacket->f.commandSpecific1 = sourcePacket->f.commandSpecific1;
	destPacket->g.commandSpecific2 = sourcePacket->g.commandSpecific2;
	destPacket->g.commandSpecific3 = sourcePacket->g.commandSpecific3;
	destPacket->h.commandSpecific4 = sourcePacket->h.commandSpecific4;
	destPacket->h.CRC = sourcePacket->h.CRC;

	// Data payload
	for(i = 0; i < sourcePacket->dataBufferInfo.dataBufferLength; i++)
		destPacket->dataBuffer[i] = sourcePacket->dataBuffer[i];
	destPacket->dataBufferInfo.dataBufferLength = sourcePacket->dataBufferInfo.dataBufferLength;

	// Non-protocol packet fields
	destPacket->dataBufferInfo.bufferIndex = sourcePacket->dataBufferInfo.bufferIndex;
	destPacket->transmissionInfo.portArrivedOn = sourcePacket->transmissionInfo.portArrivedOn;
	destPacket->transmissionInfo.source = sourcePacket->transmissionInfo.source;
	destPacket->transmissionInfo.destination = sourcePacket->transmissionInfo.destination;
}

#pragma CODE_SECTION(GetTransmitPort,"FlashCode");
Uint16 GetTransmitPort(struct Packet* packet)
{
	Uint16 i, j, nextNode = ADDRESS_UNDEFINED;
	Uint32 path;

	// Extract the next node from the path
	path = packet->path;
	for(i = 0; i < MAX_PATH_LENGTH; i++)
	{
		if((path & 0xF0000000) >> 28 == globals.protocol.address)
		{
			nextNode = (path & 0x0F000000) >> 24;
			for(j = 0; j < NUM_PORTS; j++)
			{
#if defined(IS_ROOT)
				if(globals.protocol.globalNeighborInfo[ROOT_ADDRESS][j].nodeAddress == nextNode)
					return j;
#elif defined(IS_ROUTER)
				if(globals.protocol.neighborInfo[j].nodeAddress == nextNode)
					return j;
#endif
			}
			LogError(__FILE__,__LINE__);
			return ERROR;
		}
		path = path << 4;
	}
	if(nextNode == ADDRESS_UNDEFINED)
	{
		LogError(__FILE__,__LINE__);
		return ERROR;
	}

#if defined(IS_ROOT)

	// Find the port the node is connected to
	for(i = 0; i < NUM_PORTS; i++)
		if(globals.protocol.globalNeighborInfo[ROOT_ADDRESS][i].nodeAddress == nextNode)
			return i;

#elif defined(IS_ROUTER)

	// Find the port the node is connected to
	for(i = 0; i < NUM_PORTS; i++)
		if(globals.protocol.neighborInfo[i].nodeAddress == nextNode)
			return i;

#endif

	// If it got here, it couldn't find the node
	LogError(__FILE__,__LINE__);
	return ERROR;
}

#pragma CODE_SECTION(ProcessPacket,"FlashCode");
Uint16 ProcessPacket(struct Packet* packet)
{	
	Uint16 processPacket = false;

	// Check if this packet is being forwarded through this router
	if(packet->a.communicationType == COMM_TYPE_UNICAST && packet->transmissionInfo.destination != globals.protocol.address)
	{
		// Check if this type of packet needs to be processed
		if((packet->b.command == COMMAND_REQUEST_ROUTING_TABLE && packet->b.packetSequenceStep == SEQUENCE_REQUEST_ROUTING_TABLE_REQUEST_TRANSFER) || 
			(packet->b.command == COMMAND_DATA_TRANSFER && packet->b.packetSequenceStep == SEQUENCE_DATA_TRANSFER_REQUEST_TRANSFER) ||
			(packet->b.command == COMMAND_DATA_TRANSFER && packet->b.packetSequenceStep == SEQUENCE_DATA_TRANSFER_ERROR) ||
			(packet->b.command == COMMAND_REQUEST_ROUTING_TABLE && packet->b.packetSequenceStep == SEQUENCE_REQUEST_ROUTING_TABLE_ERROR))
			processPacket = true;
		else
		{
			// Check if this is a direct transfer, or a control packet, direct transfers shouldn't show up here
			if(packet->transmissionInfo.virtualChannelArrivedOn == VIRTUAL_CHANNEL_CONTROL)
			{
				// Get the port to transmit on
				packet->transmissionInfo.portTransmittedOn = GetTransmitPort(packet);
				if(packet->transmissionInfo.portTransmittedOn >= PORT_UNDEFINED)
					return ERROR;
				else
				{
					InitializePacketFromSelf(packet);
					SendControlPacket(packet);
				}
			}
			else
				return ERROR;
		}
	}
	else
		processPacket = true;

	if(processPacket)
	{
		// Take varying actions depending on the command
		switch(packet->b.command)
		{
			case COMMAND_UNDEFINED:
				LogError(__FILE__,__LINE__);
				return ERROR;

			case COMMAND_DISCOVER_NEIGHBORS:
				return ProcessCommandDiscoverNeighbors(packet);

			case COMMAND_REQUEST_ADDRESS:
				return ProcessCommandRequestAddress(packet);

			case COMMAND_REPORT_NEIGHBORS:
				return ProcessCommandReportNeighbors(packet);

			case COMMAND_REQUEST_ROUTING_TABLE:
				return ProcessCommandRequestRoutingTable(packet);

			case COMMAND_COMM_FAILURE:
				return ProcessCommandCommFailure(packet);

			case COMMAND_NEW_ROUTING_TABLE_AVAILABLE:
				return ProcessCommandNewRoutingTableAvailable(packet);

			case COMMAND_DATA_TRANSFER:
				return ProcessCommandDataTransfer(packet);

			case COMMAND_MPI_CONTROL:
				return ProcessCommandMPIControl(packet);

			default:
				LogError(__FILE__,__LINE__);
				return ERROR;
		}
	}
	else
		return SUCCESS;
}
