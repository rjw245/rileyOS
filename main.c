#include <stdio.h>
#include <msp430.h>
#include "rileyOS_config.h"
#include "scheduler.h"

void Task1( void );
void Task2( void );

#define TASK1_STACK_BOTTOM		0x200
#define TASK2_STACK_BOTTOM		0x400

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port settings
    P1DIR |= 0x01;                          // Set P1.0 to output direction
    P4DIR |= 0x01;

    scheduler_init();

    static task_t task1_handle;
    #define TASK1_STACK_LEN 512
    static uint16_t task1_stack[TASK1_STACK_LEN/sizeof(uint16_t)];
    scheduler_add_task(&task1_handle, "Task1", &Task1, task1_stack, sizeof(task1_stack));

    static task_t task2_handle;
    #define TASK2_STACK_LEN 512
    static uint16_t task2_stack[TASK2_STACK_LEN/sizeof(uint16_t)];
    scheduler_add_task(&task2_handle, "Task2", &Task2, task2_stack, sizeof(task2_stack));

    scheduler_run();

    return 0;
}

void Task1( void ) {
	while(1) {
		//Flash LED P1.0
		P1OUT ^= 0x01;
		static int i;
		for(i=0; i<10000; i++);
	}
}

void Task2( void ) {
	while(1) {
		//Flash LED P4.0
		P4OUT ^= 0x01;
        static int i;
        for(i=0; i<9000; i++);
	}
}
