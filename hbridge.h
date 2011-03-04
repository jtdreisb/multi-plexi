// hbridge.h

#include "XiphosLibrary/globals.h"
#include <util/delay.h>
//! Specifies the valid values that can be passed to the hbridgeDirection() function.
typedef enum
{
	LEFT,
	RIGHT,
	FORWARD,
	BACKWARD,
	FBBRAKE,
	LRBRAKE,
	STOP
} HBridgeDirection;

typedef enum
{
	F = 3,
	B = 2,
	L = 7,
	R = 6
} HBridge;

typedef enum
{
	M0 = 9,
	M1 = 8,
	M2 = 5,
	M3 = 4,
	Front = 0,
	Back = 1,
	Left = 2,
	Right = 3
} HMotor;

void hbridgeInit();
void hbridgeDirection(HBridgeDirection direction);
void hbridgeSpeed(HBridge num, s08 speed);

