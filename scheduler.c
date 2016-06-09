/*
 * scheduler.c
 *
 *  Created on: Jun 8, 2016
 *      Author: riley
 */

#include "task.h"
#include "rileyOS_config.h"
#include "scheduler.h"
#include <msp430.h>

static TaskDescriptor tasks[MAX_TASKS];
static unsigned int num_tasks;
static unsigned long ticks;

void addTask(void (*routine)(), int interval) {
	if(num_tasks < MAX_TASKS) {
		TaskDescriptor new_task = {
				.interval = interval,
				.routine  = routine,
				.last_tick = 0
		};
		tasks[num_tasks] = new_task;
		num_tasks++;
	}
}

void runScheduler() {
	while(1) {
		int t;
		for(t=0; t<num_tasks; t++) {
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
}

void initScheduler() {
	num_tasks = 0;
	setupSchedulerTick();
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
