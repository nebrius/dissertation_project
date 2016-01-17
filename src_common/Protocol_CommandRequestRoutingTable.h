#ifndef _COMMAND_REQUEST_ROUTING_TABLE_H
#define _COMMAND_REQUEST_ROUTING_TABLE_H

#include"System.h"

Uint16 ProcessCommandRequestRoutingTable(struct Packet* packet);
Uint16 SequenceRequestRoutingTableTransferCompleted(struct Packet* packet);

#endif
