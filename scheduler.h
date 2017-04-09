/*
 * scheduler.h
 *
 *  Created on: Jun 8, 2016
 *      Author: riley
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include "scheduler_private.h"

typedef void (*task_func_t)( void ) ;

typedef volatile struct task_private_s {
    const char * name;
    void * volatile task_sp; //Task stack pointer
    volatile struct task_private_s * volatile next;
} task_t;


/**
 * Handy macro which blackboxes the allocation of memory
 * per task. Accepts the task function to schedule
 * and the size of stack to allocate as arguments.
 */
#define SCHEDULER_ADD(func, stack_size) \
    CREATE_TASK_HANDLE(__LINE__, func); \
    CREATE_TASK_STACK(__LINE__, func, stack_size); \
    CALL_SCHEDULER_ADD(__LINE__, func);

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
