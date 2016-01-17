#include"System.h"

#pragma CODE_SECTION(ProcessCommandDataTransfer,"FlashCode");
Uint16 ProcessCommandDataTransfer(struct Packet* packet)
{	
	Uint16 flit[NUM_CHARACTERS_IN_FLIT];
	Uint16 dataTransferIndex, portTransmittedOn, virtualChannelTransmittedOn;
	struct Packet newPacket;
#if PROFILE_LATENCY == true
	double executionTime;
#endif

#if TEST == TEST_MPI
	struct Mail message;
#endif

	// Process the packet according to which sequence step the packet is on
	switch(packet->b.packetSequenceStep)
	{
		case SEQUENCE_DATA_TRANSFER_REQUEST_TRANSFER:

			if(packet->transmissionInfo.destination != globals.protocol.address)
			{
				// Re-initialize the packet
				InitializePacketFromSelf(packet);

				// Reserve the transfer path
				if(SetupDirectTransfer(packet) == ERROR)
					return ERROR;

				// Finalize the the packet
				if(MarshallPacketHeader(packet,flit) == ERROR)
					return ERROR;

				// Transmit the packet header 
				flit[0] = (packet->transmissionInfo.destination << 8) + FLIT_TYPE_HEADER + packet->transmissionInfo.virtualChannelTransmittedOn;
				return EnqueOutboundFlit(flit,packet->transmissionInfo.portTransmittedOn);
			}
			else
			{
				// Setup the direct buffer
				if(SetupDirectBuffer(packet,SequenceDataTransferTransferCompleted) == ERROR)
				{
					SendErrorPacket(packet,PROTOCOL_ERROR_NO_BUFFERS_AVAILABLE);
					return ERROR;
				}
				else
				{
					// Grant permission for the transfer
					InitializePacketFromPreviousSequence(&newPacket,packet);
					newPacket.transmissionInfo.destination = packet->transmissionInfo.source;
					newPacket.b.packetSequenceStep = SEQUENCE_DATA_TRANSFER_REQUEST_ACCEPTED;
					return SendControlPacket(&newPacket);
				}
			}

		case SEQUENCE_DATA_TRANSFER_REQUEST_ACCEPTED:
#if PROFILE_LATENCY == true
				executionTime = TimeDifference(globals.statistics.profiling.latencyStartTime, timer0Registers.TIM.all);
				globals.statistics.profiling.avgLatency = 
					(globals.statistics.profiling.avgLatency * globals.statistics.profiling.numLatencySamples) + executionTime;
				globals.statistics.profiling.numLatencySamples++;
				globals.statistics.profiling.avgLatency /= globals.statistics.profiling.numLatencySamples;
				if(executionTime < globals.statistics.profiling.minLatency)
					globals.statistics.profiling.minLatency = executionTime;
				else if(executionTime > globals.statistics.profiling.maxLatency)
					globals.statistics.profiling.maxLatency = executionTime;
#endif
			// Get the data transfer index
			dataTransferIndex = GetDataTransferIndex(packet->a.packetID);
			if(dataTransferIndex == ERROR)
				return ERROR;

			// Set the port and virtual channel
			portTransmittedOn = GetPortFromPacketID(packet->a.packetID);
			if(portTransmittedOn >= PORT_UNDEFINED)
				return ERROR;
			virtualChannelTransmittedOn = GetVirtualChannelFromPacketID(packet->a.packetID);
			if(virtualChannelTransmittedOn >= VIRTUAL_CHANNEL_UNDEFINED)
				return ERROR;

			// Initialize the packet
			InitializePacketFromPreviousSequence(&newPacket,packet);

			// Initialize the packet
			newPacket.transmissionInfo.portTransmittedOn = portTransmittedOn;
			newPacket.transmissionInfo.virtualChannelTransmittedOn= virtualChannelTransmittedOn;
			newPacket.transmissionInfo.destination = packet->transmissionInfo.source;
			newPacket.dataBuffer = globals.processing.dataTransferPackets[dataTransferIndex].dataBuffer;
			newPacket.dataBufferInfo.dataBufferLength = globals.processing.dataTransferPackets[dataTransferIndex].dataBufferInfo.dataBufferLength;
			newPacket.path = globals.processing.dataTransferPackets[dataTransferIndex].path;

			// Send the data payload
			return SendDataPacketPayload(&newPacket);

		case SEQUENCE_DATA_TRANSFER_TRANSFER_RECEIVED:

			// Clean up the data transfer
			ClearDataTransferLookup(packet->a.packetID);
			globals.statistics.packet.numDataTransfersSucceeded++;

#if TEST == TEST_PROTOCOL

			if(packet->transmissionInfo.source == 2)
				receivedResponse[0] = true;
			if(packet->transmissionInfo.source == 3)
				receivedResponse[1] = true;
			if(packet->transmissionInfo.source == 4)
				receivedResponse[2] = true;
			if(SEM_count(&TestServiceSem) == 0)
				SEM_post(&TestServiceSem);

#elif TEST == TEST_MPI

			// Send a message to the MPI send service
			CreateMessage(&message, 
				packet->dataBuffer,
				packet->dataBufferInfo.dataBufferLength,
				MAIL_DATA_TRANSFER_SUCCESS,
				packet->a.packetID,
				MPI_ANY_REQUEST->requestType,
				MPI_ANY_REQUEST->requestID,
				packet->transmissionInfo.source,
				packet->transmissionInfo.destination,
				packet->c.sourceServiceTag,
				packet->c.destinationServiceTag);
			SendMail(packet->c.destinationServiceTag, &message);

#endif

			return SUCCESS;

		case SEQUENCE_DATA_TRANSFER_ERROR:

			// If this is not the destination node, then clear the data transfer info
			if(packet->transmissionInfo.destination != globals.protocol.address)
			{
				// Finalize the direct transfer
				ClearDirectTransferByPacketID(packet->a.packetID);
				globals.statistics.packet.numDirectTransfersFailed++;
			}

			// Otherwise handle any cleanup on the local end and notify the running test
			else
			{

#if TEST == TEST_PROTOCOL

				if(packet->transmissionInfo.source == 2)
					receivedResponse[0] = true;
				if(packet->transmissionInfo.source == 3)
					receivedResponse[1] = true;
				if(packet->transmissionInfo.source == 4)
					receivedResponse[2] = true;
				if(SEM_count(&TestServiceSem) == 0)
					SEM_post(&TestServiceSem);

#elif TEST == TEST_MPI

				// Send a message to the MPI send service
				CreateMessage(&message, 
					packet->dataBuffer,
					packet->dataBufferInfo.dataBufferLength,
					MAIL_DATA_TRANSFER_FAILED,
					packet->a.packetID,
					MPI_ANY_REQUEST->requestType,
					MPI_ANY_REQUEST->requestID,
					packet->transmissionInfo.source,
					packet->transmissionInfo.destination,
					packet->c.sourceServiceTag,
					packet->c.destinationServiceTag);
				SendMail(packet->c.destinationServiceTag, &message);

#endif

				// Clean up the data transfer
				ClearDataTransferLookup(packet->a.packetID);
				globals.statistics.packet.numDataTransfersFailed++;
			}

			// Log the error
			LogPacketError(__FILE__,__LINE__,packet);
			
			return SUCCESS;

		default: 

			LogError(__FILE__,__LINE__);
			return ERROR;
	}
}

#pragma CODE_SECTION(SequenceDataTransferTransferCompleted,"FlashCode");
Uint16 SequenceDataTransferTransferCompleted(struct Packet* packet)
{
	Uint16 i;
	struct Packet newPacket;
#if TEST == TEST_MPI
	struct Mail message;
	Uint16 j, k, transferFound, transferLength;
#endif

	globals.statistics.packet.numDirectBuffersSucceeded++;

#if TEST == TEST_PROTOCOL
	MemFree(packet->dataBuffer);
	packet->dataBuffer = NULL;
#elif TEST == TEST_MPI

	// Find the packet in the MPI transfers list and add it
	transferFound = false;
	for(i = 0; i < MAX_MPI_TRANSFERS; i++)
	{
		// Check if this is a transfer is active
		if(globals.mpiManager.receiveTransfers[i].transferInfo.destinationServiceTag != SERVICE_TAG_UNDEFINED)
		{
			// Check each data partition separately, since some may have arrived and some may have not arrived
			for(j = 0; j < globals.mpiManager.receiveTransfers[i].transferInfo.numPartitions; j++)
			{
				// If the packet IDs match, mark the transfer as complete
				if(globals.mpiManager.receiveTransfers[i].packetIDs[j] == packet->a.packetID)
				{
					// Store the information
					for(k = 0; k < globals.mpiManager.receiveTransfers[i].dataPartitionLengths[j]; k++)
					{
						globals.mpiManager.receiveTransfers[i].data[(255 * j) + k] = packet->dataBuffer[k];
						asm(" NOP");
					}
					MemFree(packet->dataBuffer);
					packet->dataBuffer = NULL;
					globals.mpiManager.receiveTransfers[i].transferInfo.partitionsLeft--;
					transferFound = true;
					break;
				}
			}

			// Check if this transfer is finished
			if(transferFound)
			{							
				// If the transfer is complete send the message to the service provider
				if(globals.mpiManager.receiveTransfers[i].transferInfo.partitionsLeft == 0)
				{
					CreateMessage(&message, 
						&(globals.mpiManager.receiveTransfers[i].data[0]),
						globals.mpiManager.receiveTransfers[i].dataLength,
						MAIL_NEW_DATA_TRANSFER_AVAILABLE,
						PACKET_ID_UNDEFINED,
						globals.mpiManager.receiveTransfers[i].request.requestType,
						globals.mpiManager.receiveTransfers[i].request.requestID, 
						globals.mpiManager.receiveTransfers[i].transferInfo.source,
						globals.protocol.address,
						globals.mpiManager.receiveTransfers[i].transferInfo.sourceServiceTag,
						globals.mpiManager.receiveTransfers[i].transferInfo.destinationServiceTag);
					SendMail(GetServiceIDFromServiceTag(globals.mpiManager.receiveTransfers[i].transferInfo.destinationServiceTag), &message);
					globals.mpiManager.receiveTransfers[i].transferInfo.destinationServiceTag = SERVICE_TAG_UNDEFINED;
				}

				break;
			}
		}
	}

	// If the transfer wasn't found, then this is an initialization packet
	if(!transferFound)
	{
		// Make sure the size is correct
		if(packet->dataBufferInfo.dataBufferLength == MPI_MAX_DATA_PARTITIONS * 2)
		{
			for(i = 0; i < MAX_MPI_TRANSFERS; i++)
			{
				// Check if this transfer is not active
				if(globals.mpiManager.receiveTransfers[i].transferInfo.destinationServiceTag == SERVICE_TAG_UNDEFINED)
				{
					globals.mpiManager.receiveTransfers[i].transferInfo.destinationServiceTag = packet->c.destinationServiceTag;
					globals.mpiManager.receiveTransfers[i].transferInfo.sourceServiceTag = packet->c.sourceServiceTag;
					globals.mpiManager.receiveTransfers[i].transferInfo.source = packet->transmissionInfo.source;
					globals.mpiManager.receiveTransfers[i].request.requestType = MPI_ANY_REQUEST->requestType;
					globals.mpiManager.receiveTransfers[i].request.requestID = MPI_ANY_REQUEST->requestID;
					globals.mpiManager.receiveTransfers[i].transferInfo.numPartitions = 0;
					transferLength = 0;
					for(j = 0; j < MPI_MAX_DATA_PARTITIONS; j++)
					{
						if(packet->dataBuffer[2 * j] != PACKET_ID_UNDEFINED)
						{
							globals.mpiManager.receiveTransfers[i].transferInfo.numPartitions++;
							globals.mpiManager.receiveTransfers[i].packetIDs[j] = packet->dataBuffer[2 * j];
							globals.mpiManager.receiveTransfers[i].dataPartitionLengths[j] = packet->dataBuffer[2 * j + 1];
							transferLength += packet->dataBuffer[2 * j + 1];
						}
						else
						{
							globals.mpiManager.receiveTransfers[i].packetIDs[j] = PACKET_ID_UNDEFINED;
							globals.mpiManager.receiveTransfers[i].dataPartitionLengths[j] = 0;
						}
					}
					globals.mpiManager.receiveTransfers[i].transferInfo.partitionsLeft = globals.mpiManager.receiveTransfers[i].transferInfo.numPartitions;
					globals.mpiManager.receiveTransfers[i].dataLength = transferLength;
					globals.mpiManager.receiveTransfers[i].data = MemAlloc(transferLength);
					while(globals.mpiManager.receiveTransfers[i].data == NULL)
					{
						TSK_sleep(1000);
						globals.mpiManager.receiveTransfers[i].data = MemAlloc(transferLength);
					}
					MemFree(packet->dataBuffer);
					packet->dataBuffer = NULL;

					break;
				}
			}
		}
	}
	
#endif
	
	// Send the acknowledgement to the sender
	InitializePacketFromPreviousSequence(&newPacket,packet);
	newPacket.transmissionInfo.destination = packet->transmissionInfo.source;
	newPacket.b.packetSequenceStep = SEQUENCE_DATA_TRANSFER_TRANSFER_RECEIVED;
	return SendControlPacket(&newPacket);
}

