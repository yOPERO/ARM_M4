// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// November 7, 2013

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

#include "Systick.h"
#include "PLL.h"

#define GoN 			0
#define WaitN_E 	1
#define WaitN_W 	2
#define GoE 			3
#define WaitE_N 	4
#define WaitE_W 	5
#define Walk 			6	
#define DontWalk	7
#define HurryUp 	8
#define HurryUp1 	9
#define HurryUp2 10
#define HurryUp3 11

//#define myPorts (*((volatile unsigned long *)0x400050FC))

// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts


void PortF_Init(void);
void PortB_Init(void);
void PortE_Init(void);
// ***** 3. Subroutines Section *****
struct State{
	unsigned long outputs[2];
	unsigned char delays;
	unsigned char next[8];
};
typedef const struct State StateType;

StateType Fsm[12] = {
	{{0x21,0x02},200,{GoN,WaitN_E,GoN,WaitN_E,WaitN_W,WaitN_W,WaitN_W,WaitN_E}},
	{{0x22,0x02},100,{GoE,GoE,GoE,GoE,GoE,GoE,GoE,GoE}},
	{{0x22,0x02},100,{Walk,Walk,Walk,Walk,Walk,Walk,Walk,Walk}},
	{{0x0C,0x02},200,{GoE,GoE,WaitE_N,WaitE_N,WaitE_W,WaitE_W,WaitE_W,WaitE_W}},
	{{0x14,0x02},100,{GoN,GoN,GoN,GoN,GoN,GoN,GoN,GoN}},
	{{0x14,0x02},100,{Walk,Walk,Walk,Walk,Walk,Walk,Walk,Walk}},
	{{0x24,0x08},200,{Walk,HurryUp,HurryUp,HurryUp,Walk,HurryUp,HurryUp,HurryUp}},
	{{0x24,0x02},50,{GoE,GoE,GoN,GoN,Walk,GoE,GoN,GoN}},
	{{0x24,0x02},50,{HurryUp1,HurryUp1,HurryUp1,HurryUp1,HurryUp1,HurryUp1,HurryUp1,HurryUp1}},
	{{0x24,0x00},50,{HurryUp2,HurryUp2,HurryUp2,HurryUp2,HurryUp2,HurryUp2,HurryUp2,HurryUp2}},
	{{0x24,0x02},50,{HurryUp3,HurryUp3,HurryUp3,HurryUp3,HurryUp3,HurryUp3,HurryUp3,HurryUp3}},
	{{0x24,0x00},50,{DontWalk,DontWalk,DontWalk,DontWalk,DontWalk,DontWalk,DontWalk,DontWalk}}
};
unsigned char State;
unsigned long input;	
int main(void){
	
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210); // activate grader and set system clock to 80 MHz
  EnableInterrupts();
	PortF_Init();
	PortB_Init();
	PortE_Init();
  PLL_Init1();               // set system clock to 50 MHz
  SysTick_Init();           // initialize SysTick 
	
	State = GoN;
  
  while(1){
		//output based on current state
		GPIO_PORTB_DATA_R = Fsm[State].outputs[0];
		GPIO_PORTF_DATA_R = Fsm[State].outputs[1];
		//wait for time acording to state
		SysTick_Wait10ms(Fsm[State].delays);
		//get input
		input = GPIO_PORTE_DATA_R&0x07;
		//change state base on the input
		State = Fsm[State].next[input];
  }
}
void PortF_Init(void){
	unsigned long volatile delay;
	SYSCTL_RCGC2_R |= 0x20;
	delay = SYSCTL_RCGC2_R;//dummy delay
//	GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
//	GPIO_PORTF_CR_R = 0x0A; 
	
	GPIO_PORTF_AFSEL_R &= ~0x0A;
	GPIO_PORTF_AMSEL_R &= ~0x0A;
	GPIO_PORTF_PCTL_R &= ~0x0000F0F0;
	GPIO_PORTF_DIR_R |= 0x0A;
	GPIO_PORTF_DEN_R |= 0x0A;
	
	GPIO_PORTF_DATA_R |= 0x00;
}
void PortB_Init(void){
	unsigned long volatile delay;
	SYSCTL_RCGC2_R |= 0x02;
	delay = SYSCTL_RCGC2_R;//dummy delay
  GPIO_PORTB_AMSEL_R &= ~0x3F;      // 3) disable analog on PA7
  GPIO_PORTB_PCTL_R &= ~0x00FFFFFF; // 4) PCTL GPIO on PA7
  GPIO_PORTB_DIR_R |= 0x3F;         // 5) PA7 out
  GPIO_PORTB_AFSEL_R &= ~0x3F;      // 6) disable alt funct on PA7
  GPIO_PORTB_DEN_R |= 0x3F;         // 7) enable digital I/O on PA7
	
	GPIO_PORTB_DATA_R = 0x00;
}
void PortE_Init(void){
	unsigned long volatile delay;
	SYSCTL_RCGC2_R |= 0x10;
	delay = SYSCTL_RCGC2_R;//dummy delay

	GPIO_PORTE_DIR_R &= ~0x07;
	GPIO_PORTE_AFSEL_R &= ~0x07;
	GPIO_PORTE_AMSEL_R &= ~0x07;
	GPIO_PORTE_PCTL_R &= ~0x00000FFF;
	GPIO_PORTE_DEN_R |= 0x07;
}


