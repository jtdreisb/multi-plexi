/* Multiplexi
 * main.c
 */
#include "XiphosLibrary/globals.h"
#include "movement.h"


#define DELAY 10
#define RB 0 //0 is blue, 1 is red, 2 is yellow
#if RB==0
	#define SHOOTER_SPEED 70
#elif RB==1	
#define SHOOTER_SPEED 95//110//was 80
	#endif
#define COLLECTOR_SPEED 164//maybe?
//CHANGE AS NEEDED BEFORE DOWNLOADING TO BOT!


void collectBackwards();
void turnOnCollector();
void turnOffCollector();
void goForwards();
void goRightWall();
void motorTest();


int main()
{
	initialize();
	hbridgeInit();

	moveX(0);
	moveY(0);
	
#if RB==0 
		//Test 1
		clearScreen();
		printString("Blue Bot!");
		lowerLine(); 
		printString("Ready");
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
			collectBackwards();
			turnOffCollector();
		}

#elif RB==1 //assuming RB = 1, red bot
		//Test 1
		clearScreen();
		printString("Red Bot!");
		lowerLine(); 
		printString("Ready");
		lowerLine();
		
		buttonWait();
		digitalDirection(0,INPUT);
		digitalDirection(1, INPUT);
	
	//	turnOnCollector();

	/*	servoRange(0, SERVO_RANGE_EXTENDED5);
		
		servo(0, 255);
		delayMs(2000);
		servo(0, 0);
*/

		clearScreen();
		printString("go other side");	
		goRightWall();
		
		delayMs(500);
		clearScreen();
		printString("collecting");
		while(1) { //!getButton1()) {
			turnOnCollector();
			collectBackwards();
			turnOffCollector();
			goForwards();
		}
#elif RB==2
		startDbot();
#endif
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
	moveX(0); 
	moveY(30);
	while(!digitalInput(0)) {
		delayMs(50);
	}
	moveY(5);
	moveX(30);
	while(analog(0) < 127) { //assuming the right side sensor is analog 0
		delayMs(50);
    }
	moveX(0);
}

void turnOnCollector()
{
	motor1(SHOOTER_SPEED);
	delayMs(3000);
	motor0(COLLECTOR_SPEED);
}

void turnOffCollector()
{
	motor1(127);
	motor0(127);
}

void motorTest() 
{
	clearScreen();
	printString("motor test");
	buttonWait();
	clearScreen();
	printString("forwards");
	moveY(10);
	delayMs(2000);
	moveY(0);
	buttonWait();
	clearScreen();
	printString("right");
	moveX(10);
	delayMs(2000);
	moveX(0);
	clearScreen();
	printString("done");
}
