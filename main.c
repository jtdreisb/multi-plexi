/* Multiplexi
 * main.c
 */
#include "XiphosLibrary/globals.h"
#include "movement.h"

#define DELAY 10
#define SHOOTER_SPEED 80
#define COLLECTOR_SPEED 105

void collectBackwards();
void turnOnCollector();
void turnOffCollector();
void goForwards();


int main()
{
	initialize();
	servoRange(0, 11);
	
	for (;;) {
		
		servo(0, 0);
		buttonWait();
		servo(0, 255);	
		buttonWait();	

	}
	
	
	hbridgeInit();

	moveX(0);
	moveY(0);
	//Test 1
	clearScreen();
	printString("Test Move");
	lowerLine();
	

	buttonWait();
	clearScreen();
	printString("collecting");	
	
	digitalDirection(0,INPUT);
	digitalDirection(1, INPUT);
	
	while(!getButton1()) {
		turnOnCollector();
		collectBackwards();
		turnOffCollector();
		goForwards();
	}
	
	clearScreen();
	printString("DONE");
}

void goForwards()
{
	moveY(30);
	while(!digitalInput(0)) {
		delayMs(50);
	}
	moveY(0);
}
void collectBackwards()
{
	// Really slow
	moveY(-8);
	
	//moveX(0);
	while(1)
	{
		// if (analog(0) > 80)
		if (digitalInput(1)) // bump sensor is tripped
		{
			moveX(0);
			moveY(0);
			break;
		}
		delayMs(50);
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


