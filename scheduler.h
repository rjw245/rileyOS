/*
 * scheduler.h
 *
 *  Created on: Jun 8, 2016
 *      Author: riley
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

typedef void (*task_func_t)(int now, void * input) ;

typedef volatile struct task_private_s {
    task_func_t func;
    const char * name;
    void * task_sp; //Task stack pointer
    volatile struct task_private_s * next;
} task_t;



/**
 * Add task to task list to be run at next context switch.
 * Push task routine pointer and empty status register
 * onto the new task stack so they can be popped off later
 * from the task switch interrupt.
 */
void scheduler_add_task(task_t * task_handle, const char * name,
                        task_func_t func, uint16_t * task_stack, uint16_t stack_bytes);

/**
 *  Kicks off the timer interrupt
 */
void scheduler_run( void );

/**
 *  Sets up the idle task
 */
void scheduler_init( void );

#endif /* SCHEDULER_H */
