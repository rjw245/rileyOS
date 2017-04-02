#include <stdio.h>
#include <msp430.h>
#include "rileyOS_config.h"
#include "scheduler.h"
#include "hal_LCD.h"

static void Task1( void );
static void Task2( void );
static void LCDTask( void );

#define TASK1_STACK_BOTTOM		0x200
#define TASK2_STACK_BOTTOM		0x400

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port settings
    P1DIR |= 0x01;                          // Set P1.0 to output direction
    P4DIR |= 0x01;

    scheduler_init();
    SCHEDULER_ADD(Task1, 512);
    SCHEDULER_ADD(Task2, 512);
    SCHEDULER_ADD(LCDTask, 512);
    scheduler_run();

    return 0;
}

static void Task1( void ) {
	while(1) {
		//Flash LED P1.0
		P1OUT ^= 0x01;
		static int i;
		for(i=0; i<10000; i++);
	}
}

static void Task2( void ) {
	while(1) {
		//Flash LED P4.0
		P4OUT ^= 0x01;
        static int i;
        for(i=0; i<9000; i++);
	}
}

static void LCDTask( void ) {
    Init_LCD();
    while(1) {
        displayScrollText("DEADBEEF0123");
    }
}
