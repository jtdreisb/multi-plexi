/* Multiplexi
 * main.c
 */
#include "XiphosLibrary/globals.h"
#include "movement.h"

#define DELAY 10
#define SHOOTER_SPEED 100
#define COLLECTOR_SPEED 95

void collectBackwards();
void turnOnCollector();
void turnOffCollector();


int main()
{
	
	initialize();
	servoInit();
	hbridgeInit();
	adcInit();
	moveX(0);
	moveY(0);
	//Test 1
	clearScreen();
	printString("Go Windows!");
	lowerLine();
	printString("Boo Macs!");
	buttonWait();
	//spiralToInfinity();

	collectBackwards();
	turnOffCollector();
	
	clearScreen();
	printString("DONE");
}

void collectBackwards()
{
	turnOnCollector();
	moveY(-50);
	
	while(1)
	{
		if (analog(0) > 80)
		{
			moveX(0);
			moveY(0);
			break;
		}
		
		if (analog(2) > 50 || analog(3) > 50)
		{
			moveX(-10);
		}
		else
		{
			moveX(0);
		}
	}
}

void turnOnCollector()
{
	motor1(SHOOTER_SPEED);
	motor0(COLLECTOR_SPEED);
}

void turnOffCollector()
{
	motor1(127);
	motor0(127);
}


