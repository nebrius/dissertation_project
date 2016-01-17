#ifndef _COMMAND_REQUEST_ADDRESS_H
#define _COMMAND_REQUEST_ADDRESS_H

#include"System.h"

Uint16 ProcessCommandRequestAddress(struct Packet* packet);
Uint16 SequenceReportNeighborsTransferCompleted(struct Packet* packet);

#endif
