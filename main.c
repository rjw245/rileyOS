#include <stdio.h>
#include <msp430.h>
#include "rileyOS_config.h"
#include "task.h"
#include "scheduler.h"

void Task1();
void Task2();

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    P1DIR |= 0x01;
    P4DIR |= 0x01;

    initScheduler();

    addTask(Task1, INTERVAL_100ms);
    addTask(Task2, INTERVAL_1000ms);

	runScheduler();

	return 0;
}

void Task1() {
	P1OUT ^= 0x01;
}

void Task2() {
	P4OUT ^= 0x01;
}
