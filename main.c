/* Multiplexi
 * main.c
 */
#include "XiphosLibrary/globals.h"
#include "hbridge.c"
#include <util/delay.h>

#define DELAY 10
#define SHOOTER_SPEED 255
#define COLLECTOR_SPEED 160

void moveX(s08 speed);
void moveY(s08 speed);

int main()
{
	initialize();
	servoInit();
	hbridgeInit();

	//Test 1
	clearScreen();
	printString("Macs rule");
	lowerLine();
	printString("you all sucks");
	buttonWait();

	
	clearScreen();
	printString("__Multi-Plexi___");
	
	moveX(80);
	moveY(10);
	
	clearScreen();
	printString("80");
	lowerLine();
	printString("10");
	buttonWait();
	
	moveX(50);
	moveY(50);
	
	clearScreen();
	printString("50");
	lowerLine();
	printString("50");
	buttonWait();
	
	moveX(20);
	moveY(70);
	
	clearScreen();
	printString("20");
	lowerLine();
	printString("70");
	buttonWait();
	
}

void turnOnCollector()
{
	motor0(SHOOTER_SPEED);
	motor1(COLLECTOR_SPEED);
}

void turnOffCollector()
{
	motor0(SHOOTER_SPEED);
	motor1(COLLECTOR_SPEED);
}

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
