/*
 * task.h
 *
 *  Created on: Jun 8, 2016
 *      Author: riley
 */

#ifndef TASK_H_
#define TASK_H_

#include "rileyOS_config.h"

#define INTERVAL_1ms		1/SCHEDULER_TICK_MS
#define INTERVAL_10ms		10/SCHEDULER_TICK_MS
#define INTERVAL_100ms		100/SCHEDULER_TICK_MS
#define INTERVAL_1000ms		1000/SCHEDULER_TICK_MS

typedef struct {
	unsigned int priority;  // Lower number = higher priority
	int interval;
	int last_tick;
	void (*routine) (void);
} TaskDescriptor;



#endif /* TASK_H_ */
