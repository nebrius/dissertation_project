#include"System.h"

#ifdef __cplusplus
#pragma DATA_SECTION("PieRegisterFile")
#else
#pragma DATA_SECTION(pieRegisters,"PieRegisterFile");
#endif
volatile struct PIERegisters pieRegisters;

#ifdef __cplusplus
#pragma DATA_SECTION("SysCtrlRegisterFile")
#else
#pragma DATA_SECTION(sysCtrlRegisters,"SysCtrlRegisterFile");
#endif
volatile struct SYSCtrlRegisters sysCtrlRegisters;

#ifdef __cplusplus
#pragma DATA_SECTION("Timer0RegisterFile")
#else
#pragma DATA_SECTION(timer0Registers,"Timer0RegisterFile");
#endif
volatile struct CPUTIMER_REGS timer0Registers;

#ifdef __cplusplus
#pragma DATA_SECTION("GpioCtrlRegisterFile")
#else
#pragma DATA_SECTION(gpioCtrlRegisters,"GpioCtrlRegisterFile");
#endif
volatile struct GPIO_CTRL_REGS gpioCtrlRegisters;

#ifdef __cplusplus
#pragma DATA_SECTION("GpioDataRegisterFile")
#else
#pragma DATA_SECTION(gpioDataRegisters,"GpioDataRegisterFile");
#endif
volatile struct GPIO_DATA_REGS gpioDataRegisters;

#ifdef __cplusplus
#pragma DATA_SECTION("GpioIntRegisterFile")
#else
#pragma DATA_SECTION(gpioIntRegisters,"GpioIntRegisterFile");
#endif
volatile struct GPIO_INT_REGS gpioIntRegisters;

#ifdef __cplusplus
#pragma DATA_SECTION("SpiaRegisterFile")
#else
#pragma DATA_SECTION(spiaRegisters,"SpiaRegisterFile");
#endif
volatile struct SPI_REGS spiaRegisters;

#ifdef __cplusplus
#pragma DATA_SECTION("SpibRegisterFile")
#else
#pragma DATA_SECTION(spibRegisters,"SpibRegisterFile");
#endif
volatile struct SPI_REGS spibRegisters;

#ifdef __cplusplus
#pragma DATA_SECTION("SpicRegisterFile")
#else
#pragma DATA_SECTION(spicRegisters,"SpicRegisterFile");
#endif
volatile struct SPI_REGS spicRegisters;

#ifdef __cplusplus
#pragma DATA_SECTION("SpidRegisterFile")
#else
#pragma DATA_SECTION(spidRegisters,"SpidRegisterFile");
#endif
volatile struct SPI_REGS spidRegisters;
