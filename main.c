/* Multiplexi
 * main.c
 */
#include "XiphosLibrary/globals.h"
#include "movement.h"

#define DELAY 10
#define SHOOTER_SPEED 255
#define COLLECTOR_SPEED 160



int main()
{
	
	initialize();
	servoInit();
	hbridgeInit();
	moveX(0);
	moveY(0);
	//Test 1
	clearScreen();
	printString("Go Ninja!");
	lowerLine();
	printString("");
	buttonWait();
	//spiralToInfinity();
	for (;;) {

		moveX(0);
		moveY(0);
		clearScreen();
		
		printString("0");
		lowerLine();
		printString("0");
		buttonWait();

		moveX(50);
		moveY(50);

		clearScreen();
		printString("50");
		lowerLine();
		printString("50");
		buttonWait();

		moveX(20);
		moveY(20);

		clearScreen();
		printString("20");
		lowerLine();
		printString("20");
		buttonWait();	
	}

		
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


