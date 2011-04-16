#include "XiphosLibrary/globals.h"
// looking from front
#define STL 10
#define STR 10
#define SBL 100
#define SBR 35

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

void expand() {
	
	
	servo(1, STL);
	delayMs(500);
	servo(5, STR);
	delayMs(500);
	servo(4, SBL);
	delayMs(500);
	servo(6, SBR);
	delayMs(500);
	
	
	servoOff(1);
	servoOff(4);
	servoOff(5);
	servoOff(6);
}
// start by moving right
// then expand and wait for abit 
// then bounce side to side
void normal() {
	clearScreen();
	printString("IN NORMAL");
	move(RIGHT);
	expand();
	delayMs(2000);
	for (;;) {
		clearScreen();
		printString("looping");
		move(RIGHT);
		delayMs(3500);
		move(LEFT);
		delayMs(3500);
	}
}

#define RESETBTN 3


void startDbot() {
	digitalDirection(RESETBTN, INPUT);
	clearScreen();
	printString("robotReady");
	buttonWait();


	
	
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




