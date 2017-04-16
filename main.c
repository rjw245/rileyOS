#include <stdio.h>
#include <msp430.h>
#include "rileyOS_config.h"
#include "scheduler.h"
#include "task.h"
#include "hal_LCD.h"

static void schedule_and_run( void );
static void Task1( void );
static void Task2( void );
static void LCDTASK( void );
static void WRITEBAR( void );
static void WRITEFOO( void );

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;       // Disable GPIO power-on
                                // default high-impedance mode
    schedule_and_run();
    return 0;
}

static void schedule_and_run( void ) {
    scheduler_init();

    SCHEDULER_ADD(Task1, 256);
    SCHEDULER_ADD(Task2, 256);
    SCHEDULER_ADD(LCDTASK, 256);

    scheduler_run();
}

static void Task1( void ) {
    P1DIR |= 0x01; // Set LED P1.0 as output
    while(1) {
        P1OUT ^= 0x01; // Flash LED P1.0
        task_sleep(100);
    }
}

static void Task2( void ) {
    P4DIR |= 0x01; // Set LED P4.0 as output
    while(1) {
        P4OUT ^= 0x01; // Flash LED P4.0
        task_sleep(500);
    }
}

static void LCDTASK( void ) {
    Init_LCD();
    SCHEDULER_ADD(WRITEFOO, 256);
    task_sleep(1400);
    SCHEDULER_ADD(WRITEBAR, 256);
    task_join();
}

static void WRITEBAR( void ) {
    while(1) {
        displayScrollText("BAR");
    }
}

static void WRITEFOO( void ) {
    while(1) {
        displayScrollText("FOO");
    }
}

