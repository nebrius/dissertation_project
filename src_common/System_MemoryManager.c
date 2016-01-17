#include"System.h"

Uint16 allocationCalls = 0;

/*#pragma CODE_SECTION(MemoryCleanup,"FlashCode");
void MemoryCleanup()
{
	Uint16 i, j, k;

	// Set all memory allocations to not accounted for
	for(i = 0; i < NUM_MEMORY_ALLOCATIONS; i++)
		globals.memoryManager.allocations[i].bufferInfo.accountedFor = false;

	// Check if there are any data transfers that need to be tagged or removed
	for(i = 0; i < NUM_DATA_PACKET_LOOKUPS; i++)
	{
		// Check if this data transfer is active
		if(globals.processing.dataTransferPackets[i].transferInfo.packetID != 0)
		{
			for(j = 0; j < NUM_MEMORY_ALLOCATIONS; j++)
			{
				if(globals.memoryManager.allocations[j].bufferBaseAddress == globals.processing.dataTransferPackets[i].dataBuffer)
					globals.memoryManager.allocations[j].bufferInfo.accountedFor = true;
			}
		}
	}

	// Check if there are any direct buffers that need to be tagged or removed
	for(i = 0; i < NUM_PORTS; i++)
	{
		for(j = 0; j < NUM_VIRTUAL_DATA_CHANNELS; j++)
		{
			// Check if this direct buffer is active
			if(globals.processing.directBuffer[i][j].packet.dataBuffer != NULL)
			{
				for(k = 0; k < NUM_MEMORY_ALLOCATIONS; k++)
				{
					if(globals.memoryManager.allocations[k].bufferBaseAddress == globals.processing.directBuffer[i][j].packet.dataBuffer)
						globals.memoryManager.allocations[k].bufferInfo.accountedFor = true;
				}
			}
		}
	}

	// Check for memory leaks (aka all data is supposed to be registered somewhere, so leaks can be identified)
	for(i = 0; i < NUM_MEMORY_ALLOCATIONS; i++)
	{
		if(globals.memoryManager.allocations[i].bufferInfo.doesNotExpire == false && globals.memoryManager.allocations[i].bufferInfo.accountedFor == false &&
			globals.memoryManager.allocations[i].bufferBaseAddress != NULL)
		{
			MemFree(globals.memoryManager.allocations[i].bufferBaseAddress);
			globals.statistics.numDynamicMemoryBuffersExpired++;
		}
	}
}*/

#pragma CODE_SECTION(MemAlloc,"FlashCode");
Uint16* MemAlloc(Uint16 requestedLength)
{
	Uint16 i, j, currentMemoryUsage = 0, currentMemoryAllocations = 0;
	Uint16* bufferBaseAddress = NULL;

	allocationCalls++;

	// Make sure there are enough allocations
	if(globals.memoryManager.allocations[NUM_MEMORY_ALLOCATIONS - 1].bufferBaseAddress != NULL)
	{
		globals.statistics.memory.numFailedMemoryAllocs++;
		LogError(__FILE__,__LINE__);
		return NULL;
	}		

	// Check if there are no allocations since it's a special case
	if(globals.memoryManager.allocations[0].bufferBaseAddress == NULL && MEMORY_BUFFER_SIZE >= requestedLength)
	{
		globals.memoryManager.allocations[0].bufferBaseAddress = globals.memoryManager.memoryBuffer;
		globals.memoryManager.allocations[0].bufferInfo.length = requestedLength;
		globals.memoryManager.allocations[0].bufferInfo.doesNotExpire = false;
		bufferBaseAddress = globals.memoryManager.allocations[0].bufferBaseAddress;
	}
	// Check if this should occur before the first allocation
	else if((Uint32)globals.memoryManager.allocations[0].bufferBaseAddress - requestedLength > (Uint32)globals.memoryManager.memoryBuffer)
	{
		// Move the allocation entries backward to keep the list ordered
		for(i = NUM_MEMORY_ALLOCATIONS - 1; i > 0; i--)
		{
			globals.memoryManager.allocations[i].bufferBaseAddress = globals.memoryManager.allocations[i - 1].bufferBaseAddress;
			globals.memoryManager.allocations[i].bufferInfo.length = globals.memoryManager.allocations[i - 1].bufferInfo.length;
		}

		// Store the new memory allocation
		globals.memoryManager.allocations[0].bufferBaseAddress = globals.memoryManager.memoryBuffer;
		globals.memoryManager.allocations[0].bufferInfo.length = requestedLength;
		globals.memoryManager.allocations[0].bufferInfo.doesNotExpire = false;
		bufferBaseAddress = globals.memoryManager.allocations[0].bufferBaseAddress;
	}
	// Check for an open allocation in the middle or end of the allocations
	else
	{
		for(i = 1; i < NUM_MEMORY_ALLOCATIONS; i++)
		{
			// Check if all memory is contiguous and we are allocating from the end
			if(globals.memoryManager.allocations[i].bufferBaseAddress == NULL && (MEMORY_BUFFER_SIZE - 
				((Uint32)globals.memoryManager.allocations[i - 1].bufferBaseAddress + globals.memoryManager.allocations[i - 1].bufferInfo.length) >= requestedLength))
			{
				// Store the allocation information
				globals.memoryManager.allocations[i].bufferBaseAddress = globals.memoryManager.allocations[i - 1].bufferBaseAddress + 
					globals.memoryManager.allocations[i - 1].bufferInfo.length;
				globals.memoryManager.allocations[i].bufferInfo.length = requestedLength;
				globals.memoryManager.allocations[i].bufferInfo.doesNotExpire = false;

				// Return the address
				bufferBaseAddress = globals.memoryManager.allocations[i].bufferBaseAddress;
				break;
			}
			// Check if there is space in between allocations
			else if((Uint32)globals.memoryManager.allocations[i].bufferBaseAddress - ((Uint32)globals.memoryManager.allocations[i - 1].bufferBaseAddress + 
				globals.memoryManager.allocations[i - 1].bufferInfo.length) >= requestedLength)
			{
				// Move the allocation entries to keep the list ordered
				for(j = NUM_MEMORY_ALLOCATIONS - 1; j > i; j--)
				{
					globals.memoryManager.allocations[j].bufferBaseAddress = globals.memoryManager.allocations[j - 1].bufferBaseAddress;
					globals.memoryManager.allocations[j].bufferInfo.length = globals.memoryManager.allocations[j - 1].bufferInfo.length;
				}

				// Store the allocation information
				globals.memoryManager.allocations[i].bufferBaseAddress = (Uint16*)((Uint32)globals.memoryManager.allocations[i - 1].bufferBaseAddress + 
					globals.memoryManager.allocations[i - 1].bufferInfo.length);
				globals.memoryManager.allocations[i].bufferInfo.length = requestedLength;
				globals.memoryManager.allocations[i].bufferInfo.doesNotExpire = false;

				// Return the address
				bufferBaseAddress = globals.memoryManager.allocations[i].bufferBaseAddress;
				break;
			}
		}
	}

	// Calculate the amount of memory currently being used and the number of allocations
	for(i = 0; i < NUM_MEMORY_ALLOCATIONS; i++)
	{
		if(globals.memoryManager.allocations[i].bufferBaseAddress != NULL)
		{
			currentMemoryAllocations++;
			currentMemoryUsage += globals.memoryManager.allocations[i].bufferInfo.length;
		}
	}
	if(currentMemoryAllocations > globals.statistics.memory.maxMemoryAllocations)
		globals.statistics.memory.maxMemoryAllocations = currentMemoryAllocations;
	if(currentMemoryUsage > globals.statistics.memory.maxMemoryUsed)
		globals.statistics.memory.maxMemoryUsed = currentMemoryUsage;

	// Return the address
	return bufferBaseAddress;
}

#pragma CODE_SECTION(MemFree,"FlashCode");
Uint16 MemFree(Uint16* location)
{
	Uint16 i,j;

	for(i = 0; i < NUM_MEMORY_ALLOCATIONS; i++)
	{
		if(globals.memoryManager.allocations[i].bufferBaseAddress == location)
		{
			// Move the allocation entries to keep the list ordered
			for(j = i; j < NUM_MEMORY_ALLOCATIONS - 2; j++)
			{
				globals.memoryManager.allocations[j].bufferBaseAddress = globals.memoryManager.allocations[j + 1].bufferBaseAddress;
				globals.memoryManager.allocations[j].bufferInfo.length = globals.memoryManager.allocations[j + 1].bufferInfo.length;
			}
			globals.memoryManager.allocations[NUM_MEMORY_ALLOCATIONS - 1].bufferBaseAddress = NULL;

			// Return success
			return SUCCESS;
		}
	}

	LogError(__FILE__,__LINE__);
	return ERROR;
}

#pragma CODE_SECTION(MemGetLength,"FlashCode");
Uint16 MemGetLength(Uint16* location)
{
	Uint16 i;

	for(i = 0; i < NUM_MEMORY_ALLOCATIONS; i++)
	{
		if(globals.memoryManager.allocations[i].bufferBaseAddress == location)
			return globals.memoryManager.allocations[i].bufferInfo.length;
	}

	// If it gets here, the location wasn't found in the allocations list
	LogError(__FILE__,__LINE__);
	return ERROR;
}
