// Lab1Main.c
// Runs on LM4F120/TM4C123
// Tests Lab 1
// patterns to the LCD.
// Original Daniel Valvano
// September 12, 2013
// Modified heavily by Michael Bartling and Behzad Boroujerdian

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected 
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO)
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground
#include <stdio.h>
#include <stdint.h>
#include "UART0.h"
#include "ST7735.h"
#include "PLL.h"
#include "inc/tm4c123gh6pm.h"
#include "interpreter.h"
#include "ADC.h"
#include "OS.h"


void test(void){
  printf("Hi\n");
}

void dummy(void){}

uint16_t Res_buffer[1024];
int main(void){

  PLL_Init();
  UART0_Init();
  //ST7735_InitR(INITR_REDTAB);
	ADC_Open(0);
	printf("\n[MAIN] %s:%d:%s() >> %s\n", __FILE__, __LINE__, __func__, "Starting Main LCD Test\n");
	int i=0, j, k=0;
	uint16_t res;
	
	
	while(k < 500)
	{
		res = ADC_In();
		printf("%f\t", ((float)res)* 3.0/4096.0);
//		if(i == 5){
		//	printf("\n");
		//	printf(">> ");
			i = 0;
//		}
		i++;
		k++;
	}
	printf("\nStarting collect test\n");
	ADC_Collect(0, 10000, Res_buffer, 1024);
	
	while(ADC_Status()){}
	for(i = 0; i < 1024; i++){
			printf("%f\t", ((float)Res_buffer[i])* 3.0/4096.0);

	}
  // Added Code
  
  k = 10;
  char str[10];
  for(j = 0; j < 4; j++){
		for(i = 0; i < 2; i++){
      sprintf(str, "Minion#%d", k);
      k++;
 //     ST7735_Message(i, j, str, 0);    
    }
  }
  
  //OS_AddPeriodicThread(test, 80000000, 4);    

  OS_AddPeriodicThread(dummy, 80000000, 4);

  // End Added Code
  // ST7735_OutString("Graphics test\n");
  // ST7735_OutString("cubic function\n");
  // ST7735_OutString("All hail Michael\n");
  // ST7735_PlotClear(0,4095);  // range from 0 to 4095
  // for(j=0;j<128;j++){
  //   ST7735_PlotPoints(j*j/2+900-(j*j/256)*j,32*j); // cubic,linear
  //   ST7735_PlotNext(); 
  // }   // called 128 times
   while(1){
		   interpreter();

   }

}

