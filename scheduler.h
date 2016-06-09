/*
 * scheduler.h
 *
 *  Created on: Jun 8, 2016
 *      Author: riley
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

// PUBLIC API
void addTask(void (*f)(), int interval);
void runScheduler();
void initScheduler();

// PRIVATE MODULE FUNCTIONS
static void setupSchedulerTick();


#endif /* SCHEDULER_H_ */
