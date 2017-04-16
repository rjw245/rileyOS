/*
 * scheduler.c
 *
 *  Created on: Jun 8, 2016
 *      Author: riley
 */

#include "rileyOS_config.h"
#include "scheduler.h"
#include "scheduler_private.h"
#include <msp430.h>
#include <stdint.h>
#include <stdlib.h>

#define NUM_REGISTERS 16

static volatile task_t * task_queue_head = NULL;
static volatile uint32_t scheduler_now_ms = 0;

static void setupSchedulerTick( void );

/**
 * Return the next item in the queue
 * (does not remove from queue, != pop)
 */
static inline volatile task_t * task_queue_get_next( void );

static void idle_task( void ) {
    while (1) {}
}


void scheduler_add_task(task_t * task_handle, const char * name,
                        task_func_t func, uint16_t * task_stack, uint16_t stack_bytes) {
    task_handle->next = NULL;
    task_handle->prev = NULL;
    task_handle->name = name;
    task_handle->sleep_start_ms = 0;
    task_handle->sleep_for_ms = 0;

    // Store initial task context in the stack
    uint16_t * stack = &task_stack[(stack_bytes/sizeof(uint16_t))-1];

    // First, store the 20-bit PC and the status
    // register. These will get popped as a last step
    // when returning from the timer interrupt.
    *(--stack) = (uint16_t)func;
    *(--stack) = GIE; // Only set GIE in the status
                      // register we want to start with
                      // Upper 4 bits of PC are also stored
                      // here but are zeroed.

    int reg;
    for(reg=4; reg<NUM_REGISTERS; reg++) {
        // Start at register 4, since
        // R0 == PC
        // R1 == SP
        // R2 == SR
        // R3 is reserved
        // and we load these separately

        // Emulate PUSHX, and save two words
        // per 20-bit register
        *(--stack) = 0x0;
        *(--stack) = 0x0;
    }

    task_handle->task_sp = (void *)stack;

    if(0 == task_queue_head) {
        task_queue_head = task_handle;
        task_queue_head->next = task_queue_head; //Queue of one wraps around on itself
        task_queue_head->prev = task_queue_head; //Queue of one wraps around on itself
    } else {
        // Insert between head and head + 1
        task_handle->next = task_queue_head->next;
        task_queue_head->next->prev = task_handle;
        task_handle->prev = task_queue_head;
        task_queue_head->next = task_handle;
    }
}

static inline volatile task_t * task_queue_get_next( void ) {
    volatile task_t * ret = task_queue_head->next;
    task_queue_head = task_queue_head->next;
    return ret;
}

volatile task_t * scheduler_get_current_task( void ) {
    return task_queue_head;
}

volatile uint32_t scheduler_now( void ) {
    return scheduler_now_ms;
}

void scheduler_run( void ) {
    // 1. Register timer interrupt
    setupSchedulerTick();

    // 2. Loop until the first
    //    context switch after
    //    which we should never
    //    execute this again.
    while(1);
}

void scheduler_init( void ) {
    SCHEDULER_ADD(idle_task, 64);
}

static void setupSchedulerTick( void ) {
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


static volatile void * volatile task_sp;

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR (void)
{
    // Must be static, can't use stack
    static volatile task_t * volatile task_ptr = NULL;

    // Fight the compiler, undo its register pushing
    asm (" POPM.A #5,R15 \n\t");

    // Push all GP registers onto current task stack
    asm (" pushm.a #12, r15 \n\t");
    asm (" mov sp, task_sp \n\t");

    // Save current task stack pointer
    // (ONLY IF WE ARE CURRENTLY IN A TASK)
    if(task_ptr != NULL) {
        task_ptr->task_sp = task_sp;
    }

    do {
        task_ptr = task_queue_get_next();
    } while((task_ptr->sleep_for_ms != 0) &&
            ((task_ptr->sleep_for_ms + task_ptr->sleep_start_ms) > scheduler_now()));
    task_ptr->sleep_for_ms = 0;
    scheduler_now_ms += SCHEDULER_TICK_MS; // TODO don't assume regular intervals

    // Change stacks
    task_sp = task_ptr->task_sp;
    asm (" mov task_sp, sp \n\t");

    // Restore all GP registers from new stack
    asm (" popm.a #12, r15 \n\t");

    asm (" RETI \n\t");
}

