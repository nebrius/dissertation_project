#include"System.h"

#pragma CODE_SECTION(ProcessCommandReportNeighbors,"FlashCode");
Uint16 ProcessCommandReportNeighbors(struct Packet* packet)
{
#if defined(IS_ROOT)
	struct Packet newPacket;
	Uint16 i;
#endif

	// Process the packet according to which sequence step the packet is on
	switch(packet->b.packetSequenceStep)
	{
		case SEQUENCE_REPORT_NEIGHBORS:

#if defined(IS_ROUTER)

			LogError(__FILE__,__LINE__);
			return ERROR;
	
#elif defined(IS_ROOT)

			// Store the neighbors
			globals.protocol.globalNeighborInfo[packet->transmissionInfo.source][PORTA].nodeAddress = packet->f.commandSpecific0;
			globals.protocol.globalNeighborInfo[packet->transmissionInfo.source][PORTB].nodeAddress = packet->f.commandSpecific1;
			globals.protocol.globalNeighborInfo[packet->transmissionInfo.source][PORTC].nodeAddress = packet->g.commandSpecific2;
			globals.protocol.globalNeighborInfo[packet->transmissionInfo.source][PORTD].nodeAddress = packet->g.commandSpecific3;

			// Acknowledge the receipt of the neighbors
			InitializePacketFromPreviousSequence(&newPacket,packet);
			newPacket.transmissionInfo.destination = packet->transmissionInfo.source;
			newPacket.b.packetSequenceStep = SEQUENCE_REPORT_NEIGHBORS_RECEIVED;
			if(SendControlPacket(&newPacket) == ERROR)
				return ERROR;

			// Check if it is time to generate the routing table
			if(!globals.routing.routingTableGenerated)
			{
				globals.routing.numRouterNeighborsCollected++;
				if(globals.routing.numRouterNeighborsCollected == globals.routing.expectedNumNodes)
				{					
					globals.processing.sevenSegmentUpperDigit = SEVENSEG_1DASH;

					// Exit limp mode
					globals.routing.operatingInLimpMode = false;

					// Generate the routing tree
					GenerateRoutingTree();

					// Setup the follow up provider
					*((Uint16*)(&globals.routing.nodeReceivedRoutingTable)) = 0xFFFF;
					AddFollowUpItem(&EnsureRoutingTableDistribution,NULL,ENSURE_ROUTING_TABLE_DISTRIBUTION_FOLLOW_UP_RATE,true);

					// Tell the other nodes that the routing tree is available
					for(i = 0; i < MAX_NUM_NODES; i++)
					{
						if(globals.root.availableAddresses[i].addressTaken && globals.root.availableAddresses[i].address != ROOT_ADDRESS)
						{
							// Set this node's flag to not received the routing table
							switch(globals.root.availableAddresses[i].address)
							{
								case 1: globals.routing.nodeReceivedRoutingTable.node1 = false; break;
								case 2: globals.routing.nodeReceivedRoutingTable.node2 = false; break;
								case 3: globals.routing.nodeReceivedRoutingTable.node3 = false; break;
								case 4: globals.routing.nodeReceivedRoutingTable.node4 = false; break;
								case 5: globals.routing.nodeReceivedRoutingTable.node5 = false; break;
								case 6: globals.routing.nodeReceivedRoutingTable.node6 = false; break;
								case 7: globals.routing.nodeReceivedRoutingTable.node7 = false; break;
								case 8: globals.routing.nodeReceivedRoutingTable.node8 = false; break;
								case 9: globals.routing.nodeReceivedRoutingTable.node9 = false; break;
								case 10: globals.routing.nodeReceivedRoutingTable.node10 = false; break;
								case 11: globals.routing.nodeReceivedRoutingTable.node11 = false; break;
								case 12: globals.routing.nodeReceivedRoutingTable.node12 = false; break;
								case 13: globals.routing.nodeReceivedRoutingTable.node13 = false; break;
								case 14: globals.routing.nodeReceivedRoutingTable.node14 = false; break;
								case 15: globals.routing.nodeReceivedRoutingTable.node15 = false; break;
								case 16: globals.routing.nodeReceivedRoutingTable.node16 = false; break;
							}

							// Send the new routing table available message to this node
							InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
							newPacket.a.communicationType = COMM_TYPE_UNICAST;
							newPacket.b.command = COMMAND_NEW_ROUTING_TABLE_AVAILABLE;
							newPacket.b.packetSequenceStep = SEQUENCE_NEW_ROUTING_TABLE_AVAILABLE;
							newPacket.transmissionInfo.destination = globals.root.availableAddresses[i].address;
							SendControlPacket(&newPacket);
						}
					}

					// Mark the routing table generation process as 
					globals.routing.routingTableGenerated = true;
				}
			}

			return SUCCESS;

#endif

		case SEQUENCE_REPORT_NEIGHBORS_RECEIVED:

#if defined(IS_ROOT)

			LogError(__FILE__,__LINE__);
			return ERROR;	

#elif defined(IS_ROUTER)

			// Remove the follow up item that reports neighbors
			if(RemoveFollowUpItem(ReportNeighbors,NULL) == ERROR)
				return ERROR;
			else
				return SUCCESS;

#endif

		case SEQUENCE_REPORT_NEIGHBORS_ERROR:

			LogPacketError(__FILE__,__LINE__,packet);
			return SUCCESS;

		default: 

			LogError(__FILE__,__LINE__);
			return ERROR;
	}
}

