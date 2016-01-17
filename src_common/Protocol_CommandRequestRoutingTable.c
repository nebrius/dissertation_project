#include"System.h"

#pragma CODE_SECTION(ProcessCommandRequestRoutingTable,"FlashCode");
Uint16 ProcessCommandRequestRoutingTable(struct Packet* packet)
{
	struct Packet newPacket;
#if defined(IS_ROOT)
	Uint16 dataTransferIndex, portTransmittedOn, virtualChannelTransmittedOn;
	Uint16 i, j;
	Uint32 nodeInformation;
#elif defined(IS_ROUTER)
	Uint16 flit[NUM_CHARACTERS_IN_FLIT];
#endif

	// Process the packet according to which sequence step the packet is on
	switch(packet->b.packetSequenceStep)
	{
		case SEQUENCE_REQUEST_ROUTING_TABLE_REQUEST:

#if defined(IS_ROOT)

			// Transfer the data
			InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
			
			// Create the data transfer packet
			newPacket.a.communicationType = COMM_TYPE_UNICAST;
			newPacket.transmissionInfo.destination = packet->transmissionInfo.source;
			newPacket.b.command = COMMAND_REQUEST_ROUTING_TABLE;
			newPacket.b.packetSequenceStep = SEQUENCE_REQUEST_ROUTING_TABLE_REQUEST_TRANSFER;
			newPacket.dataBuffer = MemAlloc(MAX_NUM_NODES + ((MAX_NUM_NODES - 1) * 2 - 1) * 6);
			if(newPacket.dataBuffer == NULL)
				return ERROR;
			newPacket.dataBufferInfo.dataBufferLength = MAX_NUM_NODES + ((MAX_NUM_NODES - 1) * 2 - 1) * 6;
			
			// Copy the adjacency matrix into the data buffer
			for(i = 0; i < MAX_NUM_NODES; i++)
				newPacket.dataBuffer[i] = *((Uint16*)(&globals.routing.graphInfo.adjacencyMatrix[i]));

			// Copy the partition tree into the data buffer
			j = MAX_NUM_NODES;
			for(i = 0; i < (MAX_NUM_NODES - 1) * 2 - 1; i++)
			{
				// Store the node information
				nodeInformation = *((Uint32*)(&globals.routing.partitionTree[i].nodeInformation));
				newPacket.dataBuffer[j++] = nodeInformation >> 16;
				newPacket.dataBuffer[j++] = nodeInformation & 0x0000FFFF;

				// Store the graph nodes in this tree node
				newPacket.dataBuffer[j] = ((globals.routing.partitionTree[i].graphNodesInTreeNode[0] & 0x000F) << 12);
				newPacket.dataBuffer[j] += ((globals.routing.partitionTree[i].graphNodesInTreeNode[1] & 0x000F) << 8);
				newPacket.dataBuffer[j] += ((globals.routing.partitionTree[i].graphNodesInTreeNode[2] & 0x000F) << 4);
				newPacket.dataBuffer[j++] += (globals.routing.partitionTree[i].graphNodesInTreeNode[3] & 0x000F);
				newPacket.dataBuffer[j] = ((globals.routing.partitionTree[i].graphNodesInTreeNode[4] & 0x000F) << 12);
				newPacket.dataBuffer[j] += ((globals.routing.partitionTree[i].graphNodesInTreeNode[5] & 0x000F) << 8);
				newPacket.dataBuffer[j] += ((globals.routing.partitionTree[i].graphNodesInTreeNode[6] & 0x000F) << 4);
				newPacket.dataBuffer[j++] += (globals.routing.partitionTree[i].graphNodesInTreeNode[7] & 0x000F);

				// Store the graph node probabilities
				newPacket.dataBuffer[j] = ((globals.routing.partitionTree[i].graphNodeProbabilities[0] & 0x000F) << 12);
				newPacket.dataBuffer[j] += ((globals.routing.partitionTree[i].graphNodeProbabilities[1] & 0x000F) << 8);
				newPacket.dataBuffer[j] += ((globals.routing.partitionTree[i].graphNodeProbabilities[2] & 0x000F) << 4);
				newPacket.dataBuffer[j++] += (globals.routing.partitionTree[i].graphNodeProbabilities[3] & 0x000F);
				newPacket.dataBuffer[j] = ((globals.routing.partitionTree[i].graphNodeProbabilities[4] & 0x000F) << 12);
				newPacket.dataBuffer[j] += ((globals.routing.partitionTree[i].graphNodeProbabilities[5] & 0x000F) << 8);
				newPacket.dataBuffer[j] += ((globals.routing.partitionTree[i].graphNodeProbabilities[6] & 0x000F) << 4);
				newPacket.dataBuffer[j++] += (globals.routing.partitionTree[i].graphNodeProbabilities[7] & 0x000F);
			}

			// Send the data packet
			return SendDataPacket(&newPacket);

#elif defined(IS_ROUTER)

			LogError(__FILE__,__LINE__);
			return ERROR;

#endif

		case SEQUENCE_REQUEST_ROUTING_TABLE_REQUEST_TRANSFER:

#if defined(IS_ROOT)

			LogError(__FILE__,__LINE__);
			return ERROR;

#elif defined(IS_ROUTER)

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
				if(SetupDirectBuffer(packet,SequenceRequestRoutingTableTransferCompleted) == ERROR)
				{
					SendErrorPacket(packet,PROTOCOL_ERROR_NO_BUFFERS_AVAILABLE);
					return ERROR;
				}
				else
				{
					// Grant permission for the transfer
					InitializePacketFromPreviousSequence(&newPacket,packet);
					newPacket.transmissionInfo.destination = packet->transmissionInfo.source;
					newPacket.b.packetSequenceStep = SEQUENCE_REQUEST_ROUTING_TABLE_REQUEST_ACCEPTED;
					return SendControlPacket(&newPacket);
				}
			}

#endif

		case SEQUENCE_REQUEST_ROUTING_TABLE_REQUEST_ACCEPTED:

#if defined(IS_ROOT)

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

#elif defined(IS_ROUTER)

			LogError(__FILE__,__LINE__);
			return ERROR;

#endif

		case SEQUENCE_REQUEST_ROUTING_TABLE_TRANSFER_RECEIVED:

#if defined(IS_ROOT)

			// Clean up the data transfer
			ClearDataTransferLookup(packet->a.packetID);
			globals.statistics.packet.numDataTransfersSucceeded++;

			// Set the partition tree received flag
			switch(packet->transmissionInfo.source)
			{
				case 1: globals.routing.nodeReceivedRoutingTable.node1 = true; break;
				case 2: globals.routing.nodeReceivedRoutingTable.node2 = true; break;
				case 3: globals.routing.nodeReceivedRoutingTable.node3 = true; break;
				case 4: globals.routing.nodeReceivedRoutingTable.node4 = true; break;
				case 5: globals.routing.nodeReceivedRoutingTable.node5 = true; break;
				case 6: globals.routing.nodeReceivedRoutingTable.node6 = true; break;
				case 7: globals.routing.nodeReceivedRoutingTable.node7 = true; break;
				case 8: globals.routing.nodeReceivedRoutingTable.node8 = true; break;
				case 9: globals.routing.nodeReceivedRoutingTable.node9 = true; break;
				case 10: globals.routing.nodeReceivedRoutingTable.node10 = true; break;
				case 11: globals.routing.nodeReceivedRoutingTable.node11 = true; break;
				case 12: globals.routing.nodeReceivedRoutingTable.node12 = true; break;
				case 13: globals.routing.nodeReceivedRoutingTable.node13 = true; break;
				case 14: globals.routing.nodeReceivedRoutingTable.node14 = true; break;
				case 15: globals.routing.nodeReceivedRoutingTable.node15 = true; break;
				case 16: globals.routing.nodeReceivedRoutingTable.node16 = true; break;
			}
			return SUCCESS;

#elif defined(IS_ROUTER)

			LogError(__FILE__,__LINE__);
			return ERROR;

#endif

		case SEQUENCE_REQUEST_ROUTING_TABLE_ERROR:

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

#pragma CODE_SECTION(SequenceRequestRoutingTableTransferCompleted,"FlashCode");
Uint16 SequenceRequestRoutingTableTransferCompleted(struct Packet* packet)
{
#if defined(IS_ROOT)

	LogError(__FILE__,__LINE__);
	return ERROR;

#elif defined(IS_ROUTER)

	struct Packet newPacket;
	Uint32 nodeInformation;
	Uint16 i, j;

	// Update the statistics
	globals.statistics.packet.numDirectBuffersSucceeded++;

	// Store the adjacency matrix
	for(i = 0; i < MAX_NUM_NODES; i++)
		*((Uint16*)(&globals.routing.graphInfo.adjacencyMatrix[i])) = packet->dataBuffer[i];

	// Copy the partition tree into the data buffer
	j = MAX_NUM_NODES;
	for(i = 0; i < (MAX_NUM_NODES - 1) * 2 - 1; i++)
	{
		// Retrieve the node information
		nodeInformation = packet->dataBuffer[j++];
		nodeInformation = nodeInformation << 16;
		nodeInformation += packet->dataBuffer[j++];
		*((Uint32*)(&globals.routing.partitionTree[i].nodeInformation)) = nodeInformation;

		// Retrieve the graph nodes in this tree node
		globals.routing.partitionTree[i].graphNodesInTreeNode[0] = (packet->dataBuffer[j] >> 12);
		globals.routing.partitionTree[i].graphNodesInTreeNode[1] = ((packet->dataBuffer[j] & 0x0F00) >> 8);
		globals.routing.partitionTree[i].graphNodesInTreeNode[2] = ((packet->dataBuffer[j] & 0x00F0) >> 4);
		globals.routing.partitionTree[i].graphNodesInTreeNode[3] = (packet->dataBuffer[j++] & 0x000F);
		globals.routing.partitionTree[i].graphNodesInTreeNode[4] = (packet->dataBuffer[j] >> 12);
		globals.routing.partitionTree[i].graphNodesInTreeNode[5] = ((packet->dataBuffer[j] & 0x0F00) >> 8);
		globals.routing.partitionTree[i].graphNodesInTreeNode[6] = ((packet->dataBuffer[j] & 0x00F0) >> 4);
		globals.routing.partitionTree[i].graphNodesInTreeNode[7] = (packet->dataBuffer[j++] & 0x000F);

		// Store the graph node probabilities
		globals.routing.partitionTree[i].graphNodeProbabilities[0] = (packet->dataBuffer[j] >> 12);
		globals.routing.partitionTree[i].graphNodeProbabilities[1] = ((packet->dataBuffer[j] & 0x0F00) >> 8);
		globals.routing.partitionTree[i].graphNodeProbabilities[2] = ((packet->dataBuffer[j] & 0x00F0) >> 4);
		globals.routing.partitionTree[i].graphNodeProbabilities[3] = (packet->dataBuffer[j++] & 0x000F);
		globals.routing.partitionTree[i].graphNodeProbabilities[4] = (packet->dataBuffer[j] >> 12);
		globals.routing.partitionTree[i].graphNodeProbabilities[5] = ((packet->dataBuffer[j] & 0x0F00) >> 8);
		globals.routing.partitionTree[i].graphNodeProbabilities[6] = ((packet->dataBuffer[j] & 0x00F0) >> 4);
		globals.routing.partitionTree[i].graphNodeProbabilities[7] = (packet->dataBuffer[j++] & 0x000F);
	}

	// Exit out of limp mode
	globals.routing.operatingInLimpMode = false;

	// Free the data buffer
	MemFree(packet->dataBuffer);
	packet->dataBuffer = NULL;
	
	// Send the acknowledgement to the sender
	InitializePacketFromPreviousSequence(&newPacket,packet);
	newPacket.transmissionInfo.destination = packet->transmissionInfo.source;
	newPacket.b.packetSequenceStep = SEQUENCE_REQUEST_ROUTING_TABLE_TRANSFER_RECEIVED;
	if(SendControlPacket(&newPacket) == ERROR)
		return ERROR;

	// Start the test service provider
	return StartServiceProvider(GetServiceIDFromServiceTag(TEST_SERVICE_TAG));

#endif
}
