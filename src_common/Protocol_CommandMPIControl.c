#include"System.h"

Uint16 nodesReachedBarrier[MAX_NUM_NODES] = {0,0,0,0,0,0,0,0,0};

#pragma CODE_SECTION(ProcessCommandMPIControl,"FlashCode");
Uint16 ProcessCommandMPIControl(struct Packet* packet)
{
	struct Packet newPacket;
#if defined(IS_ROOT)
	Uint16 i, numNodesReachedBarrier = 0;
#elif defined(IS_ROUTER)
	struct Mail message;
#endif

	// Process the packet according to which sequence step the packet is on
	switch(packet->b.packetSequenceStep)
	{
		case SEQUENCE_MPI_CONTROL:

#if defined(IS_ROOT)

			// Set this node to having reached the barrier
			nodesReachedBarrier[packet->transmissionInfo.source] = true;

			// Ack the sender
			InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
			newPacket.a.communicationType = COMM_TYPE_UNICAST;
			newPacket.b.command = COMMAND_MPI_CONTROL;
			newPacket.b.packetSequenceStep = SEQUENCE_MPI_CONTROL_ACK;
			newPacket.c.sourceServiceTag = packet->c.destinationServiceTag;
			newPacket.c.destinationServiceTag = packet->c.sourceServiceTag;
			newPacket.transmissionInfo.destination = packet->transmissionInfo.source;
			SendControlPacket(&newPacket);

			// Check if all nodes have reached the barrier
			for(i = 2; i < MAX_NUM_NODES; i++)
				numNodesReachedBarrier += nodesReachedBarrier[i];
			if(numNodesReachedBarrier >= globals.routing.expectedNumNodes - 1)
			{
				// Let each node know that the barrier has been reached and reset the nodes reached barrier flags
				*((Uint16*)(&globals.mpiManager.nodeAckedBarrier)) = 0xFFFF;
				for(i = 2; i < MAX_NUM_NODES; i++)
				{
					if(nodesReachedBarrier[i])
					{
						// Let the node know the barrier was reached
						InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
						newPacket.a.communicationType = COMM_TYPE_UNICAST;
						newPacket.b.command = COMMAND_MPI_CONTROL;
						newPacket.b.packetSequenceStep = SEQUENCE_MPI_CONTROL;
						newPacket.c.sourceServiceTag = packet->c.destinationServiceTag;
						newPacket.c.destinationServiceTag = packet->c.sourceServiceTag;
						newPacket.transmissionInfo.destination = i;
						SendControlPacket(&newPacket);

						// Set the follow-up flag
						switch(i)
						{
							case 1: globals.mpiManager.nodeAckedBarrier.node1 = false;
							case 2: globals.mpiManager.nodeAckedBarrier.node2 = false;
							case 3: globals.mpiManager.nodeAckedBarrier.node3 = false;
							case 4: globals.mpiManager.nodeAckedBarrier.node4 = false;
							case 5: globals.mpiManager.nodeAckedBarrier.node5 = false;
							case 6: globals.mpiManager.nodeAckedBarrier.node6 = false;
							case 7: globals.mpiManager.nodeAckedBarrier.node7 = false;
							case 8: globals.mpiManager.nodeAckedBarrier.node8 = false;
							case 9: globals.mpiManager.nodeAckedBarrier.node9 = false;
							case 10: globals.mpiManager.nodeAckedBarrier.node10 = false;
							case 11: globals.mpiManager.nodeAckedBarrier.node11 = false;
							case 12: globals.mpiManager.nodeAckedBarrier.node12 = false;
							case 13: globals.mpiManager.nodeAckedBarrier.node13 = false;
							case 14: globals.mpiManager.nodeAckedBarrier.node14 = false;
							case 15: globals.mpiManager.nodeAckedBarrier.node15 = false;
							case 16: globals.mpiManager.nodeAckedBarrier.node16 = false;
						}

						// Reset the flag
						nodesReachedBarrier[i] = false;
					}
				}

				// Set the barrier follow up
				AddFollowUpItem(&EnsureBarrierReached,NULL,ENSURE_BARRIER_REACHED_FOLLOW_UP_RATE,true);
				globals.mpiManager.barrierAckServiceTag = packet->c.sourceServiceTag;
			}

			return SUCCESS;

#elif defined(IS_ROUTER)

			// Send the message to the service
			CreateMessage(&message,
				NULL,
				0,
				MAIL_BARRIER_COMPLETE,
				packet->a.packetID,
				MPI_REQUEST_BARRIER,
				0,
				packet->transmissionInfo.source,
				packet->transmissionInfo.destination,
				packet->c.sourceServiceTag,
				packet->c.destinationServiceTag);
			SendMail(GetServiceIDFromServiceTag(packet->c.destinationServiceTag),&message);

			// Ack back to the root
			InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
			newPacket.a.communicationType = COMM_TYPE_UNICAST;
			newPacket.b.command = COMMAND_MPI_CONTROL;
			newPacket.b.packetSequenceStep = SEQUENCE_MPI_CONTROL_ACK;
			newPacket.c.sourceServiceTag = packet->c.destinationServiceTag;
			newPacket.c.destinationServiceTag = packet->c.sourceServiceTag;
			newPacket.transmissionInfo.destination = ROOT_ADDRESS;
			SendControlPacket(&newPacket);

			return SUCCESS;

#endif

		case SEQUENCE_MPI_CONTROL_ACK:

#if defined(IS_ROOT)

			// Let each node know that the barrier has been reached and reset the nodes reached barrier flags
			for(i = 0; i < MAX_NUM_NODES; i++)
			{
				switch(i)
				{
					case 1: globals.mpiManager.nodeAckedBarrier.node1 = true; break;
					case 2: globals.mpiManager.nodeAckedBarrier.node2 = true; break;
					case 3: globals.mpiManager.nodeAckedBarrier.node3 = true; break;
					case 4: globals.mpiManager.nodeAckedBarrier.node4 = true; break;
					case 5: globals.mpiManager.nodeAckedBarrier.node5 = true; break;
					case 6: globals.mpiManager.nodeAckedBarrier.node6 = true; break;
					case 7: globals.mpiManager.nodeAckedBarrier.node7 = true; break;
					case 8: globals.mpiManager.nodeAckedBarrier.node8 = true; break;
					case 9: globals.mpiManager.nodeAckedBarrier.node9 = true; break;
					case 10: globals.mpiManager.nodeAckedBarrier.node10 = true; break;
					case 11: globals.mpiManager.nodeAckedBarrier.node11 = true; break;
					case 12: globals.mpiManager.nodeAckedBarrier.node12 = true; break;
					case 13: globals.mpiManager.nodeAckedBarrier.node13 = true; break;
					case 14: globals.mpiManager.nodeAckedBarrier.node14 = true; break;
					case 15: globals.mpiManager.nodeAckedBarrier.node15 = true; break;
					case 16: globals.mpiManager.nodeAckedBarrier.node16 = true; break;
				}
			}

			return SUCCESS;

#elif defined(IS_ROUTER)

			CreateMessage(&message,
				NULL,
				0,
				MAIL_BARRIER_ACKED,
				packet->a.packetID,
				MPI_REQUEST_BARRIER,
				0,
				packet->transmissionInfo.source,
				packet->transmissionInfo.destination,
				packet->c.sourceServiceTag,
				packet->c.destinationServiceTag);
			SendMail(GetServiceIDFromServiceTag(packet->c.destinationServiceTag),&message);

			return SUCCESS;

#endif

		case SEQUENCE_MPI_CONTROL_ERROR:

			LogPacketError(__FILE__,__LINE__,packet);
			return SUCCESS;

		default: 

			LogError(__FILE__,__LINE__);
			return ERROR;
	}
}
