/* MCU frequency */
#define F_CPU 16000000UL

/* UART Baudrate */
#define BAUDRATE 57600

/* use "Double Speed Operation" (U2X) */
#define UART_DOUBLESPEED

/* use second UART on mega128 / can128 / mega162 / mega324p / mega644p / mega1281 */
//#define UART_USE_SECOND

/* Device-Type:
   For AVRProg the BOOT-option is prefered 
   which is the "correct" value for a bootloader.
   avrdude may only detect the part-code for ISP */
#define DEVTYPE     DEVTYPE_BOOT
//#define DEVTYPE     DEVTYPE_ISP


//Define the pin that has to be high to start the bootloader.
#define BLPORT		PORTG
#define BLDDR		DDRG
#define BLPIN		PING
#define BLNUM		0

//Define the pin for the button (used to skip past the bootloader).
#define BTNPORT		PORTD
#define BTNDDR		DDRD
#define BTNPIN		PIND
#define BTNNUM		4

//Define the pin that drives the LED.
#define LEDPORT		PORTG
#define LEDDDR		DDRG
#define LEDNUM		2


/*
 * Define if Watchdog-Timer should be disable at startup
 */
#define DISABLE_WDT_AT_STARTUP

/*
 * Watchdog-reset is issued at exit 
 * define the timeout-value here (see avr-libc manual)
 */
#define EXIT_WDT_TIME   WDTO_250MS

/*
 * SIMPLE-Mode - Jump to bootloader main BL-loop if key is
 *   pressed (Pin grounded) "during" reset or jump to the
 *   application if the pin is not grounded. The internal
 *   pull-up resistor is enabled during the startup and
 *   gets disabled before the application is started.
 */

/*
 * enable/disable readout of fuse and lock-bits
 * (AVRPROG has to detect the AVR correctly by device-code
 * to show the correct information).
 */
//#define ENABLEREADFUSELOCK

/*
 * define the following if the bootloader should not output
 * itself at flash read (will fake an empty boot-section)
 */
#define READ_PROTECT_BOOTLOADER


#define VERSION_HIGH '1'
#define VERSION_LOW  '0'

#define GET_LOCK_BITS           0x0001
#define GET_LOW_FUSE_BITS       0x0000
#define GET_HIGH_FUSE_BITS      0x0003
#define GET_EXTENDED_FUSE_BITS  0x0002


#ifdef UART_DOUBLESPEED
// #define UART_CALC_BAUDRATE(baudRate) (((F_CPU*10UL) / ((baudRate) *8UL) +5)/10 -1)
#define UART_CALC_BAUDRATE(baudRate) ((uint32_t)((F_CPU) + ((uint32_t)baudRate * 4UL)) / ((uint32_t)(baudRate) * 8UL) - 1)
#else
// #define UART_CALC_BAUDRATE(baudRate) (((F_CPU*10UL) / ((baudRate)*16UL) +5)/10 -1)
#define UART_CALC_BAUDRATE(baudRate) ((uint32_t)((F_CPU) + ((uint32_t)baudRate * 8UL)) / ((uint32_t)(baudRate) * 16UL) - 1)
#endif

//Bit manipulation macros
#define sbi(a, b) ((a) |= 1 << (b))       //sets bit b in variable a
#define cbi(a, b) ((a) &= ~(1 << (b)))    //clears bit b in variable a

//Define new datatypes as easier shorthand
typedef unsigned char u08;
typedef unsigned int  u16;
