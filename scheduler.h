/*
 * scheduler.h
 *
 *  Created on: Jun 8, 2016
 *      Author: riley
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_


// PUBLIC API


typedef void (*task_func_t)(int now, void * input) ;

typedef struct struct_task_t {
    task_func_t func;
    void * task_sp; //Task stack pointer
    int task_tos; //Index into task_sp, points to top of stack (tos)
    struct struct_task_t * next;
} task_t;


/**
 * Add task to task list
 * Allocate task stack
 * Push task routine pointer and empty status register
 * onto the new task stack so they can be popped off later
 * from the task switch interrupt
 */
void scheduler_add_task(task_t * task_handle, task_func_t func, void * task_stack_base);

/**
 *
 */
void scheduler_run();

/**
 *
 */
void scheduler_init();

void scheduler_tick();

#endif /* SCHEDULER_H_ */
