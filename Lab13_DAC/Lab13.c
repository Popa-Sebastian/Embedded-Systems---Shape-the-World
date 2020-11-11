// Lab13.c
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a 4-key digital piano
// edX Lab 13 
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// Port B bits 3-0 have the 4-bit DAC
// Port E bits 3-0 have 4 piano keys

#include "..//tm4c123gh6pm.h"
#include "Sound.h"
#include "Piano.h"
#include "TExaS.h"
#include "DAC.h"

#define C0  9556   	// 523.3 Hz
#define D   8514  	// 587.3 Hz
#define E   7584  	// 659.3 Hz
#define G   6378  	// 784 Hz
/*
#define C0  19112   // 523.3 Hz
#define D   17028  // 587.3 Hz
#define E   15168  // 659.3 Hz
#define G   12756 // 784 Hz
*/
// Global variable declaration
// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void delay(unsigned long msec);
void PortF_Init(void);
void DAC_Test(void);
int main(void){ // Real Lab13 
	// for the real board grader to work 
	// you must connect PD3 to your DAC output
	unsigned long key;
	unsigned long notes[4] = {C0, D, E, G};
	//unsigned long frequency;
  TExaS_Init(SW_PIN_PE3210, DAC_PIN_PB3210,ScopeOn); // activate grader and set system clock to 80 MHz
// PortE used for piano keys, PortB used for DAC        
	Sound_Init(); // initialize SysTick timer and DAC
  Piano_Init();
	PortF_Init(); // PortF is used for debugging function (Hearbeat, DAC test)
  EnableInterrupts();  // enable after all initialization are done
	delay(3000);	// necesarry for grader to see SysTick online
	Sound_Off(); // start with no output
  while(1){
     key = Piano_In(); // reads key pressed
		 if (key == 0){		// no key pressed
			 Sound_Off();
			// DisableInterrupts();
		 } else if (key<5){
			// EnableInterrupts();
			 Sound_Tone(notes[key-1]); // outputs sound
		 }
		}
	}
// Inputs: Number of msec to delay
// Outputs: None
void delay(unsigned long msec){ 
  unsigned long count;
  while(msec > 0 ) {  // repeat while there are still delay
    count = 16000;    // about 1ms
    while (count > 0) { 
     count--;
    } // This while loop takes approximately 3 cycles
    msec--;
  }
}
void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0       
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 input, PF3,PF2,PF1 output   
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTF_PUR_R = 0x11;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital pins PF4-PF0

// Color    LED(s) PortF
// dark     ---    0
// red      R--    0x02
// blue     --B    0x04
// green    -G-    0x08
// yellow   RG-    0x0A
// sky blue -GB    0x0C
// white    RGB    0x0E
// pink     R-B    0x06
}

// *************DAC_TEST******************** 
//Use this Routine to test the DAC in steps
// 		Input: 		Switch 1 (PF4)
// 		Outputs: 	LED (PORT_F)
// 				DAC = 0 GREEN
// 				Dac = 1->14 BLUE
// 				DAC = 15 RED
void DAC_test(void) {
	unsigned long i=0;
	unsigned long SWF;
	unsigned long Prev_SWF=1;
	while (i<16){
		if (i==0){
			GPIO_PORTF_DATA_R &= ~0x02;
			GPIO_PORTF_DATA_R |= 0x08;
			}
		else if (i<15){
			GPIO_PORTF_DATA_R &= ~0x08;
			GPIO_PORTF_DATA_R |= 0x04;
		}
		else {
			GPIO_PORTF_DATA_R &= ~0x04;
			GPIO_PORTF_DATA_R |= 0x02;
		}
		
			SWF = (GPIO_PORTF_DATA_R&0x10)>>4;
			if ((SWF==0) && (Prev_SWF==1)) {
				// Switch was pressed
				i++;
				if (i!=16){
				DAC_Out(i); // output
				}
			}
			Prev_SWF = SWF;
	}
}



