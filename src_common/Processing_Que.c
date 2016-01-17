// This file implements the main watcher thread.

#include"System.h"

#pragma CODE_SECTION(ProcessInboundFlits,"FlashCode");
void ProcessInboundFlits()
{
	struct Packet receivedPacket;
	Uint16 flit[NUM_CHARACTERS_IN_FLIT];
	Uint16 virtualChannelArrivedOn, portArrivedOn, flitType;
	Uint32 crc;
#if PROFILE_HEADER_HANDLING == true || PROFILE_PAYLOAD_HANDLING == true
	double executionTime;
	Uint32 startTime;
#endif

	// Wait for the system to initialize itself
	SEM_pend(&ProcessInboundFlitsSem,SYS_FOREVER);

	// Loop forever
	while(true)
	{
		// Wait for more information to become available
		SEM_pend(&ProcessInboundFlitsSem,SYS_FOREVER);

		// Process all flits in the que
		while(DequeInboundFlit(flit) == SUCCESS)
		{
			// Get the port and virtual channel from the flit
			virtualChannelArrivedOn = flit[0] & 0x000F;
			flitType = flit[0] & 0x00F0;
			portArrivedOn = flit[0] >> 12;

			// Make sure the port and virtual channel are OK
			if(portArrivedOn < PORT_UNDEFINED && virtualChannelArrivedOn < VIRTUAL_CHANNEL_UNDEFINED && flitType < FLIT_TYPE_UNDEFINED)
			{
				// Check if this is a packet header, and if so, enqueue it
				if(flitType == FLIT_TYPE_HEADER)
				{
					globals.statistics.flit.numHeaderFlitsReceived++;

#if PROFILE_HEADER_HANDLING == true
					startTime = timer0Registers.TIM.all;
#endif
					if(UnmarshallPacketHeader(flit,&receivedPacket) != ERROR)
					{
						receivedPacket.transmissionInfo.portArrivedOn = portArrivedOn;
						receivedPacket.transmissionInfo.virtualChannelArrivedOn = virtualChannelArrivedOn;
						globals.statistics.packet.numControlPacketsReceived++;
						ProcessPacket(&receivedPacket);
#if PROFILE_HEADER_HANDLING == true
							executionTime = TimeDifference(startTime, timer0Registers.TIM.all);
							globals.statistics.profiling.avgHeaderHandlingTime = 
								(globals.statistics.profiling.avgHeaderHandlingTime * globals.statistics.profiling.numHeaderHandlingSamples) + executionTime;
							globals.statistics.profiling.numHeaderHandlingSamples++;
							globals.statistics.profiling.avgHeaderHandlingTime /= globals.statistics.profiling.numHeaderHandlingSamples;
							if(executionTime < globals.statistics.profiling.minHeaderHandlingTime)
								globals.statistics.profiling.minHeaderHandlingTime = executionTime;
							else if(executionTime > globals.statistics.profiling.maxHeaderHandlingTime)
								globals.statistics.profiling.maxHeaderHandlingTime = executionTime;
#endif
					}
				}

				// Otherwise check if it's the middle of a data transfer
				else if(flitType == FLIT_TYPE_DATA_PAYLOAD)
				{
					globals.statistics.flit.numDataPayloadFlitsReceived++;

					// Check if this is part of a direct transfer
					if(globals.processing.directTransfers[portArrivedOn][virtualChannelArrivedOn].destinationPort != PORT_UNDEFINED)
					{
						// Mask out the previous transmission info
						flit[0] &= 0x00F0;

						// Store the virtual channel in the flit for transmission
						flit[0] += globals.processing.directTransfers[portArrivedOn][virtualChannelArrivedOn].destinationVirtualChannel;
						
						// Send the flit on its way
						EnqueOutboundFlit(flit,globals.processing.directTransfers[portArrivedOn][virtualChannelArrivedOn].destinationPort);
					}
					// Check if it is part of a direct buffer
					else if(globals.processing.directBuffer[portArrivedOn][virtualChannelArrivedOn].packet.dataBuffer != NULL)
					{
						// Buffer the flit
						UnmarshallData(flit,portArrivedOn,virtualChannelArrivedOn);						
					}
					else
					{
						LogError(__FILE__,__LINE__);
					}
				}

				// Otherwise check if it's the end of a data transfer
				else if(flitType == FLIT_TYPE_DATA_PAYLOAD_END)
				{
					globals.statistics.flit.numDataPayloadEndFlitsReceived++;

					// Check if this is part of a direct transfer
					if(globals.processing.directTransfers[portArrivedOn][virtualChannelArrivedOn].destinationPort != PORT_UNDEFINED)
					{
						// Mask out the previous transmission info
						flit[0] &= 0x00F0;

						// Store the virtual channel in the flit for transmission
						flit[0] += globals.processing.directTransfers[portArrivedOn][virtualChannelArrivedOn].destinationVirtualChannel;
						
						// Send the flit on its way
						EnqueOutboundFlit(flit,globals.processing.directTransfers[portArrivedOn][virtualChannelArrivedOn].destinationPort);

						// Finalize the direct transfer
						ReleaseVirtualChannel(globals.processing.directTransfers[portArrivedOn][virtualChannelArrivedOn].destinationPort,
							globals.processing.directTransfers[portArrivedOn][virtualChannelArrivedOn].destinationVirtualChannel);

						globals.processing.directTransfers[portArrivedOn][virtualChannelArrivedOn].destinationPort = PORT_UNDEFINED;
						globals.statistics.packet.numDirectTransfersSucceeded++;
					}
					else if(globals.processing.directBuffer[portArrivedOn][virtualChannelArrivedOn].packet.dataBuffer != NULL)
					{
						// Buffer the flit
						UnmarshallData(flit,portArrivedOn,virtualChannelArrivedOn);

#if PROFILE_PAYLOAD_HANDLING == true
						startTime = timer0Registers.TIM.all;
#endif
						// Calculate the CRC of the data
						crc = CRC32(globals.processing.directBuffer[portArrivedOn][virtualChannelArrivedOn].packet.dataBuffer,
							globals.processing.directBuffer[portArrivedOn][virtualChannelArrivedOn].packet.dataBufferInfo.dataBufferLength);

						// Check for CRC errors
						if(((crc >> 24) & 0x000000FF) != globals.processing.directBuffer[portArrivedOn][virtualChannelArrivedOn].packet.f.commandSpecific1 ||
							((crc >> 16) & 0x000000FF) != globals.processing.directBuffer[portArrivedOn][virtualChannelArrivedOn].packet.g.commandSpecific2 ||
							((crc >> 8) & 0x000000FF) != globals.processing.directBuffer[portArrivedOn][virtualChannelArrivedOn].packet.g.commandSpecific3 ||
							(crc & 0x000000FF) != globals.processing.directBuffer[portArrivedOn][virtualChannelArrivedOn].packet.h.commandSpecific4)
						{
							globals.statistics.packet.numDataCrcErrors++;
							LogWarning(__FILE__,__LINE__);
							MemFree(globals.processing.directBuffer[portArrivedOn][virtualChannelArrivedOn].packet.dataBuffer);
							globals.processing.directBuffer[portArrivedOn][virtualChannelArrivedOn].packet.dataBuffer = NULL;
							SendErrorPacket(&(globals.processing.directBuffer[portArrivedOn][virtualChannelArrivedOn].packet),PROTOCOL_ERROR_INVALID_CRC);
						}
						else
						{
							// Call the callback function
							(*globals.processing.directBuffer[portArrivedOn][virtualChannelArrivedOn].callbackFunction)(
								&globals.processing.directBuffer[portArrivedOn][virtualChannelArrivedOn].packet);
						}
#if PROFILE_PAYLOAD_HANDLING == true
						executionTime = TimeDifference(startTime, timer0Registers.TIM.all);
						globals.statistics.profiling.avgPayloadHandlingTime = 
							(globals.statistics.profiling.avgPayloadHandlingTime * globals.statistics.profiling.numPayloadHandlingSamples) + executionTime;
						globals.statistics.profiling.numPayloadHandlingSamples++;
						globals.statistics.profiling.avgPayloadHandlingTime /= globals.statistics.profiling.numPayloadHandlingSamples;
						if(executionTime < globals.statistics.profiling.minPayloadHandlingTime)
							globals.statistics.profiling.minPayloadHandlingTime = executionTime;
						else if(executionTime > globals.statistics.profiling.maxPayloadHandlingTime)
							globals.statistics.profiling.maxPayloadHandlingTime = executionTime;
#endif
					}
					else
					{
						LogError(__FILE__,__LINE__);
					}
				}
			}
		}
	}
}

#pragma CODE_SECTION(EnqueInboundFlit,"FlashCode");
Uint16 EnqueInboundFlit(Uint16* flit)
{
	Uint16 i;

	// If there is room, que the flit
	if(!globals.processing.inboundFlitQueFull)
	{
		// Copy the flit to the que
		for(i = 0; i < NUM_CHARACTERS_IN_FLIT; i++)
			globals.processing.inboundFlitQue[globals.processing.inboundFlitQueHead][i] = flit[i];
		
		// Check if the buffer is full
		if(globals.processing.inboundFlitQueHead == globals.processing.inboundFlitQueTail - 1 ||
			(globals.processing.inboundFlitQueTail == 0 && globals.processing.inboundFlitQueHead == INBOUND_FLIT_BUFFER_SIZE - 1))
		{
			// Assert all MARB pins to prevent anything else from being transmitted
			gpioCtrlRegisters.GPADIR.bit.MARBA = 1;
			gpioDataRegisters.GPADAT.bit.MARBA = 1;
			gpioCtrlRegisters.GPADIR.bit.MARBB = 1;
			gpioDataRegisters.GPADAT.bit.MARBB = 1;
			gpioCtrlRegisters.GPADIR.bit.MARBC = 1;
			gpioDataRegisters.GPADAT.bit.MARBC = 1;
			gpioCtrlRegisters.GPADIR.bit.MARBD = 1;
			gpioDataRegisters.GPADAT.bit.MARBD = 1;

			// Set the inbout flit que full flag
			globals.processing.inboundFlitQueFull = true;

			// Update the statistics
			globals.statistics.que.inboundQueFullCount++;

			// Raise an error
			LogWarning(__FILE__,__LINE__);
			return ERROR;
		}

		// Inrement the index
		globals.processing.inboundFlitQueHead++;
		if(globals.processing.inboundFlitQueHead == INBOUND_FLIT_BUFFER_SIZE)
			globals.processing.inboundFlitQueHead = 0;

		// Update the statistics
		globals.statistics.que.currentInboundQueLoad++;
		if(globals.statistics.que.currentInboundQueLoad > globals.statistics.que.maxInboundQueLoad)
			globals.statistics.que.maxInboundQueLoad = globals.statistics.que.currentInboundQueLoad;

		// Wake up the processing thread
		if(SEM_count(&ProcessInboundFlitsSem) == 0)
			SEM_post(&ProcessInboundFlitsSem);

		return SUCCESS;
	}
	else
	{
		// Wake up the processing thread
		if(SEM_count(&ProcessInboundFlitsSem) == 0)
			SEM_post(&ProcessInboundFlitsSem);

		// Set the error
		LogWarning(__FILE__,__LINE__);
		return ERROR;
	}
}

#pragma CODE_SECTION(DequeInboundFlit,"FlashCode");
Uint16 DequeInboundFlit(Uint16* flit)
{
	Uint16 i;
	
	// This is a silent error, since it is always called in a loop irregardless of whether or not there is something to deque
	if(globals.processing.inboundFlitQueHead == globals.processing.inboundFlitQueTail && !globals.processing.inboundFlitQueFull)
		return ERROR;

	// Copy the oldest flit in the que
	for(i = 0; i < NUM_CHARACTERS_IN_FLIT; i++)
		flit[i] = globals.processing.inboundFlitQue[globals.processing.inboundFlitQueTail][i];

	// Adjust the que tail index
	globals.processing.inboundFlitQueTail++;
	if(globals.processing.inboundFlitQueTail == INBOUND_FLIT_BUFFER_SIZE)
		globals.processing.inboundFlitQueTail = 0;

	// Check if the buffer was previously full but isn't anymore
	if(globals.processing.inboundFlitQueFull == true)
	{
		// If so, deassert the MOUT pins if they aren't being used
		if(!globals.spi.portActive[PORTA])
			gpioCtrlRegisters.GPADIR.bit.MARBA = 0;
		if(!globals.spi.portActive[PORTB])
			gpioCtrlRegisters.GPADIR.bit.MARBB = 0;
		if(!globals.spi.portActive[PORTC])
			gpioCtrlRegisters.GPADIR.bit.MARBC = 0;
		if(!globals.spi.portActive[PORTD])
			gpioCtrlRegisters.GPADIR.bit.MARBD = 0;

		// Clear the flag
		globals.processing.inboundFlitQueFull = false;
	}

	// Update the statistics
	globals.statistics.que.currentInboundQueLoad--;

	return SUCCESS;
}

#pragma CODE_SECTION(EnqueOutboundFlit,"FlashCode");
Uint16 EnqueOutboundFlit(Uint16* flit, Uint16 port)
{
	Uint16 i;

	// Check if the outbound que is full
	if(globals.processing.outboundFlitQueFull[port])
	{
		LogWarning(__FILE__,__LINE__);
		return ERROR;
	}

	if((flit[0] & 0x00F0) == FLIT_TYPE_HEADER)
		globals.statistics.flit.numHeaderFlitsSent++;
	else if((flit[0] & 0x00F0) == FLIT_TYPE_DATA_PAYLOAD)
		globals.statistics.flit.numDataPayloadFlitsSent++;
	else if((flit[0] & 0x00F0) == FLIT_TYPE_DATA_PAYLOAD_END)
		globals.statistics.flit.numDataPayloadEndFlitsSent++;
#if TEST != TEST_SPI
	else
	{
		LogError(__FILE__,__LINE__);
		return ERROR;
	}
#endif

	// Copy the flit into the que
	for(i = 0; i < NUM_CHARACTERS_IN_FLIT; i++)
		globals.processing.outboundFlitQue[port][globals.processing.outboundFlitQueHead[port]][i] = flit[i];

	// Update the que pointers
	globals.processing.outboundFlitQueHead[port]++;
	if(globals.processing.outboundFlitQueHead[port] == OUTBOUND_FLIT_BUFFER_SIZE)
		globals.processing.outboundFlitQueHead[port] = 0;

	// Update the statistics
	globals.statistics.que.currentOutboundQueLoad[port]++;
	if(globals.statistics.que.currentOutboundQueLoad[port] > globals.statistics.que.maxOutboundQueLoad[port])
		globals.statistics.que.maxOutboundQueLoad[port] = globals.statistics.que.currentOutboundQueLoad[port];

	// Check if the que is full
	if(globals.statistics.que.currentOutboundQueLoad[port] == OUTBOUND_FLIT_BUFFER_SIZE)
	{
		globals.processing.outboundFlitQueFull[port] = true;
		globals.statistics.que.outboundQueFullCount[port]++;
	}

	// Wake up the appropriate outbound processor
	if(SEM_count(&ProcessOutboundFlitsSem) == 0)
		SEM_post(&ProcessOutboundFlitsSem);

	return SUCCESS;
}
