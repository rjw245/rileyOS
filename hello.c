#include <stdio.h>
#include <msp430.h>

/*
 * hello.c
 */

static volatile int printNow=0;

static void setupTimerA();

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    setupTimerA();
    P1DIR |= 0x01;
	
	while(1) {
		if(printNow) {
			P1OUT ^= 0x01;
			printNow = 0;
		}
	}
	
	return 0;
}

static void setupTimerA() {
	TA0CTL |= MC_0; //Stop timer A during setup
	TA0CCR0 = 10000;

	/*
	 * Clock source: SMCLK
	 * Mode: Count up to CCR repeatedly (and start)
	 * Divisor: 1
	 * and clear the clock
	 */
	TA0CTL =  TASSEL__SMCLK | MC__UP | ID__8 | TACLR;
	TA0CCTL0 = CCIE;
	__enable_interrupt();
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR (void)
{
  printNow=1;
}

