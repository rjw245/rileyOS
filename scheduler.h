/*
 * scheduler.h
 *
 *  Created on: Jun 8, 2016
 *      Author: riley
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

typedef void (*task_func_t)( void ) ;

typedef volatile struct task_private_s {
    task_func_t func;
    const char * name;
    void * volatile task_sp; //Task stack pointer
    volatile struct task_private_s * volatile next;
} task_t;

#define SCHEDULER_ADD(func, stack_size) \
    static task_t task_##__FILE__##__LINE__##func; \
    static uint16_t task_stack_##__FILE__##__LINE__##func[(stack_size)/sizeof(uint16_t)]; \
    scheduler_add_task(&task_##__FILE__##__LINE__##func, \
                       #func, \
                       func, \
                       task_stack_##__FILE__##__LINE__##func, \
                       sizeof(task_stack_##__FILE__##__LINE__##func));

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
