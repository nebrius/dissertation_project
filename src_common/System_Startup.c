// This is the control task. it runs before all other tasks and performs system
// initialization. It also performs system-level monitoring.

#include"System.h"

extern unsigned int trcdata_loadstart; 
extern unsigned int trcdata_loadend; 
extern unsigned int trcdata_runstart; 

#pragma CODE_SECTION(SystemInitialization,"FlashCode");
void SystemInitialization()
{
	/*** Initialize the .trcdata section before main() ***/ 
      memcpy(&trcdata_runstart, &trcdata_loadstart, &trcdata_loadend - &trcdata_loadstart); 
}

#pragma CODE_SECTION(main,"FlashCode");
void main(Arg Loops_arg)
{
	// Copy initialized RAM structures from flash
	EALLOW;
	memcpy(&hwi_vec_runstart, &hwi_vec_loadstart, &hwi_vec_loadend - &hwi_vec_loadstart);
	EDIS;
	
	// Initialize the global variables structure
	InitializeGlobalVariables();

	// Create the system initialization follow-up item
	AddFollowUpItem(&InitializeSystem,NULL,1,false);
}
