#include <stdio.h>
#include <msp430.h>
#include "rileyOS_config.h"
#include "task.h"
#include "scheduler.h"

void Task1();
void Task2();

#define TASK1_STACK_BOTTOM		0x200
#define TASK2_STACK_BOTTOM		0x400

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    P1DIR |= 0x01;
    P4DIR |= 0x01;

    initScheduler();

    addTask(Task1, INTERVAL_100ms, 50);
    addTask(Task2, INTERVAL_1000ms, 50);

	runScheduler();

	return 0;
}

void Task1() {
	while(1) {
		//Flash LED P1.0
		P1OUT ^= 0x01;
	}
}

void Task2() {
	while(1) {
		//Flash LED P4.0
		P4OUT ^= 0x01;
	}
}
