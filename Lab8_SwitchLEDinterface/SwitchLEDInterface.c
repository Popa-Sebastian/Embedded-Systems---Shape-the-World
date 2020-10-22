// ***** 0. Documentation Section *****
// SwitchLEDInterface.c for Lab 8
// Runs on LM4F120/TM4C123
// Use simple programming structures in C to toggle an LED
// while a button is pressed and turn the LED on when the
// button is released.  This lab requires external hardware
// to be wired to the LaunchPad using the prototyping board.
// January 15, 2016
//      Jon Valvano and Ramesh Yerraballi

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define GPIO_PORTE_DATA_R       (*((volatile unsigned long *)0x400243FC))
#define GPIO_PORTE_DIR_R        (*((volatile unsigned long *)0x40024400))
#define GPIO_PORTE_AFSEL_R      (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_DEN_R        (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_AMSEL_R      (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_PCTL_R       (*((volatile unsigned long *)0x4002452C))

// ***** 2. Global Declarations Section *****
unsigned long SW, LED;
// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void delayms(unsigned long ms);//Delays for a number of 'ms' miliseconds
void WaitForSw(void);
void Led_ON(void);
void Led_OFF(void);
void PortE_Init(void);

// ***** 3. Subroutines Section *****

// PE0, PB0, or PA2 connected to positive logic momentary switch using 10k ohm pull down resistor
// PE1, PB1, or PA3 connected to positive logic LED through 470 ohm current limiting resistor
// To avoid damaging your hardware, ensure that your circuits match the schematic
// shown in Lab8_artist.sch (PCB Artist schematic file) or 
// Lab8_artist.pdf (compatible with many various readers like Adobe Acrobat).
int main(void){ 
//**********************************************************************
// The following version tests input on PE0 and output on PE1
//**********************************************************************
  TExaS_Init(SW_PIN_PE0, LED_PIN_PE1, ScopeOn);  // activate grader and set system clock to 80 MHz
  PortE_Init();	
  EnableInterrupts();           // enable interrupts for the grader
	
  while(1){
    Led_ON();
		SW = GPIO_PORTE_DATA_R & 0x01;
		delayms(100);
		WaitForSw();
		Led_OFF();
		delayms(100);
  }
  
}

// Subroutine to delay in units of 1 msec
// Inputs: Number of ms to delay
// Outputs: None
void delayms(unsigned long ms){
  unsigned long count;
  while(ms > 0 ) { // repeat while there are still ms to delay
    //count = 16000; // Simulator
    count = 13333; // BOARD
		while (count > 0) { 
      count--;
    } // This while loop takes approximately 3 cycles
    ms--;
  }
}

// Subroutine to wait for switch press
// Inputs: None
// Outputs: None
void WaitForSw(void){
	while(!(GPIO_PORTE_DATA_R&0x01))
		delayms(1);
}

void Led_ON(void){
	GPIO_PORTE_DATA_R |= 0x02;
}

void Led_OFF(void){
	GPIO_PORTE_DATA_R &= ~0x02;
}
void PortE_Init(void){ volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x10;
	delay = SYSCTL_RCGC2_R;
	GPIO_PORTE_DIR_R |= 0x02;
	GPIO_PORTE_DIR_R &= ~ 0x01;
	GPIO_PORTE_AFSEL_R &= ~ 0x03;
	GPIO_PORTE_AMSEL_R &= ~ 0x03;
	GPIO_PORTE_PCTL_R &= ~ 0xFF;
	GPIO_PORTE_DEN_R |= 0x3;
}
