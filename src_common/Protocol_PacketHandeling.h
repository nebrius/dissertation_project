#ifndef _PROTOCOL_H
#define _PROTOCOL_H

// Packet Header Structure
struct Packet
{
	// Packet header fields
	struct
	{
		Uint16 version:4;
		Uint16 communicationType:4;
		Uint16 packetID:8;
	} a;
	struct
	{
		Uint16 command:8;
		Uint16 packetSequenceStep:8;
	} b;
	struct
	{
		Uint16 sourceServiceTag:8;
		Uint16 destinationServiceTag:8;
	} c;
	Uint32 path;
	struct
	{
		Uint16 commandSpecific0:8;
		Uint16 commandSpecific1:8;
	} f;
	struct
	{
		Uint16 commandSpecific2:8;
		Uint16 commandSpecific3:8;
	} g;
	struct
	{
		Uint16 commandSpecific4:8;
		Uint16 CRC:8;
	} h;

	// Data payload
	Uint16* dataBuffer;
	struct
	{
		Uint16 bufferIndex:8;
		Uint16 dataBufferLength:8;
	} dataBufferInfo;

	// Non-protocol packet fields
	struct
	{
		Uint32 portArrivedOn:3;
		Uint32 portTransmittedOn:3;
		Uint32 virtualChannelArrivedOn:5;
		Uint32 virtualChannelTransmittedOn:5;
		Uint32 source:8;
		Uint32 destination:8;
	} transmissionInfo;
};

void InitializePacket(struct Packet* packet, Uint16 packetID);
void InitializePacketFromPreviousSequence(struct Packet* newPacket,struct Packet* oldPacket);
void InitializePacketFromSelf(struct Packet* packet);
Uint16 UnmarshallPacketHeader(Uint16* buffer,struct Packet* packet);
Uint16 MarshallPacketHeader(struct Packet* packet, Uint16* buffer);
void UnmarshallData(Uint16* flit, Uint16 portArrivedOn, Uint16 virtualChannelArrivedOn);
void MarshallData(Uint16* flit, Uint16* dataBuffer, Uint16 dataBufferLength, Uint16* dataBufferIndex, Uint16 virtualChannelTransmittedOn);
void CopyPacket(struct Packet* sourcePacket, struct Packet* destPacket);
Uint16 GetTransmitPort(struct Packet* packet);
Uint16 ProcessPacket(struct Packet* packet);

#endif
