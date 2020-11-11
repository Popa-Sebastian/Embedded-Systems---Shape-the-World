// Sound.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Use the SysTick timer to request interrupts at a particular period.
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// This routine calls the 4-bit DAC

#include "Sound.h"
#include "DAC.h"
#include "..//tm4c123gh6pm.h"

const unsigned long SineWave[16] = {  
	8,11,13,14,15,14,13,11,8,5,3,
  2,1,2,3,5};

unsigned long Index=0; // Index varies from 0 to 31
unsigned long period;
unsigned long counter=0;
//unsigned long counter_test=0;
//unsigned long Period_Test[100];

// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Also calls DAC_Init() to initialize DAC
// Input: none
// Output: none
void Sound_Init(void){
  DAC_Init();          // Port B is DAC
  Index = 0;
  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = 80000000;// reload value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000;
	// priority 1
  NVIC_ST_CTRL_R = 0x0007; // enable,core clock, and interrupts
}

// **************Sound_Tone*********************
// Change Systick periodic interrupts to start sound output
// Input: interrupt period
//           Units of period are 12.5ns
//           Maximum is 2^24-1
//           Minimum is determined by length of ISR
// Output: none
void Sound_Tone(unsigned long period){
	//NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  Index = 0;
	NVIC_ST_RELOAD_R = period-1;// reload value, determines frequency
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_ST_CTRL_R = 0x0007; // enable,core clock, and interrupts
	GPIO_PORTF_DATA_R &= ~0x08;
}


// **************Sound_Off*********************
// stop outputing to DAC
// Output: none
void Sound_Off(void){
 // this routine stops the sound output
		NVIC_ST_CTRL_R = 0; // stops SisTick
		Index =0;
 		DAC_Out(0);	// Output 0v
		GPIO_PORTF_DATA_R &= ~0x02; //Turns RED off
		GPIO_PORTF_DATA_R |=  0x08; // Sets GREEN = READY
}


// Interrupt service routine
// Executed every 12.5ns*(period)
void SysTick_Handler(void){
  Index = (Index+1)&0x0F;      // repeats every 16 values
  DAC_Out(SineWave[Index]);    // output one value each interrupt
	// HeartBeat function
	counter++;
	if (counter == 750){
		GPIO_PORTF_DATA_R ^= 0x02;	//Heartbeat, every 750 interrupts
		counter = 0;
	}
/*
// Use this routine to see the reaload value
	if (counter_test <100) {
		counter_test++;
		Period_Test[counter_test] = NVIC_ST_RELOAD_R;
	}
	*/
}
