MEMORY 
{
	PAGE 1: PIE: 		origin = 0x0CE0, len = 0x20
	PAGE 1: SYSCTRL: 	origin = 0x7010, len = 0x20
	PAGE 1: TIMER0: 	origin = 0x0C00, len = 0x40
	PAGE 1: GPIOCTRL: 	origin = 0x6F80, len = 0x40
	PAGE 1: GPIODATA: 	origin = 0x6FC0, len = 0x20
	PAGE 1: GPIOINT: 	origin = 0x6FE0, len = 0x20
	PAGE 1: SPIA: 		origin = 0x7040, len = 0x10
	PAGE 1: SPIB: 		origin = 0x7740, len = 0x10
	PAGE 1: SPIC: 		origin = 0x7760, len = 0x10
	PAGE 1: SPID: 		origin = 0x7780, len = 0x10
}

SECTIONS
{
	Codestart         		> FLASH_BOOT	PAGE 0
	PieRegisterFile			> PIE 			PAGE 1
	SysCtrlRegisterFile 	> SYSCTRL 		PAGE 1
	Timer0RegisterFile 		> TIMER0 		PAGE 1
	GpioCtrlRegisterFile 	> GPIOCTRL 		PAGE 1
	GpioDataRegisterFile 	> GPIODATA 		PAGE 1
	GpioIntRegisterFile 	> GPIOINT		PAGE 1
	SpiaRegisterFile 		> SPIA 			PAGE 1
	SpibRegisterFile 		> SPIB 			PAGE 1
	SpicRegisterFile 		> SPIC 			PAGE 1
	SpidRegisterFile 		> SPID 			PAGE 1
	GlobalRegistersFile		> H0SARAM		PAGE 0
	CRCTableFile			> FLASH			PAGE 0
	MemoryManagerBuffer		> H0SARAM		PAGE 0
	FlashCode				> FLASH			PAGE 0
	RamCode					> LSARAM		PAGE 1
	MpiData					> LSARAM		PAGE 1
}