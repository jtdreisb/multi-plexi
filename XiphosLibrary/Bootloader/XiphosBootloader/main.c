//Xiphos 1.0 Bootloader
//Modified 11-22-2009 by Patrick McCarty
//Original source from Martin Thomas (see license block below)

/*
Once installed, this bootloader can be used with avrdude like this:
avrdude -p atmega1281 -P COM1 -c butterfly -b 57600 -u -U flash:w:yourproject.hex
*/

/*****************************************************************************
*
* AVRPROG compatible boot-loader
* Version  : 0.85 (Dec. 2008)
* Compiler : avr-gcc 4.1.2 / avr-libc 1.4.6
* size     : depends on features and startup ( minmal features < 512 words)
* by       : Martin Thomas, Kaiserslautern, Germany
*            eversmith@heizung-thomas.de
*            Additional code and improvements contributed by:
*           - Uwe Bonnes
*           - Bjoern Riemer
*           - Olaf Rempel
*
* License  : Copyright (c) 2006-2008 M. Thomas, U. Bonnes, O. Rempel
*            Free to use. You have to mention the copyright
*            owners in source-code and documentation of derived
*            work. No warranty! (Yes, you can insert the BSD
*            license here)
*
* Tested with ATmega8, ATmega16, ATmega162, ATmega32, ATmega324P,
*             ATmega644, ATmega644P, ATmega128, AT90CAN128
*
* - Initial versions have been based on the Butterfly bootloader-code
*   by Atmel Corporation (Authors: BBrandal, PKastnes, ARodland, LHM)
*
****************************************************************************
*
*  See the makefile and readme.txt for information on how to adapt 
*  the linker-settings to the selected Boot Size (BOOTSIZE=xxxx) and 
*  the MCU-type. Other configurations futher down in this file.
*
*  With BOOT_SIMPLE, minimal features and discarded int-vectors
*  this bootloader has should fit into a a 512 word (1024, 0x400 bytes) 
*  bootloader-section. 
*
****************************************************************************/
/*
	TODOs:
	- check lock-bits set
	- __bad_interrupt still linked even with modified 
	  linker-scripts which needs a default-handler,
	  "wasted": 3 words for AVR5 (>8kB), 2 words for AVR4
	- Check watchdog-disable-function in avr-libc.
*/

#include "main.h"
#include <stdint.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/boot.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "chipdef.h"
#include "lcd.h"

uint8_t gBuffer[SPM_PAGESIZE];

#if defined(BOOTLOADERHASNOVECTORS)
#warning "This Bootloader does not link interrupt vectors - see makefile"
/* make the linker happy - it wants to see __vector_default */
// void __vector_default(void) { ; }
void __vector_default(void) { ; }
#endif

static void sendchar(uint8_t data)
{
	while (!(UART_STATUS & (1<<UART_TXREADY)));
	UART_DATA = data;
}

static uint8_t recvchar(void)
{
	while (!(UART_STATUS & (1<<UART_RXREADY)));
	return UART_DATA;
}

static inline void eraseFlash(void)
{
	// erase only main section (bootloader protection)
	uint32_t addr = 0;
	while (APP_END > addr) {
		boot_page_erase(addr);		// Perform page erase
		boot_spm_busy_wait();		// Wait until the memory is erased.
		addr += SPM_PAGESIZE;
	}
	boot_rww_enable();
}

static inline void recvBuffer(pagebuf_t size)
{
	pagebuf_t cnt;
	uint8_t *tmp = gBuffer;

	for (cnt = 0; cnt < sizeof(gBuffer); cnt++) {
		*tmp++ = (cnt < size) ? recvchar() : 0xFF;
	}
}

static inline uint16_t writeFlashPage(uint16_t waddr, pagebuf_t size)
{
	uint32_t pagestart = (uint32_t)waddr<<1;
	uint32_t baddr = pagestart;
	uint16_t data;
	uint8_t *tmp = gBuffer;

	do {
		data = *tmp++;
		data |= *tmp++ << 8;
		boot_page_fill(baddr, data);	// call asm routine.

		baddr += 2;			// Select next word in memory
		size -= 2;			// Reduce number of bytes to write by two
	} while (size);				// Loop until all bytes written

	boot_page_write(pagestart);
	boot_spm_busy_wait();
	boot_rww_enable();		// Re-enable the RWW section

	return baddr>>1;
}

static inline uint16_t writeEEpromPage(uint16_t address, pagebuf_t size)
{
	uint8_t *tmp = gBuffer;

	do {
		eeprom_write_byte( (uint8_t*)address, *tmp++ );
		address++;			// Select next byte
		size--;				// Decreas number of bytes to write
	} while (size);				// Loop until all bytes written

	// eeprom_busy_wait();

	return address;
}

static inline uint16_t readFlashPage(uint16_t waddr, pagebuf_t size)
{
	uint32_t baddr = (uint32_t)waddr<<1;
	uint16_t data;

	do {
#ifndef READ_PROTECT_BOOTLOADER
#warning "Bootloader not read-protected"
#if defined(RAMPZ)
		data = pgm_read_word_far(baddr);
#else
		data = pgm_read_word_near(baddr);
#endif
#else
		// don't read bootloader
		if ( baddr < APP_END ) {
#if defined(RAMPZ)
			data = pgm_read_word_far(baddr);
#else
			data = pgm_read_word_near(baddr);
#endif
		}
		else {
			data = 0xFFFF; // fake empty
		}
#endif
		sendchar(data);			// send LSB
		sendchar((data >> 8));		// send MSB
		baddr += 2;			// Select next word in memory
		size -= 2;			// Subtract two bytes from number of bytes to read
	} while (size);				// Repeat until block has been read

	return baddr>>1;
}

static inline uint16_t readEEpromPage(uint16_t address, pagebuf_t size)
{
	do {
		sendchar( eeprom_read_byte( (uint8_t*)address ) );
		address++;
		size--;				// Decrease number of bytes to read
	} while (size);				// Repeat until block has been read

	return address;
}

#if defined(ENABLEREADFUSELOCK)
static uint8_t read_fuse_lock(uint16_t addr)
{
	uint8_t mode = (1<<BLBSET) | (1<<SPMEN);
	uint8_t retval;

	asm volatile
	(
		"movw r30, %3\n\t"		/* Z to addr */ \
		"sts %0, %2\n\t"		/* set mode in SPM_REG */ \
		"lpm\n\t"			/* load fuse/lock value into r0 */ \
		"mov %1,r0\n\t"			/* save return value */ \
		: "=m" (SPM_REG),
		  "=r" (retval)
		: "r" (mode),
		  "r" (addr)
		: "r30", "r31", "r0"
	);
	return retval;
}
#endif

//Return software identifier (aka programmer signature). Always 7 characters.
static void send_boot(void)
{
	sendchar('X');
	sendchar('i');
	sendchar('p');
	sendchar('h');
	sendchar('o');
	sendchar('s');
	sendchar(' ');
}

static void (*jump_to_app)(void) = 0x0000;

int main(void)
{
	uint16_t address = 0;
	uint8_t device = 0, val;

	#ifdef DISABLE_WDT_AT_STARTUP
		#ifdef WDT_OFF_SPECIAL
			#warning "using target specific watchdog_off"
			bootloader_wdt_off();
		#else
			cli();
			wdt_reset();
			wdt_disable();
		#endif
	#endif

	//Set baud rate
	UART_BAUD_HIGH = (UART_CALC_BAUDRATE(BAUDRATE)>>8) & 0xFF;
	UART_BAUD_LOW = (UART_CALC_BAUDRATE(BAUDRATE) & 0xFF);

	#ifdef UART_DOUBLESPEED
		UART_STATUS = (1<<UART_DOUBLE);
	#endif

	UART_CTRL = UART_CTRL_DATA;
	UART_CTRL2 = UART_CTRL2_DATA;

	//Set bootloader selection pin as an input
	cbi(BLDDR, BLNUM);

	//if USB cable is not plugged in (pin is low), jump to app
	if ((BLPIN & (1<<BLNUM)) == 0) {
		#ifdef UART_DOUBLESPEED
			UART_STATUS &= ~( 1<<UART_DOUBLE );
		#endif
		jump_to_app();			// Jump to application sector
	}

	//configure button as an input
	cbi(BTNDDR, BTNNUM);
	//enable pullup for button
	sbi(BTNPORT, BTNNUM);

	//set LED as output
	sbi(LEDDDR, LEDNUM);

	//configure LCD/Servo bus on port C as an output
	DDRC = 0xFF;

	//initialize LCD display
	lcdInit();
	//print bootloader start message
	printString("   Xiphos 1.0   ");
	lowerLine();


	u08 counter;
	u08 gotChar = 0;
	u16 i, j;

	//count down 10 seconds (9 to 0)
	for (counter = 10; ((counter > 0) && (gotChar == 0)); counter--)
	{
		//print the countdown number
		printChar(counter - 1 + 48);
		//turn LED on (so it appears to blink every second)
		sbi(LEDPORT, LEDNUM);

		//loop about 1000 times per second
		for (i = 0; ((i < 1000) && !gotChar); i++)
		{
			//check button every millisecond, if pressed jump to user program.
			if (!(BTNPIN & (1<<BTNNUM)))
			{
				//wait for button to be released
				while (!(BTNPIN & (1<<BTNNUM)));
				//delay 30 ms for button debouncing
				delayUs(30000);

				#ifdef UART_DOUBLESPEED
					UART_STATUS &= ~(1<<UART_DOUBLE);
				#endif

				//Break out of the loop. set gotChar to 2 so we will 
				//turn off the LED and jump to the application sector.
				gotChar = 2;
				break;
			}

			//turn LED off after 100ms (so it appears to blink every second)
			if (i == 100)
			{
				cbi(LEDPORT, LEDNUM);
			}

			//delay about 1ms, and check for incoming characters about every 100 us
			for (j = 0; j < 10; j++)
			{
				_delay_loop_2(400);
				if (UART_STATUS & (1<<UART_RXREADY))
				{
					//got a character
					gotChar = 1;
					break;
				}
			}
		}
	}

	//if no character was received, then we turn off LED and jump to user program.
	if (gotChar != 1)
	{
		#ifdef UART_DOUBLESPEED
			UART_STATUS &= ~(1<<UART_DOUBLE);
		#endif

		//turn off LED in case it is still on from countdown blinking
		cbi(LEDPORT, LEDNUM);
		//jump to application sector
		jump_to_app();
	}

	//turn LED on
	sbi(LEDPORT, LEDNUM);
	lowerLine();
	printString("Downloading...");

	//otherwise, we enter the UART parsing loop
	for(;;) {
		val = recvchar();
		// Autoincrement?
		if (val == 'a') {
			sendchar('Y');			// Autoincrement is quicker

		// Write address
		} else if (val == 'A') {
			address = recvchar();		//read address 8 MSB
			address = (address<<8) | recvchar();
			sendchar('\r');

		// Buffer load support
		} else if (val == 'b') {
			sendchar('Y');					// Report buffer load supported
			sendchar((sizeof(gBuffer) >> 8) & 0xFF);	// Report buffer size in bytes
			sendchar(sizeof(gBuffer) & 0xFF);

		// Start buffer load
		} else if (val == 'B') {
			pagebuf_t size;
			size = recvchar() << 8;				// Load high byte of buffersize
			size |= recvchar();				// Load low byte of buffersize
			val = recvchar();				// Load memory type ('E' or 'F')
			recvBuffer(size);

			if (device == DEVTYPE) {
				if (val == 'F') {
					address = writeFlashPage(address, size);
				} else if (val == 'E') {
					address = writeEEpromPage(address, size);
				}
				sendchar('\r');
			} else {
				sendchar(0);
			}

		// Block read
		} else if (val == 'g') {
			pagebuf_t size;
			size = recvchar() << 8;				// Load high byte of buffersize
			size |= recvchar();				// Load low byte of buffersize
			val = recvchar();				// Get memtype

			if (val == 'F') {
				address = readFlashPage(address, size);
			} else if (val == 'E') {
				address = readEEpromPage(address, size);
			}

		// Chip erase
 		} else if (val == 'e') {
			if (device == DEVTYPE) {
				eraseFlash();
			}
			sendchar('\r');

		// Exit upgrade
		} else if (val == 'E') {
			//turn LED off
			cbi(LEDPORT, LEDNUM);

			sendchar('\r');
			//wait for the '\r' to get sent
			while (!(UART_STATUS & (1<<UART_TXREADY)));
			delayUs(1000);

			#ifdef UART_DOUBLESPEED
				UART_STATUS &= ~(1<<UART_DOUBLE);
			#endif

			//jump to application sector
			jump_to_app();

		// Enter programming mode
		} else if (val == 'P') {
			sendchar('\r');

		// Leave programming mode
		} else if (val == 'L') {
			sendchar('\r');

		// Return programmer type
		} else if (val == 'p') {
			sendchar('S');		// always serial programmer

#ifdef ENABLEREADFUSELOCK
#warning "Extension 'ReadFuseLock' enabled"
		// Read "low" fuse bits
		} else if (val == 'F') {
			sendchar(read_fuse_lock(GET_LOW_FUSE_BITS));

		// Read lock bits
		} else if (val == 'r') {
			sendchar(read_fuse_lock(GET_LOCK_BITS));

		// Read high fuse bits
		} else if (val == 'N') {
			sendchar(read_fuse_lock(GET_HIGH_FUSE_BITS));

		// Read extended fuse bits
		} else if (val == 'Q') {
			sendchar(read_fuse_lock(GET_EXTENDED_FUSE_BITS));
#endif

		// Return device type
		} else if (val == 't') {
			sendchar(DEVTYPE);
			sendchar(0); //Send list terminator.

		// Set LED
		} else if (val == 'x') {
			recvchar();
			sbi(LEDPORT, LEDNUM);
			sendchar('\r');

		// Clear LED
		} else if (val == 'y') {
			recvchar();
			cbi(LEDPORT, LEDNUM);
			sendchar('\r');

		// Set device
		} else if (val == 'T') {
			device = recvchar();
			sendchar('\r');

		// Return software identifier
		} else if (val == 'S') {
			send_boot();

		// Return Software Version
		} else if (val == 'V') {
			sendchar(VERSION_HIGH);
			sendchar(VERSION_LOW);

		// Return Signature Bytes (it seems that 
		// AVRProg expects the "Atmel-byte" 0x1E last
		// but shows it first in the dialog-window)
		} else if (val == 's') {
			sendchar(SIG_BYTE3);
			sendchar(SIG_BYTE2);
			sendchar(SIG_BYTE1);

		/* ESC */
		} else if(val != 0x1b) {
			sendchar('?');
		}
	}
	return 0;
}
