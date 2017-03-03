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

static task_t task_list_head = NULL;
static task_t task_list_tail = NULL;

static uint16_t task_stacks[MAX_TASKS][MAX_STACK_WORDS];

static task_t idle_task_handle;
#define IDLE_TASK_STACK_LEN 64
static uint8_t idle_task_stack[IDLE_TASK_STACK_LEN];
static void idle_task(int now, void * input) {
    while (1) {}
}

void scheduler_add_task(task_t * task_handle, task_func_t * func, void * task_stack) {
    task_handle->func = func;
    task_handle->task_sp = task_stack;
    task_handle->next = NULL;

    if((NULL == task_list_head) || (NULL == task_list_tail)) {
        task_list_head = task_list_tail = task_handle;
    } else {
        task_list_tail->next = task_handle;
    }
}

void scheduler_run() {

}

void initScheduler() {
    scheduler_add_task(&idle_task_handle, &idle_task, &idle_task_stack[IDLE_TASK_STACK_LEN - 1]);
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
