#include"System.h"

#pragma CODE_SECTION(ProcessCommandCommFailure,"FlashCode");
Uint16 ProcessCommandCommFailure(struct Packet* packet)
{
#if defined(IS_ROOT)
	Uint16 i, j;
	struct Packet newPacket;
#endif

	// Process the packet according to which sequence step the packet is on
	switch(packet->b.packetSequenceStep)
	{
		case SEQUENCE_COMM_FAILURE:

#if defined(IS_ROOT)

			// Find any references to the missing address and remove it from the
			for(i = 0; i < MAX_NUM_NODES; i++)
			{
				for(j = 0; j < NUM_PORTS; j++)
				{
					if(globals.protocol.globalNeighborInfo[i][j].nodeAddress == packet->f.commandSpecific0)
						globals.protocol.globalNeighborInfo[i][j].nodeAddress = ADDRESS_UNDEFINED;
				}
			}

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

			LogWarning(__FILE__,__LINE__);
			return SUCCESS;

#elif defined(IS_ROUTER)

			LogError(__FILE__,__LINE__);
			return ERROR;

#endif

		case SEQUENCE_COMM_FAILURE_ERROR:

			LogPacketError(__FILE__,__LINE__,packet);
			return SUCCESS;

		default: 

			LogError(__FILE__,__LINE__);
			return ERROR;
	}
}
