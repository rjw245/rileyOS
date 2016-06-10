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
#include <stdint.h>
#include <stdlib.h>

static TaskDescriptor tasks[MAX_TASKS];
static unsigned int num_tasks;
static unsigned long ticks;

volatile void * taskSP;

static char task_stacks[MAX_TASKS][MAX_STACK_SIZE];

void addTask(void (*routine)(), int interval, unsigned int stack_len) {
	if(num_tasks < MAX_TASKS) {
		TaskDescriptor new_task = {
				.interval = interval,
				.routine  = routine,
				.last_tick = 0,
				.task_sp = task_stacks[num_tasks],
				.ready   = 0
		};
		new_task.task_sp = (char*)new_task.task_sp + stack_len;

		tasks[num_tasks] = new_task;

		//Push task routine pointer onto stack, to be popped off later as the PC
		tasks[num_tasks].task_sp = (char*)tasks[num_tasks].task_sp - 2;
		*((TaskRoutine*)tasks[num_tasks].task_sp) = tasks[num_tasks].routine; //Initial PC value

		//Push a 0 onto the stack, to be popped off later as the status register
		tasks[num_tasks].task_sp = (char*)tasks[num_tasks].task_sp - 2;
		*((uint16_t *)tasks[num_tasks].task_sp) = GIE; //Empty status register except for GIE flag

		tasks[num_tasks].ready = 1;
		num_tasks++;
	}
}

void runScheduler() {
	setupSchedulerTick();
	while(1) {
		/*
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
		*/
	}
}

void initScheduler() {
	num_tasks = 0;
	srand(0xBEEF);
	//addTask(idleTask,0,20);
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

/*
 * Returns the number of the task to run next.
 * This is the scheduling algorithm!!
 */
static int taskSwitch() {
	int candidate = 0;
	if(tasks[candidate].ready) { return candidate; }
	else { return NO_TASK_SWITCH; }
}

static void idleTask() {
	while(1);
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR (void)
{
	int taskNum = 1;
	if(tasks[taskNum].ready) {
		taskSP = tasks[taskNum].task_sp;
		asm volatile(" mov taskSP, SP");
	}
	ticks++;
}
