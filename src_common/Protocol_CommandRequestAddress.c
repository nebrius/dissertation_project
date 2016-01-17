#include"System.h"

#pragma CODE_SECTION(ProcessCommandRequestAddress,"FlashCode");
Uint16 ProcessCommandRequestAddress(struct Packet* packet)
{
	struct Packet newPacket;
	Uint16 i;
#if defined(IS_ROOT)
	Uint32 limpModePathIntermediate;
	Uint32 newAddress = 0;
#endif

	// Process the packet according to which sequence step the packet is on
	switch(packet->b.packetSequenceStep)
	{
		case SEQUENCE_REQUEST_ADDRESS_REQUEST:

#if defined(IS_ROOT)

			// Check the address database for an available address
			for(i = 0; i < MAX_NUM_NODES; i++)
			{
				if(!globals.root.availableAddresses[i].addressTaken)
				{
					// Set the address
					newAddress = globals.root.availableAddresses[i].address;
					globals.root.availableAddresses[i].addressTaken = true;

					// Update the neighbor information
					globals.protocol.globalNeighborInfo[ROOT_ADDRESS][packet->transmissionInfo.portArrivedOn].nodeAddress = newAddress;

					// Store the limp mode path
					limpModePathIntermediate = (ROOT_ADDRESS << 4) + newAddress;
					globals.routing.limpModePaths[newAddress] = limpModePathIntermediate << 24;

					// Initialize the packet
					InitializePacketFromPreviousSequence(&newPacket,packet);

					// Create the address grant packet
					newPacket.path = globals.routing.limpModePaths[newAddress];
					newPacket.transmissionInfo.portTransmittedOn = packet->transmissionInfo.portArrivedOn;
					newPacket.b.packetSequenceStep = SEQUENCE_REQUEST_ADDRESS_GRANT;
					newPacket.f.commandSpecific0 = newAddress;

					// Send the packet
					return SendControlPacket(&newPacket);
				}
			}

			// If it got here, then there was no available addresses to hand out
			return ERROR;

#elif defined(IS_ROUTER)

			// // Sanity check. Check if this router has an address. It should if it received this packet.
			if(globals.protocol.address == ADDRESS_UNDEFINED)
			{
				LogError(__FILE__,__LINE__);
				return ERROR;
			}

			// Initialize the packet
			InitializePacketFromPreviousSequence(&newPacket,packet);

			// Create the request packet to proxy to the root node
			newPacket.a.communicationType = COMM_TYPE_UNICAST;
			newPacket.transmissionInfo.destination = ROOT_ADDRESS;
			newPacket.b.packetSequenceStep = SEQUENCE_REQUEST_ADDRESS_REQUEST_FORWARD;
			newPacket.f.commandSpecific0 = packet->transmissionInfo.portArrivedOn;

			// Send the packet
			return SendControlPacket(&newPacket);

#endif

		case SEQUENCE_REQUEST_ADDRESS_REQUEST_FORWARD:

#if defined(IS_ROOT)

			// Check the address database for an available address
			for(i = 0; i < MAX_NUM_NODES; i++)
			{
				if(!globals.root.availableAddresses[i].addressTaken)
				{
					// Set the address
					newAddress = globals.root.availableAddresses[i].address;
					globals.root.availableAddresses[i].addressTaken = true;
					
					// Store the limp mode path
					if((globals.routing.limpModePaths[packet->transmissionInfo.source] & 0xF0000000) == 0)
						globals.routing.limpModePaths[newAddress] = globals.routing.limpModePaths[packet->transmissionInfo.source] + (newAddress << 28);
					else if((globals.routing.limpModePaths[packet->transmissionInfo.source] & 0x0F000000) == 0)
						globals.routing.limpModePaths[newAddress] = globals.routing.limpModePaths[packet->transmissionInfo.source] + (newAddress << 24);
					else if((globals.routing.limpModePaths[packet->transmissionInfo.source] & 0x00F00000) == 0)
						globals.routing.limpModePaths[newAddress] = globals.routing.limpModePaths[packet->transmissionInfo.source] + (newAddress << 20);
					else if((globals.routing.limpModePaths[packet->transmissionInfo.source] & 0x000F0000) == 0)
						globals.routing.limpModePaths[newAddress] = globals.routing.limpModePaths[packet->transmissionInfo.source] + (newAddress << 16);
					else if((globals.routing.limpModePaths[packet->transmissionInfo.source] & 0x0000F000) == 0)
						globals.routing.limpModePaths[newAddress] = globals.routing.limpModePaths[packet->transmissionInfo.source] + (newAddress << 12);
					else if((globals.routing.limpModePaths[packet->transmissionInfo.source] & 0x00000F00) == 0)
						globals.routing.limpModePaths[newAddress] = globals.routing.limpModePaths[packet->transmissionInfo.source] + (newAddress << 8);
					else if((globals.routing.limpModePaths[packet->transmissionInfo.source] & 0x000000F0) == 0)
						globals.routing.limpModePaths[newAddress] = globals.routing.limpModePaths[packet->transmissionInfo.source] + (newAddress << 4);
					else if((globals.routing.limpModePaths[packet->transmissionInfo.source] & 0x0000000F) == 0)
						globals.routing.limpModePaths[newAddress] = globals.routing.limpModePaths[packet->transmissionInfo.source] + newAddress;
					
					break;
				}
			}

			// If an address is available, send the new address to the node
			if(newAddress != 0)
			{
				// Initialize the packet
				InitializePacketFromPreviousSequence(&newPacket,packet);
				
				// Create the address grant packet
				newPacket.transmissionInfo.destination = packet->transmissionInfo.source;
				newPacket.b.packetSequenceStep = SEQUENCE_REQUEST_ADDRESS_GRANT_FORWARD;
				newPacket.f.commandSpecific0 = packet->f.commandSpecific0;
				newPacket.f.commandSpecific1 = newAddress;

				// Send the packet
				return SendControlPacket(&newPacket);
			}
			// Otherwise, send an error back
			else
			{
				// Initialize the packet
				InitializePacketFromPreviousSequence(&newPacket,packet);

				// Create the address denied packet
				newPacket.transmissionInfo.destination = packet->transmissionInfo.source;
				newPacket.b.packetSequenceStep = SEQUENCE_REQUEST_ADDRESS_ERROR;
				newPacket.f.commandSpecific0 = packet->f.commandSpecific0;

				// Send the packet
				return SendControlPacket(&newPacket);
			}

#elif defined(IS_ROUTER)

			LogError(__FILE__,__LINE__);
			return ERROR;

#endif

		case SEQUENCE_REQUEST_ADDRESS_GRANT_FORWARD:

#if defined(IS_ROOT)

			LogError(__FILE__,__LINE__);
			return ERROR;

#elif defined(IS_ROUTER)
			
			// Initialize the packet
			InitializePacketFromPreviousSequence(&newPacket,packet);

			// Create the address granted packet
			newPacket.a.communicationType = COMM_TYPE_ADDRESSLESS;
			newPacket.transmissionInfo.portTransmittedOn = packet->f.commandSpecific0;
			newPacket.b.packetSequenceStep = SEQUENCE_REQUEST_ADDRESS_GRANT;
			newPacket.f.commandSpecific0 = packet->f.commandSpecific1;
			newPacket.path = packet->path;

			// Update the neighbor information
			globals.protocol.neighborInfo[newPacket.transmissionInfo.portTransmittedOn].nodeAddress = newPacket.f.commandSpecific0;

			// Send the packet
			return SendControlPacket(&newPacket);

#endif

		case SEQUENCE_REQUEST_ADDRESS_GRANT:

#if defined(IS_ROOT)

			LogError(__FILE__,__LINE__);
			return ERROR;

#elif defined(IS_ROUTER)

			// Store the address
			globals.protocol.address = packet->f.commandSpecific0;

			// Reset the random number generator to use the address
			SetRandomSeed(globals.protocol.address * 22);

			// Store the limp mode path
			globals.routing.limpModePath = 0;
			if(globals.protocol.neighborInfo[packet->transmissionInfo.portArrivedOn].nodeAddress == ROOT_ADDRESS)
			{
				globals.routing.limpModePath = (globals.protocol.address << 4) + ROOT_ADDRESS;
				globals.routing.limpModePath = (globals.routing.limpModePath << 24);
			}
			else
			{
				for(i = 0; i < MAX_PATH_LENGTH; i++)
				{
					if(packet->path & 0xF0000000)
					{
						globals.routing.limpModePath = (globals.routing.limpModePath >> 4);
						globals.routing.limpModePath += (packet->path & 0xF0000000);
						packet->path = (packet->path << 4);
					}
					else
						break;
				}
				globals.routing.limpModePath = (globals.routing.limpModePath >> 4);
				globals.routing.limpModePath += ((Uint32)globals.protocol.address << 28);
			}

			// Display the address on the seven segment display
			globals.processing.sevenSegmentLowerDigit = globals.protocol.address;

			// Slow down the neighbor follow up now that we have an address
			globals.followUpMonitor.followUpItems[globals.followUpMonitor.neighborFollowUpIndex].timeToFollowUp = NEIGHBOR_FOLLOW_UP_RATE;

			// Create the report neighbors follow-up item
#if TEST == TEST_MPI
			return AddFollowUpItem(&ReportNeighbors,NULL,REPORT_NEIGHBORS_FOLLOW_UP_RATE,true);
#else
			return SUCCESS;
#endif

#endif

		case SEQUENCE_REQUEST_ADDRESS_ERROR:

#if defined(IS_ROOT)

			LogPacketError(__FILE__,__LINE__,packet);
			return SUCCESS;

#elif defined(IS_ROUTER)

			// Check if this error needs to be forwarded to a neighbor
			if(globals.protocol.address != ADDRESS_UNDEFINED)
			{
				// Initialize the packet
				InitializePacketFromPreviousSequence(&newPacket,packet);

				// Create the address denied packet
				newPacket.b.packetSequenceStep = SEQUENCE_REQUEST_ADDRESS_ERROR;
				newPacket.transmissionInfo.destination = packet->f.commandSpecific0;
				
				// Send the packet
				return SendControlPacket(&newPacket);
			}
			else
			{
				LogPacketError(__FILE__,__LINE__,packet);
				return SUCCESS;
			}

#endif

		default: 

			LogError(__FILE__,__LINE__);
			return ERROR;
	}
}
