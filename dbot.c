#include "XiphosLibrary/globals.h"
// looking from front
#define STL 33
#define STR 15
#define SBL 95
#define SBR 45

// start by moving right
// then expand and wait for abit 
// then bounce side to side
void normal() {
	delayMs(1000);
	
}

#define RESETBTN 1
void startDbot() {
	buttonWait();
/*
	servo(1, STL);
	delayMs(500);
	servo(2, STR);
	delayMs(500);
	servo(3, SBL);
	delayMs(500);
	servo(0, SBR);
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
		}
	}
	
}




