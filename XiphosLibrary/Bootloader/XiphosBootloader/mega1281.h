#ifndef _MEGA1281_H_
#define _MEGA1281_H_

// Part-Code ISP - Don't think the 1281 has one, just copied this from the Atmega128.
// According to avrdude.conf, this code is from an extended version of AVR910.
#define DEVTYPE_ISP     0x43
// Part-Code Boot - Don't think the 1281 has one, just copied this from the Atmega128.
// According to avrdude.conf, this is an AVR910 device code from Appnote AVR109.
#define DEVTYPE_BOOT    0x44

//This signature is unique to the Atmega1281 and can be found in its datasheet.
#define SIG_BYTE1	0x1E
#define SIG_BYTE2	0x97
#define SIG_BYTE3	0x04

#ifndef UART_USE_SECOND
#define UART_BAUD_HIGH	UBRR0H
#define UART_BAUD_LOW	UBRR0L
#define UART_STATUS	UCSR0A
#define UART_TXREADY	UDRE0
#define UART_RXREADY	RXC0
#define UART_DOUBLE	U2X0
#define UART_CTRL	UCSR0B
#define UART_CTRL_DATA	((1<<TXEN0) | (1<<RXEN0))
#define UART_CTRL2	UCSR0C
#define UART_CTRL2_DATA	((1<<UCSZ01) | (1<<UCSZ00))
#define UART_DATA	UDR0
#else
#define UART_BAUD_HIGH	UBRR1H
#define UART_BAUD_LOW	UBRR1L
#define UART_STATUS	UCSR1A
#define UART_TXREADY	UDRE1
#define UART_RXREADY	RXC1
#define UART_DOUBLE	U2X1
#define UART_CTRL	UCSR1B
#define UART_CTRL_DATA	((1<<TXEN1) | (1<<RXEN1))
#define UART_CTRL2	UCSR1C
#define UART_CTRL2_DATA	((1<<UCSZ11) | (1<<UCSZ10))
#define UART_DATA	UDR1
#endif

#endif // #ifndef _MEGA1281_H_
