
#include "Switches.h"
#include "..//tm4c123gh6pm.h"
unsigned long Prev_SW;

// **************Switch_Init*********************
// Initialize key inputs
// Input: none
// Output: none
void Switch_Init(void){unsigned long volatile delay; 
	SYSCTL_RCGC2_R |= 0x10; // Port E clock
	delay = SYSCTL_RCGC2_R; // allow time for clock to start
	GPIO_PORTE_DIR_R &= ~ 0x03; // PE1-PE0 = INPUT
	GPIO_PORTE_AFSEL_R &= ~ 0x03; // Not alternative PE1-PE0
	GPIO_PORTE_AMSEL_R &= ~ 0x03; // No analog PE1-PE0
	GPIO_PORTE_PCTL_R &= ~ 0x03; //Regular GPIO PE1-PE0
	GPIO_PORTE_DEN_R |= 0x0F; // digital Digital PE1-PE0

}
// **************Switch_In*********************
// Input from key inputs
// Input: none 
// Output: 
// 0 - no key pressed
// 1 - switch 1 pressed
// 2 - switch 2 pressed
// 5 - switch held

unsigned long Switch_In(void){
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
	Prev_SW = SW;
	return 5; // continue
}
