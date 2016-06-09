#include <stdio.h>
#include <msp430.h>
#include "rileyOS_config.h"
#include "task.h"

/*
 * hello.c
 */


static volatile unsigned long ticks;
static void setupSchedulerTick();

void Task1();
void Task2();

static TaskDescriptor tasks[] = {
		{
				.priority = 1,
				.interval = 10,
				.last_tick = 0,
				.routine  = Task1
		},
		{
				.priority = 0,
				.interval = 1,
				.last_tick = 0,
				.routine  = Task2
		}
};

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    setupSchedulerTick();
	
	while(1) {
		int t;
		for(t=0; t<sizeof(tasks)/sizeof(TaskDescriptor); t++) {
			if(tasks[t].interval==0) {
				//Continuous task, always run
				(tasks[t].routine)();
			} else if (ticks - tasks[t].last_tick >= tasks[t].interval) {
				//Waited long enough, time to run this task
				(tasks[t].routine)();
				tasks[t].last_tick = ticks;
			}
		}
	}
	
	return 0;
}

void Task1() {
	printf("Task 1 here\r\n");
}

void Task2() {
	printf("Task 2 here\r\n");
}

static void setupSchedulerTick() {
	ticks=0;
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
  ticks++;
}
