/*
 * task.h
 *
 *  Created on: Jun 8, 2016
 *      Author: riley
 */

#ifndef TASK_H_
#define TASK_H_

#include "rileyOS_config.h"
#include <stdint.h>

#define MAX_STACK_WORDS		50

#define INTERVAL_1ms		1/SCHEDULER_TICK_MS
#define INTERVAL_10ms		10/SCHEDULER_TICK_MS
#define INTERVAL_100ms		100/SCHEDULER_TICK_MS
#define INTERVAL_1000ms		1000/SCHEDULER_TICK_MS

#define NO_TASK_SWITCH		-1

typedef void (*TaskRoutine)() ;

typedef struct {
	int interval;
	int last_tick;
	TaskRoutine routine;
	uint16_t * task_sp; //Task stack pointer, uint16_t used so that it is aligned on words
	int ready;
} TaskDescriptor;




#endif /* TASK_H_ */
