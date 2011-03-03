#include "XiphosLibrary/globals.h"
//! Specifies the valid values that can be passed to the hbridgeDirection() function.
typedef enum
{
	LEFT,
	RIGHT,
	FORWARD,
	BACKWARD,
	FBBRAKE,
	LRBRAKE,
	STOP
} HBridgeDirection;

typedef enum
{
	F = 3,
	B = 2,
	L = 7,
	R = 6
} HBridge;

typedef enum
{
	M0 = 9,
	M1 = 8,
	M2 = 5,
	M3 = 4,
	Front = 0,
	Back = 1,
	Left = 2,
	Right = 3
} HMotor;

u08 counter = 0;
u08 motor[4] = {50, 50, 50, 50};

void hbridgeInit()
{
	// initialize stuff
	
	//Set up Timer 5 in 8-bit fast PWM mode with the prescaler at I/O clock / 64,
	//with the PWM in normal polarity (meaning motor on or brake on is logic 1).
	//TCCR5B = (1 << CS50);
	TCCR5A = (1 << WGM50);
	TCCR5B = (1 << CS51) | (1 << WGM52);

	//enable interrupt for output compare unit 3C
	TIMSK5 |= _BV(OCIE5C);
	
	digitalDirection(M0, OUTPUT);
	digitalDirection(M1, OUTPUT);
	digitalDirection(M2, OUTPUT);
	digitalDirection(M3, OUTPUT);
	digitalDirection(L, OUTPUT);
	digitalDirection(R, OUTPUT);
	digitalDirection(B, OUTPUT);
	digitalDirection(F, OUTPUT);
}

void hbridgeOutput(HBridge num, u08 value)
{
	//if the output should be set
	if (value > 0)
	{
		sbi(PORTA, num - 2);
	}
	//else the output should be cleared
	else
	{
		cbi(PORTA, num - 2);
	}
}

// sets the speed
// speed is between 0 and 100
// 0 is coast
// 100 is full speed
void hbridgeSpeed(HBridge num, s08 speed)
{
	switch (num)
	{
		case F:
			motor[0] = speed;
			break;
			
		case B:
			motor[1] = speed;
			break;
			
		case L:
			motor[2] = speed;
			break;
			
		case R:
			motor[3] = speed;
			break;
	}
}

// sets the direction of movement
void hbridgeDirection(HBridgeDirection direction)
{
	switch (direction)
	{
		case LEFT:
			hbridgeOutput(L, 1);
			hbridgeOutput(R, 0);
			break;
			
		case RIGHT:
			hbridgeOutput(R, 1);
			hbridgeOutput(L, 0);
			break;
			
		case FORWARD:
			hbridgeOutput(F, 1);
			hbridgeOutput(B, 0);
			break;
			
		case BACKWARD:
			hbridgeOutput(B, 1);
			hbridgeOutput(F, 0);
			break;
			
		case FBBRAKE:
			hbridgeOutput(F, 0);
			hbridgeOutput(B, 0);
			break;
			
		case LRBRAKE:
			hbridgeOutput(L, 0);
			hbridgeOutput(R, 0);
			break;
			
		case STOP:
			hbridgeOutput(F, 0);
			hbridgeOutput(B, 0);
			hbridgeOutput(L, 0);
			hbridgeOutput(R, 0);
			break;
	}
}

ISR(TIMER5_COMPC_vect)
{
	counter++;
	
	if (counter == 100)
	{
		counter = 0;
		hbridgeOutput(M0, 1);
		hbridgeOutput(M0, 1);
		hbridgeOutput(M1, 1);
		hbridgeOutput(M2, 1);
		hbridgeOutput(M3, 1);
	}
	
	if (motor[0] == counter)
	{
		hbridgeOutput(M0, 0);
	}
	
	if (motor[1] == counter)
	{
		hbridgeOutput(M1, 0);
	}
	
	if (motor[2] == counter)
	{
		hbridgeOutput(M2, 0);
	}
		 		 		 		 	
	if (motor[3] == counter)
	{
		hbridgeOutput(M3, 0);
	}
}
