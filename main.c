#include <stdio.h>
#include "uartlib.h"
#include <avr/interrupt.h>
#include <stdlib.h>

struct resultStruct {
  int x;
  int y;
  int result;
  int isSupported;
  int pass;
} temp;

void displayresult(struct resultStruct result);
void displaysignedresult(struct resultStruct result);

struct resultStruct runTest(int i, int j,int isSigned) {
  struct resultStruct returnResult;
  if(isSigned) PORTD |= 4; else PORTD &= ~4;
  PORTB = (i<<4) | (j&15); // Bitmask and align the outputs from the matto
  returnResult.x=i;
  returnResult.y=j;
  _delay_ms(1); // Wait for pins to stabailize
  returnResult.result=PINA; // Read in the result
  if(PINA==((i*j)&255)) returnResult.pass=1; else returnResult.pass=0;
  // ^ Check whether the answer was correct
  if(PIND & 8) returnResult.isSupported=1; else {
    // If the result was not supported, it passes the specification
    // so consider it a pass, but flag it as not supported
    returnResult.isSupported=0;
    returnResult.pass=1;
  }
  return returnResult;
}

void runFullTest() {
  int i;
  int j;
  int numPass=0;
  int numFail=0;
  int numNotSupported=0;

  for (i=0;i<16;i++) {
    for (j=0;j<16;j++) {
      temp=runTest(i,j,0);
      displayresult(temp);
      if(temp.pass) numPass++; else numFail++;
      if(!temp.isSupported) numNotSupported++;
    }
  }
  tx_str("\r\n  \033[0;35mUnsigned\033[0;37m test completed with \033[0;32m");
  tx_int(numPass);
  tx_str("\033[0;37m passes of which \033[0;31m");
  tx_int(numNotSupported);
  tx_str("\033[0;37m was not supported and \033[0;31m");
  tx_int(numFail);
  tx_str("\033[0;37m fails.\r\n");
  return;
}

void runFullSignedTest() {
  int i;
  int j;
  int numPass=0;
  int numFail=0;
  int numNotSupported=0;

  for (i=-8;i<8;i++) {
    for (j=-8;j<8;j++) {
      temp=runTest(i,j,1);
      displaysignedresult(temp);
      if(temp.pass) {
        numPass++;
      } else {
        numFail++;
      }
      if(!temp.isSupported) numNotSupported++;
    }
  }
  tx_str("\r\n \033[0;37mSigned\033[0;35m test completed with \033[0;32m");
  tx_int(numPass);
  tx_str("\033[0;37m passes of which \033[0;31m");
  tx_int(numNotSupported);
  tx_str("\033[0;37m was not supported and \033[0;31m");
  tx_int(numFail);
  tx_str("\033[0;37m fails.\r\n");
  return;
}

int main(void) {

	char option1, option2, option3;

	uart0_initialise();

	DDRA = 0x00;
 	DDRB = 0xFF;
  DDRD |= 4;
  DDRD &= ~8;
 	PORTB = 0x00;

  while (1){
    // -------MAIN MENU----------
  	tx_str("Press \033[0;33m1\033[0;37m to run unsigned  tests, \033[0;33m2\033[0;37m to run signed tests:\033[5;37m_ \033[0;37m \r\n");
  	option1 = rx_char();

  	if(option1 == '1'){
      //---------------UNSIGNED------------
    	tx_str("\033[0;35mUnsigned\033[0;37m: Press \033[0;33m1\033[0;37m to run test program, \033[0;33m2\033[0;37m for manual input:\033[5;37m_ \033[0;37m \r\n");
      	option2 = rx_char();
      	if(option2 == '1'){
      		runFullTest();
      		option2 = '0';
      	} else if(option2 == '2'){
          // Manual Test
      		int mult = 1;
          // Mult is used to check whether the user would like to input another value
      		while(mult){
              int c=0;
          		tx_str("\033[0;35mUnsigned\033[0;37m: Please type the two numbers you would like to multiply with the format:  \033[1;33m2x3\033[0;33m:\r\n");
          		int a = rx_char() - '0'; // Convert from ascii to decimal
          		tx_char(a+'0'); // Echo the number back
          		c=rx_char();
              if((c>='0')&(c<='9')) {
                // If the user has typed another number, we need to times the previous number by 10
                // and add it on to form a 2 digit number
                a = a*10+(c-'0');
                tx_char(c);
              }
          		tx_char('x');
          		int b = rx_char() - '0'; // Convert from ascii to decimal
          		tx_char(b+'0');
              c=rx_char();
              if((c>='0')&(c<='9')) {
                // If the user has typed another number, we need to times the previous number by 10
                // and add it on to form a 2 digit number
                b = b*10+(c-'0');
                tx_char(c);
              }
          		tx_str("\n\r");
              // Run and display the test, unsigned mode
          		displayresult(runTest(a,b,0));

          		tx_str("\033[0;35mUnsigned\033[0;37m: Would you like to try another pair? (Y/N)\r\n");
          		option3 = rx_char();
          		tx_char(option3);
              tx_str("\r\n");

          		if(option3 == 'n'){ mult = 0;} // If not, exit to main loop
          	}
      	}
      	else{tx_str("\033[5;31mInvalid option\033[0;35m\r\n");}

  	}
  	else if(option1 == '2'){
      //-----------SIGNED----------
      tx_str("\033[0;35mSigned\033[0;37m: Press \033[0;33m1\033[0;37m to run test program, \033[0;33m2\033[0;37m for manual input:\033[5;37m_ \033[0;37m \r\n");
    	option2 = rx_char();
    	if(option2 == '1'){
    		runFullSignedTest();
    		option2 = '0';
    	} else if (option2=='2'){
        // Manual test
    		int mult = 1;
        // Mult is used to check whether the user would like to input another value
    		while(mult){
            // Whether numbers are negative or not
            int neg1 = 0;
            int neg2 = 0;
        		tx_str("\033[0;35mSigned\033[0;37m: Please type the two numbers you would like to multiply with the format:  \033[1;33m2x3\033[0;33m:\r\n");
            int a = rx_char();
            if(a=='-') {
              // Has the user decided they want a negative number
              tx_char('-');
              neg1=1; // Flag it, we'll deal with it later
              a=rx_char()-'0'; // Read in another number
            } else a=a-'0';
        		tx_int(a); // Echo the number back
        		rx_char();
            tx_char('x');

            int b = rx_char();
            if(b=='-') {
              tx_char('-');
              neg2=1;
              b=rx_char()-'0';
            } else b=b-'0';
        		tx_int(b);
        		tx_str("\n\r");
            if(neg1) a = -a; // Negate the numbers if they were negative
            if(neg2) b = -b;
            // Run a signed test
        		displaysignedresult(runTest(a,b,1));

        		tx_str("\033[0;35mSigned\033[0;37m: Would you like to try another pair? (Y/N)\r\n");
        		option3 = rx_char();
        		tx_char(option3);
            tx_str("\r\n");

        		if(option3 == 'n'){ mult = 0;} // If no, return to main menu
        	}
    	}
  	}
    // If the user has entered something silly
  	else{tx_str("\033[5;31mInvalid option\033[0;35m\r\n");}


  }
  return 0;
}

void displayresult(struct resultStruct result){
  tx_str("\033[0;34m");
  // This section turns negative numbers to positive to be displayed properly
  // with - signs, this code is somewhat depreciated as now only displays unsigned results
  if(result.x<0) tx_char('-');
	tx_int(abs(result.x));
	tx_str("\033[0;37mx\033[0;34m");
  if(result.y<0) tx_char('-');
	tx_int(abs(result.y));

  // Now print the statistics
	tx_str("\033[0;37m results in \033[0;34m");
  if(result.result<0) {
    tx_char('-');
  }
	tx_int(abs(result.result));
	if(result.pass){
		tx_str(" \033[0;32mPass\033[0;37m");
	}
	else{tx_str(" \033[0;31mFail\033[0;37m");}
  if(result.isSupported) {
    tx_str(" \033[0;32mSupported\033[0;37m\r\n");
  } else {
    tx_str("  \033[0;31mnot Supported\033[0;37m\r\n");
  }
  return;
}


void displaysignedresult(struct resultStruct result){
  result.x=result.x & 15; // Take the first 4 bits of the inputs (as these are stored in uint16)
  result.y=result.y & 15;
  result.result=result.result & 255; // Take the first 8 bits of the result
  if(result.x>7) result.x-=16; // If the result appears to be greater than 7, it is negative
  if(result.y>7) result.y-=16; // so subtract 16 to get the correct signed number
  if(result.result>127) result.result-=256; // or subtract 256 in this case
  tx_str("\033[0;34m");
  if(result.x<0) tx_char('-'); // If it's negative, print a - symbol
	tx_int(abs(result.x));       // and print the absolute of the value
	tx_str("\033[0;37mx\033[0;34m");
  if(result.y<0) tx_char('-');
	tx_int(abs(result.y));
	tx_str("\033[0;37m results in \033[0;34m");
  if(result.result<0) {
    tx_char('-');
  }
	tx_int(abs(result.result));
  // Now print the statistics
	if(result.pass){
		tx_str(" \033[0;32mPass\033[0;37m");
	}
	else{tx_str(" \033[0;31mFail\033[0;37m");}
  if(result.isSupported) {
    tx_str(" \033[0;32mSupported\033[0;37m\r\n");
  } else {
    tx_str("  \033[0;31mnot Supported\033[0;37m\r\n");
  }
  return;
}
