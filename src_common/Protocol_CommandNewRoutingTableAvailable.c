#include"System.h"

#pragma CODE_SECTION(ProcessCommandNewRoutingTableAvailable,"FlashCode");
Uint16 ProcessCommandNewRoutingTableAvailable(struct Packet* packet)
{
	// Process the packet according to which sequence step the packet is on
	switch(packet->b.packetSequenceStep)
	{
		case SEQUENCE_NEW_ROUTING_TABLE_AVAILABLE:

#if defined(IS_ROOT)

			LogError(__FILE__,__LINE__);
			return ERROR;

#elif defined(IS_ROUTER)

			RequestRoutingTable();
			return SUCCESS;

#endif

		case SEQUENCE_NEW_ROUTING_TABLE_AVAILABLE_ERROR:

			LogPacketError(__FILE__,__LINE__,packet);
			return SUCCESS;

		default: 

			LogError(__FILE__,__LINE__);
			return ERROR;
	}
}
