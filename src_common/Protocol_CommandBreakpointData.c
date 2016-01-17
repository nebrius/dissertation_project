#include"System.h"

#pragma CODE_SECTION(ProcessCommandBreakpointData,"FlashCode");
Uint16 ProcessCommandBreakpointData(struct Packet* packet)
{
	// Process the packet according to which sequence step the packet is on
	switch(packet->b.packetSequenceStep)
	{
		case SEQUENCE_BREAKPOINT_DATA_REQUEST_TRANSFER:
			return SequenceBreakpointDataRequestTransfer(packet);

		case SEQUENCE_BREAKPOINT_DATA_REQUEST_ACCEPTED:
			return SequenceBreakpointDataRequestAccepted(packet);

		case SEQUENCE_BREAKPOINT_DATA_TRANSFER:
			return SequenceBreakpointDataTransfer(packet);

		case SEQUENCE_BREAKPOINT_DATA_TRANSFER_RECEIVED:
			return SequenceBreakpointDataTransferReceived(packet);

		case SEQUENCE_BREAKPOINT_DATA_ERROR:
			return SequenceBreakpointDataError(packet);

		default: 
			LogError(__FILE__,__LINE__);
			return ERROR;
	}
}

#pragma CODE_SECTION(SequenceBreakpointDataRequestTransfer,"FlashCode");
Uint16 SequenceBreakpointDataRequestTransfer(struct Packet* packet)
{
	return SUCCESS;
}

#pragma CODE_SECTION(SequenceBreakpointDataRequestAccepted,"FlashCode");
Uint16 SequenceBreakpointDataRequestAccepted(struct Packet* packet)
{
	return SUCCESS;
}

#pragma CODE_SECTION(SequenceBreakpointDataTransfer,"FlashCode");
Uint16 SequenceBreakpointDataTransfer(struct Packet* packet)
{
	return SUCCESS;
}

#pragma CODE_SECTION(SequenceBreakpointDataTransferReceived,"FlashCode");
Uint16 SequenceBreakpointDataTransferReceived(struct Packet* packet)
{
	return SUCCESS;
}

#pragma CODE_SECTION(SequenceBreakpointDataError,"FlashCode");
Uint16 SequenceBreakpointDataError(struct Packet* packet)
{
	LogPacketError(__FILE__,__LINE__,packet);
	return SUCCESS;
}

