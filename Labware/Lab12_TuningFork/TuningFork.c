// TuningFork.c Lab 12
// Runs on LM4F120/TM4C123
// Use SysTick interrupts to create a squarewave at 440Hz.  
// There is a positive logic switch connected to PA3, PB3, or PE3.
// There is an output on PA2, PB2, or PE2. The output is 
//   connected to headphones through a 1k resistor.
// The volume-limiting resistor can be any value from 680 to 2000 ohms
// The tone is initially off, when the switch goes from
// not touched to touched, the tone toggles on/off.
//                   |---------|               |---------|     
// Switch   ---------|         |---------------|         |------
//
//                    |-| |-| |-| |-| |-| |-| |-|
// Tone     ----------| |-| |-| |-| |-| |-| |-| |---------------
//
// Daniel Valvano, Jonathan Valvano
// March 8, 2014

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2013
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013

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


#include "TExaS.h"
#include "..//tm4c123gh6pm.h"

unsigned volatile char ISR_status = 0x00;
unsigned volatile char SW_status = 0x00;
// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode
void Sound_Init(void);			//enable speaker and switch
// input from PA3, output from PA2, SysTick interrupts
void Sound_Init(void){
	/***PA2 and PA3 setup****/
	unsigned long volatile delay;
	SYSCTL_RCGC2_R |= 0x00000001; // activate port A
  delay = SYSCTL_RCGC2_R;
  GPIO_PORTA_AMSEL_R &= ~0x0C;     // no analog 
  GPIO_PORTA_PCTL_R &= ~0x0000FF00; // regular function
  GPIO_PORTA_DIR_R |= 0x04;     // make PA2 output
	GPIO_PORTA_DIR_R &= ~0x08; 		// make PA3 input
	
//  GPIO_PORTA_DR8R_R |= 0x04;    // PA2 can drive up to 8mA out
  GPIO_PORTA_AFSEL_R &= ~0x0C;  // disable alt funct on PA5
  GPIO_PORTA_DEN_R |= 0x0C;     // enable digital I/O on 

	GPIO_PORTA_DATA_R &= ~0x04;
	/**Systick setup**/
	NVIC_ST_CTRL_R = 0; //disable systick during setup
	NVIC_ST_RELOAD_R = 90909-1;//Reload value for 880Hz wave 2.2727ms
	NVIC_ST_CURRENT_R = 0;//Any value to current
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0X00FFFFFF) | 0X40000000;//Priority 2
	NVIC_ST_CTRL_R  = 0X07;//Enable systick with core clock and interrupts
}

// called at 880 Hz
void SysTick_Handler(void){
	if(SW_status){		
		GPIO_PORTA_DATA_R ^= 0x04;     // toggle PA2
	}else{
		GPIO_PORTA_DATA_R &= ~0x04;     // Keep PA2 off
	}
}

int main(void){// activate grader and set system clock to 80 MHz
	unsigned volatile char previous_SW;
	unsigned volatile char current_SW;
	
  TExaS_Init(SW_PIN_PA3, HEADPHONE_PIN_PA2,ScopeOn); 
	DisableInterrupts();
	Sound_Init();         
  EnableInterrupts();   // enable after all initialization are done
  
	while(1){
		current_SW = (GPIO_PORTA_DATA_R&0x08)>>3;
		if(previous_SW == 0x00 && current_SW == 0x01){
			SW_status ^= 0x01;
		}
		previous_SW = current_SW;
    // main program is free to perform other tasks
    // do not use WaitForInterrupt() here, it may cause the TExaS to crash
  }
}
