// This file implements various SPI helper functions

#include"System.h"

/* 
Important note: Due to problems with SPISTE and lack of foresight in designing the communication link, 
the arbitration scheme using MIN and MOUT has been replaced with a single arbitration signal, MARB.
MIN was designed to replace the STE signal, which as it turns out is necessary for proper operation of the SPI
on the F2808. This new mechanism works, but cannot gaurantee collision avoidance. When a collision does occur,
it means that two nodes tried to arbitrate at the exact time (plus or minus a few clock cycles), and were in addressless mode. 
Fortunately, when this happens neither node ever sees any of the transmission from the other, so there is no chance of 
recieving a partial or garbled message. Unfortunately, this will completely ruin a data transfer. This mechanism 
is not discussed in the documentation because it is not the way arbitration SHOULD be done, and if there were 
time to modify the boards to use the original arbitration scheme (the four GPIO pins would be repurposed), it would be done.
*/

#pragma CODE_SECTION(SetupPort,"FlashCode");
void SetupPort(int portNumber)
{
	volatile struct SPI_REGS* spiReg;

	// Allow the system to write to the SPI registers
	EALLOW;
	
	// Determine which port to initialize and perform non spiregs initialization
	switch(portNumber)
	{
		case PORTA:
			spiReg = &spiaRegisters;

			// Set  non spireg control bits
			sysCtrlRegisters.PCLKCR0.bit.SPIAENCLK = 1;		// Enable the SPI clock
			pieRegisters.PIEIER6.bit.INTx1 = 1;				// Enable the PIE rx interrupt

			// Set the I/O pin mode
			gpioCtrlRegisters.GPAMUX2.bit.SPISIMOA = 1;		// SPI mode, SIMO
			gpioCtrlRegisters.GPAMUX2.bit.MARBA = 0;		// GPIO mode, Master out
			gpioCtrlRegisters.GPAMUX2.bit.SPICLKA = 1;		// SPI mode, Clock
			gpioCtrlRegisters.GPAMUX2.bit.SPISTEA = 1;		// SPI mode, SPISTE

			// Set the I/O pin sampling
			gpioCtrlRegisters.GPAQSEL2.bit.SPISIMOA = 3;	// Asynchronous for SPI mode
		    gpioCtrlRegisters.GPAQSEL2.bit.MARBA = 0;		// Synchronous for GPIO mode
		    gpioCtrlRegisters.GPAQSEL2.bit.SPICLKA = 3;		// Asynchronous for SPI mode
		    gpioCtrlRegisters.GPAQSEL2.bit.SPISTEA = 3;		// Asynchronous for GPIO mode

			// Disable the pull-up resistors
			gpioCtrlRegisters.GPAPUD.bit.SPISIMOA = 1;
		    gpioCtrlRegisters.GPAPUD.bit.MARBA = 1;
		    gpioCtrlRegisters.GPAPUD.bit.SPICLKA = 1;
		    gpioCtrlRegisters.GPAPUD.bit.SPISTEA = 1;

			// Set the pin directions to slave mode
			gpioCtrlRegisters.GPADIR.bit.SPISIMOA = 0;
			gpioCtrlRegisters.GPADIR.bit.MARBA = 0;
			gpioCtrlRegisters.GPADIR.bit.SPICLKA = 0;
			gpioCtrlRegisters.GPADIR.bit.SPISTEA = 0;

			// Set the external SPISTEA pull up
			gpioCtrlRegisters.GPADIR.bit.SPISTE_PULLUP = 1;
			gpioDataRegisters.GPADAT.bit.SPISTE_PULLUP = 1;

			break;

		case PORTB:
			spiReg = &spibRegisters;

			// Set  non spireg control bits 
			sysCtrlRegisters.PCLKCR0.bit.SPIBENCLK = 1;		// Enable the SPI clock
			pieRegisters.PIEIER6.bit.INTx3 = 1;				// Enable the PIE interrupt

			// Set the I/O pin mode
			gpioCtrlRegisters.GPAMUX1.bit.SPISIMOB = 3;		// SPI mode, SIMO
			gpioCtrlRegisters.GPAMUX1.bit.MARBB = 0;		// GPIO mode, Master out
			gpioCtrlRegisters.GPAMUX1.bit.SPICLKB = 3;		// SPI mode, Clock
			gpioCtrlRegisters.GPAMUX1.bit.SPISTEB = 3;		// SPI mode, SPISTE

			// Set the I/O pin sampling
			gpioCtrlRegisters.GPAQSEL1.bit.SPISIMOB = 3;	// Asynchronous for SPI mode
		    gpioCtrlRegisters.GPAQSEL1.bit.MARBB = 0;		// Synchronous for GPIO mode
		    gpioCtrlRegisters.GPAQSEL1.bit.SPICLKB = 3;		// Asynchronous for SPI mode
		    gpioCtrlRegisters.GPAQSEL1.bit.SPISTEB = 3;		// Asynchronous for GPIO mode

			// Disable the pull-up resistors
			gpioCtrlRegisters.GPAPUD.bit.SPISIMOB = 1;
		    gpioCtrlRegisters.GPAPUD.bit.MARBB = 1;
		    gpioCtrlRegisters.GPAPUD.bit.SPICLKB = 1;
		    gpioCtrlRegisters.GPAPUD.bit.SPISTEB = 1;

			// Set the pin directions to slave mode
			gpioCtrlRegisters.GPADIR.bit.SPISIMOB = 0;
			gpioCtrlRegisters.GPADIR.bit.MARBB = 0;
			gpioCtrlRegisters.GPADIR.bit.SPICLKB = 0;
			gpioCtrlRegisters.GPADIR.bit.SPISTEB = 0;

			break;

		case PORTC:
			spiReg = &spicRegisters;

			// Set  non spireg control bits 
			sysCtrlRegisters.PCLKCR0.bit.SPICENCLK = 1;		// Enable the SPI clock
			pieRegisters.PIEIER6.bit.INTx5 = 1;				// Enable the PIE interrupt

			// Set the I/O pin mode
			gpioCtrlRegisters.GPAMUX2.bit.SPISIMOC = 2;		// SPI mode, SIMO
			gpioCtrlRegisters.GPAMUX2.bit.MARBC = 0;		// GPIO mode, Master out
			gpioCtrlRegisters.GPAMUX2.bit.SPICLKC = 2;		// SPI mode, Clock
			gpioCtrlRegisters.GPAMUX2.bit.SPISTEC = 2;		// SPI mode, SPISTE

			// Set the I/O pin sampling
			gpioCtrlRegisters.GPAQSEL2.bit.SPISIMOC = 3;	// Asynchronous for SPI mode
		    gpioCtrlRegisters.GPAQSEL2.bit.MARBC = 0;		// Synchronous for GPIO mode
		    gpioCtrlRegisters.GPAQSEL2.bit.SPICLKC = 3;		// Asynchronous for SPI mode
		    gpioCtrlRegisters.GPAQSEL2.bit.SPISTEC = 3;		// Asynchronous for GPIO mode

			// Disable the pull-up resistors
			gpioCtrlRegisters.GPAPUD.bit.SPISIMOC = 1;
		    gpioCtrlRegisters.GPAPUD.bit.MARBC = 1;
		    gpioCtrlRegisters.GPAPUD.bit.SPICLKC = 1;
		    gpioCtrlRegisters.GPAPUD.bit.SPISTEC = 1;

			// Set the pin directions to slave mode
			gpioCtrlRegisters.GPADIR.bit.SPISIMOC = 0;
			gpioCtrlRegisters.GPADIR.bit.MARBC = 0;
			gpioCtrlRegisters.GPADIR.bit.SPICLKC = 0;
			gpioCtrlRegisters.GPADIR.bit.SPISTEC = 0;

			break;

		case PORTD:
			spiReg = &spidRegisters;

			// Set  non spireg control bits 
			sysCtrlRegisters.PCLKCR0.bit.SPIDENCLK = 1;		// Enable the SPI clock
			pieRegisters.PIEIER6.bit.INTx7 = 1;				// Enable the PIE interrupt

			// Set the I/O pin mode
			gpioCtrlRegisters.GPAMUX1.bit.SPISIMOD = 2;		// SPI mode, SIMO
			gpioCtrlRegisters.GPAMUX1.bit.MARBD = 0;		// GPIO mode, Master out
			gpioCtrlRegisters.GPAMUX1.bit.SPICLKD = 2;		// SPI mode, Clock
			gpioCtrlRegisters.GPAMUX1.bit.SPISTED = 2;		// SPI mode, SPISTE

			// Set the I/O pin sampling
			gpioCtrlRegisters.GPAQSEL1.bit.SPISIMOD = 3;	// Asynchronous for SPI mode
		    gpioCtrlRegisters.GPAQSEL1.bit.MARBD = 0;		// Synchronous for GPIO mode
		    gpioCtrlRegisters.GPAQSEL1.bit.SPICLKD = 3;		// Asynchronous for SPI mode
		    gpioCtrlRegisters.GPAQSEL1.bit.SPISTED = 3;		// Asynchronous for GPIO mode

			// Disable the pull-up resistors
			gpioCtrlRegisters.GPAPUD.bit.SPISIMOD = 1;
		    gpioCtrlRegisters.GPAPUD.bit.MARBD = 1;
		    gpioCtrlRegisters.GPAPUD.bit.SPICLKD = 1;
		    gpioCtrlRegisters.GPAPUD.bit.SPISTED = 1;

			// Set the pin directions to slave mode
			gpioCtrlRegisters.GPADIR.bit.SPISIMOD = 0;
			gpioCtrlRegisters.GPADIR.bit.MARBD = 0;
			gpioCtrlRegisters.GPADIR.bit.SPICLKD = 0;
			gpioCtrlRegisters.GPADIR.bit.SPISTED = 0;
			break;

		default:
			LogError(__FILE__,__LINE__);
			return;
	}
	
	// Clear the reset so that the SPI settings can be changed safely
	spiReg->SPICCR.bit.SPISWRESET = 0;
	
	// Set up the configuration control  register
	spiReg->SPICCR.bit.CLKPOLARITY = 0;					// Input on the falling edge of the clock
	spiReg->SPICCR.bit.SPILBK = false;					// No loopback
	spiReg->SPICCR.bit.SPICHAR = CHARACTER_LENGTH;		// Set data length
		
	// Set up the operational control register
	spiReg->SPICTL.bit.SPIINTENA = true;				// Enable interrupt callback
	spiReg->SPICTL.bit.TALK = true;						// Enable transmission
	spiReg->SPICTL.bit.MASTER_SLAVE = 0;				// Set to slave mode
	spiReg->SPICTL.bit.CLK_PHASE = 0;					// No half clock cycle delay		
	spiReg->SPICTL.bit.OVERRUNINTENA = false;			// No interrupt on overrun

	// Set the baudrate
	spiReg->SPIBRR = 4;	// Set the baud rate to as fast as possible

	// Clear the status register
	spiReg->SPISTS.all=0x0000;
	
	// Configure the FIFOs
	spiReg->SPIFFTX.bit.SPIFFENA = 1;					// Enable the FIFOs
	spiReg->SPIFFRX.bit.RXFFIENA = 1;					// Enable the receive FIFO interrupt
	spiReg->SPIFFRX.bit.RXFFIL = NUM_CHARACTERS_IN_FLIT;// Set the receive to interrupt when 128 bits plus virtual channel length
														//  (one packet+virtual channel) are in the receive buffer
	spiReg->SPIFFTX.bit.SPIRST = 0;						// Reset the  FIFO

	// Start the device
	Delay(SPI_HOLD_IN_RESET_TIME);
	spiReg->SPIFFTX.bit.SPIRST = 1;						// Reset the  FIFO
	spiReg->SPICCR.bit.SPISWRESET = 1;
	
	// Disallow the system to write to the SPI registers
	EDIS;
}

#pragma CODE_SECTION(ProcessOutboundFlits,"FlashCode");
void ProcessOutboundFlits()
{
	Uint16 outboundState[NUM_PORTS] = {PORT_STATE_IDLE,PORT_STATE_IDLE,PORT_STATE_IDLE,PORT_STATE_IDLE};
	Uint16 disableInterrupts;
	Uint16 numAttempts[NUM_PORTS] = {0,0,0,0};
	Uint16 collisionTimeLeft[NUM_PORTS] = {0,0,0,0};
	Uint16 i;
	Uns interruptStatus;
#if PROFILE_TRANSMIT_TIME == true
	double executionTime;
	Uint32 startTime;
	Uint16 timeThisRun;
#endif

	// Loop forever
	while(true)
	{
		// Wait for more information to become available
		if(globals.statistics.que.currentOutboundQueLoad[PORTA] == 0 && globals.statistics.que.currentOutboundQueLoad[PORTB] == 0 && 
			globals.statistics.que.currentOutboundQueLoad[PORTC] == 0 && globals.statistics.que.currentOutboundQueLoad[PORTD] == 0)
		{
			SEM_pend(&ProcessOutboundFlitsSem,SYS_FOREVER);
		}
#if PROFILE_TRANSMIT_TIME == true
		timeThisRun = false;
		startTime = timer0Registers.TIM.all;
#endif


		/*********************************************************
		******			  Update Collision Info				******
		**********************************************************/
				
		/******** Port A ***********/

		// Update the collision information
		if(outboundState[PORTA] == PORT_STATE_COLLISION)
		{
			collisionTimeLeft[PORTA]--;
			if(collisionTimeLeft[PORTA] == 0)
			{
				outboundState[PORTA] = PORT_STATE_WAITING_ON_NEIGHBOR;
			}
		}

		/******** Port B ***********/

		// Update the collision information
		if(outboundState[PORTB] == PORT_STATE_COLLISION)
		{
			collisionTimeLeft[PORTB]--;
			if(collisionTimeLeft[PORTB] == 0)
			{
				outboundState[PORTB] = PORT_STATE_WAITING_ON_NEIGHBOR;
			}
		}

		/******** Port C ***********/

		// Update the collision information
		if(outboundState[PORTC] == PORT_STATE_COLLISION)
		{
			collisionTimeLeft[PORTC]--;
			if(collisionTimeLeft[PORTC] == 0)
			{
				outboundState[PORTC] = PORT_STATE_WAITING_ON_NEIGHBOR;
			}
		}

		/******** Port D ***********/

		// Update the collision information
		if(outboundState[PORTD] == PORT_STATE_COLLISION)
		{
			collisionTimeLeft[PORTD]--;
			if(collisionTimeLeft[PORTD] == 0)
			{
				outboundState[PORTD] = PORT_STATE_WAITING_ON_NEIGHBOR;
			}
		}


		/*********************************************************
		******				Check for new Flits				******
		**********************************************************/

		// Set the disable interrupts flag because we only want to disable interrupts if we are transmitting something
		disableInterrupts = false;
				
		/******** Port A ***********/

		// Check if there are any new flits to transmit
		if(outboundState[PORTA] == PORT_STATE_IDLE && globals.statistics.que.currentOutboundQueLoad[PORTA] > 0)
		{
			// Set the port as active, for use if the que's overflow and other parts of the system start messing with MARB
			globals.spi.portActive[PORTA] = true;
			disableInterrupts = true;

			numAttempts[PORTA] = 0;

			// Set the port state
			outboundState[PORTA] = PORT_STATE_WAITING_ON_NEIGHBOR;
		}

		/******** Port B ***********/

		// Check if there are any new flits to transmit
		if(outboundState[PORTB] == PORT_STATE_IDLE && globals.statistics.que.currentOutboundQueLoad[PORTB] > 0)
		{
			// Set the port as active, for use if the que's overflow and other parts of the system start messing with MARB
			globals.spi.portActive[PORTB] = true;
			disableInterrupts = true;

			numAttempts[PORTB] = 0;

			// Set the port state
			outboundState[PORTB] = PORT_STATE_WAITING_ON_NEIGHBOR;
		}

		/******** Port C ***********/

		// Check if there are any new flits to transmit
		if(outboundState[PORTC] == PORT_STATE_IDLE && globals.statistics.que.currentOutboundQueLoad[PORTC] > 0)
		{
			// Set the port as active, for use if the que's overflow and other parts of the system start messing with MARB
			globals.spi.portActive[PORTC] = true;
			disableInterrupts = true;

			numAttempts[PORTC] = 0;

			// Set the port state
			outboundState[PORTC] = PORT_STATE_WAITING_ON_NEIGHBOR;
		}

		/******** Port D ***********/

		// Check if there are any new flits to transmit
		if(outboundState[PORTD] == PORT_STATE_IDLE && globals.statistics.que.currentOutboundQueLoad[PORTD] > 0)
		{
			// Set the port as active, for use if the que's overflow and other parts of the system start messing with MARB
			globals.spi.portActive[PORTD] = true;
			disableInterrupts = true;

			numAttempts[PORTD] = 0;

			// Set the port state
			outboundState[PORTD] = PORT_STATE_WAITING_ON_NEIGHBOR;
		}


		/*********************************************************
		******				Begin Arbitration				******
		**********************************************************/	

		// Allow registers to be written to.
		EALLOW;

		// Disable interrupts to avoid timing issues
		if(disableInterrupts)
		{
			interruptStatus = HWI_disable();
		}
				
		/******** Port A ***********/

		// Begin arbitrating any ports that need it
		if(outboundState[PORTA] == PORT_STATE_WAITING_ON_NEIGHBOR)
		{
			// Check and see if the other end is transmitting, if so, mark the number of attempts
			if(gpioDataRegisters.GPADAT.bit.MARBA == 0)
			{
				// Assert the arbitration
				gpioCtrlRegisters.GPADIR.bit.MARBA = 1;
				gpioDataRegisters.GPASET.bit.MARBA = 1;

				outboundState[PORTA] = PORT_STATE_ARBITRATING;
			}
			else
			{
				numAttempts[PORTA]++;
				globals.statistics.flit.numBlockedCollisions[PORTA]++;
				if(numAttempts[PORTA] > MAX_ARBITRATION_ATTEMPTS)
				{
					outboundState[PORTA] = PORT_STATE_IDLE;
					globals.statistics.flit.numSpiArbitrationsFailed++;

					// Update the tail pointer
					globals.processing.outboundFlitQueTail[PORTA]++;
					if(globals.processing.outboundFlitQueTail[PORTA] == OUTBOUND_FLIT_BUFFER_SIZE)
						globals.processing.outboundFlitQueTail[PORTA] = 0;

					// Update the statistics
					globals.statistics.que.currentOutboundQueLoad[PORTA]--;

					// Set the que as not full
					globals.processing.outboundFlitQueFull[PORTA] = false;
				}
			}
		}

		/******** Port B ***********/

		// Begin arbitrating any ports that need it
		if(outboundState[PORTB] == PORT_STATE_WAITING_ON_NEIGHBOR)
		{
			// Check and see if the other end is transmitting, if so, mark the number of attempts
			if(gpioDataRegisters.GPADAT.bit.MARBB == 0)
			{
				// Assert the arbitration
				gpioCtrlRegisters.GPADIR.bit.MARBB = 1;
				gpioDataRegisters.GPASET.bit.MARBB = 1;

				outboundState[PORTB] = PORT_STATE_ARBITRATING;
			}
			else
			{
				numAttempts[PORTB]++;
				globals.statistics.flit.numBlockedCollisions[PORTB]++;
				if(numAttempts[PORTB] > MAX_ARBITRATION_ATTEMPTS)
				{
					outboundState[PORTB] = PORT_STATE_IDLE;
					globals.statistics.flit.numSpiArbitrationsFailed++;

					// Update the tail pointer
					globals.processing.outboundFlitQueTail[PORTB]++;
					if(globals.processing.outboundFlitQueTail[PORTB] == OUTBOUND_FLIT_BUFFER_SIZE)
						globals.processing.outboundFlitQueTail[PORTB] = 0;

					// Update the statistics
					globals.statistics.que.currentOutboundQueLoad[PORTB]--;

					// Set the que as not full
					globals.processing.outboundFlitQueFull[PORTB] = false;
				}
			}
		}

		/******** Port C ***********/

		// Begin arbitrating any ports that need it
		if(outboundState[PORTC] == PORT_STATE_WAITING_ON_NEIGHBOR)
		{
			// Check and see if the other end is transmitting, if so, mark the number of attempts
			if(gpioDataRegisters.GPADAT.bit.MARBC == 0)
			{
				// Assert the arbitration
				gpioCtrlRegisters.GPADIR.bit.MARBC = 1;
				gpioDataRegisters.GPASET.bit.MARBC = 1;

				outboundState[PORTC] = PORT_STATE_ARBITRATING;
			}
			else
			{
				numAttempts[PORTC]++;
				globals.statistics.flit.numBlockedCollisions[PORTC]++;
				if(numAttempts[PORTC] > MAX_ARBITRATION_ATTEMPTS)
				{
					outboundState[PORTC] = PORT_STATE_IDLE;
					globals.statistics.flit.numSpiArbitrationsFailed++;

					// Update the tail pointer
					globals.processing.outboundFlitQueTail[PORTC]++;
					if(globals.processing.outboundFlitQueTail[PORTC] == OUTBOUND_FLIT_BUFFER_SIZE)
						globals.processing.outboundFlitQueTail[PORTC] = 0;

					// Update the statistics
					globals.statistics.que.currentOutboundQueLoad[PORTC]--;

					// Set the que as not full
					globals.processing.outboundFlitQueFull[PORTC] = false;
				}
			}
		}

		/******** Port D ***********/

		// Begin arbitrating any ports that need it
		if(outboundState[PORTD] == PORT_STATE_WAITING_ON_NEIGHBOR)
		{
			// Check and see if the other end is transmitting, if so, mark the number of attempts
			if(gpioDataRegisters.GPADAT.bit.MARBD == 0)
			{
				// Assert the arbitration
				gpioCtrlRegisters.GPADIR.bit.MARBD = 1;
				gpioDataRegisters.GPASET.bit.MARBD = 1;

				outboundState[PORTD] = PORT_STATE_ARBITRATING;
			}
			else
			{
				numAttempts[PORTD]++;
				globals.statistics.flit.numBlockedCollisions[PORTD]++;
				if(numAttempts[PORTD] > MAX_ARBITRATION_ATTEMPTS)
				{
					outboundState[PORTD] = PORT_STATE_IDLE;
					globals.statistics.flit.numSpiArbitrationsFailed++;

					// Update the tail pointer
					globals.processing.outboundFlitQueTail[PORTD]++;
					if(globals.processing.outboundFlitQueTail[PORTD] == OUTBOUND_FLIT_BUFFER_SIZE)
						globals.processing.outboundFlitQueTail[PORTD] = 0;

					// Update the statistics
					globals.statistics.que.currentOutboundQueLoad[PORTD]--;

					// Set the que as not full
					globals.processing.outboundFlitQueFull[PORTD] = false;
				}
			}
		}


		/*********************************************************
		******			  Finalize Arbitration				******
		**********************************************************/	
#define SETTLE_TIME 10
#define WAIT_TIME 40
				
		/******** Port A ***********/

		/*startTransmissionTime = timer0Registers.TIM.all;
		if(startTransmissionTime < WAIT_TIME)
		{
			endTransmissionTime = 0xFFFFFFFF - (WAIT_TIME - startTransmissionTime);

			while(timer0Registers.TIM.all < startTransmissionTime || timer0Registers.TIM.all > endTransmissionTime);
		}
		else
		{
			endTransmissionTime = startTransmissionTime - WAIT_TIME;

			while(timer0Registers.TIM.all > endTransmissionTime);
		}*/

		if(outboundState[PORTA] == PORT_STATE_ARBITRATING)
		{
			gpioCtrlRegisters.GPADIR.bit.MARBA = 0;
		}

		/*startTransmissionTime = timer0Registers.TIM.all;
		if(startTransmissionTime < SETTLE_TIME)
		{
			endTransmissionTime = 0xFFFFFFFF - (SETTLE_TIME - startTransmissionTime);

			while(timer0Registers.TIM.all < startTransmissionTime || timer0Registers.TIM.all > endTransmissionTime);
		}
		else
		{
			endTransmissionTime = startTransmissionTime - SETTLE_TIME;

			while(timer0Registers.TIM.all > endTransmissionTime);
		}*/

		if(outboundState[PORTA] == PORT_STATE_ARBITRATING)
		{
			if(gpioDataRegisters.GPADAT.bit.MARBA == 0)
			{
				// Assert the arbitration
				gpioCtrlRegisters.GPADIR.bit.MARBA = 1;
				gpioDataRegisters.GPASET.bit.MARBA = 1;

				outboundState[PORTA] = PORT_STATE_READY_TO_TRANSMIT;
			}
			else
			{
				outboundState[PORTA] = PORT_STATE_COLLISION;
				collisionTimeLeft[PORTA] = PORTA + 1;
				globals.statistics.flit.numSpiCollisions[PORTA]++;

				numAttempts[PORTA]++;
				if(numAttempts[PORTA] > MAX_ARBITRATION_ATTEMPTS)
				{
					outboundState[PORTA] = PORT_STATE_IDLE;
					globals.statistics.flit.numSpiArbitrationsFailed++;

					// Update the tail pointer
					globals.processing.outboundFlitQueTail[PORTA]++;
					if(globals.processing.outboundFlitQueTail[PORTA] == OUTBOUND_FLIT_BUFFER_SIZE)
						globals.processing.outboundFlitQueTail[PORTA] = 0;

					// Update the statistics
					globals.statistics.que.currentOutboundQueLoad[PORTA]--;

					// Set the que as not full
					globals.processing.outboundFlitQueFull[PORTA] = false;
				}
			}
		}
				
		/******** Port C ***********/

		/*startTransmissionTime = timer0Registers.TIM.all;
		if(startTransmissionTime < WAIT_TIME)
		{
			endTransmissionTime = 0xFFFFFFFF - (WAIT_TIME - startTransmissionTime);

			while(timer0Registers.TIM.all < startTransmissionTime || timer0Registers.TIM.all > endTransmissionTime);
		}
		else
		{
			endTransmissionTime = startTransmissionTime - WAIT_TIME;

			while(timer0Registers.TIM.all > endTransmissionTime);
		}*/

		if(outboundState[PORTC] == PORT_STATE_ARBITRATING)
		{
			gpioCtrlRegisters.GPADIR.bit.MARBC = 0;
		}

		/*startTransmissionTime = timer0Registers.TIM.all;
		if(startTransmissionTime < SETTLE_TIME)
		{
			endTransmissionTime = 0xFFFFFFFF - (SETTLE_TIME - startTransmissionTime);

			while(timer0Registers.TIM.all < startTransmissionTime || timer0Registers.TIM.all > endTransmissionTime);
		}
		else
		{
			endTransmissionTime = startTransmissionTime - SETTLE_TIME;

			while(timer0Registers.TIM.all > endTransmissionTime);
		}*/

		if(outboundState[PORTC] == PORT_STATE_ARBITRATING)
		{
			if(gpioDataRegisters.GPADAT.bit.MARBC == 0)
			{
				// Assert the arbitration
				gpioCtrlRegisters.GPADIR.bit.MARBC = 1;
				gpioDataRegisters.GPASET.bit.MARBC = 1;

				outboundState[PORTC] = PORT_STATE_READY_TO_TRANSMIT;
			}
			else
			{
				outboundState[PORTC] = PORT_STATE_COLLISION;
				collisionTimeLeft[PORTC] = PORTC + 1;
				globals.statistics.flit.numSpiCollisions[PORTC]++;

				numAttempts[PORTC]++;
				if(numAttempts[PORTC] > MAX_ARBITRATION_ATTEMPTS)
				{
					outboundState[PORTC] = PORT_STATE_IDLE;
					globals.statistics.flit.numSpiArbitrationsFailed++;

					// Update the tail pointer
					globals.processing.outboundFlitQueTail[PORTC]++;
					if(globals.processing.outboundFlitQueTail[PORTC] == OUTBOUND_FLIT_BUFFER_SIZE)
						globals.processing.outboundFlitQueTail[PORTC] = 0;

					// Update the statistics
					globals.statistics.que.currentOutboundQueLoad[PORTC]--;

					// Set the que as not full
					globals.processing.outboundFlitQueFull[PORTC] = false;
				}
			}
		}
				
		/******** Port B ***********/

		/*startTransmissionTime = timer0Registers.TIM.all;
		if(startTransmissionTime < WAIT_TIME)
		{
			endTransmissionTime = 0xFFFFFFFF - (WAIT_TIME - startTransmissionTime);

			while(timer0Registers.TIM.all < startTransmissionTime || timer0Registers.TIM.all > endTransmissionTime);
		}
		else
		{
			endTransmissionTime = startTransmissionTime - WAIT_TIME;

			while(timer0Registers.TIM.all > endTransmissionTime);
		}*/

		if(outboundState[PORTB] == PORT_STATE_ARBITRATING)
		{
			gpioCtrlRegisters.GPADIR.bit.MARBB = 0;
		}

		/*startTransmissionTime = timer0Registers.TIM.all;
		if(startTransmissionTime < SETTLE_TIME)
		{
			endTransmissionTime = 0xFFFFFFFF - (SETTLE_TIME - startTransmissionTime);

			while(timer0Registers.TIM.all < startTransmissionTime || timer0Registers.TIM.all > endTransmissionTime);
		}
		else
		{
			endTransmissionTime = startTransmissionTime - SETTLE_TIME;

			while(timer0Registers.TIM.all > endTransmissionTime);
		}*/

		if(outboundState[PORTB] == PORT_STATE_ARBITRATING)
		{
			if(gpioDataRegisters.GPADAT.bit.MARBB == 0)
			{
				// Assert the arbitration
				gpioCtrlRegisters.GPADIR.bit.MARBB = 1;
				gpioDataRegisters.GPASET.bit.MARBB = 1;

				outboundState[PORTB] = PORT_STATE_READY_TO_TRANSMIT;
			}
			else
			{
				outboundState[PORTB] = PORT_STATE_COLLISION;
				collisionTimeLeft[PORTB] = PORTB + 1;
				globals.statistics.flit.numSpiCollisions[PORTB]++;

				numAttempts[PORTB]++;
				if(numAttempts[PORTB] > MAX_ARBITRATION_ATTEMPTS)
				{
					outboundState[PORTB] = PORT_STATE_IDLE;
					globals.statistics.flit.numSpiArbitrationsFailed++;

					// Update the tail pointer
					globals.processing.outboundFlitQueTail[PORTB]++;
					if(globals.processing.outboundFlitQueTail[PORTB] == OUTBOUND_FLIT_BUFFER_SIZE)
						globals.processing.outboundFlitQueTail[PORTB] = 0;

					// Update the statistics
					globals.statistics.que.currentOutboundQueLoad[PORTB]--;

					// Set the que as not full
					globals.processing.outboundFlitQueFull[PORTB] = false;
				}
			}
		}
				
		/******** Port D ***********/

		/*startTransmissionTime = timer0Registers.TIM.all;
		if(startTransmissionTime < WAIT_TIME)
		{
			endTransmissionTime = 0xFFFFFFFF - (WAIT_TIME - startTransmissionTime);

			while(timer0Registers.TIM.all < startTransmissionTime || timer0Registers.TIM.all > endTransmissionTime);
		}
		else
		{
			endTransmissionTime = startTransmissionTime - WAIT_TIME;

			while(timer0Registers.TIM.all > endTransmissionTime);
		}*/

		if(outboundState[PORTD] == PORT_STATE_ARBITRATING)
		{
			gpioCtrlRegisters.GPADIR.bit.MARBD = 0;
		}

		/*startTransmissionTime = timer0Registers.TIM.all;
		if(startTransmissionTime < SETTLE_TIME)
		{
			endTransmissionTime = 0xFFFFFFFF - (SETTLE_TIME - startTransmissionTime);

			while(timer0Registers.TIM.all < startTransmissionTime || timer0Registers.TIM.all > endTransmissionTime);
		}
		else
		{
			endTransmissionTime = startTransmissionTime - SETTLE_TIME;

			while(timer0Registers.TIM.all > endTransmissionTime);
		}*/

		if(outboundState[PORTD] == PORT_STATE_ARBITRATING)
		{
			if(gpioDataRegisters.GPADAT.bit.MARBD == 0)
			{
				// Assert the arbitration
				gpioCtrlRegisters.GPADIR.bit.MARBD = 1;
				gpioDataRegisters.GPASET.bit.MARBD = 1;

				outboundState[PORTD] = PORT_STATE_READY_TO_TRANSMIT;
			}
			else
			{
				outboundState[PORTD] = PORT_STATE_COLLISION;
				collisionTimeLeft[PORTD] = PORTD + 2;
				globals.statistics.flit.numSpiCollisions[PORTD]++;

				numAttempts[PORTD]++;
				if(numAttempts[PORTD] > MAX_ARBITRATION_ATTEMPTS)
				{
					outboundState[PORTD] = PORT_STATE_IDLE;
					globals.statistics.flit.numSpiArbitrationsFailed++;

					// Update the tail pointer
					globals.processing.outboundFlitQueTail[PORTD]++;
					if(globals.processing.outboundFlitQueTail[PORTD] == OUTBOUND_FLIT_BUFFER_SIZE)
						globals.processing.outboundFlitQueTail[PORTD] = 0;

					// Update the statistics
					globals.statistics.que.currentOutboundQueLoad[PORTD]--;

					// Set the que as not full
					globals.processing.outboundFlitQueFull[PORTD] = false;
				}
			}
		}


		/*********************************************************
		******			  	Transmit Flits					******
		**********************************************************/

		/******** Port A ***********/
		
		// Transmit any flits that are ready
		if(outboundState[PORTA] == PORT_STATE_READY_TO_TRANSMIT)
		{
			globals.statistics.flit.successfullyTransmittedOnPort[PORTA] = true;
			globals.statistics.flit.numFlitsSent[PORTA]++;
			
			// Enable transmission as a master
			spiaRegisters.SPICCR.bit.SPISWRESET = 0;
			spiaRegisters.SPIFFTX.bit.SPIRST = 0;
			spiaRegisters.SPICTL.bit.SPIINTENA = false;
			spiaRegisters.SPIFFRX.bit.RXFFIENA = false;
			spiaRegisters.SPICTL.bit.MASTER_SLAVE = 1;
			gpioCtrlRegisters.GPADIR.bit.SPISIMOA = 1;
			gpioCtrlRegisters.GPADIR.bit.SPICLKA = 1;
			gpioCtrlRegisters.GPADIR.bit.SPISTEA = 1;
			spiaRegisters.SPIFFTX.bit.SPIRST = 1;
			spiaRegisters.SPICCR.bit.SPISWRESET = 1;

			// Transmit the flits
			for(i = 0; i < NUM_CHARACTERS_IN_FLIT; i++)
				spiaRegisters.SPITXBUF = globals.processing.outboundFlitQue[PORTA][globals.processing.outboundFlitQueTail[PORTA]][i];

			// Update the tail pointer
			globals.processing.outboundFlitQueTail[PORTA]++;
			if(globals.processing.outboundFlitQueTail[PORTA] == OUTBOUND_FLIT_BUFFER_SIZE)
				globals.processing.outboundFlitQueTail[PORTA] = 0;

			// Update the statistics
			globals.statistics.que.currentOutboundQueLoad[PORTA]--;

			// Set the que as not full
			globals.processing.outboundFlitQueFull[PORTA] = false;
			outboundState[PORTA] = PORT_STATE_TRANSMITTED;
		}

		/******** Port B ***********/
		
		// Transmit any flits that are ready
		if(outboundState[PORTB] == PORT_STATE_READY_TO_TRANSMIT)
		{
			globals.statistics.flit.successfullyTransmittedOnPort[PORTB] = true;
			globals.statistics.flit.numFlitsSent[PORTB]++;
			
			// Enable transmission as a master
			spibRegisters.SPICCR.bit.SPISWRESET = 0;
			spibRegisters.SPIFFTX.bit.SPIRST = 0;
			spibRegisters.SPICTL.bit.SPIINTENA = false;
			spibRegisters.SPIFFRX.bit.RXFFIENA = false;
			spibRegisters.SPICTL.bit.MASTER_SLAVE = 1;
			gpioCtrlRegisters.GPADIR.bit.SPISIMOB = 1;
			gpioCtrlRegisters.GPADIR.bit.SPICLKB = 1;
			gpioCtrlRegisters.GPADIR.bit.SPISTEB = 1;
			spibRegisters.SPIFFTX.bit.SPIRST = 1;
			spibRegisters.SPICCR.bit.SPISWRESET = 1;

			// Transmit the flits
			for(i = 0; i < NUM_CHARACTERS_IN_FLIT; i++)
				spibRegisters.SPITXBUF = globals.processing.outboundFlitQue[PORTB][globals.processing.outboundFlitQueTail[PORTB]][i];

			// Update the tail pointer
			globals.processing.outboundFlitQueTail[PORTB]++;
			if(globals.processing.outboundFlitQueTail[PORTB] == OUTBOUND_FLIT_BUFFER_SIZE)
				globals.processing.outboundFlitQueTail[PORTB] = 0;

			// Update the statistics
			globals.statistics.que.currentOutboundQueLoad[PORTB]--;

			// Set the que as not full
			globals.processing.outboundFlitQueFull[PORTB] = false;
			outboundState[PORTB] = PORT_STATE_TRANSMITTED;
		}

		/******** Port C ***********/
		
		// Transmit any flits that are ready
		if(outboundState[PORTC] == PORT_STATE_READY_TO_TRANSMIT)
		{
			globals.statistics.flit.successfullyTransmittedOnPort[PORTC] = true;
			globals.statistics.flit.numFlitsSent[PORTC]++;
			
			// Enable transmission as a master
			spicRegisters.SPICCR.bit.SPISWRESET = 0;
			spicRegisters.SPIFFTX.bit.SPIRST = 0;
			spicRegisters.SPICTL.bit.SPIINTENA = false;
			spicRegisters.SPIFFRX.bit.RXFFIENA = false;
			spicRegisters.SPICTL.bit.MASTER_SLAVE = 1;
			gpioCtrlRegisters.GPADIR.bit.SPISIMOC = 1;
			gpioCtrlRegisters.GPADIR.bit.SPICLKC = 1;
			gpioCtrlRegisters.GPADIR.bit.SPISTEC = 1;
			spicRegisters.SPIFFTX.bit.SPIRST = 1;
			spicRegisters.SPICCR.bit.SPISWRESET = 1;

			// Transmit the flits
			for(i = 0; i < NUM_CHARACTERS_IN_FLIT; i++)
				spicRegisters.SPITXBUF = globals.processing.outboundFlitQue[PORTC][globals.processing.outboundFlitQueTail[PORTC]][i];

			// Update the tail pointer
			globals.processing.outboundFlitQueTail[PORTC]++;
			if(globals.processing.outboundFlitQueTail[PORTC] == OUTBOUND_FLIT_BUFFER_SIZE)
				globals.processing.outboundFlitQueTail[PORTC] = 0;

			// Update the statistics
			globals.statistics.que.currentOutboundQueLoad[PORTC]--;

			// Set the que as not full
			globals.processing.outboundFlitQueFull[PORTC] = false;
			outboundState[PORTC] = PORT_STATE_TRANSMITTED;
		}

		/******** Port D ***********/
		
		// Transmit any flits that are ready
		if(outboundState[PORTD] == PORT_STATE_READY_TO_TRANSMIT)
		{
			globals.statistics.flit.successfullyTransmittedOnPort[PORTD] = true;
			globals.statistics.flit.numFlitsSent[PORTD]++;
			
			// Enable transmission as a master
			spidRegisters.SPICCR.bit.SPISWRESET = 0;
			spidRegisters.SPIFFTX.bit.SPIRST = 0;
			spidRegisters.SPICTL.bit.SPIINTENA = false;
			spidRegisters.SPIFFRX.bit.RXFFIENA = false;
			spidRegisters.SPICTL.bit.MASTER_SLAVE = 1;
			gpioCtrlRegisters.GPADIR.bit.SPISIMOD = 1;
			gpioCtrlRegisters.GPADIR.bit.SPICLKD = 1;
			gpioCtrlRegisters.GPADIR.bit.SPISTED = 1;
			spidRegisters.SPIFFTX.bit.SPIRST = 1;
			spidRegisters.SPICCR.bit.SPISWRESET = 1;

			// Transmit the flits
			for(i = 0; i < NUM_CHARACTERS_IN_FLIT; i++)
				spidRegisters.SPITXBUF = globals.processing.outboundFlitQue[PORTD][globals.processing.outboundFlitQueTail[PORTD]][i];

			// Update the tail pointer
			globals.processing.outboundFlitQueTail[PORTD]++;
			if(globals.processing.outboundFlitQueTail[PORTD] == OUTBOUND_FLIT_BUFFER_SIZE)
				globals.processing.outboundFlitQueTail[PORTD] = 0;

			// Update the statistics
			globals.statistics.que.currentOutboundQueLoad[PORTD]--;

			// Set the que as not full
			globals.processing.outboundFlitQueFull[PORTD] = false;
			outboundState[PORTD] = PORT_STATE_TRANSMITTED;
		}

		// Wait for the transmission(s) to finish
		/*
		startTransmissionTime = timer0Registers.TIM.all;
		if(startTransmissionTime < 100)
		{
			endTransmissionTime = 0xFFFFFFFF - (100 - startTransmissionTime);

			while(timer0Registers.TIM.all < startTransmissionTime || timer0Registers.TIM.all > endTransmissionTime);
		}
		else
		{
			endTransmissionTime = startTransmissionTime - 100;

			while(timer0Registers.TIM.all > endTransmissionTime);
		}*/

		
		/*********************************************************
		******			  	Demote Ports					******
		**********************************************************/

		/******** Port A ***********/

		if(outboundState[PORTA] == PORT_STATE_TRANSMITTED)
		{
#if PROFILE_TRANSMIT_TIME == true
			timeThisRun = true;
#endif
			// Set the port as a slave
			spiaRegisters.SPICCR.bit.SPISWRESET = 0;
			spiaRegisters.SPIFFTX.bit.SPIRST = 0;
			spiaRegisters.SPICTL.bit.MASTER_SLAVE = 0;
			gpioCtrlRegisters.GPADIR.bit.SPISIMOA = 0;
			gpioCtrlRegisters.GPADIR.bit.SPICLKA = 0;
			gpioCtrlRegisters.GPADIR.bit.SPISTEA = 0;
			spiaRegisters.SPIFFRX.bit.RXFFIENA = true;
			spiaRegisters.SPICTL.bit.SPIINTENA = true;
			spiaRegisters.SPIFFTX.bit.SPIRST = 1;
			spiaRegisters.SPICCR.bit.SPISWRESET = 1;

			// Set the port as inactive
			globals.spi.portActive[PORTA] = false;

			// Set the outbound port state
			outboundState[PORTA] = PORT_STATE_IDLE;
			gpioCtrlRegisters.GPADIR.bit.MARBA = 0;
		}

		/******** Port B ***********/

		if(outboundState[PORTB] == PORT_STATE_TRANSMITTED)
		{
#if PROFILE_TRANSMIT_TIME == true
			timeThisRun = true;
#endif
			// Set the port as a slave
			spibRegisters.SPICCR.bit.SPISWRESET = 0;
			spibRegisters.SPIFFTX.bit.SPIRST = 0;
			spibRegisters.SPICTL.bit.MASTER_SLAVE = 0;
			gpioCtrlRegisters.GPADIR.bit.SPISIMOB = 0;
			gpioCtrlRegisters.GPADIR.bit.SPICLKB = 0;
			gpioCtrlRegisters.GPADIR.bit.SPISTEB = 0;
			spibRegisters.SPIFFRX.bit.RXFFIENA = true;
			spibRegisters.SPICTL.bit.SPIINTENA = true;
			spibRegisters.SPIFFTX.bit.SPIRST = 1;
			spibRegisters.SPICCR.bit.SPISWRESET = 1;

			// Set the port as inactive
			globals.spi.portActive[PORTB] = false;

			// Set the outbound port state
			outboundState[PORTB] = PORT_STATE_IDLE;
			gpioCtrlRegisters.GPADIR.bit.MARBB = 0;
		}

		/******** Port C ***********/

		if(outboundState[PORTC] == PORT_STATE_TRANSMITTED)
		{
#if PROFILE_TRANSMIT_TIME == true
			timeThisRun = true;
#endif
			// Set the port as a slave
			spicRegisters.SPICCR.bit.SPISWRESET = 0;
			spicRegisters.SPIFFTX.bit.SPIRST = 0;
			spicRegisters.SPICTL.bit.MASTER_SLAVE = 0;
			gpioCtrlRegisters.GPADIR.bit.SPISIMOC = 0;
			gpioCtrlRegisters.GPADIR.bit.SPICLKC = 0;
			gpioCtrlRegisters.GPADIR.bit.SPISTEC = 0;
			spicRegisters.SPIFFRX.bit.RXFFIENA = true;
			spicRegisters.SPICTL.bit.SPIINTENA = true;
			spicRegisters.SPIFFTX.bit.SPIRST = 1;
			spicRegisters.SPICCR.bit.SPISWRESET = 1;

			// Set the port as inactive
			globals.spi.portActive[PORTC] = false;

			// Set the outbound port state
			outboundState[PORTC] = PORT_STATE_IDLE;
			gpioCtrlRegisters.GPADIR.bit.MARBC = 0;
		}

		/******** Port D ***********/

		if(outboundState[PORTD] == PORT_STATE_TRANSMITTED)
		{
#if PROFILE_TRANSMIT_TIME == true
			timeThisRun = true;
#endif
			// Set the port as a slave
			spidRegisters.SPICCR.bit.SPISWRESET = 0;
			spidRegisters.SPIFFTX.bit.SPIRST = 0;
			spidRegisters.SPICTL.bit.MASTER_SLAVE = 0;
			gpioCtrlRegisters.GPADIR.bit.SPISIMOD = 0;
			gpioCtrlRegisters.GPADIR.bit.SPICLKD = 0;
			gpioCtrlRegisters.GPADIR.bit.SPISTED = 0;
			spidRegisters.SPIFFRX.bit.RXFFIENA = true;
			spidRegisters.SPICTL.bit.SPIINTENA = true;
			spidRegisters.SPIFFTX.bit.SPIRST = 1;
			spidRegisters.SPICCR.bit.SPISWRESET = 1;

			// Set the port as inactive
			globals.spi.portActive[PORTD] = false;

			// Set the outbound port state
			outboundState[PORTD] = PORT_STATE_IDLE;
			gpioCtrlRegisters.GPADIR.bit.MARBD = 0;
		}

		// Disallow writing to registers
		EDIS;

		// Disable interrupts to avoid timing issues
		if(disableInterrupts)
		{
			HWI_restore(interruptStatus);
		}

#if PROFILE_TRANSMIT_TIME == true
		if(timeThisRun)
		{
			executionTime = TimeDifference(startTime, timer0Registers.TIM.all);
			globals.statistics.profiling.avgTxTime = (globals.statistics.profiling.avgTxTime * globals.statistics.profiling.numTxSamples) + executionTime;
			globals.statistics.profiling.numTxSamples++;
			globals.statistics.profiling.avgTxTime /= globals.statistics.profiling.numTxSamples;
			if(executionTime < globals.statistics.profiling.minTxTime)
				globals.statistics.profiling.minTxTime = executionTime;
			else if(executionTime > globals.statistics.profiling.maxTxTime)
				globals.statistics.profiling.maxTxTime = executionTime;
		}
#endif
	}
}
