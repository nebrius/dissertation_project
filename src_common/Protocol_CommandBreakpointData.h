#ifndef _COMMAND_BREAKPOINT_DATA_H
#define _COMMAND_BREAKPOINT_DATA_H

#include"System.h"

Uint16 ProcessCommandBreakpointData(struct Packet* packet);
Uint16 SequenceBreakpointDataRequestTransfer(struct Packet* packet);
Uint16 SequenceBreakpointDataRequestAccepted(struct Packet* packet);
Uint16 SequenceBreakpointDataTransfer(struct Packet* packet);
Uint16 SequenceBreakpointDataTransferReceived(struct Packet* packet);
Uint16 SequenceBreakpointDataError(struct Packet* packet);

#endif
