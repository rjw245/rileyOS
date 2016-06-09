#include <stdio.h>
#include <msp430.h>
#include "rileyOS_config.h"

/*
 * hello.c
 */

static volatile int printNow=0;

static void setupSchedulerTick();

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    setupSchedulerTick();
    P1DIR |= 0x01;
	
	while(1) {
		if(printNow) {
			P1OUT ^= 0x01;
			printNow = 0;
		}
	}
	
	return 0;
}

static void setupSchedulerTick() {
	TA0CTL |= MC_0; //Stop timer A during setup
	TA0CCR0 = 32*SCHEDULER_TICK_MS; //Assume ACLK=32kHz

	/*
	 * Clock source: ACLK
	 * Mode: Count up to CCR repeatedly (and start)
	 * Divisor: 1
	 * and clear the clock
	 */
	TA0CTL =  TASSEL__ACLK|MC__UP|ID__1|TACLR;
	TA0CCTL0 = CCIE; //Enable interrupt for CCR0
	__enable_interrupt();
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR (void)
{
  printNow=1;
}

