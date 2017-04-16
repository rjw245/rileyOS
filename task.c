/*
 * task.c
 *
 *  Created on: Apr 16, 2017
 *      Author: riley
 */

#include "task.h"
#include "scheduler.h"
#include "scheduler_private.h"
#include "rileyOS_config.h"
#include <msp430.h>

void task_sleep( uint32_t milliseconds ) {
    // This should store the sleep count in the
    // task descriptor and do a context switch
    __disable_interrupt();
    task_t * cur_task = scheduler_get_current_task();
    cur_task->sleep_start_ms = scheduler_now();
    cur_task->sleep_for_ms = milliseconds;
    __enable_interrupt();

    // Wait for the next context switch
    while(cur_task->sleep_for_ms);
}

const char * task_get_name( void ) {
    __disable_interrupt();
    const char * name = (scheduler_get_current_task())->name;
    __enable_interrupt();
    return name;
}
