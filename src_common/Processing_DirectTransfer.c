#include"System.h"

#pragma CODE_SECTION(SetDataTransferLookup,"FlashCode");
Uint16 SetDataTransferLookup(struct Packet* packet)
{
	Uint16 i, j;

	// Find the next available data transfer lookup slot
	for(i = 0; i < NUM_DATA_PACKET_LOOKUPS; i++)
	{
		if(globals.processing.dataTransferPackets[i].transferInfo.packetID == 0)
		{
			// Copy the data buffer
			globals.processing.dataTransferPackets[i].dataBuffer = MemAlloc(packet->dataBufferInfo.dataBufferLength);
			if(globals.processing.dataTransferPackets[i].dataBuffer == NULL)
			{
				LogWarning(__FILE__,__LINE__);
				return ERROR;
			}
			for(j = 0; j < packet->dataBufferInfo.dataBufferLength; j++)
				globals.processing.dataTransferPackets[i].dataBuffer[j] = packet->dataBuffer[j];
			globals.processing.dataTransferPackets[i].dataBufferInfo.dataBufferLength = packet->dataBufferInfo.dataBufferLength;

			// Store the direct transfer information
			globals.processing.dataTransferPackets[i].transferInfo.packetID = packet->a.packetID;
			globals.processing.dataTransferPackets[i].transferInfo.port = packet->transmissionInfo.portTransmittedOn;
			globals.processing.dataTransferPackets[i].transferInfo.virtualChannel = packet->transmissionInfo.virtualChannelTransmittedOn;
			globals.processing.dataTransferPackets[i].dataBufferInfo.timeToLive = TIME_TO_LIVE_DATA_TRANSFER;
			globals.processing.dataTransferPackets[i].path = packet->path;
			globals.processing.dataTransferPackets[i].serviceTags.sourceServiceTag = packet->c.sourceServiceTag;
			globals.processing.dataTransferPackets[i].serviceTags.destinationServiceTag = packet->c.destinationServiceTag;

			// Update the statistics
			globals.statistics.packet.numDataTransfersSetup++;

			return SUCCESS;
		}
	}

	// If it got here, there weren't any available data transfers
	LogError(__FILE__,__LINE__);
	return ERROR;
}

#pragma CODE_SECTION(GetDataTransferIndex,"FlashCode");
Uint16 GetDataTransferIndex(Uint16 packetID)
{
	Uint16 i;

	// Search for the packet
	for(i = 0; i < NUM_PORTS * NUM_VIRTUAL_DATA_CHANNELS; i++)
	{
		if(globals.processing.dataTransferPackets[i].transferInfo.packetID == packetID)
			return i;
	}

	// If it's here, it means it didn't find the packet
	LogError(__FILE__,__LINE__);
	return ERROR;
}

#pragma CODE_SECTION(ClearDataTransferLookup,"FlashCode");
Uint16 ClearDataTransferLookup(Uint16 packetID)
{
	Uint16 i;

	// Search for the packet
	for(i = 0; i < NUM_PORTS * NUM_VIRTUAL_DATA_CHANNELS; i++)
	{
		if(globals.processing.dataTransferPackets[i].transferInfo.packetID == packetID)
		{
			globals.processing.dataTransferPackets[i].transferInfo.packetID = 0;
			ReleaseVirtualChannel(globals.processing.dataTransferPackets[i].transferInfo.port,
				globals.processing.dataTransferPackets[i].transferInfo.virtualChannel);
			return MemFree(globals.processing.dataTransferPackets[i].dataBuffer);
		}
	}

	// If it's here, it means it didn't find the packet
	LogError(__FILE__,__LINE__);
	return ERROR;
}

#pragma CODE_SECTION(SetupDirectTransfer,"FlashCode");
Uint16 SetupDirectTransfer(struct Packet* packet)
{
	Uint16 startTime;

	if(packet->a.packetID == PACKET_ID_UNDEFINED)
	{
		LogError(__FILE__,__LINE__);
		return ERROR;
	}

	// Get the port to transmit on
	packet->transmissionInfo.portTransmittedOn = GetTransmitPort(packet);
	if(packet->transmissionInfo.portTransmittedOn == PORTA || packet->transmissionInfo.portTransmittedOn == PORTB ||
		packet->transmissionInfo.portTransmittedOn == PORTC || packet->transmissionInfo.portTransmittedOn == PORTD)
	{
		// Get the virtual channel to transmit on, trying multiple times if necessary
		packet->transmissionInfo.virtualChannelTransmittedOn = 
			ReserveVirtualChannel(packet->transmissionInfo.portTransmittedOn,packet->a.packetID);	
		startTime = globals.statistics.system.ticksPassed;
		while(packet->transmissionInfo.virtualChannelTransmittedOn >= VIRTUAL_CHANNEL_CONTROL)
		{
			if(globals.statistics.system.ticksPassed - startTime > RESERVE_TIMEOUT)
			{
				LogError(__FILE__,__LINE__);
				return ERROR;
			}
			packet->transmissionInfo.virtualChannelTransmittedOn = 
				ReserveVirtualChannel(packet->transmissionInfo.portTransmittedOn,packet->a.packetID);
			TSK_sleep(10);
		}
		globals.processing.directTransfers[packet->transmissionInfo.portArrivedOn][packet->transmissionInfo.virtualChannelArrivedOn].destinationVirtualChannel = 
			packet->transmissionInfo.virtualChannelTransmittedOn;

		// Store the port to transmit on
		globals.processing.directTransfers[packet->transmissionInfo.portArrivedOn][packet->transmissionInfo.virtualChannelArrivedOn].destinationPort = 
			packet->transmissionInfo.portTransmittedOn;
		globals.processing.directTransfers[packet->transmissionInfo.portArrivedOn][packet->transmissionInfo.virtualChannelArrivedOn].packetID = 
			packet->a.packetID;
		globals.processing.directTransfers[packet->transmissionInfo.portArrivedOn][packet->transmissionInfo.virtualChannelArrivedOn].timeToLive = 
			TIME_TO_LIVE_DIRECT_TRANSFER;

		// Update the statistics
		globals.statistics.packet.numDirectTransfersSetup++;
	}
	else
		return ERROR;

	return SUCCESS;
}

#pragma CODE_SECTION(ClearDirectTransferByPacketID,"FlashCode");
Uint16 ClearDirectTransferByPacketID(Uint16 packetID)
{
	Uint16 i, j;

	for(i = 0; i < NUM_PORTS; i++)
	{
		for(j = 0; j < NUM_VIRTUAL_DATA_CHANNELS; j++)
		{
			if(globals.processing.virtualChannels[i][j].packetID == packetID)
			{
				// Release the virtual channel
				globals.processing.virtualChannels[i][j].packetID = VIRTUAL_CHANNEL_AVAILABLE;
				globals.processing.virtualChannels[i][j].virtualChannelBlockedCount = 0;
				
				// Update the virtual channel statistics
				globals.statistics.virtualChannel.currentVirtualChannelLoad[i]--;

				// Clear the direct transfer
				globals.processing.directTransfers[i][j].destinationPort = PORT_UNDEFINED;

				return SUCCESS;
			}
		}
	}

	LogWarning(__FILE__,__LINE__);
	return ERROR;
}

#pragma CODE_SECTION(SetupDirectBuffer,"FlashCode");
Uint16 SetupDirectBuffer(struct Packet* packet,Uint16 (*callbackFunction)(struct Packet*))
{
	if(globals.processing.directBuffer[packet->transmissionInfo.portArrivedOn][packet->transmissionInfo.virtualChannelArrivedOn].packet.dataBuffer != NULL)
	{
		LogError(__FILE__,__LINE__);
		globals.statistics.system.numErrors++;
		return ERROR;
	}
	else
	{
		// Create the data buffer
		globals.processing.directBuffer[packet->transmissionInfo.portArrivedOn][packet->transmissionInfo.virtualChannelArrivedOn].packet.dataBuffer = 
			MemAlloc(packet->f.commandSpecific0);
		if(globals.processing.directBuffer[packet->transmissionInfo.portArrivedOn][packet->transmissionInfo.virtualChannelArrivedOn].packet.dataBuffer == NULL)
			return ERROR;

		// Copy the packet to the buffer description
		CopyPacket(packet,&globals.processing.directBuffer[packet->transmissionInfo.portArrivedOn][packet->transmissionInfo.virtualChannelArrivedOn].packet);

		// Set the rest of the buffer description info
		globals.processing.directBuffer[packet->transmissionInfo.portArrivedOn][
			packet->transmissionInfo.virtualChannelArrivedOn].packet.dataBufferInfo.dataBufferLength = packet->f.commandSpecific0;
		globals.processing.directBuffer[packet->transmissionInfo.portArrivedOn][
			packet->transmissionInfo.virtualChannelArrivedOn].bufferStatus.currentDataIndex = 0;
		globals.processing.directBuffer[packet->transmissionInfo.portArrivedOn][
			packet->transmissionInfo.virtualChannelArrivedOn].callbackFunction = callbackFunction;
		globals.processing.directBuffer[packet->transmissionInfo.portArrivedOn][
			packet->transmissionInfo.virtualChannelArrivedOn].bufferStatus.timeToLive = TIME_TO_LIVE_DIRECT_BUFFER;

		// Update the statistics
		globals.statistics.packet.numDirectBuffersSetup++;
		
		return SUCCESS;
	}
}

#pragma CODE_SECTION(CleanupDirectData,"FlashCode");
void CleanupDirectData(void* parameter, Uint16 followUpItemIndex)
{
	Uint16 i, j, port, virtualChannel;
	struct Mail message;

	// Check if there are any virtual channels that need to be cleared
	for(i = 0; i < NUM_PORTS; i++)
	{
		for(j = 0; j < NUM_VIRTUAL_DATA_CHANNELS; j++)
		{
			// Check if this virtual channel is active
			if(globals.processing.virtualChannels[i][j].virtualChannelBlockedCount > 0)
				globals.processing.virtualChannels[i][j].virtualChannelBlockedCount--;
		}
	}

	// Check if there are any data transfers that need to be tagged or removed
	for(i = 0; i < NUM_DATA_PACKET_LOOKUPS; i++)
	{
		// Check if this data transfer is active
		if(globals.processing.dataTransferPackets[i].transferInfo.packetID != 0)
		{
			// Decrement the time to live for the transfer
			globals.processing.dataTransferPackets[i].dataBufferInfo.timeToLive--;
			
			// Check if this transfer has expired
			if(globals.processing.dataTransferPackets[i].dataBufferInfo.timeToLive <= 0)
			{
#if TEST == TEST_PROTOCOL
				
				if(globals.processing.dataTransferPackets[i].transferInfo.packetID == protocolTestPacketIDs[0])
					receivedResponse[0] = true;
				if(globals.processing.dataTransferPackets[i].transferInfo.packetID == protocolTestPacketIDs[1])
					receivedResponse[1] = true;
				if(globals.processing.dataTransferPackets[i].transferInfo.packetID == protocolTestPacketIDs[2])
					receivedResponse[2] = true;
				if(SEM_count(&TestServiceSem) == 0)
					SEM_post(&TestServiceSem);

#elif TEST == TEST_MPI
				
				// Send a message to the MPI send service
				CreateMessage(&message, 
					NULL,
					0,
					MAIL_DATA_TRANSFER_FAILED,
					globals.processing.dataTransferPackets[i].transferInfo.packetID,
					MPI_ANY_REQUEST->requestType,
					MPI_ANY_REQUEST->requestID,
					ADDRESS_UNDEFINED,
					ADDRESS_UNDEFINED,
					TEST_SERVICE_TAG,
					TEST_SERVICE_TAG);
				SendMail(TEST_SERVICE_TAG, &message);

#endif
				port = globals.processing.dataTransferPackets[i].transferInfo.port;
				virtualChannel = globals.processing.dataTransferPackets[i].transferInfo.virtualChannel;
				ClearDataTransferLookup(globals.processing.dataTransferPackets[i].transferInfo.packetID);
				globals.processing.virtualChannels[port][virtualChannel].virtualChannelBlockedCount = VIRTUAL_CHANNEL_INITIAL_BLOCKED_COUNT;
				globals.statistics.packet.numDataTransfersExpired++;
			}
		}
	}

	// Check if there are any direct transfers that need to be tagged or removed
	for(i = 0; i < NUM_PORTS; i++)
	{
		for(j = 0; j < NUM_VIRTUAL_DATA_CHANNELS; j++)
		{
			// Check if this direct transfer is active
			if(globals.processing.directTransfers[i][j].destinationPort != PORT_UNDEFINED)
			{
				// Decrement the time to live for the transfer
				globals.processing.directTransfers[i][j].timeToLive--;
					
				// Check if this transfer has expired
				if(globals.processing.directTransfers[i][j].timeToLive <= 0)
				{
					ReleaseVirtualChannel(globals.processing.directTransfers[i][j].destinationPort,
						globals.processing.directTransfers[i][j].destinationVirtualChannel);
					globals.processing.directTransfers[i][j].destinationPort = PORT_UNDEFINED;
					globals.statistics.packet.numDirectTransfersExpired++;
				}
			}
		}
	}

	// Check if there are any direct buffers that need to be tagged or removed
	for(i = 0; i < NUM_PORTS; i++)
	{
		for(j = 0; j < NUM_VIRTUAL_DATA_CHANNELS; j++)
		{
			// Check if this direct buffer is active
			if(globals.processing.directBuffer[i][j].packet.dataBuffer != NULL)
			{
				// Decrement the time to live for the buffer
				globals.processing.directBuffer[i][j].bufferStatus.timeToLive--;
				
				// Check if this buffer has expired
				if(globals.processing.directBuffer[i][j].bufferStatus.timeToLive <= 0)
				{
					MemFree(globals.processing.directBuffer[i][j].packet.dataBuffer);
					globals.processing.directBuffer[i][j].packet.dataBuffer = NULL;
					globals.statistics.packet.numDirectBuffersExpired++;
				}
			}
		}
	}
}
