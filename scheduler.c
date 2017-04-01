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

#define NUM_REGISTERS 16

static task_t * task_list_head = NULL;
static task_t * task_list_tail = NULL;

static void idle_task(int now, void * input) {
    while (1) {}
}

void do_context_switch(task_t * next_task) {
    static void * cur_task_sp;
    cur_task_sp = next_task->task_sp;
    asm (" push r15 \n\t"
         " push r14 \n\t"
         " push r13 \n\t"
         " push r12 \n\t"
         " push r11 \n\t"
         " push r10 \n\t"
         " push r9 \n\t"
         " push r8 \n\t"
         " push r7 \n\t"
         " push r6 \n\t"
         " push r5 \n\t"
         " push r4 \n\t"
         " push r3 \n\t"
         " push r2 \n\t"
         " push r1 \n\t"
         " push r0 \n\t"
         " push sr \n\t"
         " mov cur_task_sp, sp \n\t"
         " pop sr \n\t"
         " pop r0 \n\t"
         " pop r1 \n\t"
         " pop r2 \n\t"
         " pop r3 \n\t"
         " pop r4 \n\t"
         " pop r5 \n\t"
         " pop r6 \n\t"
         " pop r7 \n\t"
         " pop r8 \n\t"
         " pop r9 \n\t"
         " pop r10 \n\t"
         " pop r11 \n\t"
         " pop r12 \n\t"
         " pop r13 \n\t"
         " pop r14 \n\t"
         " pop r15 \n\t");
}

void scheduler_add_task(task_t * task_handle, task_func_t func, void * task_stack_base) {
    task_handle->func = func;
    task_handle->task_sp = task_stack_base;
    task_handle->next = NULL;

    // Store initial task context in the stack
    uint16_t * stack = (uint8_t *)task_stack_base;

    // First, store the 20-bit PC and the status
    // register. These will get popped as a last step
    // when returning from the timer interrupt.
    *(stack++) = (uint16_t)func;
    *(stack++) = GIE; // Only set GIE in the status
                      // register we want to start with
                      // Upper 4 bits of PC are also stored
                      // here but are zeroed.
    int reg;
    for(reg=0; reg<NUM_REGISTERS; reg++) {
        *(stack++) = 0;
    }
    *(stack++) = 4; // Save interrupt state

    if((0 == task_list_head) || (0 == task_list_tail)) {
        task_list_head = task_handle;
    } else {
        task_list_tail->next = task_handle;
    }
    task_list_tail = task_handle;
}

void scheduler_run() {
    // 1. Register timer interrupt


    // 2. Assert, should never reach here
    //    If we are not running any tasks,
    //    we should be in the idle task
    assert();
}

void scheduler_init() {
    static task_t idle_task_handle;
    #define IDLE_TASK_STACK_LEN 64
    static uint16_t idle_task_stack[IDLE_TASK_STACK_LEN/sizeof(uint16_t)];

    scheduler_add_task(&idle_task_handle, &idle_task, idle_task_stack);
}

void scheduler_tick() {

}

