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
volatile int cur_task = 0;

static uint16_t task_stacks[MAX_TASKS][MAX_STACK_WORDS];

void addTask(void (*routine)(), int interval, unsigned int stack_len) {
	if(num_tasks < MAX_TASKS) {
		TaskDescriptor new_task = {
				.interval = interval,
				.routine  = routine,
				.last_tick = 0,
				.task_sp = task_stacks[num_tasks],
				.task_tos = MAX_STACK_WORDS-1, //All the way at the bottom
				.ready   = 0
		};

		tasks[num_tasks] = new_task;

		int tos = tasks[num_tasks].task_tos;

		//Push task routine pointer onto stack, to be popped off later as the PC
		tos--;
		tasks[num_tasks].task_sp[tos] = (uint16_t)(tasks[num_tasks].routine); //Initial PC value

		//Push a word onto the stack, to be popped off later as the status register
		tos--;
		tasks[num_tasks].task_sp[tos] = GIE; //Empty status register except for GIE flag

		//Save back TOS
		tasks[num_tasks].task_tos = tos;

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
	ticks++;
	int tos = tasks[cur_task].task_tos;
	taskSP = tasks[cur_task].task_sp+tos;
	cur_task = (cur_task+1)%num_tasks;
	asm volatile(" mov taskSP, SP");
	asm volatile(" NOP");
	asm volatile(" NOP");
	asm volatile(" NOP");
	asm volatile(" RETI"); //Manually return from interrupt
	asm volatile(" NOP");
	asm volatile(" NOP");
	asm volatile(" NOP");
}
