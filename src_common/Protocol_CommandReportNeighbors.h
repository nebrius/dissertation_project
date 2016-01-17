#ifndef _COMMAND_REPORT_NEIGHBORS_H
#define _COMMAND_REPORT_NEIGHBORS_H

#include"System.h"

Uint16 ProcessCommandReportNeighbors(struct Packet* packet);
Uint16 SequenceReportNeighborsTransferComplete(struct Packet* packet);

#endif
