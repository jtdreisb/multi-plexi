#include "movement.h"
#include "hbridge.h"
#include <math.h>

// speed is between -100 and 100
void moveX(s08 speed)
{
	if (speed > 0)
	{
		hbridgeDirection(RIGHT);
		hbridgeSpeed(L, speed);
		hbridgeSpeed(R, speed);
	}
	else if (speed < 0)
	{
		hbridgeDirection(LEFT);
		hbridgeSpeed(L, speed&~0x80);
		hbridgeSpeed(R, speed&~0x80);
	}
	else
	{
		hbridgeSpeed(L, 0);
		hbridgeSpeed(R, 0);
	}
}

// speed is between -100 and 100
void moveY(s08 speed)
{
	if (speed > 0)
	{
		hbridgeDirection(FORWARD);
		hbridgeSpeed(F, speed);
		hbridgeSpeed(B, speed);
	}
	else if (speed < 0)
	{
		hbridgeDirection(BACKWARD);
		hbridgeSpeed(F, speed&~0x80);
		hbridgeSpeed(B, speed&~0x80);
	}
	else
	{
		hbridgeSpeed(F, 0);
		hbridgeSpeed(B, 0);
	}
}

#define MAX_SPEED 	40
#define SPIRAL_STEP 0.1

void spiralToInfinity()
{
	double theta = 0.0;

	for (;;) {
		if (theta > M_PI) {
			theta = 0.0;
		}
		moveX((s08)(MAX_SPEED * cos(theta)));
		moveY((s08)(MAX_SPEED * sin(theta)));
		delayMs(10);
		theta += SPIRAL_STEP;
	}
	return;
}
