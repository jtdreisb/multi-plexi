//Copyright (C) 2009-2010  Patrick J. McCarty.
//Licensed under X11 License. See LICENSE.txt for details.

/*! @file
    Implements support for a generic 16-column x 2-row LCD display compatible with
    the common HD44780 LCD driver chip. It is interfaced in 8-bit mode (8 data lines)
    and uses an additional 2 control lines: E (Enable) and RS (Register Select).
    The R/W (Read/Write) pin is hardwired to ground so the LCD is write-only. Therefore, this
    driver uses fixed delays following every command to the LCD instead of polling the LCD's
    status register. Consequently, it may delay longer than necessary, but it saves a pin.
 */

#include "globals.h"

//! LCD RAM address for the home position (row 0, col 0).
#define HOME         0x80
//! LCD RAM address for the second line (row 1, col 0).
#define SECOND_LINE  0XC0

/*! Macro function to reverse the bit order of an 8-bit variable as efficiently as possible.
    Should compile down to just 15 AVR assembly instructions, running in 15 clock cycles.
    Note the use of the swap assembly instruction to swap the two nibbles of a register.
    @param a The 8-bit variable to reverse.
 */
#define REVERSE(a) do                     \
{                                         \
  a=((a>>1)&0x55)|((a<<1)&0xAA);          \
  a=((a>>2)&0x33)|((a<<2)&0xCC);          \
  asm volatile("swap %0":"=r"(a):"0"(a)); \
} while (0)

//! Writes a byte of data to the LCD.
static void writeLcd(u08 data)
{
	//Reverse the bit order of the data, due to LCD connections to the data bus being backwards.
	//This line doesn't affect the bus, so this can be done before the interrupts are disabled.
	REVERSE(data);
	//Disable interrupts to prevent the servo ISR (which shares the same data bus)
	//from interrupting in the middle of the sequence and messing with the bus.
	cli();
	//set the LCD's E (Enable) line high, so it can fall later
	sbi(PORTD, PD6);
	//write the data to the bus
	PORTC = data;
	//delay to allow the data to fully propagate to the LCD
	delayUs(1);
	//set the LCD's E (Enable) line low to latch in the data
	cbi(PORTD, PD6);
	//re-enable interrupts
	sei();
}

//! Writes a command byte to the LCD.
static void writeControl(const u08 data)
{
	//set RS (Register Select) line low to select command register
	cbi(PORTD, PD7);
	writeLcd(data);
	//wait for the instruction to be executed
	delayUs(100);
}

//! Clears all characters on the display and resets the cursor to the home position.
void clearScreen()
{
	writeControl(0x01);
	delayUs(3300);
}

//! Shows the characters on the screen, if they were hidden with lcdOff().
void lcdOn()
{
	writeControl(0x0C);
}

//! Hides the characters on the screen. Can be unhidden again with lcdOn().
void lcdOff()
{
	writeControl(0x08);
}

/*! Initializes the LCD as described in the HD44780 datasheet.
    Normally called only by the initialize() function in utility.c.
 */
inline void lcdInit()
{
	//configure LCD E (Enable) control pin as an output
	sbi(DDRD, DDD6);
	//configure LCD RS (Register Select) control pin as an output
	sbi(DDRD, DDD7);
	//set LCD E (Enable) line low inititally, so it can rise later
	cbi(PORTD, PD6);

	//wait 15ms after power on
	delayMs(15);

	//Issue 'Function Set' commands to initialize LCD for 8-bit interface mode
	writeControl(0x38);
	delayUs(4900); //+100us in writeControl = 5000us or 5ms total
	writeControl(0x38);
	delayUs(50); //+100us in writeControl = 150us total
	writeControl(0x38);

	//Function Set command to specify 2 display lines and character font
	writeControl(0x38);

	//Display off
	lcdOff();

	//Clear display
	clearScreen();

	//Set entry mode
	writeControl(0x06);

	//Display on
	lcdOn();
}

/*! Prints a single character specified by its ASCII code to the display.
    Most LCDs can also print some special characters, such as those in LCDSpecialChars.h.
 */
void printChar(const u08 data)
{
	//set RS (Register Select) line high to select data register
	sbi(PORTD, PD7);
	writeLcd(data);
	delayUs(50);
}

/*! Prints a null-terminated string starting at the current cursor position.
    Strings exceeding the length of the display go into the display's buffer.
    This is useful if you are using the scrolling option, but wastes write time otherwise.
    Once the buffer for the first line has been filled, the cursor wraps to the next line.
 */
void printString(const char *const string)
{
	u08 i = 0;
	//keep printing until we encounter a null terminator
	while (string[i] != 0)
	{
		printChar(string[i++]);
	}
}

//! Prints an 8-bit unsigned integer as 3 digits (with '0' padding in front). Example: "007"
void print_u08(const u08 number)
{
	//print hundreds digit
	printChar((number / 100) + 48);
	//print tens digit
	printChar(((number % 100 ) / 10) + 48);
	//print ones digit
	printChar((number % 10) + 48);
}

//! Prints an 8-bit unsigned integer as 1-3 digits (no padding). Example: "7"
void printPlain_u08(const u08 number)
{
	//if there is a digit in the tens place
	if (number >= 10)
	{
		//if there is a digit in the hundreds place
		if (number >= 100)
		{
			//print the hundreds digit
			printChar((number / 100) + 48);
		}
		//print the tens digit
		printChar(((number % 100 ) / 10) + 48);
	}
	//always print the ones digit
	printChar((number % 10) + 48);
}

//! Prints an 8-bit signed integer as +/- and 3 digits (with '0' padding in front). Example: "+007"
void print_s08(const s08 number)
{
	if (number >= 0)
	{
		//print plus
		printChar('+');
		//cast to print the number as a u08
		print_u08((u08)number);
	}
	else
	{
		//print minus
		printChar('-');
		//make the number positive and print it as a u08
		print_u08((u08)(-number));
	}
}

//! Prints an 8-bit signed integer as +/- and 1-3 digits (no padding). Example: "+7"
void printPlain_s08(const s08 number)
{
	if (number >= 0)
	{
		//print plus
		printChar('+');
		//cast to print the number as a u08
		printPlain_u08((u08)number);
	}
	else
	{
		//print minus
		printChar('-');
		//make the number positive and print it as a u08
		printPlain_u08((u08)(-number));
	}
}

//! Prints a 16-bit unsigned integer as 5 digits (with '0' padding in front). Example: "00042"
void print_u16(const u16 number)
{
	//print the ten-thousands digit
	printChar((number / 10000) + 48);
	//print the thousands digit
	printChar(((number % 10000) / 1000) + 48);
	//print the hundreds digit
	printChar(((number % 1000) / 100) + 48);
	//print the tens digit
	printChar(((number % 100 ) / 10) + 48);
	//print the ones digit
	printChar((number % 10) + 48);
}

//! Prints a 16-bit unsigned integer as 1-5 digits (no padding). Example: "42"
void printPlain_u16(const u16 number)
{
	//if there is a digit in the tens place
	if (number >= 10)
	{
		//if there is a digit in the hundreds place
		if (number >= 100)
		{
			//if there is a digit in the thousands place
			if (number >= 1000)
			{
				//if there is a digit in the ten-thousands place
				if (number >= 10000)
				{
					//print the ten-thousands digit
					printChar((number / 10000) + 48);
				}
				//print the thousands digit
				printChar(((number % 10000) / 1000) + 48);
			}
			//print the hundreds digit
			printChar(((number % 1000) / 100) + 48);
		}
		//print the tens digit
		printChar(((number % 100 ) / 10) + 48);
	}
	//always print the ones digit
	printChar((number % 10) + 48);
}

//! Prints a 16-bit signed integer as +/- and 5 digits (with '0' padding in front). Example: "+00042"
void print_s16(const s16 number)
{
	if (number >= 0)
	{
		//print plus
		printChar('+');
		//cast to print the number as a u16
		print_u16((u16)number);
	}
	else
	{
		//print minus
		printChar('-');
		//make the number positive and print it as a u16
		print_u16((u16)(-number));
	}
}

//! Prints a 16-bit signed integer as +/- and 1-5 digits (no padding). Example: "+42"
void printPlain_s16(const s16 number)
{
	if (number >= 0)
	{
		//print plus
		printChar('+');
		//cast to print the number as a u16
		printPlain_u16((u16)number);
	}
	else
	{
		//print minus
		printChar('-');
		//make the number positive and print it as a u16
		printPlain_u16((u16)(-number));
	}
}

//! Prints a single uppercase hex digit representing the lower 4 bits of the argument.
void printHexDigit(u08 nibble)
{
	//strip off everything except the lower nibble (4 bits)
	nibble &= 0x0F;
	//check what ASCII character offset we need to add
	if (nibble < 10)
	{
		//print 0 through 9. ASCII char '0' evaluates to 48.
		printChar(nibble + '0');
	}
	else
	{
		//print A through F. You can change to lowercase if you want by using 'a' instead.
		//'A' (ASCII char 65) - 10 = 55
		//'a' (ASCII char 97) - 10 = 87
		printChar(nibble + ('A' - 10));
	}
}

//! Prints an 8-bit unsigned integer as 2 hex digits. Example(255): "FF"
void printHex_u08(const u08 number)
{
	//print the upper nibble
	printHexDigit(number >> 4);
	//print the lower nibble
	printHexDigit(number);
}

//! Prints a 16-bit unsigned integer as 4 hex digits. Example(65535): "FFFF"
void printHex_u16(const u16 number)
{
	//print the high nibble
	printHexDigit((u08)(number >> 12));
	//print the upper middle nibble
	printHexDigit((u08)(number >> 8));
	//print the lower middle nibble
	printHexDigit(((u08)number) >> 4);
	//print the low nibble
	printHexDigit((u08)number);
}

//! Prints a floating point number.
void printFloat(const float number)
{
	char s[10];
	dtostre(number, s, 3, 0);
	printString(s);
}

//! Moves the LCD cursor to the beginning of the first line of the display (row 0, col 0).
void upperLine()
{
	writeControl(HOME);
}

//! Moves the LCD cursor to the beginning of the second line of the display (row 1, col 0).
void lowerLine()
{
	writeControl(SECOND_LINE);
}

/*! Moves the LCD cursor position directly to the specified row and column.
    @param row Valid row values are 0 to 1.
    @param column Valid column values are 0 to 15.
 */
void lcdCursor(const u08 row, const u08 column)
{
	writeControl(HOME | (row << 6) | (column % 17));
}
