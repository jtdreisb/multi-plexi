//Copyright (C) 2009-2010  Patrick J. McCarty.
//Licensed under X11 License. See LICENSE.txt for details.

/*! @file
    Global defines.
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#include <avr/io.h>          //I/O definitions (port/pin names, register names, named register bits, etc.)
#include <avr/interrupt.h>   //interrupt support
//#include "projectGlobals.h"  //user-configured project-specific global #defines. To work, this requires "-I ." in the avr-gcc arguments.

#if USE_LCD == 1
	#include "LCDSpecialChars.h" //defines friendly names for various LCD special character codes.
#endif

//! CPU speed (16MHz) for timer and delay loop calculations.
#define F_CPU 16000000UL

//! Define a Boolean type and boolean values.
typedef enum
{
	FALSE = 0,
	TRUE = 1
} bool;

//! Specifies the valid values that can be passed to the digitalDirection() function.
typedef enum
{
	INPUT,
	OUTPUT,
	INPUT_PULLUP
} DigitalDirection;

//! Specifies the valid values that can be passed to the servoRange() function.
typedef enum
{
	SERVO_RANGE_STANDARD = 7,
	SERVO_RANGE_DEFAULT = 8,
	SERVO_RANGE_EXTENDED1 = 9,
	SERVO_RANGE_EXTENDED2 = 10,
	SERVO_RANGE_EXTENDED3 = 11,
	SERVO_RANGE_EXTENDED4 = 12,
} ServoRange;

//Define new datatypes as easier shorthand
typedef unsigned char u08; //!< Unsigned 8-bit integer, range: 0 to +255
typedef signed char   s08; //!< Signed 8-bit integer, range: -128 to +127
typedef unsigned int  u16; //!< Unsigned 16-bit integer, range: 0 to +65,535
typedef signed int    s16; //!< Signed 16-bit integer, range: -32,768 to +32,767
typedef unsigned long u32; //!< Unsigned 32-bit integer, range: 0 to +4,294,967,295
typedef signed long   s32; //!< Signed 32-bit integer, range: -2,147,483,648 to +2,147,483,647

//Bit manipulation macros
#define sbi(a, b) ((a) |= 1 << (b))       //!< Sets bit b in variable a.
#define cbi(a, b) ((a) &= ~(1 << (b)))    //!< Clears bit b in variable a.
#define tbi(a, b) ((a) ^= 1 << (b))       //!< Toggles bit b in variable a.
#define gbi(a, b) ((a) & (1 << (b)))      //!< Gets bit b in variable a (masks out everything else).
#define gbis(a, b) (gbi((a), (b)) >> (b)) //!< Gets bit b in variable a and shifts it to the LSB.

#endif //ifndef GLOBALS_H
