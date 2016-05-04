#ifndef UARTLIB_H_
#define UARTLIB_H_

#include <avr/io.h>
#include <util/delay.h>

#define F_CPU 12000000
#define BAUD 9600

void uart0_initialise(void); 	// Initialises the UART1 hardware
char rx_char(void);				// Listens to the Rx Buffer for individual characters
char* rx_str(void);				// Listens to the Rx Buffer for entire strings
void tx_char(char a);			// Sends a character to the Tx Buffer
void tx_str(char* str);			// Sends a string to the Tx Buffer
void tx_int(uint16_t a);			// Sends a number to the Tx Buffer
void tx_test(void);				// Tool to test Tx functions
void rx_chartest(void);			// Tool to test Rx character functions
void rx_strtest(void);			// Tool to test Rx string functions

#endif //UARTLIB_H_
