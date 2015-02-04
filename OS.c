// OS.c
// Runs on the TM4C123
// Use 32b GP periodic timer to run a task periodically.
// Michael Bartling
// 1/31/2015
// Lab 1
// Last Modified: 2/3/2015

#include "OS.h"

/*Private global variables */
void (*PeriodicTask)(void); 	//!< Function pointer to periodic task */
volatile uint32_t TaskCount;	//!< Global Counter in units specified by 
								//!< OS_AddPeriodicThread periods


/**
 * @brief Initializes a 32 bit general purpose timer in uSec
 * Assume 80 MHz system clock
 * @param period period in us to set timer
 * @param priority NVIC timer priority
 * 
 * returns 0 if successful, -1 if invalid parameters given
 * Note period must be greater than 0
 */
int TimerOpen(unsigned long period, unsigned long priority){
	if(period < 1 || priority < 0){
		return -1;
	} 

	return 0; 
}

/**
 * Task: pointer to function to execute periodically
 * period: Period at which to execute task in us.
 * priority: of the NVIC controller
 * returns 0 if successful, -1 if invalid parameters given
 * note period must be greater than 0
 */
int OS_AddPeriodicThread(void(*task) (void), unsigned long period,
  unsigned long priority){
	int status;
	status = TimerOpen(period, priority);

	if(status < 0){
		return status; // Periodic Task will not be set if TimerOpen Fails
	} 

	PeriodicTask = task;
	return 0;
}

/**
 * reset the 32 bit counter to 0
 */
void OS_ClearPeriodicTime(void){
	TaskCount = 0;
}

/**
 * @brief Returns the current system count of the running thread
 * @return global system count in period units of added thread
 */
unsigned long OS_ReadPeriodicTime(void){
	return TaskCount;
}