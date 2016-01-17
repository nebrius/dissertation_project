#include"System.h"

extern Uint16 dataReceived;
double minPathSelection = 10000, maxPathSelection = 0;

#pragma CODE_SECTION(FollowUpMonitorWakeup,"FlashCode");
void FollowUpMonitorWakeup()
{
	globals.statistics.system.ticksPassed++;
	if(SEM_count(&FollowUpMonitorSem) == 0)
		SEM_post(&FollowUpMonitorSem);
}

#pragma CODE_SECTION(HeartbeatLED,"FlashCode");
void HeartbeatLED()
{
	// Toggle the heartbeat LED
	gpioDataRegisters.GPBTOGGLE.bit.HEARTBEAT = 1;
}

#pragma CODE_SECTION(FollowUpMonitor,"FlashCode");
void FollowUpMonitor()
{
	int i;

	while(true)
	{
		// Check on each follow up item
		for(i = 0; i < NUM_FOLLOW_UP_ITEMS; i++)
		{
			// If the item is active, take care of it
			if(globals.followUpMonitor.followUpItems[i].active == true)
			{
				// Decrement the counter
				globals.followUpMonitor.followUpItems[i].followUpCounter--;

				// If the counter has zeroed out, call the callback function
				if(globals.followUpMonitor.followUpItems[i].followUpCounter == 0)
				{
					// Call the callback function
					(*globals.followUpMonitor.followUpItems[i].callbackFunction)(globals.followUpMonitor.followUpItems[i].callbackFunctionParameters,i);

					// If the item doesn't expire, reset its counter, otherwise deactivate it so it can be used later
					if(globals.followUpMonitor.followUpItems[i].doesNotExpire)
						globals.followUpMonitor.followUpItems[i].followUpCounter = globals.followUpMonitor.followUpItems[i].timeToFollowUp;
					else
						globals.followUpMonitor.followUpItems[i].active = false;
				}
			}
		}
		SEM_pend(&FollowUpMonitorSem,SYS_FOREVER);
	}
}


#pragma CODE_SECTION(AddFollowUpItem,"FlashCode");
Uint16 AddFollowUpItem(void (*callbackFunction)(void*, Uint16), void* callbackFunctionParameters, Uint16 timeToFollowUp, Uint16 doesNotExpire)
{
	Uint16 i;

	// Loop through each item, looking for an available follow-up item
	for(i = 0; i < NUM_FOLLOW_UP_ITEMS; i++)
	{
		// Check if this follow up item is available
		if(globals.followUpMonitor.followUpItems[i].active == false)
		{
			// Set the follow up item parameters
			globals.followUpMonitor.followUpItems[i].active = true;
			globals.followUpMonitor.followUpItems[i].callbackFunction = callbackFunction;
			globals.followUpMonitor.followUpItems[i].callbackFunctionParameters = callbackFunctionParameters;
			globals.followUpMonitor.followUpItems[i].doesNotExpire = doesNotExpire;
			globals.followUpMonitor.followUpItems[i].timeToFollowUp = timeToFollowUp;
			globals.followUpMonitor.followUpItems[i].followUpCounter = timeToFollowUp;
			return i;
		}
	}

	// Return an error, meaning there are no more available follow up item slots
	LogError(__FILE__,__LINE__);
	return ERROR;
}

#pragma CODE_SECTION(RemoveFollowUpItem,"FlashCode");
Uint16 RemoveFollowUpItem(void (*callbackFunction)(void*, Uint16), void* callbackFunctionParameters)
{
	Uint16 i;

	// Sanity check
	if(callbackFunction == NULL)
	{
		LogError(__FILE__,__LINE__);
		return ERROR;
	}

	// Loop through each item, looking for the follow-up item
	for(i = 0; i < NUM_FOLLOW_UP_ITEMS; i++)
	{
		// Check if this follow up item is the one we are looking for
		if(globals.followUpMonitor.followUpItems[i].callbackFunction == callbackFunction &&
			globals.followUpMonitor.followUpItems[i].callbackFunctionParameters == callbackFunctionParameters)
		{
			globals.followUpMonitor.followUpItems[i].active = false;
			return SUCCESS;
		}
	}

	LogError(__FILE__,__LINE__);
	return ERROR;
}

#pragma CODE_SECTION(InitializeSystem,"FlashCode");
void InitializeSystem(void* parameter, Uint16 followUpItemIndex)
{

	Uns interruptStatus;
	Uint16 i, j;
	double executionTime = 0;
	double genAvgTime = 0;
	double genMinTime = DBL_MAX;
	double genMaxTime = DBL_MIN;
	double avgTime[9] = {0,0,0,0,0,0,0,0,0};
	double minTime[9] = {DBL_MAX,DBL_MAX,DBL_MAX,DBL_MAX,DBL_MAX,DBL_MAX,DBL_MAX,DBL_MAX,DBL_MAX};
	double maxTime[9] = {DBL_MIN,DBL_MIN,DBL_MIN,DBL_MIN,DBL_MIN,DBL_MIN,DBL_MIN,DBL_MIN,DBL_MIN};
	Uint32 startTime = 0;
	Uint32 numSamples;
	struct Packet newPacket;

	// Allow the system to write to the system registers
	EALLOW;

	// Set up the heartbeat
	gpioCtrlRegisters.GPBDIR.bit.HEARTBEAT = 1;
	gpioDataRegisters.GPBDAT.bit.HEARTBEAT = 1;

	// Set up the seven segment display
	gpioCtrlRegisters.GPADIR.bit.SEVENSEG_A = 1;
	gpioDataRegisters.GPADAT.bit.SEVENSEG_A = 0;
	gpioCtrlRegisters.GPADIR.bit.SEVENSEG_B = 1;
	gpioDataRegisters.GPADAT.bit.SEVENSEG_B = 0;
	gpioCtrlRegisters.GPADIR.bit.SEVENSEG_C = 1;
	gpioDataRegisters.GPADAT.bit.SEVENSEG_C = 0;
	gpioCtrlRegisters.GPADIR.bit.SEVENSEG_D = 1;
	gpioDataRegisters.GPADAT.bit.SEVENSEG_D = 0;
	gpioCtrlRegisters.GPADIR.bit.SEVENSEG_E = 1;
	gpioDataRegisters.GPADAT.bit.SEVENSEG_E = 0;
	gpioCtrlRegisters.GPADIR.bit.SEVENSEG_F = 1;
	gpioDataRegisters.GPADAT.bit.SEVENSEG_F = 0;
	gpioCtrlRegisters.GPADIR.bit.SEVENSEG_G = 1;
	gpioDataRegisters.GPADAT.bit.SEVENSEG_G = 0;
	gpioCtrlRegisters.GPADIR.bit.SEVENSEG_DIGIT = 1;
	gpioDataRegisters.GPADAT.bit.SEVENSEG_DIGIT = 1;
	
	// Set up timer 0 for use with SPI related timings
	sysCtrlRegisters.HISPCP.all = 0;				// Set the HSPCLK to run at SYSCLK
	sysCtrlRegisters.LOSPCP.bit.LSPCLK = 0; 		// Set the LSPCLK to run at SYSCLK
	timer0Registers.TCR.bit.TSS = 1;				// Stop the timer
	timer0Registers.TPR.bit.TDDR = 0;				// Do not prescale the timer
	timer0Registers.PRD.all = 0xFFFFFFFF;			// Set the period register to it's largest possible value
	timer0Registers.TCR.bit.TIE = false;			// Disable timer interrupts
	timer0Registers.TCR.bit.FREE = 0;				// Set the timer to stop when a breakpoint occrs
	timer0Registers.TCR.bit.SOFT = 0;
	timer0Registers.TCR.bit.TRB = 1;				// Load the period register
	timer0Registers.TCR.bit.TSS = 0;				// Start the timer
	
	// Set up the global SPI-related settings
	IER |= 0x20;									// Enable CPU INT6
	
	// Disallow the system to write to the system registers
	EDIS;

	interruptStatus = HWI_disable();
	numSamples = 0;
	for(j = 0; j < 1000; j++)
	{
		
		InitializeGlobalVariables();

		globals.root.availableAddresses[0].address = 1;
		globals.root.availableAddresses[0].addressTaken = true;
		globals.root.availableAddresses[1].address = 2;
		globals.root.availableAddresses[1].addressTaken = true;
		globals.root.availableAddresses[2].address = 3;
		globals.root.availableAddresses[2].addressTaken = true;
		globals.root.availableAddresses[3].address = 4;
		globals.root.availableAddresses[3].addressTaken = true;
		globals.root.availableAddresses[4].address = 5;
		globals.root.availableAddresses[4].addressTaken = true;
		globals.root.availableAddresses[5].address = 6;
		globals.root.availableAddresses[5].addressTaken = true;
		globals.root.availableAddresses[6].address = 7;
		globals.root.availableAddresses[6].addressTaken = true;
		globals.root.availableAddresses[7].address = 8;
		globals.root.availableAddresses[7].addressTaken = true;	

		globals.protocol.globalNeighborInfo[1][PORTA].nodeAddress = 2;
		globals.protocol.globalNeighborInfo[1][PORTB].nodeAddress = 3;

		globals.protocol.globalNeighborInfo[2][PORTA].nodeAddress = 1;
		globals.protocol.globalNeighborInfo[2][PORTB].nodeAddress = 4;

		globals.protocol.globalNeighborInfo[3][PORTA].nodeAddress = 1;
		globals.protocol.globalNeighborInfo[3][PORTB].nodeAddress = 4;
		globals.protocol.globalNeighborInfo[3][PORTC].nodeAddress = 5;

		globals.protocol.globalNeighborInfo[4][PORTA].nodeAddress = 2;
		globals.protocol.globalNeighborInfo[4][PORTB].nodeAddress = 3;
		globals.protocol.globalNeighborInfo[4][PORTC].nodeAddress = 6;

		globals.protocol.globalNeighborInfo[5][PORTA].nodeAddress = 3;
		globals.protocol.globalNeighborInfo[5][PORTB].nodeAddress = 6;
		globals.protocol.globalNeighborInfo[5][PORTC].nodeAddress = 7;

		globals.protocol.globalNeighborInfo[6][PORTA].nodeAddress = 4;
		globals.protocol.globalNeighborInfo[6][PORTB].nodeAddress = 5;
		globals.protocol.globalNeighborInfo[6][PORTC].nodeAddress = 8;

		globals.protocol.globalNeighborInfo[7][PORTA].nodeAddress = 5;
		globals.protocol.globalNeighborInfo[7][PORTB].nodeAddress = 8;

		globals.protocol.globalNeighborInfo[8][PORTA].nodeAddress = 6;
		globals.protocol.globalNeighborInfo[8][PORTB].nodeAddress = 7;

		startTime = timer0Registers.TIM.all;
		GenerateRoutingTree();
		executionTime = TimeDifference(startTime, timer0Registers.TIM.all);
		genAvgTime = (genAvgTime * numSamples) + executionTime;
		numSamples++;
		genAvgTime /= numSamples;
		if(executionTime < genMinTime)
			genMinTime = executionTime;
		else if(executionTime > genMaxTime)
			genMaxTime = executionTime;
	}

	for(i = 2; i <= 8; i++)
	{
		numSamples = 0;
		for(j = 0; j < 1000; j++)
		{
			startTime = timer0Registers.TIM.all;
			GenerateRoutingPath(1, i, &newPacket);
			executionTime = TimeDifference(startTime, timer0Registers.TIM.all);
			avgTime[i] = (avgTime[i] * numSamples) + executionTime;
			numSamples++;
			avgTime[i] /= numSamples;
			if(executionTime < minTime[i])
				minTime[i] = executionTime;
			else if(executionTime > maxTime[i])
				maxTime[i] = executionTime;
		}
	}
	HWI_restore(interruptStatus);
	asm(" NOP");

	// Set the seed for the random number generator
	SetRandomSeed(0x175E);

	// Set up each individual port
	SetupPort(PORTA);
	SetupPort(PORTB);
	SetupPort(PORTC);
	SetupPort(PORTD);

	// Create the direct data cleanup follow-up item
	AddFollowUpItem(&CleanupDirectData,NULL,DIRECT_DATA_CLEANUP_RATE,true);

	// Create the neighbor check follow-up item
#if defined(IS_ROOT)

#if TEST != TEST_SPI
	globals.followUpMonitor.neighborFollowUpIndex = AddFollowUpItem(&NeighborFollowUp,NULL,NEIGHBOR_FOLLOW_UP_RATE,true);
#endif
	SetSevenSegmentDisplay(SEVENSEG_BLANK);
	globals.processing.sevenSegmentLowerDigit = globals.protocol.address;

#elif defined (IS_ROUTER)

#if TEST != TEST_SPI
	globals.followUpMonitor.neighborFollowUpIndex = AddFollowUpItem(&NeighborFollowUp,NULL,NEIGHBOR_FOLLOW_UP_RATE_WITHOUT_ADDRESS,true);
#endif
	SetSevenSegmentDisplay(SEVENSEG_BLANK);

	// Register the test service
	RegisterServiceProvider(TEST_SERVICE_TAG,TEST_SERVICE_TASK_PRIORITY,&TestServiceTask,&TestServiceSem);

#endif

	

	// Let the other threads know that initialization is finished
	SEM_post(&ProcessInboundFlitsSem);
	SEM_post(&TestServiceSem);
}
