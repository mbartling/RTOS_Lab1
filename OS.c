// OS.c
// Runs on the TM4C123
// Use 32b GP periodic timer to run a task periodically.
// Michael Bartling
// 1/31/2015
// Lab 1
// Last Modified: 2/3/2015

#include "OS.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode


/*Private global variables */
void (*PeriodicTask)(void); 	//!< Function pointer to periodic task */
volatile uint32_t TaskCount;	//!< Global Counter in units specified by 
								//!< OS_AddPeriodicThread periods


/**
 * @brief Initializes a 32 bit general wide purpose timer in 12.5ns
 * Assume 80 MHz system clock
 * @param period period in us to set timer
 * @param priority NVIC timer priority
 * 
 * returns 0 if successful, -1 if invalid parameters given
 * Note period must be greater than 0
 * 
 * Uses the wide timer to keep more timers open for later projects
 * Max time is 53.687s
 */
int TimerOpen(unsigned long period, unsigned long priority){
	if(period < 1 || priority < 0){
		return -1;
	} 

 DisableInterrupts();
  SYSCTL_RCGCADC_R |= 0x01;     // activate ADC0 
  SYSCTL_RCGCTIMER_R |= 0x01;   // activate timer0 
  delay = SYSCTL_RCGCTIMER_R;   // allow time to finish activating

  TIMER0_CTL_R = 0x00000000;    // disable timer0A during setup
  TIMER0_CTL_R |= 0x00000020;   // enable timer0A trigger to ADC
  TIMER0_CFG_R = 0;             // configure for 32-bit timer mode
  TIMER0_TAMR_R = 0x00000002;   // configure for periodic mode, default down-count settings
  TIMER0_TAPR_R = 0;            // prescale value for trigger
  TIMER0_TAILR_R = (80000000/fs)-1;    // start value for trigger
  TIMER0_IMR_R = 0x00000000;    // disable all interrupts
  TIMER0_CTL_R |= 0x00000001;   // enable timer0A 32-b, periodic, no interrupts

	EnableInterrupts();

	return 0; 
}

/**
 * Task: pointer to function to execute periodically
 * period: Period at which to execute task in 12.5ns.
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

/**
 * @brief Increment global timer and run task
 * 
 */
void WideTimer0A_Handler(void){
	//ack timer

	TaskCount++;
	PeriodicTask();

}