//Copyright (C) 2009-2010  Patrick J. McCarty.
//Licensed under X11 License. See LICENSE.txt for details.

/*! @file
    Software-based PWM implementation for controlling up to 8 PWM servo outputs via a 16-bit timer interrupt.
    To minimize interrupt overhead, the timeslots for the servos are spread out over 20ms (the standard servo period)
    such that only one servo output, if any, is high at any given time.
 */

#include "globals.h"
#include <util/delay.h>

//Validate the number of enabled servos
#if NUM_SERVOS < 0 || NUM_SERVOS > 8
	#error "NUM_SERVOS must be set to a value of 0 to 8 in the project's Makefile"
#endif

/*! The period of time during which an individual servo's output can be controlled (only one is controlled at a time).
 *  20ms (the standard servo period) divided by NUM_SERVOS = number of milliseconds to work on each servo.
 *  F_CPU/(8*1000) = number of timer ticks in 1 millisecond with a prescaler of 8.
 */
static const u16 MAX_PERIOD = (20 * F_CPU) / (8 * 1000) / NUM_SERVOS;

//! Index of the servo currently being controlled.
static volatile u08 activeServoNumber = 0;
//! State of the active servo's output pulse.
static volatile u08 high = 0;
//! Array of the pulse widths (high times) of all servos, in timer counts.
static u16 servoHighTime[NUM_SERVOS];
//! Array of the pulse widths (low times) of all servos, in timer counts.
static u16 servoLowTime[NUM_SERVOS];
//! Array of the range multipliers of all servos.
static u08 servoRangeValues[NUM_SERVOS];

/*
Derivation of formula in servo()/servo2() and ServoRange enum values:

The traditional servo pulse-width range is 1ms to 2ms with 1.5ms as the center.
With a clock speed of 16E6 and timer prescaler of 8, the ideal ticks for standard center and end positions are:
16E6 [clocks/second] * 1/8 [tick/clocks] * 1/1000 [second/ms] * 1.0 [ms] = 2000 ticks for the min position
16E6 [clocks/second] * 1/8 [tick/clocks] * 1/1000 [second/ms] * 1.5 [ms] = 3000 ticks for the center position
16E6 [clocks/second] * 1/8 [tick/clocks] * 1/1000 [second/ms] * 2.0 [ms] = 4000 ticks for the max position

Assuming we want 128 to be the center position argument for the servoRange() function, we have:
0 to 127: 128 values below center
128 is center = 3000 ticks
129 to 255: 127 values above center

SERVO_RANGE_STANDARD - Closest match within the standard range uses a multiplier of 7:
position 0:   3000 - 128*7 = 2104 ticks, 2104/(16E6/8/1000) = 1.052 ms
position 255: 3000 + 127*7 = 3889 ticks, 3889/(16E6/8/1000) = 1.9445 ms

SERVO_RANGE_DEFAULT - Best match which only slightly exceeds the standard range uses a multiplier of 8:
position 0:   3000 - 128*8 = 1976 ticks, 1976/(16E6/8/1000) = 0.988 ms
position 255: 3000 + 127*8 = 4016 ticks, 4016/(16E6/8/1000) = 2.008 ms

SERVO_RANGE_EXTENDED1 - Extended range, using a multiplier of 9:
position 0:   3000 - 128*9 = 1848 ticks, 1848/(16E6/8/1000) = 0.924 ms
position 255: 3000 + 127*9 = 4143 ticks, 4143/(16E6/8/1000) = 2.0715 ms

SERVO_RANGE_EXTENDED2 - Extended range, using a multiplier of 10:
position 0:   3000 - 128*10 = 1720 ticks, 1720/(16E6/8/1000) = 0.86 ms
position 255: 3000 + 127*10 = 4270 ticks, 4270/(16E6/8/1000) = 2.135 ms

SERVO_RANGE_EXTENDED3 - Extended range, using a multiplier of 11:
position 0:   3000 - 128*11 = 1592 ticks, 1592/(16E6/8/1000) = 0.796 ms
position 255: 3000 + 127*11 = 4397 ticks, 4397/(16E6/8/1000) = 2.1985 ms

SERVO_RANGE_EXTENDED4 - Extended range, using a multiplier of 12:
position 0:   3000 - 128*12 = 1464 ticks, 1464/(16E6/8/1000) = 0.732 ms
position 255: 3000 + 127*12 = 4524 ticks, 4524/(16E6/8/1000) = 2.262 ms

The theoretical maximum range multiplier that could be used with all 8 servos enabled would be 15.
This is because 20 [ms] / 8 [servos] = 2.5 [ms/servo] and a range multiplier of 16 or above would
result in a max position with a pulse width longer than 2.5 ms, so the 20 ms servo period would not be met.
*/

/*! Sets the pulse-width range of a servo.
    The new range will not take effect until servo() or servo2() is called for this servo.
    @warning Most servos can support some additional range, but exactly how much varies;
    your servo might be damaged if you try to drive it beyond its supported range.
    You might want to test it manually first, incrementing the position gradually until it stops or buzzes.
    @param servoNum Selects the servo (0 to NUM_SERVOS-1).
    @param range Specifies a range setting for the servo.
    Valid values specified by the ::ServoRange enumeration.
 */
void servoRange(const u08 servoNum, const ServoRange range)
{
	//Validate servoNum parameter so that we don't overwrite other memory locations.
	if (servoNum < NUM_SERVOS)
	{
		servoRangeValues[servoNum] = range;
	}
}

/*! Sets the position of a servo using an unsigned integer.
    @param servoNum Selects the servo (0 to NUM_SERVOS-1).
    @param position Sets the servo position (0 to 255). 128 is the center value (1.5ms pulse width).
    @see Use servo2() instead if you prefer to pass position as a signed integer.
 */
void servo(const u08 servoNum, const u08 position)
{
	//Validate servoNum parameter so that we don't overwrite other memory locations.
	if (servoNum < NUM_SERVOS)
	{
		//Set the highTime for the servo, based on the configured range and commanded position.
		servoHighTime[servoNum] = 3000 + (servoRangeValues[servoNum] * (position - 128));

		//Set the lowTime for the servo to fill the remaining time for this servo period.
		servoLowTime[servoNum] = MAX_PERIOD - servoHighTime[servoNum];
	}
}

/*! Sets the position of a servo using a signed integer.
    @param servoNum Selects the servo (0 to NUM_SERVOS-1).
    @param position Sets the servo position (-128 to +127). 0 is the center value (1.5ms pulse width).
    @see Use servo() instead if you prefer to pass position as an unsigned integer.
 */
void servo2(const u08 servoNum, const s08 position)
{
	//Validate servoNum parameter so that we don't overwrite other memory locations.
	if (servoNum < NUM_SERVOS)
	{
		//Set the highTime for the servo based on the configured range and commanded position.
		servoHighTime[servoNum] = 3000 + (servoRangeValues[servoNum] * position);

		//Set the lowTime for the servo to fill the remaining time for this servo period.
		servoLowTime[servoNum] = MAX_PERIOD - servoHighTime[servoNum];
	}
}

/*! Turns a servo output off.
    Servo will still have power, but no PWM control pulses will be sent to the servo.
    @param servoNum Selects the servo (0 to NUM_SERVOS-1).
 */
void servoOff(const u08 servoNum)
{
	//Validate servoNum parameter so that we don't overwrite other memory locations.
	if (servoNum < NUM_SERVOS)
	{
		servoHighTime[servoNum] = 0;
		servoLowTime[servoNum] = MAX_PERIOD;
	}
}

/*! Writes to the octal D flip-flop that maintains the servo pin output values.
    @param servoOutput The new servo pin output values (only one will be high, if any).
 */
static inline void writeServoOutput(const u08 servoOutput)
{
	//put the new servo pin values on the data bus
	PORTC = servoOutput;
	//drive the octal D flip-flop's clock pin high to change it's values
	sbi(PORTD, PD5);
	//brief delay to ensure the values fully propagate to the D flip-flop
	_delay_loop_1(1);
	//drive the octal D flip-flop's clock pin low to latch the new values
	cbi(PORTD, PD5);
}

//! This is the interrupt service routine to control 1-8 servos.
ISR(TIMER3_COMPC_vect)
{
	if (high == 1)
	{
		//servo output was previously high, so set it low
		writeServoOutput(0);
		OCR3C += servoLowTime[activeServoNumber];
		high = 0;
	}
	else
	{
		activeServoNumber++;

		//when activeServoNumber goes beyond the last enabled servo, start over at servo 0
		if (activeServoNumber >= NUM_SERVOS)
		{
			activeServoNumber = 0;
		}

		//if the servo was not turned off via servoOff()
		if (servoHighTime[activeServoNumber] > 0)
		{
			writeServoOutput(_BV(activeServoNumber));
			OCR3C += servoHighTime[activeServoNumber];
			high = 1;
		}
		else
		{
			OCR3C += servoLowTime[activeServoNumber];
		}
	}
}

/*! Initializes the servo timer and variables.
    Normally called only by the initialize() function in utility.c.
 */
inline void servoInit()
{
	//Configure all servos with the default range and turn them off.
	u08 i;
	for (i = 0; i < NUM_SERVOS; i++)
	{
		servoOff(i);
		servoRange(i, SERVO_RANGE_DEFAULT);
	}

	//initialize D flip-flop to all lows
	writeServoOutput(0);

	//enable timer (set prescaler to /8)
	TCCR3B |= _BV(CS31);

	//enable interrupt for output compare unit 3C
	TIMSK3 |= _BV(OCIE3C);

	//enable interrupts
	sei();
}
