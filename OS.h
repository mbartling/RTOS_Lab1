// OS.h
// Runs on the TM4C123
// Use 32b GP periodic timer to run a task periodically.
// Michael Bartling
// 1/31/2015
// Lab 1
// Last Modified: 2/3/2015

#ifndef OS_H__
#define OS_H__

/**
 * Globals
 */

// volatile unsigned int 

/**
 * Task: pointer to function to execute periodically
 * period: Period at which to execute task
 * priority: of the NVIC controller
 */
int OS_AddPeriodicThread(void(*task) (void), unsigned long period,
  unsigned long priority);

/**
 * reset the 32 bit counter to 0
 */
void OS_ClearPeriodicTime(void);

/**
 * @brief Returns the current system count of the running thread
 * @return global system count in period units of added thread
 */
unsigned long OS_ReadPeriodicTime(void);

#endif /*OS_H__*/