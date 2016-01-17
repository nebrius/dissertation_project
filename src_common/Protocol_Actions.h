#ifndef _PROTOCOL_ACTIONS_H
#define _PROTOCOL_ACTIONS_H

#include"System.h"

void NeighborFollowUp(void* parameter, Uint16 followUpItemIndex);

#if defined(IS_ROUTER)

void ReportNeighbors(void* parameter, Uint16 followUpItemIndex);
Uint16 HandleCommFailure(Uint16 nodeAddress);
void RequestRoutingTable();

#endif

void EnsureRoutingTableDistribution(void* parameter, Uint16 followUpItemIndex);
void EnsureBarrierReached(void* parameter, Uint16 followUpItemIndex);

#endif
