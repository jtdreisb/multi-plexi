//Copyright (C) 2009-2010  Patrick J. McCarty.
//Licensed under X11 License. See LICENSE.txt for details.

/*! @file
    Implements support for controlling two brushed DC motors using two VNH3SP30 H-Bridges.
 */
#include "globals.h"

/*! Initialize the enabled motor channels.
    Normally called only by the initialize() function in utility.c.
 */
inline void motorInit()
{
	//Set up Timer 1 in 8-bit fast PWM mode with the prescaler at I/O clock / 64,
	//with the PWM in normal polarity (meaning motor on or brake on is logic 1).
	TCCR1A = (1 << WGM10);
	TCCR1B = (1 << CS11) | (1 << CS10) | (1 << WGM12);

	#if USE_MOTOR0 == 1
		//Configure INA and INB as outputs
		DDRE |= _BV(DDE7) | _BV(DDE6);
		//Configure PWM as an ouput
		sbi(DDRB, DDB5);

		//Set OC1A (output compare unit 1A) for fast PWM, normal polarity
		TCCR1A |= (1 << COM1A1);
		//Set the duty cycle to zero
		OCR1AH = 0;
		OCR1AL = 0;
	#endif

	#if USE_MOTOR1 == 1
		//Configure INA and INB as outputs
		DDRE |= _BV(DDE3) | _BV(DDE2);
		//Configure PWM as an ouput
		sbi(DDRB, DDB6);

		//Set OC1B (output compare unit 1B) for fast PWM, normal polarity
		TCCR1A |= (1 << COM1B1);
		//Set the duty cycle to zero
		OCR1BH = 0;
		OCR1BL = 0;
	#endif
}

//Don't compile the motor 0 functions if motor 0 is not enabled (prevents accidental use).
#if USE_MOTOR0 == 1
/*! Sets motor speed and direction for Motor 0.
    @param speedAndDirection Values are interpreted such that:
    0 = full speed reverse,
    127 = glide to a stop (no braking),
    255 = full speed forward
 */
void motor0(const u08 speedAndDirection)
{
	//Glide to a stop (no braking)
	if (speedAndDirection == 127)
	{
		//***TODO: disable the interrupt watching for a DIAGA/DIAGB fault condition

		//Set PWM to lowest duty cycle
		OCR1AL = 0;

		//Configure the combined DIAGA/DIAGB as an output so it can force the H-Bridge off.
		sbi(DDRE, DDE5);
		//Drive the combined DIAGA/DIAGB low to disable the H-bridge chip.
		cbi(PORTE, PE5);
	}
	else
	{
		//***TODO: enable the interrupt watching for a DIAGA/DIAGB fault condition

		//Configure the combined DIAGA/DIAGB pin as an input so it can detect a fault condition.
		cbi(DDRE, DDE5);
		//Enable pullup resistor to pull the combined DIAGA/DIAGB high to enable the H-bridge chip.
		sbi(PORTE, PE5);

		//Drive forward
		if (speedAndDirection > 127)
		{
			//set INA high and INB low to drive "clockwise"
			PORTE = (PORTE & ~(_BV(PE6) | _BV(PE7))) | _BV(PE7);

			//Set the duty cycle by writing to the output compare register for Timer 1,
			//which is the duty cycle register when the timer is in 8-bit fast PWM mode.
			//Subtract 128 from the parameter to produce a range of 0-127 from 128-255.
			//Multiply the new 0-127 range by 2 to make it 0-254.
			//Add 1 to make the final range 1-255.
			OCR1AL = ((speedAndDirection - 128) * 2) + 1;
		}
		//Drive backward
		else
		{
			//set INA low and INB high to drive "counterclockwise"
			PORTE = (PORTE & ~(_BV(PE6) | _BV(PE7))) | _BV(PE6);

			//Set the duty cycle by writing to the output compare register for Timer 1,
			//which is the duty cycle register when the timer is in 8-bit fast PWM mode.
			//Subtract the parameter from 127 to produce a range of 127-1 from 0-126.
			//Multiply the new 127-1 range by 2 to make it 254-2.
			OCR1AL = (127 - speedAndDirection) * 2;
		}
	}
}

/*! Shorts both of motor0's terminals to ground to oppose motor0 movement.
    @param brakingPower Specifies the duty cycle to apply the brake at (0 to 255):
	0 = no braking, 255 = maximum braking
 */
void brake0(const u08 brakingPower)
{
	//set PWM to specified braking duty cycle
	OCR1AL = brakingPower;
	//set INA low and INB low to brake to GND
	PORTE &= ~(_BV(PE6) | _BV(PE7));
}

/*! Checks if the H-Bridge for motor0 is in shutdown mode due to a fault.
    Detectable fault conditions include overtemperature and shorted to battery.
	@return 0 = no fault or not in drive mode, non-zero = fault detected
*/
u08 motor0Faulted()
{
	//We can't detect a fault when the user is forcing the H-bridge to disable (stopped mode), so return false.
	if (gbi(DDRE, DDE5))
	{
		//DIAGA/DIAGB is set as an output, so we can't read the fault.
		return 0;
	}
	else
	{
		//Read the state of the input pin, low means a fault was detected.
		return !gbi(PINE, PINE5);
	}
}
#endif //USE_MOTOR0 == 1

//Don't compile the motor 1 functions if motor 1 is not enabled (prevents accidental use).
#if USE_MOTOR1 == 1
/*! Sets motor speed and direction for Motor 1.
    @param speedAndDirection Values are interpreted such that:
    0 = full speed reverse,
    127 = glide to a stop (no braking),
    255 = full speed forward
 */
void motor1(const u08 speedAndDirection)
{
	//Glide to a stop (no braking)
	if (speedAndDirection == 127)
	{
		//***TODO: disable the interrupt watching for a DIAGA/DIAGB fault condition

		//Set PWM to lowest duty cycle
		OCR1BL = 0;

		//Configure the combined DIAGA/DIAGB as an output so it can force the H-Bridge off.
		sbi(DDRE, DDE4);
		//Drive the combined DIAGA/DIAGB low to disable the H-bridge chip.
		cbi(PORTE, PE4);
	}
	else
	{
		//***TODO: enable the interrupt watching for a DIAGA/DIAGB fault condition

		//Configure the combined DIAGA/DIAGB pin as an input so it can detect a fault condition.
		cbi(DDRE, DDE4);
		//Enable pullup resistor to pull the combined DIAGA/DIAGB high to enable the H-bridge chip.
		sbi(PORTE, PE4);

		//Drive forward
		if (speedAndDirection > 127)
		{
			//set INA high and INB low to drive "clockwise"
			PORTE = (PORTE & ~(_BV(PE3) | _BV(PE2))) | _BV(PE2);

			//Set the duty cycle by writing to the output compare register for Timer 1,
			//which is the duty cycle register when the timer is in 8-bit fast PWM mode.
			//Subtract 128 from the parameter to produce a range of 0-127 from 128-255.
			//Multiply the new 0-127 range by 2 to make it 0-254.
			//Add 1 to make the final range 1-255.
			OCR1BL = ((speedAndDirection - 128) * 2) + 1;
		}
		//Drive backward
		else
		{
			//set INA low and INB high to drive "counterclockwise"
			PORTE = (PORTE & ~(_BV(PE3) | _BV(PE2))) | _BV(PE3);

			//Set the duty cycle by writing to the output compare register for Timer 1,
			//which is the duty cycle register when the timer is in 8-bit fast PWM mode.
			//Subtract the parameter from 127 to produce a range of 127-1 from 0-126.
			//Multiply the new 127-1 range by 2 to make it 254-2.
			OCR1BL = (127 - speedAndDirection) * 2;
		}
	}
}

/*! Shorts both of motor1's terminals to ground to oppose motor1 movement.
    @param brakingPower Specifies the duty cycle to apply the brake at (0 to 255):
	0 = no braking, 255 = maximum braking
 */
void brake1(const u08 brakingPower)
{
	//set PWM to specified braking duty cycle
	OCR1BL = brakingPower;
	//set INA low and INB low to brake to GND
	PORTE &= ~(_BV(PE2) | _BV(PE3));
}

/*! Checks if the H-Bridge for motor1 is in shutdown mode due to a fault.
    Detectable fault conditions include overtemperature and shorted to battery.
	@return 0 = no fault or not in drive mode, non-zero = fault detected
*/
u08 motor1Faulted()
{
	//We can't detect a fault when the user is forcing the H-bridge to disable (stopped mode), so return false.
	if (gbi(DDRE, DDE4))
	{
		//DIAGA/DIAGB is set as an output, so we can't read the fault.
		return 0;
	}
	else
	{
		//Read the state of the input pin, low means a fault was detected.
		return !gbi(PINE, PINE4);
	}
}
#endif //USE_MOTOR1 == 1
