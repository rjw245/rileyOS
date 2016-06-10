/*
 * scheduler.h
 *
 *  Created on: Jun 8, 2016
 *      Author: riley
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

// PUBLIC API

/**
 * Add task to task list
 * Allocate task stack
 * Push task routine pointer and empty status register
 * onto the new task stack so they can be popped off later
 * from the task switch interrupt
 */
void addTask(void (*f)(), int interval, unsigned int stack_len);

/**
 *
 */
void runScheduler();

/**
 *
 */
void initScheduler();

// PRIVATE MODULE FUNCTIONS
static void setupSchedulerTick();
static int taskSwitch();
static void idleTask();

#endif /* SCHEDULER_H_ */
