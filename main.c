/* Multiplexi
 * main.c
 */
#include "XiphosLibrary/globals.h"
#include "movement.h"

#define DELAY 10
#define SHOOTER_SPEED 70//was 80
#define COLLECTOR_SPEED 90
//CHANGE AS NEEDED BEFORE DOWNLOADING TO BOT!
#define RB 0 //0 is blue, 1 is red

void collectBackwards();
void turnOnCollector();
void turnOffCollector();
void goForwards();
void goRightWall();


int main()
{
	initialize();
	hbridgeInit();
	adcInit();


	moveX(0);
	moveY(0);
	
	if(RB == 0) { 
		//Test 1
		clearScreen();
		printString("Blue Bot!");
		lowerLine(); 
		printString("Test Move");
		lowerLine();
		

		buttonWait();
		clearScreen();
		printString("waiting");

		while(analog(0) > 127) {
			delayMs(50);
		}
		
		clearScreen();
		printString("collecting");	
		
		digitalDirection(0,INPUT);
		digitalDirection(1, INPUT);
		
		while(1) { //!getButton1()) {
			goForwards();
			turnOnCollector();
			delayMs(3000);
			collectBackwards();
			turnOffCollector();
		}

	}
	else { //assuming RB = 1, red bot
		//Test 1
		clearScreen();
		printString("Red Bot!");
		lowerLine(); 
		printString("Test Move");
		lowerLine();
		

		buttonWait();
		clearScreen();
		printString("collecting");	
		
		digitalDirection(0,INPUT);
		digitalDirection(1, INPUT);
		
		goRightWall();
		while(1) { //!getButton1()) {
			turnOnCollector();
			collectBackwards();
			turnOffCollector();
			goForwards();
		}
	}

    delayMs(5000);	
	clearScreen();
	printString("DONE");
}

void goForwards()
{
	moveX(10);
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

void goRightWall() 
{
	moveX(20); //might be backwards/wrong speed
	moveY(30);
	while(!digitalInput(0)) {
		delayMs(50);
	}
	moveY(0); //moveY(1) to keep up against the front wall?
	while(analog(0) < 127) { //assuming the right side sensor is analog 0
		delayMs(50);
    }
	moveX(0);
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
