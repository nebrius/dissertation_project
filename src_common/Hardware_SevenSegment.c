#include"System.h"

// Define the seven segment number to segment mappings
const struct SevenSegmentEntry sevenSegmentDigits[21] = {
	{0,1,1,1,1,1,1,0}, // 0
	{0,0,1,1,0,0,0,0}, // 1
	{0,1,1,0,1,1,0,1}, // 2
	{0,1,1,1,1,0,0,1}, // 3
	{0,0,1,1,0,0,1,1}, // 4
	{0,1,0,1,1,0,1,1}, // 5
	{0,1,0,1,1,1,1,1}, // 6
	{0,1,1,1,0,0,0,0}, // 7
	{0,1,1,1,1,1,1,1}, // 8
	{0,1,1,1,1,0,1,1}, // 9
	{0,1,1,1,0,1,1,1}, // A
	{0,0,0,1,1,1,1,1}, // B
	{0,1,0,0,1,1,1,0}, // C
	{0,0,1,1,1,1,0,1}, // D
	{0,1,0,0,1,1,1,1}, // E
	{0,1,0,0,0,1,1,1}, // F
	{0,0,0,0,0,0,0,0}, // Blank
	{0,0,0,0,1,0,0,0}, // 1 dash
	{0,0,0,0,1,0,0,1}, // 2 dash
	{0,1,0,0,1,0,0,1}, // 3 dash
	{0,0,1,1,0,1,1,0} // Finished Symbol
};

#pragma CODE_SECTION(SevenSegmentRefresh,"FlashCode");
void SevenSegmentRefresh()
{
	Uint16 digit;

	// Clear the display to prevent ghosting
	gpioDataRegisters.GPACLEAR.bit.SEVENSEG_A = 1;
	gpioDataRegisters.GPACLEAR.bit.SEVENSEG_B = 1;
	gpioDataRegisters.GPACLEAR.bit.SEVENSEG_C = 1;
	gpioDataRegisters.GPACLEAR.bit.SEVENSEG_D = 1;
	gpioDataRegisters.GPACLEAR.bit.SEVENSEG_E = 1;
	gpioDataRegisters.GPACLEAR.bit.SEVENSEG_F = 1;
	gpioDataRegisters.GPACLEAR.bit.SEVENSEG_G = 1;

	// Get the digit and set which digit to display
	if(gpioDataRegisters.GPADAT.bit.SEVENSEG_DIGIT == 1)
	{
		gpioDataRegisters.GPATOGGLE.bit.SEVENSEG_DIGIT = 1;
		digit = globals.processing.sevenSegmentUpperDigit;
	}
	else
	{
		gpioDataRegisters.GPATOGGLE.bit.SEVENSEG_DIGIT = 1;
		digit = globals.processing.sevenSegmentLowerDigit;
	}

	// Set segment A
	if(sevenSegmentDigits[digit].A)
		gpioDataRegisters.GPASET.bit.SEVENSEG_A = 1;
	else
		gpioDataRegisters.GPACLEAR.bit.SEVENSEG_A = 1;

	// Set segment B
	if(sevenSegmentDigits[digit].B)
		gpioDataRegisters.GPASET.bit.SEVENSEG_B = 1;
	else
		gpioDataRegisters.GPACLEAR.bit.SEVENSEG_B = 1;

	// Set segment C
	if(sevenSegmentDigits[digit].C)
		gpioDataRegisters.GPASET.bit.SEVENSEG_C = 1;
	else
		gpioDataRegisters.GPACLEAR.bit.SEVENSEG_C = 1;

	// Set segment D
	if(sevenSegmentDigits[digit].D)
		gpioDataRegisters.GPASET.bit.SEVENSEG_D = 1;
	else
		gpioDataRegisters.GPACLEAR.bit.SEVENSEG_D = 1;

	// Set segment E
	if(sevenSegmentDigits[digit].E)
		gpioDataRegisters.GPASET.bit.SEVENSEG_E = 1;
	else
		gpioDataRegisters.GPACLEAR.bit.SEVENSEG_E = 1;

	// Set segment F
	if(sevenSegmentDigits[digit].F)
		gpioDataRegisters.GPASET.bit.SEVENSEG_F = 1;
	else
		gpioDataRegisters.GPACLEAR.bit.SEVENSEG_F = 1;

	// Set segment G
	if(sevenSegmentDigits[digit].G)
		gpioDataRegisters.GPASET.bit.SEVENSEG_G = 1;
	else
		gpioDataRegisters.GPACLEAR.bit.SEVENSEG_G = 1;
}

#pragma CODE_SECTION(SetSevenSegmentDisplay,"FlashCode");
void SetSevenSegmentDisplay(Int16 digit)
{
	// If we are setting a number, convert the number to a two character string (basically)
	// Decimal
	if(digit != SEVENSEG_BLANK)
	{
		globals.processing.sevenSegmentUpperDigit = digit / 10;
		globals.processing.sevenSegmentLowerDigit = digit % 10;
	}
	else
	{
		globals.processing.sevenSegmentUpperDigit = SEVENSEG_BLANK_DIGIT;
		globals.processing.sevenSegmentLowerDigit = SEVENSEG_BLANK_DIGIT;
	}
	// Hexadecimal
/*	if(digit != SEVENSEG_BLANK)
	{
		globals.processing.sevenSegmentUpperDigit = (digit >> 0x0004) & 0x000F;
		globals.processing.sevenSegmentLowerDigit = digit & 0x000F;
	}
	else
	{
		globals.processing.sevenSegmentUpperDigit = SEVENSEG_BLANK_DIGIT;
		globals.processing.sevenSegmentLowerDigit = SEVENSEG_BLANK_DIGIT;
	}*/
}
