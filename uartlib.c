#include "uartlib.h"

void uart0_initialise(void){

	UBRR0H = 0x00; 						// 0x00  00000000
	UBRR0L = 0x4D; 						// 0x4D  01001101  Baud rate setting of 9600

	UCSR0B = _BV(RXEN0) | _BV(TXEN0); 	// Enables transmit and recieve
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); // Sets frame size to be 8 bits

	return;
}

char rx_char(void){

	while(!(UCSR0A & _BV(RXC0)));		// Waits for data to be in the UART recieve register
	return UDR0;						// Passes contents of register out
}

char* rx_str(void){

	char str[10];
	int i;

	for(i=0; i<10; i++){				// Moves through each element and copies contents of receive register into it
		str[i] = rx_char();
	}

	return str;							// Passes address of string out

}

void tx_char(char a){

	while(!(UCSR0A & (1<<UDRE0)));		// Waits for the UART transmit register to be empty
	UDR0 = a;							// Writes character to be sent into transmit register
	return;
}

void tx_str(char* str){

	int i;
	for(i = 0; str[i]; i++){			// Moves through each element of the string passed into this function
		tx_char(str[i]);				// Transmits the current element of the string
	}
	return;
}

void tx_int(uint16_t a){

	int num[3];
	int renum[3];
	int i, j;
	char b;

	j=0;								// Converts int to an array where each digit is stored as an element
	num[0] = a %10;
	a = a/10;
	num[1] = a%10;
	a = a/10;
	num[2] = a%10;

	for(i=2; i>=0; i--){				// Reverses order of array so that most significant digit is sent first
		renum[i] = num[j];
		j++;
	}

	for(i=0; i<3; i++){
		b = (char) renum[i] + '0';		// Converts elements of array from an int to the ASCII character of that digit
		tx_char(b);						// Transmits digit
	}

	return;
}

void tx_test(void){

	uint8_t i;
	char a = 'a';
	char str[] = "Hi there, I am a string\n\r";

	while(1){

		for(i = 0; i<51 ; i++){			// Transmits a series of consecutive integers
			tx_int(i);
			tx_char('\n');
			tx_char('\r');
		}

		_delay_ms(1000);				// Waits for 1 second

		tx_char(a);						// Transmits character declared earlier
		tx_char('\n');
		tx_char('\r');

		_delay_ms(1000);				// Waits for 1 second

		tx_str(str);					// Transmits string declared earlier

		_delay_ms(1000);				// Waits for 1 second
	}
}

void rx_chartest(void){

	while(1){

		char character = rx_char();		// Listens for a character and writes it to the character variable

		tx_str("I have received '");
		tx_char(character);				// Transmits the character received to confirm correct operation
		tx_str("'\n\r");
	}
}

void rx_strtest(void){

	char* str;

	while(1){
		str = rx_str();					// Listens for characters and populates string with them
		tx_str(str);					// re-transmits string to confirm correct operation
		tx_str("\n\r");
	}

}
