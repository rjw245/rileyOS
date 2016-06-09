/*
 * task.h
 *
 *  Created on: Jun 8, 2016
 *      Author: riley
 */

#ifndef TASK_H_
#define TASK_H_

typedef struct {
	unsigned int priority;  // Lower number = higher priority
	int interval;
	int last_tick;
	void (*routine) (void);
} TaskDescriptor;



#endif /* TASK_H_ */
