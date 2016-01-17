// This file defines the main functions for watching the SPI receive ports.

#ifndef _PROCESSING_H
#define _PROCESSING_H

// Task functions
void ProcessInboundFlits();

// Que management functions
Uint16 EnqueInboundFlit(Uint16* flit);
Uint16 DequeInboundFlit(Uint16* flit);
Uint16 EnqueOutboundFlit(Uint16* flit, Uint16 port);

#endif
