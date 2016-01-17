#ifndef _TRANSMIT_PACKET_H
#define _TRANSMIT_PACKET_H

#include"System.h"

// Packet sending functions
Uint16 SendControlPacket(struct Packet* packet);
Uint16 SendDataPacket(struct Packet* packet);
Uint16 SendDataPacketPayload(struct Packet* packet);
Uint16 SendErrorPacket(struct Packet* packet, Uint16 errorCode);
Uint16 ReserveVirtualChannel(Uint16 port, Uint16 packetID);
Uint16 ReleaseVirtualChannel(Uint16 port, Uint16 virtualChannel);
Uint16 GetPortFromPacketID(Uint16 packetID);
Uint16 GetVirtualChannelFromPacketID(Uint16 packetID);

#endif
