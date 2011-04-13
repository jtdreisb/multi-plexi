// hbridge.h

#include "XiphosLibrary/globals.h"
#include <util/delay.h>
#include "pindefs.h"
//! Specifies the valid values that can be passed to the hbridgeDirection() function.
typedef enum
{
	LEFT,
	RIGHT,
	FORWARD,
	BACKWARD,
	YBRAKE,
	XBRAKE,
	BRAKE
} HBridgeDirection;


void hbridgeInit();
void hbridgeDirection(HBridgeDirection direction);
void hbridgeSpeed(u08 num, s08 speed);

