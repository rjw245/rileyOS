#include <stdio.h>
#include <msp430.h>
#include "rileyOS_config.h"
#include "scheduler.h"

void Task1();
void Task2();

static int i;
static int j;



#define TASK1_STACK_BOTTOM		0x200
#define TASK2_STACK_BOTTOM		0x400

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    P1DIR |= 0x01;
    P4DIR |= 0x01;

    scheduler_init();

    static task_t task1_handle;
    #define TASK1_STACK_LEN 512
    static uint16_t task1_stack[TASK1_STACK_LEN/sizeof(uint16_t)];
    scheduler_add_task(&task1_handle, &Task1, task1_stack, sizeof(task1_stack));

    scheduler_run();

    return 0;
}

void Task1(int now, void * input) {
	while(1) {
		//Flash LED P1.0
		P1OUT ^= 0x01;
		for(i=0; i<100000; i++);
	}
}

void Task2(int now, void * input) {
	while(1) {
		//Flash LED P4.0
		P4OUT ^= 0x01;
		for(j=0; j<50000; j++);
	}
}
