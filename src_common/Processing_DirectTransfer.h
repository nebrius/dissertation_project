#ifndef _DIRECT_TRANSFER_H
#define _DIRECT_TRANSFER_H

#include"System.h"

Uint16 SetDataTransferLookup(struct Packet* packet);
Uint16 GetDataTransferIndex(Uint16 packetID);
Uint16 ClearDataTransferLookup(Uint16 packetID);
Uint16 SetupDirectTransfer(struct Packet* packet);
Uint16 ClearDirectTransferByPacketID(Uint16 packetID);
Uint16 SetupDirectBuffer(struct Packet* packet, Uint16 (*callbackFunction)(struct Packet*));
void CleanupDirectData(void* parameter, Uint16 followUpItemIndex);

#endif
