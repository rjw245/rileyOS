/*
 * scheduler_private.h
 *
 *  Created on: Apr 2, 2017
 *      Author: riley
 */

#ifndef SCHEDULER_PRIVATE_H_
#define SCHEDULER_PRIVATE_H_

volatile struct task_private_s {
    const char * name;
    volatile void * task_sp; //Task stack pointer
    volatile struct task_private_s * prev;
    volatile struct task_private_s * next;
    volatile uint32_t sleep_start_ms;
    volatile uint32_t sleep_for_ms;
};

/*********** PRIVATE MACROS *************/

#define CREATE_TASK_HANDLE_INDIRECT(line, func) \
    static task_t task_ ## func ## _ ## line

#define CREATE_TASK_HANDLE(line, func) \
    CREATE_TASK_HANDLE_INDIRECT(line, func)

#define CREATE_TASK_STACK_INDIRECT(line, func, size) \
    static uint16_t task_stack_ ## func ## _ ## line[(size)/sizeof(uint16_t)]

#define CREATE_TASK_STACK(line, func, size) \
    CREATE_TASK_STACK_INDIRECT(line, func, size)


#define CALL_SCHEDULER_ADD_INDIRECT(line, func) \
    scheduler_add_task(&task_ ## func ## _ ## line, \
                       #func, \
                       func, \
                       task_stack_ ## func ## _ ## line, \
                       sizeof(task_stack_ ## func ## _ ## line))

#define CALL_SCHEDULER_ADD(line, func) \
    CALL_SCHEDULER_ADD_INDIRECT(line, func)

/*********** PRIVATE FUNCTIONS *************/

volatile struct task_private_s * scheduler_get_current_task( void );
volatile uint32_t scheduler_now( void );

#endif /* SCHEDULER_PRIVATE_H_ */
