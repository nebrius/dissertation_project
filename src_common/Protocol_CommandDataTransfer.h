#ifndef _COMMAND_DATA_TRANSFER_H
#define _COMMAND_DATA_TRANSFER_H

#include"System.h"

Uint16 ProcessCommandDataTransfer(struct Packet* packet);
Uint16 SequenceDataTransferTransferCompleted(struct Packet* packet);

#endif
