#include <stdio.h>
#include <msp430.h>
#include "rileyOS_config.h"
#include "scheduler.h"
#include "hal_LCD.h"

static void schedule_and_run( void );
static void Task1( void );
static void Task2( void );
static void LCDTask( void );

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;       // Disable GPIO power-on
                                // default high-impedance mode
    schedule_and_run();
    return 0;
}

static void schedule_and_run( void ) {
    scheduler_init();

    SCHEDULER_ADD(Task1, 512);
    SCHEDULER_ADD(Task2, 512);
    SCHEDULER_ADD(LCDTask, 512);

    scheduler_run();
}

static void Task1( void ) {
    P1DIR |= 0x01; // Set LED P1.0 as output
    while(1) {
        P1OUT ^= 0x01; // Flash LED P1.0
        int i; for(i=0; i<10000; i++);
    }
}

static void Task2( void ) {
    P4DIR |= 0x01; // Set LED P4.0 as output
    while(1) {
        P4OUT ^= 0x01; // Flash LED P4.0
        int i; for(i=0; i<9000; i++);
    }
}

static void LCDTask( void ) {
    Init_LCD();
    while(1) { displayScrollText("DEADBEEF0123"); }
}

