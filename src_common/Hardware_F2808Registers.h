#ifndef _F2808_REGISTERS_H
#define _F2808_REGISTERS_H

extern cregister volatile unsigned int IFR;
extern cregister volatile unsigned int IER;

#define  EINT   asm(" clrc INTM")
#define  DINT   asm(" setc INTM")
#define  ERTM   asm(" clrc DBGM")
#define  DRTM   asm(" setc DBGM")
#define  EALLOW asm(" EALLOW")
#define  EDIS   asm(" EDIS")
#define  ESTOP0 asm(" ESTOP0")

extern unsigned int hwi_vec_loadstart; 
extern unsigned int hwi_vec_loadend; 
extern unsigned int hwi_vec_runstart;
extern unsigned int trcdata_loadstart; 
extern unsigned int trcdata_loadend; 
extern unsigned int trcdata_runstart;

//---------------------------------------------------------------------------
// SPI Register Definitions
//---------------------------------------------------------------------------
struct  SPI_REGS 
{ 
   	// SPI configuration register bit definitions:
	union
	{
		Uint16 all;
		struct
		{        					// bits   description
		   Uint16 SPICHAR:4;        // 3:0    Character length control
		   Uint16 SPILBK:1;         // 4      Loop-back enable/disable
		   Uint16 rsvd1:1;          // 5      reserved
		   Uint16 CLKPOLARITY:1;    // 6      Clock polarity
		   Uint16 SPISWRESET:1;     // 7      SPI SW Reset
		   Uint16 rsvd2:8;          // 15:8   reserved   
		} bit;
	} SPICCR;

	// SPI operation control register bit definitions:
	union
	{
		Uint16 all;
		struct
		{        					// bits   description
		   Uint16 SPIINTENA:1;      // 0      Interrupt enable
		   Uint16 TALK:1;           // 1      Master/Slave transmit enable
		   Uint16 MASTER_SLAVE:1;   // 2      Network control mode
		   Uint16 CLK_PHASE:1;      // 3      Clock phase select
		   Uint16 OVERRUNINTENA:1;  // 4      Overrun interrupt enable
		   Uint16 rsvd:11;          // 15:5   reserved      
		} bit;
	} SPICTL;

	// SPI status register bit definitions:
	union
	{
		Uint16 all;
		struct
		{        					// bits   description
		   Uint16 rsvd1:5;          // 4:0    reserved
		   Uint16 BUFFULL_FLAG:1;   // 5      SPI transmit buffer full flag
		   Uint16 INT_FLAG:1;       // 6      SPI interrupt flag
		   Uint16 OVERRUN_FLAG:1;   // 7      SPI receiver overrun flag
		   Uint16 rsvd2:8;          // 15:8   reserved   
		} bit;
	} SPISTS;

	Uint16 rsvd1;      				// reserved
	Uint16 SPIBRR;      			// Baud Rate
	Uint16 rsvd2;       			// reserved
	Uint16 SPIRXEMU;    			// Emulation buffer
	Uint16 SPIRXBUF;    			// Serial input buffer
	Uint16 SPITXBUF;    			// Serial output buffer
	Uint16 SPIDAT;      			// Serial data

   	// SPI FIFO Transmit register
	union
	{
		Uint16 all;
		struct
		{       					// bit    description
		   Uint16 TXFFIL:5;         // 4:0    Interrupt level
		   Uint16 TXFFIENA:1;       // 5      Interrupt enable
		   Uint16 TXFFINTCLR:1;     // 6      Clear INT flag
		   Uint16 TXFFINT:1;        // 7      INT flag
		   Uint16 TXFFST:5;         // 12:8   FIFO status
		   Uint16 TXFIFO:1;         // 13     FIFO reset
		   Uint16 SPIFFENA:1;       // 14     Enhancement enable
		   Uint16 SPIRST:1;         // 15     Reset SPI
		} bit;
	} SPIFFTX;

   	// SPI FIFO receive register bit definitions:
	union
	{
		Uint16 all;
		struct 
		{       					// bits   description
		   Uint16 RXFFIL:5;         // 4:0    Interrupt level
		   Uint16 RXFFIENA:1;       // 5      Interrupt enable
		   Uint16 RXFFINTCLR:1;     // 6      Clear INT flag
		   Uint16 RXFFINT:1;        // 7      INT flag
		   Uint16 RXFFST:5;         // 12:8   FIFO status
		   Uint16 RXFIFORESET:1;    // 13     FIFO reset
		   Uint16 RXFFOVFCLR:1;     // 14     Clear overflow
		   Uint16 RXFFOVF:1;        // 15     FIFO overflow
		} bit;
	} SPIFFRX;

   	// SPI FIFO control register bit definitions:
	union
	{
		Uint16 all;
		struct
		{       					// bits   description
		   Uint16 TXDLY:8;          // 7:0    FIFO transmit delay
		   Uint16 rsvd:8;           // 15:8   reserved
		} bit;
	} SPIFFCT;

   Uint16               rsvd3[2];    // reserved

   	// SPI priority control register bit definitions:
	union
	{
		Uint16 all;
		struct  SPIPRI_BITS 
		{        					// bits   description
		   Uint16 rsvd1:4;          // 3:0    reserved
		   Uint16 FREE:1;           // 4      Free emulation mode control
		   Uint16 SOFT:1;           // 5      Soft emulation mode control
		   Uint16 PRIORITY:1;       // 6      Interrupt priority select
		   Uint16 rsvd2:9;          // 15:7   reserved
		} bit;
	} SPIPRI;
};

//---------------------------------------------------------------------------
// GPIO Register Definitions
//---------------------------------------------------------------------------

union GPADAT_REG
{
	Uint32 all;
	struct
	{          					// bits   description
	   Uint16 SEVENSEG_C:1;     // 0      SEVENSEG_C (GPIO0, 1) 
	   Uint16 SPISIMOD:1;       // 1      SPISIMOD (GPIO1, 5)
	   Uint16 SEVENSEG_E:1;     // 2      SEVENSEG_E (GPIO2, 3) 
	   Uint16 MARBD:1;          // 3      MARBD (GPIO3)
	   Uint16 SEVENSEG_D:1;     // 4      SEVENSEG_D (GPIO4)
	   Uint16 SPICLKD:1;        // 5      SPICLKD (GPIO5)
	   Uint16 SEVENSEG_DIGIT:1; // 6      SEVENSEG_DIGIT (GPIO6)   
	   Uint16 SPISTED:1;        // 7      SPISTED (GPIO7)
	   Uint16 SEVENSEG_F:1;     // 8      SEVENSEG_F (GPIO8, 6) 
	   Uint16 GPIO9:1;          // 9      GPIO9    
	   Uint16 SEVENSEG_B:1;     // 10     SEVENSEG_B (GPIO10, 8)
	   Uint16 GPIO11:1;         // 11     GPIO11       
	   Uint16 SPISIMOB:1;       // 12     SPISIMOB (GPIO12)
	   Uint16 MARBB:1;          // 13     MARBB (GPIO13)
	   Uint16 SPICLKB:1;        // 14     SPICLKB (GPIO14)
	   Uint16 SPISTEB:1;        // 15     SPISTEB (GPIO15)
	   Uint16 SPISIMOA:1;       // 16     SPISIMOA (GPIO16)
	   Uint16 MARBA:1;          // 17     MARBA (GPIO17)
	   Uint16 SPICLKA:1;        // 18     SPICLKA (GPIO18)
	   Uint16 SPISTEA:1;        // 19     SPISTEA (GPIO19)
	   Uint16 SPISIMOC:1;       // 20     SPISIMOC (GPIO20)
	   Uint16 MARBC:1;          // 21     MARBC (GPIO21)
	   Uint16 SPICLKC:1;        // 22     SPICLKC (GPIO22)
	   Uint16 SPISTEC:1;        // 23     SPISTEC (GPIO23)
	   Uint16 SEVENSEG_G:1;     // 24     SEVENSEG_G (GPIO24, 7)
	   Uint16 SEVENSEG_A:1;     // 25     SEVENSEG_A (GPIO25, 10)    
	   Uint16 GPIO26:1;         // 26     GPIO26 
	   Uint16 SPISTE_PULLUP:1;  // 27     SPISTE_PULLUP (GPIO27)       
	   Uint16 GPIO28:1;         // 28     GPIO28 
	   Uint16 GPIO29:1;         // 29     GPIO29 
	   Uint16 SWITCH4:1;        // 30     SWITCH4 (GPIO30)
	   Uint16 SWITCH3:1;        // 31     SWITCH3 (GPIO31)
	} bit;
};

union GPBDAT_REG
{
	Uint32 all;
	struct
	{       					// bits   description
	   Uint16 SWITCH2:1;        // 0      SWITCH2 (GPIO32)
	   Uint16 SWITCH1:1;        // 1      SWITCH1 (GPIO33)
	   Uint16 HEARTBEAT:1;      // 2      HEARTBEAT (GPIO34)
	   Uint16 rsvd1:1;          // 3      reserved   
	   Uint16 rsvd2:12;         // 15:4   reserved
	   Uint16 rsvd3:16;         // 31:16  reserved
	} bit;
};

union GPA1_REG
{
	Uint32 all;
	struct
	{            				// bits   description
	   Uint16 SEVENSEG_C:2;     // 1:0     SEVENSEG_C (GPIO0)
	   Uint16 SPISIMOD:2;       // 3:2    SPISIMOD (GPIO1)
	   Uint16 SEVENSEG_E:2;     // 5:4    SEVENSEG_E (GPIO2)
	   Uint16 MARBD:2;          // 7:6    MARBD (GPIO3)
	   Uint16 SEVENSEG_D:2;     // 9:8    SEVENSEG_D (GPIO4)
	   Uint16 SPICLKD:2;        // 11:10  SPICLKD (GPIO5)
	   Uint16 SEVENSEG_DIGIT:2; // 13:12  SEVENSEG_DIGIT (GPIO6)
	   Uint16 SPISTED:2;        // 15:14  SPISTED (GPIO7)
	   Uint16 SEVENSEG_F:2;     // 17:16  SEVENSEG_F (GPIO8)
	   Uint16 GPIO9:2;          // 19:18  GPIO9    
	   Uint16 SEVENSEG_B:2;     // 21:20  SEVENSEG_B (GPIO10)
	   Uint16 GPIO11:2;         // 23:22  GPIO11       
	   Uint16 SPISIMOB:2;       // 25:24  SPISIMOB (GPIO12)
	   Uint16 MARBB:2;          // 27:26  MARBB (GPIO13)
	   Uint16 SPICLKB:2;        // 29:28  SPICLKB (GPIO14)
	   Uint16 SPISTEB:2;        // 31:30  SPISTEB (GPIO15)
	} bit;
}; 

union GPA2_REG 
{
	Uint32 all;
	struct
	{            				// bits   description
	   Uint16 SPISIMOA:2;       // 1:0    SPISIMOA (GPIO16)
	   Uint16 MARBA:2;          // 3:2    MARBA (GPIO17)
	   Uint16 SPICLKA:2;        // 5:4    SPICLKA (GPIO18)
	   Uint16 SPISTEA:2;        // 7:6    SPISTEA (GPIO19)
	   Uint16 SPISIMOC:2;       // 9:8    SPISIMOC (GPIO20)
	   Uint16 MARBC:2;          // 11:10  MARBC (GPIO21)
	   Uint16 SPICLKC:2;        // 13:12  SPICLKC (GPIO22)
	   Uint16 SPISTEC:2;        // 15:14  SPISTEC (GPIO23)
	   Uint16 SEVENSEG_G:2;     // 17:16  SEVENSEG_G (GPIO24)
	   Uint16 SEVENSEG_A:2;     // 19:18  SEVENSEG_A (GPIO25)
	   Uint16 GPIO26:2;         // 21:20  GPIO26 
	   Uint16 SPISTE_PULLUP:2;         // 23:22  SPISTE_PULLUP       
	   Uint16 GPIO28:2;         // 25:24  GPIO28 
	   Uint16 GPIO29:2;         // 27:26  GPIO29 
	   Uint16 SWITCH4:2;        // 29:28  SWITCH4 (GPIO30)
	   Uint16 SWITCH3:2;        // 31:30  SWITCH3 (GPIO31)
	} bit;
};

union GPB1_REG
{
	Uint32 all;
	struct
	{            				// bits   description
	   Uint16 SWITCH2:2;        // 1:0    SWITCH2 (GPIO32)
	   Uint16 SWITCH1:2;        // 3:2    SWITCH1 (GPIO33)
	   Uint16 HEARTBEAT:2;      // 5:4    HEARTBEAT (GPIO34)
	   Uint16 rsvd1:2;          // 7:6    reserved  
	   Uint16 rsvd2:8;          // 15:8   reserved
	   Uint16 rsvd3:16;         // 31:16  reserved
	} bit;
};

union GPB2_REG
{
	Uint32 all;
	struct
	{           				// bits   description
	   Uint16 rsvd1:16;         // 15:0   reserved
	   Uint16 rsvd2:16;         // 31:16  reserved
	} bit;
};
                           
union GPIOXINT_REG 
{
	Uint16 all;
	struct 
	{        					// bits   description
	    Uint16 GPIOSEL:5;       // 4:0    Select GPIO interrupt input source
	    Uint16 rsvd1:11;        // 15:5   reserved
	} bit;
};


struct GPIO_CTRL_REGS 
{
	union
	{
		Uint32 all;
		struct GPACTRL_BITS 
		{        					// bits   description
		   Uint16 QUALPRD0:8;       // 7:0    Qual period 
		   Uint16 QUALPRD1:8;       // 15:8   Qual period 
		   Uint16 QUALPRD2:8;       // 23:16  Qual period 
		   Uint16 QUALPRD3:8;  	   	// 31:24  Qual period  
		} bit;
	} GPACTRL;
	union GPA1_REG 			GPAQSEL1;  // GPIO A Qualifier Select 1 Register (GPIO0 to 15)
	union GPA2_REG 			GPAQSEL2;  // GPIO A Qualifier Select 2 Register (GPIO16 to 31)
	union GPA1_REG 			GPAMUX1;   // GPIO A Mux 1 Register (GPIO0 to 15)
	union GPA2_REG 			GPAMUX2;   // GPIO A Mux 2 Register (GPIO16 to 31)
	union GPADAT_REG 		GPADIR;    // GPIO A Direction Register (GPIO0 to 31)
	union GPADAT_REG 		GPAPUD;    // GPIO A Pull Up Disable Register (GPIO0 to 31)
	Uint32 rsvd1;

	union
	{
		Uint32 all;
		struct
		{        					// bits   description
		   Uint16 QUALPRD0:8;       // 7:0    Qual period
		   Uint16 rsvd1:8;          // 15:8   reserved 
		   Uint16 rsvd2:16;         // 31:16  reserved   
		} bit;
	} GPBCTRL;
	union GPB1_REG 			GPBQSEL1;  // GPIO B Qualifier Select 1 Register (GPIO32 to 47)
	union GPB2_REG 			GPBQSEL2;  // GPIO B Qualifier Select 2 Register (GPIO48 to 63)
	union GPB1_REG 			GPBMUX1;   // GPIO B Mux 1 Register (GPIO32 to 47)
	union GPB2_REG 			GPBMUX2;   // GPIO B Mux 2 Register (GPIO48 to 63)
	union GPBDAT_REG 		GPBDIR;    // GPIO B Direction Register (GPIO32 to 63)
	union GPBDAT_REG 		GPBPUD;    // GPIO B Pull Up Disable Register (GPIO32 to 63)
	Uint16  rsvd2[33];
};

struct GPIO_DATA_REGS 
{
   union  GPADAT_REG       GPADAT;       // GPIO Data Register (GPIO0 to 31)
   union  GPADAT_REG       GPASET;       // GPIO Data Set Register (GPIO0 to 31)
   union  GPADAT_REG       GPACLEAR;     // GPIO Data Clear Register (GPIO0 to 31)
   union  GPADAT_REG       GPATOGGLE;    // GPIO Data Toggle Register (GPIO0 to 31) 
   union  GPBDAT_REG       GPBDAT;       // GPIO Data Register (GPIO32 to 63)
   union  GPBDAT_REG       GPBSET;       // GPIO Data Set Register (GPIO32 to 63)
   union  GPBDAT_REG       GPBCLEAR;     // GPIO Data Clear Register (GPIO32 to 63)
   union  GPBDAT_REG       GPBTOGGLE;    // GPIO Data Toggle Register (GPIO32 to 63)
   Uint16                  rsvd1[16];
};

struct GPIO_INT_REGS 
{
   union  GPIOXINT_REG     GPIOXINT1SEL; // XINT1 GPIO Input Selection
   union  GPIOXINT_REG     GPIOXINT2SEL; // XINT2 GPIO Input Selection
   union  GPIOXINT_REG     GPIOXNMISEL;  // XNMI_XINT13 GPIO Input Selection
   Uint16                  rsvd2[5];
   union  GPADAT_REG       GPIOLPMSEL;   // Low power modes GP I/O input select
};

//---------------------------------------------------------------------------
// PIE Register Definitions
//---------------------------------------------------------------------------

// PIEIER: Register bit definitions:
union PIEIER_REG 
{
	Uint16 all;
	struct
	{       					// bits description
	   Uint16 INTx1:1;          // 0    INTx.1
	   Uint16 INTx2:1;          // 1    INTx.2
	   Uint16 INTx3:1;          // 2    INTx.3
	   Uint16 INTx4:1;          // 3    INTx.4
	   Uint16 INTx5:1;          // 4    INTx.5
	   Uint16 INTx6:1;          // 5    INTx.6
	   Uint16 INTx7:1;          // 6    INTx.7
	   Uint16 INTx8:1;          // 7    INTx.8
	   Uint16 rsvd:8;           // 15:8 reserved
	} bit;
}; 

// PIEIFR: Register bit definitions:
union PIEIFR_REG 
{
	Uint16 all;
	struct
	{       					// bits description
	   Uint16 INTx1:1;          // 0    INTx.1
	   Uint16 INTx2:1;          // 1    INTx.2
	   Uint16 INTx3:1;          // 2    INTx.3
	   Uint16 INTx4:1;          // 3    INTx.4
	   Uint16 INTx5:1;          // 4    INTx.5
	   Uint16 INTx6:1;          // 5    INTx.6
	   Uint16 INTx7:1;          // 6    INTx.7
	   Uint16 INTx8:1;          // 7    INTx.8
	   Uint16 rsvd:8;           // 15:8 reserved
	} bit;
};

// PIE Control Register File:
struct PIERegisters 
{
   union
	{
		Uint16 all;
		struct
		{      						// bits description
		   Uint16  ENPIE:1;         // 0    Enable PIE block
		   Uint16  PIEVECT:15;      // 15:1 Fetched vector address
		} bit;
	} PIECTRL;						// PIE control register

	union
	{
		Uint16 all;
		struct
		{       					// bits description
		   Uint16 ACK1:1;           // 0    Acknowledge PIE interrupt group 1
		   Uint16 ACK2:1;           // 1    Acknowledge PIE interrupt group 2
		   Uint16 ACK3:1;           // 2    Acknowledge PIE interrupt group 3
		   Uint16 ACK4:1;           // 3    Acknowledge PIE interrupt group 4
		   Uint16 ACK5:1;           // 4    Acknowledge PIE interrupt group 5
		   Uint16 ACK6:1;           // 5    Acknowledge PIE interrupt group 6
		   Uint16 ACK7:1;           // 6    Acknowledge PIE interrupt group 7
		   Uint16 ACK8:1;           // 7    Acknowledge PIE interrupt group 8
		   Uint16 ACK9:1;           // 8    Acknowledge PIE interrupt group 9
		   Uint16 ACK10:1;          // 9    Acknowledge PIE interrupt group 10
		   Uint16 ACK11:1;          // 10   Acknowledge PIE interrupt group 11
		   Uint16 ACK12:1;          // 11   Acknowledge PIE interrupt group 12
		   Uint16 rsvd:4;           // 15:12 reserved
		} bit;
	} PIEACK;       				// PIE acknowledge

	union PIEIER_REG  PIEIER1;      // PIE INT1 IER register  
	union PIEIFR_REG  PIEIFR1;      // PIE INT1 IFR register
	union PIEIER_REG  PIEIER2;      // PIE INT2 IER register 
	union PIEIFR_REG  PIEIFR2;      // PIE INT2 IFR register
	union PIEIER_REG  PIEIER3;      // PIE INT3 IER register 
	union PIEIFR_REG  PIEIFR3;      // PIE INT3 IFR register
	union PIEIER_REG  PIEIER4;      // PIE INT4 IER register             
	union PIEIFR_REG  PIEIFR4;      // PIE INT4 IFR register
	union PIEIER_REG  PIEIER5;      // PIE INT5 IER register  
	union PIEIFR_REG  PIEIFR5;      // PIE INT5 IFR register
	union PIEIER_REG  PIEIER6;      // PIE INT6 IER register 
	union PIEIFR_REG  PIEIFR6;      // PIE INT6 IFR register
	union PIEIER_REG  PIEIER7;      // PIE INT7 IER register 
	union PIEIFR_REG  PIEIFR7;      // PIE INT7 IFR register
	union PIEIER_REG  PIEIER8;      // PIE INT8 IER register
	union PIEIFR_REG  PIEIFR8;      // PIE INT8 IFR register
	union PIEIER_REG  PIEIER9;      // PIE INT9 IER register  
	union PIEIFR_REG  PIEIFR9;      // PIE INT9 IFR register
	union PIEIER_REG  PIEIER10;     // PIE INT10 IER register 
	union PIEIFR_REG  PIEIFR10;     // PIE INT10 IFR register
	union PIEIER_REG  PIEIER11;     // PIE INT11 IER register 
	union PIEIFR_REG  PIEIFR11;     // PIE INT11 IFR register
	union PIEIER_REG  PIEIER12;     // PIE INT12 IER register
	union PIEIFR_REG  PIEIFR12;     // PIE INT12 IFR register
};    

//---------------------------------------------------------------------------
// System Control Individual Register Bit Definitions:
//---------------------------------------------------------------------------
struct SYSCtrlRegisters 
{
	union
	{
		Uint16 all;
		struct
		{    						// bits  description
		   Uint16 XCLKOUTDIV:2; 	// 1:0   XCLKOUT Divide Ratio
		   Uint16 XCLKOUTDAT:1; 	// 2     Status of XCLKOUT
		   Uint16 X1DAT:1;      	// 3     Output of the oscillator cell
		   Uint16 XCLKINDAT:1;  	// 4     Status of the XCLKIN pin
		   Uint16 rsvd1:3;      	// 7:5   reserved
		   Uint16 X1CNT:4;      	// 11:8  4-bit counter
		   Uint16 XCLKINCNT:4;  	// 15:12 4-bit counter
		} bit;
	} XCLK;

	union
	{
		Uint16 all;
		struct
		{    						// bits  description
		   Uint16 PLLLOCKS:1;     	// 0     PLL lock status
		   Uint16 CLKINDIV:1;     	// 1     CLKIN div by 2
		   Uint16 PLLOFF:1;     	// 2     PLL off bit
		   Uint16 MCLKSTS:1;      	// 3     Missing clock status bit
		   Uint16 MCLKCLR:1;      	// 4     Missing clock clear bit
		   Uint16 OSCOFF:1;       	// 5     Oscillator clock off
		   Uint16 MCLKOFF:1;      	// 6     Missing clock detect
		   Uint16 rsvd2:9;        	// 15:7  reserved
		} bit;
	} PLLSTS;    // 1

	Uint16              rsvd1[8];  // 2-9

	union
	{
		Uint16 all;
		struct
		{							// bits  description
		   Uint16 HSPCLK:3;     	// 2:0   Rate relative to SYSCLKOUT
		   Uint16 rsvd1:13;     	// 15:3  reserved
		} bit;
	} HISPCP;    // 10: High-speed peripheral clock pre-scaler

	union
	{
		Uint16 all;
		struct
		{   						// bits  description
		   Uint16 LSPCLK:3;     	// 2:0   Rate relative to SYSCLKOUT
		   Uint16 rsvd1:13;     	// 15:3  reserved
		} bit;
	} LOSPCP;    // 11: Low-speed peripheral clock pre-scaler

	union
	{
		Uint16 all;
		struct
		{  							// bits  description
		   Uint16 rsvd1:2;      	// 1:0   reserved
		   Uint16 TBCLKSYNC:1;  	// 2     EWPM Module TBCLK enable/sync 
		   Uint16 ADCENCLK:1;   	// 3     Enable high speed clk to ADC
		   Uint16 I2CAENCLK:1;  	// 4     Enable SYSCLKOUT to I2C-A
		   Uint16 rsvd2:1;      	// 5     reserved
		   Uint16 SPICENCLK:1;  	// 6     Enable low speed clk to SPI-C
		   Uint16 SPIDENCLK:1;  	// 7     Enable low speed clk to SPI-D
		   Uint16 SPIAENCLK:1;  	// 8     Enable low speed clk to SPI-A
		   Uint16 SPIBENCLK:1;  	// 9     Enable low speed clk to SPI-B   
		   Uint16 SCIAENCLK:1;  	// 10    Enable low speed clk to SCI-A
		   Uint16 SCIBENCLK:1;  	// 11    Enable low speed clk to SCI-B
		   Uint16 rsvd3:2;      	// 13:12 reserved
		   Uint16 ECANAENCLK:1; 	// 14    Enable system clk to eCAN-A
		   Uint16 ECANBENCLK:1; 	// 15    Enable system clk to eCAN-B
		} bit;
	} PCLKCR0;   // 12: Peripheral clock control register

	union
	{
		Uint16 all;
		struct
		{    						// bits  description
		   Uint16 EPWM1ENCLK:1;   	// 0     Enable SYSCLKOUT to EPWM1
		   Uint16 EPWM2ENCLK:1;   	// 1     Enable SYSCLKOUT to EPWM2
		   Uint16 EPWM3ENCLK:1;   	// 2     Enable SYSCLKOUT to EPWM3
		   Uint16 EPWM4ENCLK:1;   	// 3     Enable SYSCLKOUT to EPWM4
		   Uint16 EPWM5ENCLK:1;   	// 4     Enable SYSCLKOUT to EPWM5
		   Uint16 EPWM6ENCLK:1;   	// 5     Enable SYSCLKOUT to EPWM6               
		   Uint16 rsvd1:2;        	// 7:6   reserved
		   Uint16 ECAP1ENCLK:1;   	// 8     Enable SYSCLKOUT to ECAP1   
		   Uint16 ECAP2ENCLK:1;   	// 9     Enable SYSCLKOUT to ECAP2   
		   Uint16 ECAP3ENCLK:1;   	// 10    Enable SYSCLKOUT to ECAP3   
		   Uint16 ECAP4ENCLK:1;   	// 11    Enable SYSCLKOUT to ECAP4   
		   Uint16 rsvd2:2;        	// 13:12 reserved
		   Uint16 EQEP1ENCLK:1;   	// 11    Enable SYSCLKOUT to EQEP1      
		   Uint16 EQEP2ENCLK:1;   	// 11    Enable SYSCLKOUT to EQEP2         
		} bit;
	} PCLKCR1;   // 13: Peripheral clock control register

	union
	{
		Uint16 all;
		struct LPMCR0_BITS 
		{    						// bits  description
		   Uint16 LPM:2;         	// 1:0   Set the low power mode
		   Uint16 QUALSTDBY:6;   	// 7:2   Qualification   
		   Uint16 rsvd1:7;       	// 14:8  reserved
		   Uint16 WDINTE:1;      	// 15    Enables WD to wake the device from STANDBY
		} bit;
	} LPMCR0;    // 14: Low-power mode control register 0
	
	Uint16              rsvd2;     // 15: reserved
	Uint16              rsvd3;     // 16: reserved
	
	union
	{
		Uint16 all;
		struct
		{      						// bits  description
		   Uint16 DIV:4;         	// 3:0   Set clock ratio for the PLL
		   Uint16 rsvd1:12;      	// 15:4  reserved
		} bit;
	} PLLCR;     // 17: PLL control register
	
	// No bit definitions are defined for SCSR because
	// a read-modify-write instruction can clear the WDOVERRIDE bit
	
	Uint16              SCSR;      // 18: System control and status register
	Uint16              WDCNTR;    // 19: WD counter register
	Uint16              rsvd4;     // 20
	Uint16              WDKEY;     // 21: WD reset key register
	Uint16              rsvd5[3];  // 22-24
	
	// No bit definitions are defined for WDCR because
	// the proper value must be written to the WDCHK field
	// whenever writing to this register. 
	
	Uint16              WDCR;      // 25: WD timer control register
	Uint16              rsvd6[6];  // 26-31
};

//---------------------------------------------------------------------------
// CPU Timer Register Bit Definitions:
//---------------------------------------------------------------------------
struct CPUTIMER_REGS 
{
	union 
	{
		Uint32 all;
		struct TIM_REG 
		{
		   Uint16  LSW;
		   Uint16  MSW;
		} half;
	}  TIM;   // Timer counter register

   	union
	{
		Uint32 all;
		struct PRD_REG 
		{
			Uint16  LSW;
			Uint16  MSW;
		} half;
	}  PRD;   // Period register

	union
	{
		Uint16 all;
		struct
		{          					// bits  description
		   Uint16    rsvd1:4;       // 3:0   reserved
		   Uint16    TSS:1;         // 4     Timer Start/Stop
		   Uint16    TRB:1;         // 5     Timer reload
		   Uint16    rsvd2:4;       // 9:6   reserved
		   Uint16    SOFT:1;        // 10    Emulation modes
		   Uint16    FREE:1;        // 11
		   Uint16    rsvd3:2;       // 12:13 reserved
		   Uint16    TIE:1;         // 14    Output enable
		   Uint16    TIF:1;         // 15    Interrupt flag
		} bit;
	} TCR;   // Timer control register

	Uint16          rsvd1; // reserved

	union
	{
		Uint16 all;
		struct
		{        					// bits  description
		   Uint16     TDDR:8;     	// 7:0   Divide-down low
		   Uint16     PSC:8;      	// 15:8  Prescale counter low
		} bit;
	} TPR;   // Timer pre-scale low

	union
	{
		Uint16 all;
		struct
		{       					// bits  description
		   Uint16     TDDRH:8;      // 7:0   Divide-down high
		   Uint16     PSCH:8;       // 15:8  Prescale counter high
		} bit;
	} TPRH;  // Timer pre-scale high
};

extern volatile struct PIERegisters pieRegisters;
extern volatile struct SYSCtrlRegisters sysCtrlRegisters;
extern volatile struct CPUTIMER_REGS timer0Registers;
extern volatile struct GPIO_CTRL_REGS gpioCtrlRegisters;
extern volatile struct GPIO_DATA_REGS gpioDataRegisters;
extern volatile struct GPIO_INT_REGS gpioIntRegisters;
extern volatile struct SPI_REGS spiaRegisters;
extern volatile struct SPI_REGS spibRegisters;
extern volatile struct SPI_REGS spicRegisters;
extern volatile struct SPI_REGS spidRegisters;

#endif
