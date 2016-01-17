// This file implements the interrupt routines that process a packet

#include"System.h"

int spiSuccesses[4] = {0,0,0,0};
int spiFailures[4] = {0,0,0,0};

#pragma CODE_SECTION(PortARXInterrupt,"FlashCode");
void PortARXInterrupt()
{
	Uint16 i;
	Uint16 flit[NUM_CHARACTERS_IN_FLIT];
#if PROFILE_INTERRUPTS
	double executionTime;
	Uint32 startTime;
	startTime = timer0Registers.TIM.all;
#endif

	// Buffer the flit
	for(i = 0; i < NUM_CHARACTERS_IN_FLIT; i++)
		flit[i] = spiaRegisters.SPIRXBUF;

#if TEST == TEST_SPI
	if(flit[0] == 0xCA5F && flit[1] == 0xCA5F && flit[2] == 0xCA5F && flit[3] == 0xCA5F && 
		flit[4] == 0xCA5F && flit[5] == 0xCA5F && flit[6] == 0xCA5F && flit[7] == 0xCA5F && 
		flit[8] == 0xCA5F)
		globals.statistics.flit.numFlitsReceived[PORTA]++;
	else
		spiFailures[PORTA]++;
#else

	globals.statistics.flit.numFlitsReceived[PORTA]++;

	// Add the port to the left-most 4 bits of the first flit characer
	flit[0] += PORTA_SHIFTED;

	// Enque the flit if there is room and wake up the processing thread
	EnqueInboundFlit(flit);

#endif

#if PROFILE_INTERRUPTS == true
	executionTime = TimeDifference(startTime, timer0Registers.TIM.all);
	globals.statistics.profiling.avgInterruptsTime = 
		(globals.statistics.profiling.avgInterruptsTime * globals.statistics.profiling.numInterruptsSamples) + executionTime;
	globals.statistics.profiling.numInterruptsSamples++;
	globals.statistics.profiling.avgInterruptsTime /= globals.statistics.profiling.numInterruptsSamples;
	if(executionTime < globals.statistics.profiling.minInterruptsTime)
		globals.statistics.profiling.minInterruptsTime = executionTime;
	else if(executionTime > globals.statistics.profiling.maxInterruptsTime)
		globals.statistics.profiling.maxInterruptsTime = executionTime;
#endif

	// Acknowledge the interrupt and re-enable it
	pieRegisters.PIEACK.bit.ACK6 = 1;
	spiaRegisters.SPIFFRX.bit.RXFFINTCLR = 1;
}

#pragma CODE_SECTION(PortBRXInterrupt,"FlashCode");
void PortBRXInterrupt()
{
	Uint16 i;
	Uint16 flit[NUM_CHARACTERS_IN_FLIT];
#if PROFILE_INTERRUPTS
	double executionTime;
	Uint32 startTime;
	startTime = timer0Registers.TIM.all;
#endif

	// Buffer the flit
	for(i = 0; i < NUM_CHARACTERS_IN_FLIT; i++)
		flit[i] = spibRegisters.SPIRXBUF;

#if TEST == TEST_SPI
	if(flit[0] == 0xCA5F && flit[1] == 0xCA5F && flit[2] == 0xCA5F && flit[3] == 0xCA5F && 
		flit[4] == 0xCA5F && flit[5] == 0xCA5F && flit[6] == 0xCA5F && flit[7] == 0xCA5F && 
		flit[8] == 0xCA5F)
		globals.statistics.flit.numFlitsReceived[PORTB]++;
	else
		spiFailures[PORTB]++;
#else

	globals.statistics.flit.numFlitsReceived[PORTB]++;

	// Add the port to the left-most 4 bits of the first flit characer
	flit[0] += PORTB_SHIFTED;

	// Enque the flit if there is room and wake up the processing thread
	EnqueInboundFlit(flit);

#endif

#if PROFILE_INTERRUPTS == true
	executionTime = TimeDifference(startTime, timer0Registers.TIM.all);
	globals.statistics.profiling.avgInterruptsTime = 
		(globals.statistics.profiling.avgInterruptsTime * globals.statistics.profiling.numInterruptsSamples) + executionTime;
	globals.statistics.profiling.numInterruptsSamples++;
	globals.statistics.profiling.avgInterruptsTime /= globals.statistics.profiling.numInterruptsSamples;
	if(executionTime < globals.statistics.profiling.minInterruptsTime)
		globals.statistics.profiling.minInterruptsTime = executionTime;
	else if(executionTime > globals.statistics.profiling.maxInterruptsTime)
		globals.statistics.profiling.maxInterruptsTime = executionTime;
#endif

	// Acknowledge the interrupt and re-enable it
	pieRegisters.PIEACK.bit.ACK6 = 1;
	spibRegisters.SPIFFRX.bit.RXFFINTCLR = 1;
}

#pragma CODE_SECTION(PortCRXInterrupt,"FlashCode");
void PortCRXInterrupt()
{
	Uint16 i;
	Uint16 flit[NUM_CHARACTERS_IN_FLIT];
#if PROFILE_INTERRUPTS
	double executionTime;
	Uint32 startTime;
	startTime = timer0Registers.TIM.all;
#endif

	// Buffer the flit
	for(i = 0; i < NUM_CHARACTERS_IN_FLIT; i++)
		flit[i] = spicRegisters.SPIRXBUF;

#if TEST == TEST_SPI
	if(flit[0] == 0xCA5F && flit[1] == 0xCA5F && flit[2] == 0xCA5F && flit[3] == 0xCA5F && 
		flit[4] == 0xCA5F && flit[5] == 0xCA5F && flit[6] == 0xCA5F && flit[7] == 0xCA5F && 
		flit[8] == 0xCA5F)
		globals.statistics.flit.numFlitsReceived[PORTC]++;
	else
		spiFailures[PORTC]++;
#else

	globals.statistics.flit.numFlitsReceived[PORTC]++;

	// Add the port to the left-most 4 bits of the first flit characer
	flit[0] += PORTC_SHIFTED;

	// Enque the flit if there is room and wake up the processing thread
	EnqueInboundFlit(flit);

#endif

#if PROFILE_INTERRUPTS == true
	executionTime = TimeDifference(startTime, timer0Registers.TIM.all);
	globals.statistics.profiling.avgInterruptsTime = 
		(globals.statistics.profiling.avgInterruptsTime * globals.statistics.profiling.numInterruptsSamples) + executionTime;
	globals.statistics.profiling.numInterruptsSamples++;
	globals.statistics.profiling.avgInterruptsTime /= globals.statistics.profiling.numInterruptsSamples;
	if(executionTime < globals.statistics.profiling.minInterruptsTime)
		globals.statistics.profiling.minInterruptsTime = executionTime;
	else if(executionTime > globals.statistics.profiling.maxInterruptsTime)
		globals.statistics.profiling.maxInterruptsTime = executionTime;
#endif

	// Acknowledge the interrupt and re-enable it
	pieRegisters.PIEACK.bit.ACK6 = 1;
	spicRegisters.SPIFFRX.bit.RXFFINTCLR = 1;
}

#pragma CODE_SECTION(PortDRXInterrupt,"FlashCode");
void PortDRXInterrupt()
{
	Uint16 i;
	Uint16 flit[NUM_CHARACTERS_IN_FLIT];
#if PROFILE_INTERRUPTS
	double executionTime;
	Uint32 startTime;
	startTime = timer0Registers.TIM.all;
#endif

	// Buffer the flit
	for(i = 0; i < NUM_CHARACTERS_IN_FLIT; i++)
		flit[i] = spidRegisters.SPIRXBUF;

#if TEST == TEST_SPI
	if(flit[0] == 0xCA5F && flit[1] == 0xCA5F && flit[2] == 0xCA5F && flit[3] == 0xCA5F && 
		flit[4] == 0xCA5F && flit[5] == 0xCA5F && flit[6] == 0xCA5F && flit[7] == 0xCA5F && 
		flit[8] == 0xCA5F)
		globals.statistics.flit.numFlitsReceived[PORTD]++;
	else
		spiFailures[PORTD]++;
#else

	globals.statistics.flit.numFlitsReceived[PORTD]++;

	// Add the port to the left-most 4 bits of the first flit characer
	flit[0] += PORTD_SHIFTED;

	// Enque the flit if there is room and wake up the processing thread
	EnqueInboundFlit(flit);

#endif

#if PROFILE_INTERRUPTS == true
	executionTime = TimeDifference(startTime, timer0Registers.TIM.all);
	globals.statistics.profiling.avgInterruptsTime = 
		(globals.statistics.profiling.avgInterruptsTime * globals.statistics.profiling.numInterruptsSamples) + executionTime;
	globals.statistics.profiling.numInterruptsSamples++;
	globals.statistics.profiling.avgInterruptsTime /= globals.statistics.profiling.numInterruptsSamples;
	if(executionTime < globals.statistics.profiling.minInterruptsTime)
		globals.statistics.profiling.minInterruptsTime = executionTime;
	else if(executionTime > globals.statistics.profiling.maxInterruptsTime)
		globals.statistics.profiling.maxInterruptsTime = executionTime;
#endif

	// Acknowledge the interrupt and re-enable it
	pieRegisters.PIEACK.bit.ACK6 = 1;
	spidRegisters.SPIFFRX.bit.RXFFINTCLR = 1;
}
