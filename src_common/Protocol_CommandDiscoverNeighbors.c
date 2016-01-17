#include"System.h"

#pragma CODE_SECTION(ProcessCommandDiscoverNeighbors,"FlashCode");
Uint16 ProcessCommandDiscoverNeighbors(struct Packet* packet)
{
	struct Packet newPacket;

	// Process the packet according to which sequence step the packet is on
	switch(packet->b.packetSequenceStep)
	{
		case SEQUENCE_DISCOVER_NEIGHBORS_REQUEST_NEIGHBOR:

			// Initialize the packet
			InitializePacketFromPreviousSequence(&newPacket,packet);

			// Create the response that indicates what type of router this is
			newPacket.b.packetSequenceStep = SEQUENCE_DISCOVER_NEIGHBORS_NEIGHBOR_RESPONSE;
#if defined(IS_ROOT)
			newPacket.f.commandSpecific0 = NODE_TYPE_ROOT_ROUTER;
#elif defined(IS_ROUTER)
			newPacket.f.commandSpecific0 = NODE_TYPE_NON_ROOT_ROUTER;
#endif
			newPacket.f.commandSpecific1 = globals.protocol.address;
			
			// Send the response back
			newPacket.transmissionInfo.portTransmittedOn = packet->transmissionInfo.portArrivedOn;
			return SendControlPacket(&newPacket);

		case SEQUENCE_DISCOVER_NEIGHBORS_NEIGHBOR_RESPONSE:

#if defined(IS_ROOT)

			// Check if this neighbor was not here on the previous discovery attemp
			if(globals.protocol.globalNeighborInfo[ROOT_ADDRESS][packet->transmissionInfo.portArrivedOn].nodeType == NODE_TYPE_UNDEFINED)
			{
				// Store the basic neighbor information
				globals.protocol.globalNeighborInfo[ROOT_ADDRESS][packet->transmissionInfo.portArrivedOn].commFailLevel = MAX_COMM_FAILURE_LEVEL;
				globals.protocol.globalNeighborInfo[ROOT_ADDRESS][packet->transmissionInfo.portArrivedOn].nodeType = packet->f.commandSpecific0;
				globals.protocol.globalNeighborInfo[ROOT_ADDRESS][packet->transmissionInfo.portArrivedOn].nodeAddress = packet->f.commandSpecific1;
			}
			else
			{
				// Check if the previous address was undefined
				if(globals.protocol.globalNeighborInfo[ROOT_ADDRESS][packet->transmissionInfo.portArrivedOn].nodeAddress == ADDRESS_UNDEFINED)
				{
					globals.protocol.globalNeighborInfo[ROOT_ADDRESS][packet->transmissionInfo.portArrivedOn].nodeType = packet->f.commandSpecific0;
					globals.protocol.globalNeighborInfo[ROOT_ADDRESS][packet->transmissionInfo.portArrivedOn].nodeAddress = packet->f.commandSpecific1;
				}
				else if(globals.protocol.globalNeighborInfo[ROOT_ADDRESS][packet->transmissionInfo.portArrivedOn].nodeAddress != packet->f.commandSpecific1)
				{
					globals.protocol.globalNeighborInfo[ROOT_ADDRESS][packet->transmissionInfo.portArrivedOn].nodeAddress = packet->f.commandSpecific1;
				}
				globals.protocol.globalNeighborInfo[ROOT_ADDRESS][packet->transmissionInfo.portArrivedOn].commFailLevel = MAX_COMM_FAILURE_LEVEL;
			}

#elif defined(IS_ROUTER)

			// Check if this neighbor was here on the previous discovery attemp
			if(globals.protocol.neighborInfo[packet->transmissionInfo.portArrivedOn].nodeType == NODE_TYPE_UNDEFINED)
			{
				// Store the basic neighbor information
				globals.protocol.neighborInfo[packet->transmissionInfo.portArrivedOn].nodeType = packet->f.commandSpecific0;
				globals.protocol.neighborInfo[packet->transmissionInfo.portArrivedOn].nodeAddress = packet->f.commandSpecific1;
				globals.protocol.neighborInfo[packet->transmissionInfo.portArrivedOn].commFailLevel = MAX_COMM_FAILURE_LEVEL;
			}
			else
			{
				// Check if the previous address was undefined
				if(globals.protocol.neighborInfo[packet->transmissionInfo.portArrivedOn].nodeAddress == ADDRESS_UNDEFINED)
				{
					globals.protocol.neighborInfo[packet->transmissionInfo.portArrivedOn].nodeType = packet->f.commandSpecific0;
					globals.protocol.neighborInfo[packet->transmissionInfo.portArrivedOn].nodeAddress = packet->f.commandSpecific1;
				}
				else if(globals.protocol.neighborInfo[packet->transmissionInfo.portArrivedOn].nodeAddress != packet->f.commandSpecific1)
				{
					if(HandleCommFailure(globals.protocol.neighborInfo[packet->transmissionInfo.portArrivedOn].nodeAddress) == ERROR)
						return ERROR;
					globals.protocol.neighborInfo[packet->transmissionInfo.portArrivedOn].nodeAddress = packet->f.commandSpecific1;
				}
				globals.protocol.neighborInfo[packet->transmissionInfo.portArrivedOn].commFailLevel = MAX_COMM_FAILURE_LEVEL;
			}
		

#endif

			return SUCCESS;

		case SEQUENCE_DISCOVER_NEIGHBORS_ERROR:

			LogPacketError(__FILE__,__LINE__,packet);
			return SUCCESS;

		default: 

			LogError(__FILE__,__LINE__);
			return ERROR;
	}
}
