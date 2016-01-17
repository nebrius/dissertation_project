#ifndef _MEMORY_MANAGER_H
#define _MEMORY_MANAGER_H

#include"System.h"

//void MemoryCleanup();
Uint16* MemAlloc(Uint16 size);
Uint16 MemFree(Uint16* location);
Uint16 MemGetLength(Uint16* location);

#endif
