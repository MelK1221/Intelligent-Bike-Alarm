/* UART interface driver
* Sourced from https://github.com/Sovichea/avr-uart-library.git
* UART driver for AVR library by Joerg Wunsch, posted by Tep Sovichea
*/

/*
* ----------------------------------------------------------------------------
* "THE BEER-WARE LICENSE" (Revision 42):
* <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
* can do whatever you want with this stuff. If we meet some day, and you think
* this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
* ----------------------------------------------------------------------------
*
* Stdio demo, UART implementation
*
* $Id: uart.c,v 1.1 2005/12/28 21:38:59 joerg_wunsch Exp $
*
* Mod for mega644	BRL Jan2009
* Mod for mega328p	TS	Feb2019
*/


#include "uart.h"

char *cp, *cp2;
char b[RX_BUFSIZE];

/*
* Setup standard io stream to use UART functions
*/

FILE uart_io = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

/*
* Initialize the UART to tx/rx, 8N1.
*/
void uart_init(uint32_t baudrate)
{
	stdout = stdin = &uart_io;
	
	float ubrr_tmp = (float)F_CPU/16.0/baudrate-1;
	uint16_t ubrr = round(ubrr_tmp);
	
	UBRR0H = ubrr >> 8;
	UBRR0L = ubrr;
	
	/* Enable double speed */
	//UCSR0A |= (1 << U2X0);
	
	/* Enable receiver and transmitter */
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
	
	/* Set frame format: 8 data bits, parity None, 1 stop bit */
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);
	
	// initialize rx pointer
	cp = b;
}

/*
* Send character c down the UART Tx, wait until tx holding register
* is empty.
*/
int uart_putchar(char c, FILE *stream)
{
	if (c == '\a')
	{
		fputs("*ring*\n", stderr);
		return 0;
	}

	if (c == '\n')
	uart_putchar('\r', stream);
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;

	return 0;
}

/*
* Receive a character from the UART Rx.
*
* This features a simple line-editor that allows to delete and
* re-edit the characters entered, until either CR or NL is entered.
* Printable characters entered will be echoed using uart_putchar().
*
* Editing characters:
*
* . \b (BS) or \177 (DEL) delete the previous character
* . ^u kills the entire input buffer
* . ^w deletes the previous word
* . \t will be replaced by a single space
*
* All other control characters will be ignored.
*
* The internal line buffer is RX_BUFSIZE (80) characters long, which
* includes the terminating \n (but no terminating \0).  If the buffer
* is full (i. e., at RX_BUFSIZE-1 characters in order to keep space for
* the trailing \n), any further input attempts will send a \a to
* uart_putchar() (BEL character), although line editing is still
* allowed.
*
* Input errors while talking to the UART will cause an immediate
* return of -1 (error indication).  Notably, this will be caused by a
* framing error (e. g. serial line "break" condition), by an input
* overrun, and by a parity error (if parity was enabled and automatic
* parity recognition is supported by hardware).
*
* Successive calls to uart_getchar() will be satisfied from the
* internal buffer until that buffer is emptied again.
*/


int uart_getchar(FILE *stream)
{
	uint8_t c;
	
	loop_until_bit_is_set(UCSR0A, RXC0);
	if (UCSR0A & _BV(FE0)) return _FDEV_EOF;
	
	if (UCSR0A & _BV(DOR0)) return _FDEV_ERR;
	
	c = UDR0;
	
	/* behaviour similar to Unix stty ICRNL */
	if (c == '\r') c = '\n';
	
	if (c == '\n')
	{
		*cp = c;
		uart_putchar(c, stream);
		// add null character to terminate the string
		cp[+1] = '\0';
		// reset the pointer
		cp = b;
		return c;
	}
	else if (c == '\t') c = ' ';

	if ((c >= (uint8_t)' ' && c <= (uint8_t)'\x7e') ||
	c >= (uint8_t)'\xa0')
	{
		if (cp == b + RX_BUFSIZE - 1)
		uart_putchar('\a', stream);
		else
		{
			*cp++ = c;
			uart_putchar(c, stream);
		}
	}

	switch (c)
	{
		case '\b':
		case '\x7f':
		if (cp > b)
		{
			uart_putchar('\b', stream);
			uart_putchar(' ', stream);
			uart_putchar('\b', stream);
			cp--;
		}
		break;
		
		case 'c' & 0x1f:
		// reset the pointer
		cp = b;
		return -1;
		break;

		case 'u' & 0x1f:
		while (cp > b)
		{
			uart_putchar('\b', stream);
			uart_putchar(' ', stream);
			uart_putchar('\b', stream);
			cp--;
		}
		break;

		case 'w' & 0x1f:
		if (cp[-1] == ' ')
		{
			uart_putchar('\b', stream);
			uart_putchar(' ', stream);
			uart_putchar('\b', stream);
			cp--;
		}
		else
		{
			while (cp > b && cp[-1] != ' ')
			{
				uart_putchar('\b', stream);
				uart_putchar(' ', stream);
				uart_putchar('\b', stream);
				cp--;
			}
		}
		break;
	}

	return c;
}

/*
* Check rx buffer
*/
int uart_available(void)
{
	if (UCSR0A & (1 << RXC0)) return 1;
	return 0;
}

int gets_nb(char* buf)
{
	int c = getchar();
	
	if (c == -1)
	{
		return -1;
	}
	
	if (c == '\n')
	{
		char* ptr = b;
		while (*ptr != '\0')
		{
			*buf = *(ptr++);
			buf++;
		}
		// add null terminator back to string
		*buf = '\0';
		return 1;
	}
	return 0;
}

int cli_flag = 0;
void cli_print(void)
{
	if (cli_flag == 0)
	{
		printf(BY ">> " RESET);
		cli_flag = 1;
	}
}

void cli_done(void)
{
	cli_flag = 0;
}

void cli_reset(void)
{
	printf(CLEAR_SCREEN CLEAR_SCROLL);
}
