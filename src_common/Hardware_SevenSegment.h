#ifndef _SEVEN_SEGMENT_H
#define _SEVEN_SEGMENT_H

#include"System.h"

struct SevenSegmentEntry
{
	Uint16 rsvd:9;
	Uint16 A:1;
	Uint16 B:1;
	Uint16 C:1;
	Uint16 D:1;
	Uint16 E:1;
	Uint16 F:1;
	Uint16 G:1;
};

void SevenSegmentRefresh();
void SetSevenSegmentDisplay(Int16);

#endif
