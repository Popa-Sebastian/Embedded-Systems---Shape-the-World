// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// January 15, 2016
// Sebastian Popa
// October 24, 2020

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

#define GPIO_PORTB_DIR_R        (*((volatile unsigned long *)0x40005400))
#define GPIO_PORTB_AFSEL_R      (*((volatile unsigned long *)0x40005420))
#define GPIO_PORTB_DEN_R        (*((volatile unsigned long *)0x4000551C))
#define GPIO_PORTB_AMSEL_R      (*((volatile unsigned long *)0x40005528))
#define GPIO_PORTB_PCTL_R       (*((volatile unsigned long *)0x4000552C))

#define GPIO_PORTE_DIR_R        (*((volatile unsigned long *)0x40024400))
#define GPIO_PORTE_AFSEL_R      (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_DEN_R        (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_AMSEL_R      (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_PCTL_R       (*((volatile unsigned long *)0x4002452C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
	
#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_LOCK_R       (*((volatile unsigned long *)0x40025520))
#define GPIO_PORTF_CR_R         (*((volatile unsigned long *)0x40025524))
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))
	
#define SYSCTL_RCGC2_GPIOE      0x00000010  // port E Clock Gating Control
#define SYSCTL_RCGC2_GPIOB      0x00000002  // port B Clock Gating Control

#define NVIC_ST_CTRL_R      (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R    (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R   (*((volatile unsigned long *)0xE000E018))

// ***** 2. Global Declarations Section *****
unsigned long State;  // index to the current state 
unsigned long Input; 

// Linked data structure
struct State {
  unsigned long PBOut;
  unsigned long PFOut;	
  unsigned long Time;  
  unsigned long Next[9];
};
typedef const struct State STyp;
	
#define GoW   		0
#define WaitW 		1
#define GoS   		2
#define WaitS		3
#define Walk		4
#define Flash1ON	5
#define Flash1OFF	6
#define Flash2ON	7
#define Flash2OFF	8

// The functioning of the program is described with a Moore Finite State Machine defined in a Struct State
// There are 9 total states. Every state has an output(PBout and PFout) that only depends on the current state 
// For every possible input (8 inputs from 000 to 111) there is a State Transition
STyp FSM[9]={
 {0x0C, 0x02, 100,{GoW, GoW, WaitW, WaitW, WaitW, WaitW, WaitW, WaitW}},//0
 {0x14, 0x02, 50, {GoS, GoS, GoS, GoS, Walk, Walk, GoS, GoS}},//1
 {0x21, 0x02, 100,{GoS, WaitS, GoS, WaitS, WaitS, WaitS, WaitS, WaitS}},//2
 {0x22, 0x02, 50, {Walk, GoW, Walk, GoW, Walk, Walk, Walk, Walk}},//3
 {0x24, 0x08, 100,{Walk, Flash1ON, Flash1ON, Flash1ON, Walk, Flash1ON, Flash1ON, Flash1ON}},//4
 {0x24, 0x02, 15, {Flash1OFF, Flash1OFF, Flash1OFF, Flash1OFF, Flash1OFF, Flash1OFF, Flash1OFF, Flash1OFF}},//5
 {0x24, 0x00, 15, {Flash2ON, Flash2ON, Flash2ON, Flash2ON, Flash2ON, Flash2ON, Flash2ON, Flash2ON}},//6
 {0x24, 0x02, 15, {Flash2OFF, Flash2OFF, Flash2OFF, Flash2OFF, Flash2OFF, Flash2OFF, Flash2OFF, Flash2OFF}},//7
 {0x24, 0x00, 15, {GoW, GoW, GoS, GoW, Walk, GoW, GoS, GoW}}//8
};

// FUNCTION PROTOTYPES: 
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

// Port Initializations
void PortB_Init(void);
void PortE_Init(void);
void PortF_Init(void);

// SysTick Initalization and Delay Functions
void SysTick_Init(void);
void SysTick_Wait10ms(unsigned long delay);
void SysTick_Wait(unsigned long delay);

// ***** 3. Subroutines Section *****
int main(void){ 
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210,ScopeOff); // activate grader and set system clock to 80 MHz
  EnableInterrupts();
	
  PortB_Init();		// OUTPUTS WEST:PB5-PB3=RYG, SOUTH:PB2-PB0=RYG
  PortE_Init();		// INPUTS: PE2=WALK, PE1=SOUTH, PE0=WEST
  PortF_Init();		// OUTPUTS PEDESTRIAN: PF3=G, PF1=R
  SysTick_Init();	// Initializez SysTick for the Delay functions
	
  State = GoW; 
  while(1) {
	GPIO_PORTB_DATA_R = FSM[State].PBOut;
	GPIO_PORTF_DATA_R = FSM[State].PFOut;
	SysTick_Wait10ms(FSM[State].Time);
	Input = GPIO_PORTE_DATA_R &0x07;
	State = FSM[State].Next[Input];  
  }
}

// Intilizes PORT_B
void PortB_Init(void) {
  volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x02; 	// 1) Port B Clock
  delay = SYSCTL_RCGC2_R;	// 2) no need to unlock
  GPIO_PORTB_AMSEL_R &= ~0x3F;	// 3) disable analog function on PB5-0
  GPIO_PORTB_PCTL_R &= ~0x00FFFFFF; // 4) enable regular GPIO
  GPIO_PORTB_DIR_R |= 0x3F;  	// 5) outputs on PB5-0
  GPIO_PORTB_AFSEL_R &= ~0x3F;	// 6) regular function on PB5-0
  GPIO_PORTB_DEN_R |= 0x3F;   	// 7) enable digital on PB5-0
}
// Intilizes PORT_E
void PortE_Init(void) {
  volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x10;      	// 1) PORT E Clock
  delay = SYSCTL_RCGC2_R;      	// 2) no need to unlock
  GPIO_PORTE_AMSEL_R &= 0x00; 	// 3) disable analog function
  GPIO_PORTE_PCTL_R &= ~0x000000FF; // 4) enable regular GPIO
  GPIO_PORTE_DIR_R &= ~0x07;   	// 5) inputs on PE2, PE1, PE0
  GPIO_PORTE_AFSEL_R &= 0x00; 	// 6) regular function on PE2-0
  GPIO_PORTE_DEN_R |= 0x07;   	// 7) enable digital on PE2-0
}
// Intilizes PORT_F
void PortF_Init(void) {
  volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x20;					// 1) Port F Clock
  delay = SYSCTL_RCGC2_R;					// allow time for clock to start
  GPIO_PORTF_LOCK_R = 0x4C4F434B;	// 2) unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x1F;					// allow changes to PF4-0
  // only PF0 needs to be unlocked, other bits can't be locked	
  GPIO_PORTF_AMSEL_R = 0x00;    	// 3) disable analog on PF
  GPIO_PORTF_PCTL_R = 0x00000000; // 4) PCTL GPIO on PF4-0
  GPIO_PORTF_DIR_R |= 0x0A;     	// 5) PF3, PF1 OUT
  GPIO_PORTF_AFSEL_R = 0x00;    	// 6) disable alt funct on PF7-0
  //GPIO_PORTF_PUR_R = 0x11;    	// enable pull-up on PF0 and PF4
  GPIO_PORTF_DEN_R = 0x1F;      	// 7) enable digital I/O on PF4-0
}
// Initializez SysTick (clock counter, very precise when using external crystal and PLL)
void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;             // disable SysTick during setup
  NVIC_ST_CTRL_R = 0x00000005;    // enable SysTick with core clock
}
// Waits 10ms
// The delay parameter is in units of the 80 MHz core clock. (12.5 ns)
void SysTick_Wait(unsigned long delay){
  NVIC_ST_RELOAD_R = delay-1;  // number of counts to wait
  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears
  while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait for count flag
  }
}
// Waits input * 10ms
// 10000us equals 10ms
void SysTick_Wait10ms(unsigned long delay){
  unsigned long i;
  for(i=0; i<delay; i++){
    SysTick_Wait(800000);  // wait 10ms
  }
}
