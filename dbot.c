#include "XiphosLibrary/globals.h"
// looking from front
#define STL 0 //was 10
#define STR 0 //was 10
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
	delayMs(100);
	servo(5, STR);
	delayMs(100);
/*	servo(4, SBL);
	delayMs(100);
	servo(6, SBR);
	delayMs(100);
*/	
	servoOff(1);
//	servoOff(4);
	servoOff(5);
//	servoOff(6);
}
// start by moving right
// then expand and wait for abit 
// then bounce side to side
void normal() {
	clearScreen();
	printString("IN NORMAL");
//	delayMs(10000);
//	move(RIGHT);
	expand();
/*	delayMs(900);
	for (;;) {
		clearScreen();
		printString("looping");
		move(RIGHT);
		delayMs(3450);
		move(LEFT);
		delayMs(3500);
	}
*/}
void moveNow() {
	delayMs(10000);
	move(RIGHT);
	delayMs(2000);
	for (;;) {
		clearScreen();
		printString("looping");
		move(RIGHT);
		delayMs(3450);
		move(LEFT);
		delayMs(3500);
	}
}

//covers the middle of the net
void notNormal() {
	clearScreen();
	printString("IN NOT NORMAL");
//	delayMs(10000);
//	move(RIGHT);
	expand();
/*	delayMs(700);
	for (;;) {
		clearScreen();
		printString("looping");
		move(RIGHT);
		delayMs(2500);
		move(LEFT);
		delayMs(2500);
	}
*/}

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
			if (!digitalInput(RESETBTN)) {
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
			if (!digitalInput(RESETBTN)) {
				notNormal();
			}
			delayMs(50);
		}
		clearScreen();
		printString("No Expand");
		for (;;) {
			if (getButton1()) {
				for (;getButton1();)
					;
				break;
			}
			if (!digitalInput(RESETBTN)) {
				moveNow();
			}
			delayMs(50);
		}
	}
	
}




