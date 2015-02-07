// OS.c
// Runs on the TM4C123
// Use 32b GP periodic timer to run a task periodically.
// Michael Bartling
// 1/31/2015
// Lab 1
// Last Modified: 2/3/2015

#include "OS.h"
#include <stdint.h>
#include "inc/tm4c123gh6pm.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

#define MODE_MEASURE_ISR 1

#if MODE_MEASURE_ISR
// Taken from ST7735 test main

#define PF2   (*((volatile uint32_t *)0x40025010))
//    PF2 ^= 0x04;

// Make PF2 an output, enable digital I/O, ensure alt. functions off
void SSR_Init(void){ 
  SYSCTL_RCGCGPIO_R |= 0x20;        // 1) activate clock for Port F
  while((SYSCTL_PRGPIO_R&0x20)==0){}; // allow time for clock to start
                                    // 2) no need to unlock PF2
  GPIO_PORTF_PCTL_R &= ~0x00000F00; // 3) regular GPIO
  GPIO_PORTF_AMSEL_R &= ~0x04;      // 4) disable analog function on PF2
  GPIO_PORTF_DIR_R |= 0x04;         // 5) set direction to output
  GPIO_PORTF_AFSEL_R &= ~0x04;      // 6) regular port function
  GPIO_PORTF_DEN_R |= 0x04;         // 7) enable digital port
}
#endif

/*Private global variables */
void (*PeriodicTask)(void); 	//!< Function pointer to periodic task */
volatile uint32_t TaskCount;	//!< Global Counter in units specified by 
								//!< OS_AddPeriodicThread periods


/**
 * @brief Initializes a 32 bit general purpose timer in 12.5ns
 * Assume 80 MHz system clock
 * @param period period in us to set timer
 * @param priority NVIC timer priority
 * 
 * returns 0 if successful, -1 if invalid parameters given
 * Note period must be greater than 0
 * 
 * Max time is 53.687s
 */
int TimerOpen(unsigned long period, unsigned long priority){
	 volatile uint32_t delay;

	if(period < 1 || priority < 0){
		return -1;
	} 

 DisableInterrupts();
  
  SYSCTL_RCGCTIMER_R |= 0x02;   // activate timer1 
  delay = SYSCTL_RCGCTIMER_R;   // allow time to finish activating

  TIMER1_CTL_R = 0x00000000;    // disable timer1A during setup

  TIMER1_CFG_R = 0;             // configure for 32-bit timer mode
  TIMER1_TAMR_R = 0x00000002;   // configure for periodic mode, default down-count settings
  TIMER1_TAPR_R = 0;            // prescale value for trigger
  TIMER1_TAILR_R = period-1;    // start value for trigger
  TIMER1_IMR_R  = 0x0000000;    // disable all interrupts
 // TIMER1_CTL_R |= 0x0000001;    // enable timer1A 32-b

  TIMER1_ICR_R |= 0x0000001;
  TIMER1_IMR_R  = 0x0000001;		// enable timer1A interrupt timeout


  NVIC_PRI5_R = (NVIC_PRI5_R & 0xFFFF00FF) | (priority << 12); // 7) priority 
  NVIC_EN0_R = 1 << 21;     // 8) enable interrupt 21 in NVIC
  TIMER1_CTL_R |= 0x0000001;  // 9) enable timer1A
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
  #if MODE_MEASURE_ISR
  SSR_Init();
  #endif

	TaskCount = 0;
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
void Timer1A_Handler(void){
#if MODE_MEASURE_ISR
  PF2 ^= 0x04; // Toggle PF2 Bit
#endif
	//ack timer
	TIMER1_ICR_R |= 0x0000001;

	TaskCount++;
	PeriodicTask();
#if MODE_MEASURE_ISR
  PF2 ^= 0x04;
#endif

}
