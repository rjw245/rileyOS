#include <stdio.h>
#include <msp430.h>
#include "rileyOS_config.h"
#include "task.h"
#include "scheduler.h"


void Task1();
void Task2();



int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    initScheduler();

    addTask(Task1, 1, 0);
    addTask(Task2, 10, 0);

	runScheduler();

	return 0;
}

void Task1() {
	printf("Task 1 here\r\n");
}

void Task2() {
	printf("Task 2 here\r\n");
}
