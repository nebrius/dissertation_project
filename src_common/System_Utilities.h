// Utilities definitions file

#ifndef _UTILITIES_H
#define _UTILITIES_H

extern const Uint16 CRC8Table[];
extern const Uint32 CRC32Table[];

void Delay(Uint32 numTicks);
int SetNonBlockingDelay(unsigned long int numTicks);
void DeleteNonBlockingDelay(int delayHandle);
int NonBlockingDelay(int delayHandle);
double TimeDifference(Uint32 time1, Uint32 time2);
void SetRandomSeed(Uint16 seed);
int Random(Uint16 floor, Uint16 ceiling);
void LogWarning(char* file, Uint16 line);
void LogError(char* file, Uint16 line);
void LogPacketError(char* file, int line, struct Packet* packet);
Uint16 CRC8(Uint16* buffer, Uint16 bufferSize);
Uint32 CRC32(Uint16* buffer, Uint16 bufferSize);

#endif
