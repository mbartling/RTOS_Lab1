// ADC.h
// Runs on LM4F120/TM4C123
// Basic ADC driver. Includes support for selecting ADC0-11
// Michael Bartling
// January 2015

#include "ADC.h"

//------------ADC0_InSeq3------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
// Taken from ADC SW trigger lab, modified for our use
/**
 * @brief ADC_In gets one sample from the current ADC driver
 * @details Retrieve a 10 bit scaled value from the ADC driver
 * Must run ADC_Open before calling ADC_In, else returns error codes
 * @return 10 bit scaled sample from configured ADC. Error Codes are 
 * indicated by masking with 0xFC00. 0xFC00 denotes device not initialized.
 * Other error codes are reserved.
 */

volatile int Open = 0;				//Default to not open
volatile int Collecting = 0;		//Is ADC_Collect active?
volatile unsigned short* Buffer;	//Pointer to ADC_Collect buffer
volatile int SampleCount;			//Current Sample count 
volatile int TargetCount;			//Target cound for collect


uint16_t ADC_In(void){  
  uint32_t result;
  if(!Open)
  {
  	return 0xFC00; //ADC Not initialized
  }
  NVIC_DIS0_R = 1<<17; // Disable interrupt 17 in NVIC
  									   // Disables sample sequencer conversion 
  									   // interrupt

  ADC0_PSSI_R = 0x0008;             // 1) initiate SS3
  while((ADC0_RIS_R&0x08)==0){};    // 2) wait for conversion done
    // if you have an A0-A3 revision number, you need to add an 8 usec wait here
  result = ADC0_SSFIFO3_R&0xFFF;    // 3) read result
  ADC0_ISC_R = 0x0008;              // 4) acknowledge completion
  NVIC_EN0_R = 1<<17;               // enable interrupt 17 in NVIC
    								// Enables sample sequencer conversion 
  									// interrupt

  return (uint16_t) result;
}

/**
 * @brief Set up ADC on specified channel number
 * @details The parameters are default as follows
 * Timer0A: enabled
 * Mode: 32-bit, down counting
 * One-shot or periodic: periodic
 * Interval value: programmable using 32-bit period
 * Sample time is busPeriod*period
 * sample rate: <=125,000 samples/second
 * 
 * @param channelNum the desired ADC channel
 * @return 0 if successful, -1 for device driver error
 * likely indicating driver already configured. 
 */
int ADC_Open(unsigned int channelNum)
{

  Open = 1; // ADC successully opened
  return Open;
}



/**
 * @brief Collect a set number of samples
 * @details [long description]
 * 
 * @param channelNum Channel to configure
 * @param fs Sample Frequency (Hz)
 * @param buffer Array to buffer data, does not bounds check buffer 
 * @param numberOfSamples number of samples to record
 * @return 0 if initialization successful
 */
int ADC_Collect(unsigned int channelNum, unsigned int fs,
				unsigned short buffer[], unsigned int numberOfSamples)
{

}

/**
 * @brief returns 0 when ADC_collect finishes
 * @return 0 if ADC_collect is complete
 */
int ADC_Status(void)
{

}
