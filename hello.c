#include <stdio.h>
#include <msp430.h> 

/*
 * hello.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	
	printf("Hello World!\n");
	
	return 0;
}
