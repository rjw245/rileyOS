/*
 * task.h
 *
 * Provides functions which scheduler
 * tasks can use at runtime to interact
 * with the scheduler.
 *
 *  Created on: Apr 16, 2017
 *      Author: riley
 */

#ifndef TASK_H_
#define TASK_H_

#include <stdint.h>

void task_sleep( uint32_t milliseconds );
const char * task_get_name( void );
void task_join( void );

#endif /* TASK_H_ */
