/*
 * scheduler.c
 *
 *  Created on: Jun 8, 2016
 *      Author: riley
 */

#include "rileyOS_config.h"
#include "scheduler.h"
#include <msp430.h>
#include <stdint.h>
#include <stdlib.h>

#define NUM_REGISTERS 16

static volatile task_t * task_queue_head = NULL;

static void setupSchedulerTick( void );

/**
 * Return the next item in the queue
 * (does not remove from queue, != pop)
 */
static inline volatile task_t * task_queue_rotate( void );
static inline volatile task_t * task_queue_current( void );

static void idle_task(int now, void * input) {
    while (1) {}
}


void scheduler_add_task(task_t * task_handle, const char * name,
                        task_func_t func, uint16_t * task_stack, uint16_t stack_bytes) {
    task_handle->func = func;
    task_handle->next = NULL;
    task_handle->name = name;

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
    } else {
        // Insert between head and head + 1
        task_handle->next = task_queue_head->next;
        task_queue_head->next = task_handle;
    }
}

static inline volatile task_t * task_queue_rotate( void ) {
    volatile task_t * ret = task_queue_head;
    task_queue_head = task_queue_head->next;
    return ret;
}

static inline volatile task_t * task_queue_current( void ) {
    return task_queue_head;
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
    static task_t idle_task_handle;
    #define IDLE_TASK_STACK_LEN 512
    static uint16_t idle_task_stack[IDLE_TASK_STACK_LEN/sizeof(uint16_t)];

    scheduler_add_task(&idle_task_handle, "IDLE", &idle_task, idle_task_stack, sizeof(idle_task_stack));
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


static volatile task_t * cur_task = NULL;
static volatile void * cur_task_sp;
static volatile void * next_task_sp;

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR (void)
{
    // Must be static, can't use stack
    static volatile task_t * cur_task;
    static volatile task_t * next_task;

    next_task = task_queue_rotate();

    asm (" pushx.a r15 \n\t");
    asm (" pushx.a r14 \n\t");
    asm (" pushx.a r13 \n\t");
    asm (" pushx.a r12 \n\t");
    asm (" pushx.a r11 \n\t");
    asm (" pushx.a r10 \n\t");
    asm (" pushx.a r9 \n\t");
    asm (" pushx.a r8 \n\t");
    asm (" pushx.a r7 \n\t");
    asm (" pushx.a r6 \n\t");
    asm (" pushx.a r5 \n\t");
    asm (" pushx.a r4 \n\t");

    asm (" mov sp, cur_task_sp \n\t");

    if(cur_task != NULL) {
        cur_task->task_sp = cur_task_sp; // Store the stack pointer back in our
                                         // task struct so we can get it back later
                                         // (ONLY IF WE ARE CURRENTLY IN A TASK)
    }
    cur_task = next_task;

    next_task_sp = next_task->task_sp;
    asm (" mov next_task_sp, sp \n\t");

    asm (" popx.a r4 \n\t");
    asm (" popx.a r5 \n\t");
    asm (" popx.a r6 \n\t");
    asm (" popx.a r7 \n\t");
    asm (" popx.a r8 \n\t");
    asm (" popx.a r9 \n\t");
    asm (" popx.a r10 \n\t");
    asm (" popx.a r11 \n\t");
    asm (" popx.a r12 \n\t");
    asm (" popx.a r13 \n\t");
    asm (" popx.a r14 \n\t");
    asm (" popx.a r15 \n\t");

    asm (" mov sp, next_task_sp \n\t");
    next_task->task_sp = next_task_sp;

    asm (" RETI \n\t");
}

