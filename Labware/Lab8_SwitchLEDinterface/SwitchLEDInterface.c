// ***** 0. Documentation Section *****
// SwitchLEDInterface.c for Lab 8
// Runs on LM4F120/TM4C123
// Use simple programming structures in C to toggle an LED
// while a button is pressed and turn the LED on when the
// button is released.  This lab requires external hardware
// to be wired to the LaunchPad using the prototyping board.
// January 11, 2014

// Lab 8
//      Jon Valvano and Ramesh Yerraballi
//      November 21, 2013

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

void init_LED(void);
void init_SW(void);
void delay(unsigned long hundredMs);
void delay1(unsigned long hundredMs);
void toogleLed(void);
void LedOn(void);

#define SW_PRESSED (GPIO_PORTB_DATA_R & 0x01)
// ***** 3. Subroutines Section *****

// PE0, PB0, or PA2 connected to positive logic momentary switch using 10 k ohm pull down resistor
// PE1, PB1, or PA3 connected to positive logic LED through 470 ohm current limiting resistor
// To avoid damaging your hardware, ensure that your circuits match the schematic
// shown in Lab8_artist.sch (PCB Artist schematic file) or 
// Lab8_artist.pdf (compatible with many various readers like Adobe Acrobat).
int main(void){ 
//**********************************************************************
// The following version tests input on PE0 and output on PE1
//**********************************************************************
  TExaS_Init(SW_PIN_PB0, LED_PIN_PE1);  // activate grader and set system clock to 80 MHz
  
	
  EnableInterrupts();           // enable interrupts for the grader
	init_SW();
	init_LED();
  while(1){	
		delay(1);
		while(SW_PRESSED){
			toogleLed();
			delay(1);
		}
		LedOn();
  } 
}
void init_LED(){//PE1
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x10;           // 1) activate clock for Port E
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
                                    // 2) no need to unlock PE1
  GPIO_PORTE_PCTL_R &= ~0x000000F0; // 3) regular GPIO
  GPIO_PORTE_AMSEL_R &= ~0x02;      // 4) disable analog function on PE1
  GPIO_PORTE_DIR_R |= 0x02;         // 5) set direction to output
  GPIO_PORTE_AFSEL_R &= ~0x02;      // 6) regular port function
  GPIO_PORTE_DEN_R |= 0x02;         // 7) enable digital port
	//GPIO_PORTE_DATA_R |= 0x02; 				//Initialing led as ON
	LedOn();
	
}
void toogleLed(){
	GPIO_PORTE_DATA_R ^= 0x02;
}
void LedOn(){
	GPIO_PORTE_DATA_R |= 0x02; 
}
void init_SW(){//PB0
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x02;						//Activate clock for port B
	delay = SYSCTL_RCGC2_R;							//allow time for clock start
																			//unlock PB0
	GPIO_PORTB_PCTL_R &= ~0x0000000F;	//Regular GPIO
	GPIO_PORTB_AMSEL_R &= ~0x01;			//Disable analog function on PB0
	GPIO_PORTB_DIR_R &= ~0x01; 				//Set directions to input 0 output 1
	GPIO_PORTB_AFSEL_R &= ~0x01;			//Regular port function
	GPIO_PORTB_DEN_R |= 0x01;					//enable digital port
}

// Subroutine to delay in units of half seconds
// We will make a precise estimate later: 
//   For now we assume it takes 1/2 sec to count down
//   from 2,000,000 down to zero
// Inputs: Number of half seconds to delay
// Outputs: None
// Notes: ...
void delay(unsigned long hundredMs){
  unsigned long count;
  while(hundredMs > 0 ) { // repeat while still halfsecs to delay
    count = 1598400; 
// originally count was 400000, which took 0.13 sec to complete
// later we change it to 400000*0.1/0.13=307692 that it takes 0.1 sec 100ms
    while (count > 0) { 
      count--;
    } // This while loop takes approximately 3 cycles
    hundredMs--;
  }
}

///////////////////////////////////
// Subroutine to delay in units of half seconds
// We will make a precise estimate later: 
//   For now we assume it takes 1/2 sec to count down
//   from 2,000,000 down to zero
// Inputs: Number of half seconds to delay
// Outputs: None
// Notes: ...
void delay1(unsigned long halfsecs){
  unsigned long count;
  
  while(halfsecs > 0 ) { // repeat while there are still halfsecs to delay
    count = 1538460; // 400000*0.5/0.13 that it takes 0.13 sec to count down to zero
    while (count > 0) { 
      count--;
    } // This while loop takes approximately 3 cycles
    halfsecs--;
  }
}

	


