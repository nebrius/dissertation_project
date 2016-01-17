#ifndef _TEST_SERVICE_H
#define _TEST_SERVICE_H

#include"System.h"

void TestService(Void);
void SPITest();
void ProtocolTest1();
void ProtocolTest2();
void ProtocolTest3();
void MPITest();
void Convolution(int* x, int* y, int xLength, int yLength, int* result, int numNodesToUse);

extern Uint16 receivedResponse[];
extern Uint16 protocolTestPacketIDs[];

#endif
