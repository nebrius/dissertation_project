#include"System.h"

#pragma CODE_SECTION(NeighborFollowUp,"FlashCode");
void NeighborFollowUp(void* parameter, Uint16 followUpItemIndex)
{
	struct Packet newPacket;
	//Uint16 i;

#if defined(IS_ROUTER) && TEST != TEST_SPI
	Uint16 portToTransmitOn = PORT_UNDEFINED;
	Uint16 i;
#endif

	// Initialize the packet
/*	InitializePacket(&newPacket, PACKET_ID_UNDEFINED);

	// Check if any neighbors have dissapeared
	for(i = 0; i < 4; i++)
	{
#if defined(IS_ROOT)

		if(globals.protocol.globalNeighborInfo[ROOT_ADDRESS][i].nodeType != NODE_TYPE_UNDEFINED &&
			globals.protocol.globalNeighborInfo[ROOT_ADDRESS][i].commFailLevel == 0 && 
			globals.protocol.address != ADDRESS_UNDEFINED)
		{
			// Remove the neigbor information
			globals.statistics.numNeighborsMissing++;
			globals.protocol.globalNeighborInfo[ROOT_ADDRESS][i].nodeType = NODE_TYPE_UNDEFINED;
		}

		// Decrement the communication failure level
		globals.protocol.globalNeighborInfo[ROOT_ADDRESS][i].commFailLevel--;

#elif defined(IS_ROUTER)

		if(globals.protocol.neighborInfo[i].nodeType != NODE_TYPE_UNDEFINED &&
			globals.protocol.neighborInfo[i].commFailLevel == 0 && 
			globals.protocol.address != ADDRESS_UNDEFINED)
		{
			// Send the root router a message saying this neighbor is missing
			if(globals.protocol.neighborInfo[i].nodeAddress != ADDRESS_UNDEFINED)
				HandleCommFailure(globals.protocol.neighborInfo[i].nodeAddress);

			// Remove the neigbor information
			globals.statistics.numNeighborsMissing++;
			globals.protocol.neighborInfo[i].nodeType = NODE_TYPE_UNDEFINED;
		}

		// Decrement the communication failure level
		globals.protocol.neighborInfo[i].commFailLevel--;

#endif
	}*/

	// Find out who the neighbor on Port A is 
	InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
	newPacket.a.communicationType = COMM_TYPE_ADDRESSLESS;
	newPacket.b.command = COMMAND_DISCOVER_NEIGHBORS;
	newPacket.b.packetSequenceStep = SEQUENCE_DISCOVER_NEIGHBORS_REQUEST_NEIGHBOR;
	newPacket.transmissionInfo.portTransmittedOn = PORTA;
	SendControlPacket(&newPacket);

	// Find out who the neighbor on Port B is 
	InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
	newPacket.a.communicationType = COMM_TYPE_ADDRESSLESS;
	newPacket.b.command = COMMAND_DISCOVER_NEIGHBORS;
	newPacket.b.packetSequenceStep = SEQUENCE_DISCOVER_NEIGHBORS_REQUEST_NEIGHBOR;
	newPacket.transmissionInfo.portTransmittedOn = PORTB;
	SendControlPacket(&newPacket);

	// Find out who the neighbor on Port C is 
	InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
	newPacket.a.communicationType = COMM_TYPE_ADDRESSLESS;
	newPacket.b.command = COMMAND_DISCOVER_NEIGHBORS;
	newPacket.b.packetSequenceStep = SEQUENCE_DISCOVER_NEIGHBORS_REQUEST_NEIGHBOR;
	newPacket.transmissionInfo.portTransmittedOn = PORTC;
	SendControlPacket(&newPacket);

	// Find out who the neighbor on Port D is 
	InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
	newPacket.a.communicationType = COMM_TYPE_ADDRESSLESS;
	newPacket.b.command = COMMAND_DISCOVER_NEIGHBORS;
	newPacket.b.packetSequenceStep = SEQUENCE_DISCOVER_NEIGHBORS_REQUEST_NEIGHBOR;
	newPacket.transmissionInfo.portTransmittedOn = PORTD;
	SendControlPacket(&newPacket);

	// Request an address if this isn't the root
#if defined(IS_ROUTER) && TEST != TEST_SPI

	if(globals.protocol.address == ADDRESS_UNDEFINED)
	{
		// Initialize the packet
		InitializePacket(&newPacket,PACKET_ID_UNDEFINED);

		// Find the port to send the packet on
		for(i = 0; i < 4; i++)
		{
			if((globals.protocol.neighborInfo[i].nodeType ==  NODE_TYPE_ROOT_ROUTER) ||
			   (globals.protocol.neighborInfo[i].nodeType == NODE_TYPE_NON_ROOT_ROUTER &&
				globals.protocol.neighborInfo[i].nodeAddress != ADDRESS_UNDEFINED))
			{
				portToTransmitOn = i;
				break;
			}
		}

		// Ask for an address
		if(portToTransmitOn != PORT_UNDEFINED)
		{
			// Send the address request
			newPacket.a.communicationType = COMM_TYPE_ADDRESSLESS;
			newPacket.transmissionInfo.portTransmittedOn = portToTransmitOn;
			newPacket.b.command = COMMAND_REQUEST_ADDRESS;
			newPacket.b.packetSequenceStep = SEQUENCE_REQUEST_ADDRESS_REQUEST;
			SendControlPacket(&newPacket);
		}
	}

#endif
}

#if defined(IS_ROUTER)
#pragma CODE_SECTION(ReportNeighbors,"FlashCode");
void ReportNeighbors(void* parameter, Uint16 followUpItemIndex)
{
	struct Packet newPacket;

	// Make sure all neighbors have an adress before reporting
	if((globals.protocol.neighborInfo[PORTA].nodeAddress != ADDRESS_UNDEFINED || globals.protocol.neighborInfo[PORTA].nodeType == NODE_TYPE_UNDEFINED) &&
		(globals.protocol.neighborInfo[PORTB].nodeAddress != ADDRESS_UNDEFINED || globals.protocol.neighborInfo[PORTB].nodeType == NODE_TYPE_UNDEFINED) &&
		(globals.protocol.neighborInfo[PORTC].nodeAddress != ADDRESS_UNDEFINED || globals.protocol.neighborInfo[PORTC].nodeType == NODE_TYPE_UNDEFINED) &&
		(globals.protocol.neighborInfo[PORTD].nodeAddress != ADDRESS_UNDEFINED || globals.protocol.neighborInfo[PORTD].nodeType == NODE_TYPE_UNDEFINED))
	{
		// Initialize the packet
		InitializePacket(&newPacket,PACKET_ID_UNDEFINED);

		// Create the neighbor report packet
		newPacket.a.communicationType = COMM_TYPE_UNICAST;
		newPacket.transmissionInfo.destination = ROOT_ADDRESS;
		newPacket.b.command = COMMAND_REPORT_NEIGHBORS;
		newPacket.b.packetSequenceStep = SEQUENCE_REPORT_NEIGHBORS;
		newPacket.f.commandSpecific0 = globals.protocol.neighborInfo[PORTA].nodeAddress;
		newPacket.f.commandSpecific1 = globals.protocol.neighborInfo[PORTB].nodeAddress;
		newPacket.g.commandSpecific2 = globals.protocol.neighborInfo[PORTC].nodeAddress;
		newPacket.g.commandSpecific3 = globals.protocol.neighborInfo[PORTD].nodeAddress;

		SendControlPacket(&newPacket);
	}
}

#pragma CODE_SECTION(HandleCommFailure,"FlashCode");
Uint16 HandleCommFailure(Uint16 nodeAddress)
{
	struct Packet newPacket;

	// Initialize the packet
	InitializePacket(&newPacket,PACKET_ID_UNDEFINED);
	
	// Create the comm failure packet
	newPacket.a.communicationType = COMM_TYPE_UNICAST;
	newPacket.b.command = COMMAND_COMM_FAILURE;
	newPacket.b.packetSequenceStep = SEQUENCE_COMM_FAILURE;
	newPacket.transmissionInfo.destination = ROOT_ADDRESS;
	newPacket.f.commandSpecific0 = nodeAddress;

	// Send the packet
	return SendControlPacket(&newPacket);
}

#pragma CODE_SECTION(RequestRoutingTable,"FlashCode");
void RequestRoutingTable()
{
	struct Packet newPacket;

	// Initialize the packet
	InitializePacket(&newPacket,PACKET_ID_UNDEFINED);

	// Create the neighbor report packet
	newPacket.a.communicationType = COMM_TYPE_UNICAST;
	newPacket.transmissionInfo.destination = ROOT_ADDRESS;
	newPacket.b.command = COMMAND_REQUEST_ROUTING_TABLE;
	newPacket.b.packetSequenceStep = SEQUENCE_REQUEST_ROUTING_TABLE_REQUEST;

	SendControlPacket(&newPacket);
}
#endif

#if defined(IS_ROOT)
#pragma CODE_SECTION(EnsureRoutingTableDistribution,"FlashCode");
void EnsureRoutingTableDistribution(void* parameter, Uint16 followUpItemIndex)
{
	Uint16 i, allNodesReceivedRoutingTree = true;
	struct Packet newPacket;

	for(i = 0; i < MAX_NUM_NODES; i++)
	{
		switch(i)
		{
			case 1: 
				if(!globals.routing.nodeReceivedRoutingTable.node1)
				{
					// Send the new routing table available message to this node
					allNodesReceivedRoutingTree = false;
					InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.b.command = COMMAND_NEW_ROUTING_TABLE_AVAILABLE;
					newPacket.b.packetSequenceStep = SEQUENCE_NEW_ROUTING_TABLE_AVAILABLE;
					newPacket.transmissionInfo.destination = i;
					SendControlPacket(&newPacket);
				}
				break;
			case 2: 
				if(!globals.routing.nodeReceivedRoutingTable.node2)
				{
					// Send the new routing table available message to this node
					allNodesReceivedRoutingTree = false;
					InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.b.command = COMMAND_NEW_ROUTING_TABLE_AVAILABLE;
					newPacket.b.packetSequenceStep = SEQUENCE_NEW_ROUTING_TABLE_AVAILABLE;
					newPacket.transmissionInfo.destination = i;
					SendControlPacket(&newPacket);
				}
				break;
			case 3: 
				if(!globals.routing.nodeReceivedRoutingTable.node3)
				{
					// Send the new routing table available message to this node
					allNodesReceivedRoutingTree = false;
					InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.b.command = COMMAND_NEW_ROUTING_TABLE_AVAILABLE;
					newPacket.b.packetSequenceStep = SEQUENCE_NEW_ROUTING_TABLE_AVAILABLE;
					newPacket.transmissionInfo.destination = i;
					SendControlPacket(&newPacket);
				}
				break;
			case 4: 
				if(!globals.routing.nodeReceivedRoutingTable.node4)
				{
					// Send the new routing table available message to this node
					allNodesReceivedRoutingTree = false;
					InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.b.command = COMMAND_NEW_ROUTING_TABLE_AVAILABLE;
					newPacket.b.packetSequenceStep = SEQUENCE_NEW_ROUTING_TABLE_AVAILABLE;
					newPacket.transmissionInfo.destination = i;
					SendControlPacket(&newPacket);
				}
				break;
			case 5: 
				if(!globals.routing.nodeReceivedRoutingTable.node5)
				{
					// Send the new routing table available message to this node
					allNodesReceivedRoutingTree = false;
					InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.b.command = COMMAND_NEW_ROUTING_TABLE_AVAILABLE;
					newPacket.b.packetSequenceStep = SEQUENCE_NEW_ROUTING_TABLE_AVAILABLE;
					newPacket.transmissionInfo.destination = i;
					SendControlPacket(&newPacket);
				}
				break;
			case 6: 
				if(!globals.routing.nodeReceivedRoutingTable.node6)
				{
					// Send the new routing table available message to this node
					allNodesReceivedRoutingTree = false;
					InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.b.command = COMMAND_NEW_ROUTING_TABLE_AVAILABLE;
					newPacket.b.packetSequenceStep = SEQUENCE_NEW_ROUTING_TABLE_AVAILABLE;
					newPacket.transmissionInfo.destination = i;
					SendControlPacket(&newPacket);
				}
				break;
			case 7: 
				if(!globals.routing.nodeReceivedRoutingTable.node7)
				{
					// Send the new routing table available message to this node
					allNodesReceivedRoutingTree = false;
					InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.b.command = COMMAND_NEW_ROUTING_TABLE_AVAILABLE;
					newPacket.b.packetSequenceStep = SEQUENCE_NEW_ROUTING_TABLE_AVAILABLE;
					newPacket.transmissionInfo.destination = i;
					SendControlPacket(&newPacket);
				}
				break;
			case 8: 
				if(!globals.routing.nodeReceivedRoutingTable.node8)
				{
					// Send the new routing table available message to this node
					allNodesReceivedRoutingTree = false;
					InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.b.command = COMMAND_NEW_ROUTING_TABLE_AVAILABLE;
					newPacket.b.packetSequenceStep = SEQUENCE_NEW_ROUTING_TABLE_AVAILABLE;
					newPacket.transmissionInfo.destination = i;
					SendControlPacket(&newPacket);
				}
				break;
			case 9: 
				if(!globals.routing.nodeReceivedRoutingTable.node9)
				{
					// Send the new routing table available message to this node
					allNodesReceivedRoutingTree = false;
					InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.b.command = COMMAND_NEW_ROUTING_TABLE_AVAILABLE;
					newPacket.b.packetSequenceStep = SEQUENCE_NEW_ROUTING_TABLE_AVAILABLE;
					newPacket.transmissionInfo.destination = i;
					SendControlPacket(&newPacket);
				}
				break;
			case 10: 
				if(!globals.routing.nodeReceivedRoutingTable.node10)
				{
					// Send the new routing table available message to this node
					allNodesReceivedRoutingTree = false;
					InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.b.command = COMMAND_NEW_ROUTING_TABLE_AVAILABLE;
					newPacket.b.packetSequenceStep = SEQUENCE_NEW_ROUTING_TABLE_AVAILABLE;
					newPacket.transmissionInfo.destination = i;
					SendControlPacket(&newPacket);
				}
				break;
			case 11: 
				if(!globals.routing.nodeReceivedRoutingTable.node11)
				{
					// Send the new routing table available message to this node
					allNodesReceivedRoutingTree = false;
					InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.b.command = COMMAND_NEW_ROUTING_TABLE_AVAILABLE;
					newPacket.b.packetSequenceStep = SEQUENCE_NEW_ROUTING_TABLE_AVAILABLE;
					newPacket.transmissionInfo.destination = i;
					SendControlPacket(&newPacket);
				}
				break;
			case 12: 
				if(!globals.routing.nodeReceivedRoutingTable.node12)
				{
					// Send the new routing table available message to this node
					allNodesReceivedRoutingTree = false;
					InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.b.command = COMMAND_NEW_ROUTING_TABLE_AVAILABLE;
					newPacket.b.packetSequenceStep = SEQUENCE_NEW_ROUTING_TABLE_AVAILABLE;
					newPacket.transmissionInfo.destination = i;
					SendControlPacket(&newPacket);
				}
				break;
			case 13: 
				if(!globals.routing.nodeReceivedRoutingTable.node13)
				{
					// Send the new routing table available message to this node
					allNodesReceivedRoutingTree = false;
					InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.b.command = COMMAND_NEW_ROUTING_TABLE_AVAILABLE;
					newPacket.b.packetSequenceStep = SEQUENCE_NEW_ROUTING_TABLE_AVAILABLE;
					newPacket.transmissionInfo.destination = i;
					SendControlPacket(&newPacket);
				}
				break;
			case 14: 
				if(!globals.routing.nodeReceivedRoutingTable.node14)
				{
					// Send the new routing table available message to this node
					allNodesReceivedRoutingTree = false;
					InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.b.command = COMMAND_NEW_ROUTING_TABLE_AVAILABLE;
					newPacket.b.packetSequenceStep = SEQUENCE_NEW_ROUTING_TABLE_AVAILABLE;
					newPacket.transmissionInfo.destination = i;
					SendControlPacket(&newPacket);
				}
				break;
			case 15: 
				if(!globals.routing.nodeReceivedRoutingTable.node15)
				{
					// Send the new routing table available message to this node
					allNodesReceivedRoutingTree = false;
					InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.b.command = COMMAND_NEW_ROUTING_TABLE_AVAILABLE;
					newPacket.b.packetSequenceStep = SEQUENCE_NEW_ROUTING_TABLE_AVAILABLE;
					newPacket.transmissionInfo.destination = i;
					SendControlPacket(&newPacket);
				}
				break;
			case 16: 
				if(!globals.routing.nodeReceivedRoutingTable.node16)
				{
					// Send the new routing table available message to this node
					allNodesReceivedRoutingTree = false;
					InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.b.command = COMMAND_NEW_ROUTING_TABLE_AVAILABLE;
					newPacket.b.packetSequenceStep = SEQUENCE_NEW_ROUTING_TABLE_AVAILABLE;
					newPacket.transmissionInfo.destination = i;
					SendControlPacket(&newPacket);
				}
				break;
		}
	}

	if(allNodesReceivedRoutingTree)
	{
		globals.processing.sevenSegmentUpperDigit = SEVENSEG_2DASH;
		RemoveFollowUpItem(EnsureRoutingTableDistribution,NULL);
	}
}

#pragma CODE_SECTION(EnsureBarrierReached,"FlashCode");
void EnsureBarrierReached(void* parameter, Uint16 followUpItemIndex)
{
	Uint16 i, allNodesAckedBarrier = true;
	struct Packet newPacket;

	for(i = 0; i < MAX_NUM_NODES; i++)
	{
		switch(i)
		{
			case 1: 
				if(!globals.routing.nodeReceivedRoutingTable.node1)
				{
					allNodesAckedBarrier = false;

					// Let the node know the barrier was reached
					InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.b.command = COMMAND_MPI_CONTROL;
					newPacket.b.packetSequenceStep = SEQUENCE_MPI_CONTROL;
					newPacket.c.destinationServiceTag = globals.mpiManager.barrierAckServiceTag;
					newPacket.transmissionInfo.destination = i;
					SendControlPacket(&newPacket);
				}
				break;
			case 2: 
				if(!globals.routing.nodeReceivedRoutingTable.node2)
				{
					allNodesAckedBarrier = false;

					// Let the node know the barrier was reached
					InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.b.command = COMMAND_MPI_CONTROL;
					newPacket.b.packetSequenceStep = SEQUENCE_MPI_CONTROL;
					newPacket.c.destinationServiceTag = globals.mpiManager.barrierAckServiceTag;
					newPacket.transmissionInfo.destination = i;
					SendControlPacket(&newPacket);
				}
				break;
			case 3: 
				if(!globals.routing.nodeReceivedRoutingTable.node3)
				{
					allNodesAckedBarrier = false;

					// Let the node know the barrier was reached
					InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.b.command = COMMAND_MPI_CONTROL;
					newPacket.b.packetSequenceStep = SEQUENCE_MPI_CONTROL;
					newPacket.c.destinationServiceTag = globals.mpiManager.barrierAckServiceTag;
					newPacket.transmissionInfo.destination = i;
					SendControlPacket(&newPacket);
				}
				break;
			case 4: 
				if(!globals.routing.nodeReceivedRoutingTable.node4)
				{
					allNodesAckedBarrier = false;

					// Let the node know the barrier was reached
					InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.b.command = COMMAND_MPI_CONTROL;
					newPacket.b.packetSequenceStep = SEQUENCE_MPI_CONTROL;
					newPacket.c.destinationServiceTag = globals.mpiManager.barrierAckServiceTag;
					newPacket.transmissionInfo.destination = i;
					SendControlPacket(&newPacket);
				}
				break;
			case 5: 
				if(!globals.routing.nodeReceivedRoutingTable.node5)
				{
					allNodesAckedBarrier = false;

					// Let the node know the barrier was reached
					InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.b.command = COMMAND_MPI_CONTROL;
					newPacket.b.packetSequenceStep = SEQUENCE_MPI_CONTROL;
					newPacket.c.destinationServiceTag = globals.mpiManager.barrierAckServiceTag;
					newPacket.transmissionInfo.destination = i;
					SendControlPacket(&newPacket);
				}
				break;
			case 6: 
				if(!globals.routing.nodeReceivedRoutingTable.node6)
				{
					allNodesAckedBarrier = false;

					// Let the node know the barrier was reached
					InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.b.command = COMMAND_MPI_CONTROL;
					newPacket.b.packetSequenceStep = SEQUENCE_MPI_CONTROL;
					newPacket.c.destinationServiceTag = globals.mpiManager.barrierAckServiceTag;
					newPacket.transmissionInfo.destination = i;
					SendControlPacket(&newPacket);
				}
				break;
			case 7: 
				if(!globals.routing.nodeReceivedRoutingTable.node7)
				{
					allNodesAckedBarrier = false;

					// Let the node know the barrier was reached
					InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.b.command = COMMAND_MPI_CONTROL;
					newPacket.b.packetSequenceStep = SEQUENCE_MPI_CONTROL;
					newPacket.c.destinationServiceTag = globals.mpiManager.barrierAckServiceTag;
					newPacket.transmissionInfo.destination = i;
					SendControlPacket(&newPacket);
				}
				break;
			case 8: 
				if(!globals.routing.nodeReceivedRoutingTable.node8)
				{
					allNodesAckedBarrier = false;

					// Let the node know the barrier was reached
					InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.b.command = COMMAND_MPI_CONTROL;
					newPacket.b.packetSequenceStep = SEQUENCE_MPI_CONTROL;
					newPacket.c.destinationServiceTag = globals.mpiManager.barrierAckServiceTag;
					newPacket.transmissionInfo.destination = i;
					SendControlPacket(&newPacket);
				}
				break;
			case 9: 
				if(!globals.routing.nodeReceivedRoutingTable.node9)
				{
					allNodesAckedBarrier = false;

					// Let the node know the barrier was reached
					InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.b.command = COMMAND_MPI_CONTROL;
					newPacket.b.packetSequenceStep = SEQUENCE_MPI_CONTROL;
					newPacket.c.destinationServiceTag = globals.mpiManager.barrierAckServiceTag;
					newPacket.transmissionInfo.destination = i;
					SendControlPacket(&newPacket);
				}
				break;
			case 10: 
				if(!globals.routing.nodeReceivedRoutingTable.node10)
				{
					allNodesAckedBarrier = false;

					// Let the node know the barrier was reached
					InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.b.command = COMMAND_MPI_CONTROL;
					newPacket.b.packetSequenceStep = SEQUENCE_MPI_CONTROL;
					newPacket.c.destinationServiceTag = globals.mpiManager.barrierAckServiceTag;
					newPacket.transmissionInfo.destination = i;
					SendControlPacket(&newPacket);
				}
				break;
			case 11: 
				if(!globals.routing.nodeReceivedRoutingTable.node11)
				{
					allNodesAckedBarrier = false;

					// Let the node know the barrier was reached
					InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.b.command = COMMAND_MPI_CONTROL;
					newPacket.b.packetSequenceStep = SEQUENCE_MPI_CONTROL;
					newPacket.c.destinationServiceTag = globals.mpiManager.barrierAckServiceTag;
					newPacket.transmissionInfo.destination = i;
					SendControlPacket(&newPacket);
				}
				break;
			case 12: 
				if(!globals.routing.nodeReceivedRoutingTable.node12)
				{
					allNodesAckedBarrier = false;

					// Let the node know the barrier was reached
					InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.b.command = COMMAND_MPI_CONTROL;
					newPacket.b.packetSequenceStep = SEQUENCE_MPI_CONTROL;
					newPacket.c.destinationServiceTag = globals.mpiManager.barrierAckServiceTag;
					newPacket.transmissionInfo.destination = i;
					SendControlPacket(&newPacket);
				}
				break;
			case 13: 
				if(!globals.routing.nodeReceivedRoutingTable.node13)
				{
					allNodesAckedBarrier = false;

					// Let the node know the barrier was reached
					InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.b.command = COMMAND_MPI_CONTROL;
					newPacket.b.packetSequenceStep = SEQUENCE_MPI_CONTROL;
					newPacket.c.destinationServiceTag = globals.mpiManager.barrierAckServiceTag;
					newPacket.transmissionInfo.destination = i;
					SendControlPacket(&newPacket);
				}
				break;
			case 14: 
				if(!globals.routing.nodeReceivedRoutingTable.node14)
				{
					allNodesAckedBarrier = false;

					// Let the node know the barrier was reached
					InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.b.command = COMMAND_MPI_CONTROL;
					newPacket.b.packetSequenceStep = SEQUENCE_MPI_CONTROL;
					newPacket.c.destinationServiceTag = globals.mpiManager.barrierAckServiceTag;
					newPacket.transmissionInfo.destination = i;
					SendControlPacket(&newPacket);
				}
				break;
			case 15: 
				if(!globals.routing.nodeReceivedRoutingTable.node15)
				{
					allNodesAckedBarrier = false;

					// Let the node know the barrier was reached
					InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.b.command = COMMAND_MPI_CONTROL;
					newPacket.b.packetSequenceStep = SEQUENCE_MPI_CONTROL;
					newPacket.c.destinationServiceTag = globals.mpiManager.barrierAckServiceTag;
					newPacket.transmissionInfo.destination = i;
					SendControlPacket(&newPacket);
				}
				break;
			case 16: 
				if(!globals.routing.nodeReceivedRoutingTable.node16)
				{
					allNodesAckedBarrier = false;

					// Let the node know the barrier was reached
					InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.b.command = COMMAND_MPI_CONTROL;
					newPacket.b.packetSequenceStep = SEQUENCE_MPI_CONTROL;
					newPacket.c.sourceServiceTag = globals.mpiManager.barrierAckServiceTag;
					newPacket.transmissionInfo.destination = i;
					SendControlPacket(&newPacket);
				}
				break;
		}
	}

	if(allNodesAckedBarrier)
		RemoveFollowUpItem(EnsureBarrierReached,NULL);
}

#endif
