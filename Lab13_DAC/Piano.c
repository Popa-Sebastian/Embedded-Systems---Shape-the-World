// Piano.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// There are four keys in the piano
// Daniel Valvano
// December 29, 2014

// Port E bits 3-0 have 4 piano keys

#include "Piano.h"
#include "..//tm4c123gh6pm.h"

unsigned long Prev_SW;


// **************Piano_Init*********************
// Initialize piano key inputs
// Input: none
// Output: none
void Piano_Init(void){unsigned long volatile delay; 
	SYSCTL_RCGC2_R |= 0x10; // Port E clock
	delay = SYSCTL_RCGC2_R; // allow time for clock to start
	GPIO_PORTE_DIR_R &= ~ 0x00F; // PE3-PE0 = INPUT
	GPIO_PORTE_AFSEL_R &= ~ 0x0F; // Not alternative PE3-PE0
	GPIO_PORTE_AMSEL_R &= ~ 0x0F; // No analog PE3-PE0
	GPIO_PORTE_PCTL_R &= ~ 0x00FFFFFF; //Regulat GPIO PE3-PE0
	GPIO_PORTE_DEN_R |= 0x0F; // digital Digital PE3-PE0

}
// **************Piano_In*********************
// Input from piano key inputs
// Input: none 
// Output: 0 to 15 depending on keys
// 0x01 is key 0 pressed, 0x02 is key 1 pressed,
// 0x04 is key 2 pressed, 0x08 is key 3 pressed
unsigned long Piano_In(void){
	unsigned long SW;
	//read all sw positions
	SW = GPIO_PORTE_DATA_R & 0x0F;
	if (SW == 0) {
		Prev_SW = SW;
		return 0; // no switch pressed
	}
	if ((SW&0x01)&& !(Prev_SW&0x01)){
		Prev_SW = SW;
		return 1;	// switch 1 pressed
	}
	if ((SW&0x02)&& !(Prev_SW&0x02)){
		Prev_SW = SW;
		return 2; // switch 2 pressed
	}
	if ((SW&0x04)&& !(Prev_SW&0x04)){
		Prev_SW = SW;
		return 3; // switch 3 pressed
	}
	if ((SW&0x08)&& !(Prev_SW&0x08)){
		Prev_SW = SW;
		return 4; // switch 5 pressed
	}
	Prev_SW = SW;
	return 5; // continue
}
