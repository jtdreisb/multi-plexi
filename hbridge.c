#include "hbridge.h"

u08 counter = 0;
u08 motor[4] = {0, 0, 0, 0};

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
	
	// Setup Y motor ouputs
	digitalDirection(Y_MOTOR_FRONT, OUTPUT);
	digitalDirection(Y_MOTOR_BACK, OUTPUT);
	digitalDirection(Y_A, OUTPUT);
	digitalDirection(Y_B, OUTPUT);
	
	// X motor ouputs
	digitalDirection(X_MOTOR_LEFT, OUTPUT);
	digitalDirection(X_MOTOR_RIGHT, OUTPUT);
	digitalDirection(X_A, OUTPUT);
	digitalDirection(X_B, OUTPUT);
}


// sets the speed
// speed is between 0 and 100
// 0 is coast
// 100 is full speed
void hbridgeSpeed(u08 num, s08 speed)
{
	switch (num)
	{
		case Y_MOTOR_FRONT:
			motor[0] = speed;
			break;
			
		case Y_MOTOR_BACK:
			motor[1] = speed;
			break;
			
		case X_MOTOR_LEFT:
			motor[2] = speed;
			break;
			
		case X_MOTOR_RIGHT:
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
			digitalOutput(X_A, 1);
			digitalOutput(X_B, 0);
			break;
			
		case RIGHT:
			digitalOutput(X_A, 0);
			digitalOutput(X_B, 1);
			break;
			
		case FORWARD:
			digitalOutput(Y_A, 0);
			digitalOutput(Y_B, 1);
			break;
			
		case BACKWARD:
			digitalOutput(Y_A, 1);
			digitalOutput(Y_B, 0);
			break;
			
		case YBRAKE:
			digitalOutput(Y_A, 0);
			digitalOutput(Y_B, 0);
			break;
			
		case XBRAKE:
			digitalOutput(X_A, 0);
			digitalOutput(X_B, 0);
			break;
			
		case BRAKE:
			digitalOutput(Y_A, 0);
			digitalOutput(Y_B, 0);
			digitalOutput(X_A, 0);
			digitalOutput(X_B, 0);
			break;
	}
}

ISR(TIMER5_COMPC_vect)
{
	counter++;
	
	if (counter == 100)
	{
		counter = 0;
		digitalOutput(Y_MOTOR_FRONT,	1);
		digitalOutput(Y_MOTOR_BACK,		1);
		digitalOutput(X_MOTOR_LEFT,		1);
		digitalOutput(X_MOTOR_RIGHT,	1);
	}
	
	if (motor[0] == counter)
	{
		digitalOutput(Y_MOTOR_FRONT, 0);
	}
	
	if (motor[1] == counter)
	{
		digitalOutput(Y_MOTOR_BACK, 0);
	}
	
	if (motor[2] == counter)
	{
		digitalOutput(X_MOTOR_LEFT, 0);
	}
		 		 		 		 	
	if (motor[3] == counter)
	{
		digitalOutput(X_MOTOR_RIGHT, 0);
	}
}
