#include "XiphosLibrary/globals.h"
// looking from front
#define STL 33
#define STR 15
#define SBL 95
#define SBR 45

#define RIGHT 50
#define LEFT  205 

void move(u08 dir) {
	motor0(dir);
	motor1(dir);
}
void stop(){
	motor0(127);
	motor1(127);
}
// start by moving right
// then expand and wait for abit 
// then bounce side to side
void normal() {
	clearScreen();
	printString("IN NORMAL");
	
	delayMs(1000);
	for (;;) {
		move(RIGHT);
		delayMs(2000);
		move(LEFT);
		delayMs(2000);
	}
}

#define RESETBTN 0


void startDbot() {
	digitalDirection(RESETBTN, INPUT);
	clearScreen();
	printString("robotReady");
	buttonWait();
/*
	servo(1, STL);
	delayMs(500);
	servo(2, STR);
	delayMs(500);
	servo(3, SBL);
	delayMs(500);
	servo(7, SBR);
*/	
	
	for (;;) {
		clearScreen();
		printString("Normal Mode");
		for (;;) {
			if (getButton1()) {
				for (;getButton1();)
					;
				break;
			}
			if (digitalInput(RESETBTN)) {
				normal();
			}
			delayMs(50);
		}
		clearScreen();
		printString("Not Normal");
		for (;;) {
			if (getButton1()) {
				for (;getButton1();)
					;
				break;
			}
			if (digitalInput(RESETBTN)) {
				normal();
			}
			delayMs(50);
		}
	}
	
}




