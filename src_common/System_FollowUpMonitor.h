#ifndef PENDING_ITEMS_H
#define PENDING_ITEMS_H

#include"System.h"

void FollowUpMonitorWakeup();
void HeartbeatLED();
void FollowUpMonitor();
Uint16 AddFollowUpItem(void (*callbackFunction)(void*, Uint16), void* callbackFunctionParameters, Uint16 timeToFollowUp, Uint16 doesNotExpire);
Uint16 RemoveFollowUpItem(void (*callbackFunction)(void*, Uint16), void* callbackFunctionParameters);
void InitializeSystem(void* parameter, Uint16 followUpItemIndex);

#endif
