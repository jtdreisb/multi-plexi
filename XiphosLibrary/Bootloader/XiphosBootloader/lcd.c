#include "main.h"
#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"

//LCD RAM addresses
#define HOME         0x80
#define SECOND_LINE  0XC0

//Macro function to reverse the bit order of an 8-bit variable.
#define REVERSE(a) do                     \
{                                         \
  a=((a>>1)&0x55)|((a<<1)&0xaa);          \
  a=((a>>2)&0x33)|((a<<2)&0xcc);          \
  asm volatile("swap %0":"=r"(a):"0"(a)); \
} while (0)

//Provides a busy wait loop for an approximate number of microseconds
void delayUs(u16 num)
{
	for (; num > 0; num--)
	{
		_delay_loop_2(4);
	}
}

//Writes a byte of data to the LCD.
void writeLcd(u08 data)
{
	//Reverse the bit order of the data, due to LCD connections to data bus being backwards.
	REVERSE(data);
	//set the LCD's E (Enable) line high, so it can fall later
	sbi(PORTD, 6);
	//write the data to the bus
	PORTC = data;
	//delay to allow the data to fully propagate to the LCD
	delayUs(1);
	//set the LCD's E (Enable) line low to latch in the data
	cbi(PORTD, 6);
}

//Writes a command byte to the LCD.
void writeControl(u08 data)
{
	//set RS (Register Select) line low to select command register
	cbi(PORTD, 7);
	writeLcd(data);
	//wait for the instruction to be executed
	delayUs(100);
}

//Clears all characters on the display and resets the cursor to the home position.
void clearScreen(void)
{
	writeControl(0x01);
	delayUs(3300);
}

//Initializes the LCD as described in the HD44780 datasheet.
//Normally called only by the initialize() function in utility.c.
void lcdInit(void)
{
	//configure LCD E (Enable) control pin as an output
	sbi(DDRD, 6);
	//configure LCD RS (Register Select) control pin as an output
	sbi(DDRD, 7);
	//set LCD E (Enable) line low inititally, so it can rise later
	cbi(PORTD, 6);

	//wait 15ms after power on
	delayUs(15000);

	//Issue 'Function Set' commands to initialize LCD for 8-bit interface mode
	writeControl(0x38);
	delayUs(4900); //+100us in writeControl = 5000us or 5ms total
	writeControl(0x38);
	delayUs(50); //+100us in writeControl = 150us total
	writeControl(0x38);

	//Function Set command to specify 2 display lines and character font
	writeControl(0x38);

	//Display off
	writeControl(0x08);

	//Clear display
	clearScreen();

	//Set entry mode
	writeControl(0x06);

	//Display on
	writeControl(0x0C);
}

//Prints a single character specified by its ASCII code to the display.
void printChar(u08 data)
{
	//set RS (Register Select) line high to select data register
	sbi(PORTD, 7);
	writeLcd(data);
	delayUs(50);
}

//Prints a string starting at the current cursor position.
//Strings exceeding the length of the display go into the display's buffer.
//This is useful if you are using the scrolling option, but wastes write time otherwise.
//Once the buffer for the first line has been filled, the cursor wraps to the next line.
void printString(char* string)
{
	u08 i = 0;
	//keep printing until we encounter a null terminator
	while (string[i] != 0)
	{
		printChar(string[i++]);
	}
}

/*
//Moves the LCD cursor to the beginning of the first line of the display (row 0).
void upperLine(void)
{
	writeControl(HOME);
}
*/

//Moves the LCD cursor to the beginning of the second line of the display (row 1).
void lowerLine(void)
{
	writeControl(SECOND_LINE);
}

/*
//Moves the LCD cursor position to row <row> and column <col>.
//<row> ranges from 0 to 1.
//<col> ranges from 0 to 15.
void lcdCursor(u08 row, u08 col)
{
	writeControl(HOME | (row << 6) | (col % 17));
}
*/
