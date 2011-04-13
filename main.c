/* Multiplexi
 * main.c
 */
#include "XiphosLibrary/globals.h"
#include "movement.h"

#define DELAY 10
#define SHOOTER_SPEED 80
#define COLLECTOR_SPEED 105
//CHANGE AS NEEDED BEFORE DOWNLOADING TO BOT!
#define RB 1 //0 is blue, 1 is red

void collectBackwards();
void turnOnCollector();
void turnOffCollector();
void goForwards();


int main()
{
	initialize();
	hbridgeInit();

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
		printString("collecting");	
		
		digitalDirection(0,INPUT);
		digitalDirection(1, INPUT);
		
		while(!getButton1()) {
			goForwards();
			turnOnCollector();
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
		while(!getButton1()) {
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
	moveY(0); //moveY(1) to keep up against the wall?
	while(!digitalInput(2)) { //assuming the right side sensor is di2
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
