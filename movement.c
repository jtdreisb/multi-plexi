#include "movement.h"
#include "hbridge.h"
#include <math.h>

// speed is between -100 and 100
void moveX(s08 speed)
{
	if (speed > 0)
	{
		hbridgeDirection(RIGHT);
		hbridgeSpeed(X_MOTOR_LEFT, speed);
		hbridgeSpeed(X_MOTOR_RIGHT, speed);
	}
	else if (speed < 0)
	{
		hbridgeDirection(LEFT);
		hbridgeSpeed(X_MOTOR_LEFT, -speed);
		hbridgeSpeed(X_MOTOR_RIGHT, -speed);
	}
	else
	{
		hbridgeDirection(XBRAKE);
	}
}

void moveXAdjusted(s08 speed) {
	if (speed > 0)
	{
		hbridgeDirection(RIGHT);
		hbridgeSpeed(X_MOTOR_LEFT, speed);
		hbridgeSpeed(X_MOTOR_RIGHT, speed);
	}
	else if (speed < 0)
	{
		hbridgeDirection(LEFT);
		hbridgeSpeed(X_MOTOR_LEFT, -speed);
		hbridgeSpeed(X_MOTOR_RIGHT, -speed);
	}
	else
	{
		hbridgeDirection(XBRAKE);
	}
}

// speed is between -100 and 100
void moveY(s08 speed)
{
	if (speed > 0)
	{
		hbridgeDirection(FORWARD);
		hbridgeSpeed(Y_MOTOR_FRONT, speed);
		hbridgeSpeed(Y_MOTOR_BACK, speed);
	}
	else if (speed < 0)
	{
		hbridgeDirection(BACKWARD);
		hbridgeSpeed(Y_MOTOR_FRONT, -speed);
		hbridgeSpeed(Y_MOTOR_BACK, -speed);
	}
	else
	{
		hbridgeDirection(YBRAKE);
	}
}

void moveY2(s08 speed)
{
	
	if (speed > 0)
	{
		hbridgeDirection(FORWARD);
		hbridgeSpeed(Y_MOTOR_BACK, speed);
		hbridgeSpeed(Y_MOTOR_FRONT, 0);
	}
	else if (speed < 0)
	{
		hbridgeDirection(BACKWARD);
		hbridgeSpeed(Y_MOTOR_BACK, -speed);
		hbridgeSpeed(Y_MOTOR_FRONT, 0);
	}
	else
	{
		hbridgeDirection(YBRAKE);
	}
}

void moveY3(s08 speed)
{
	
	if (speed > 0)
	{
		hbridgeDirection(FORWARD);
		hbridgeSpeed(Y_MOTOR_FRONT, speed);
		hbridgeSpeed(Y_MOTOR_BACK, 0);
	}
	else if (speed < 0)
	{
		hbridgeDirection(BACKWARD);
		hbridgeSpeed(Y_MOTOR_FRONT, -speed);
		hbridgeSpeed(Y_MOTOR_BACK, 0);
	}
	else
	{
		hbridgeDirection(YBRAKE);
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
