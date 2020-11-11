

#include "GameEngine.h"
#include "ADC.h"
#include "..//tm4c123gh6pm.h"

unsigned long ADCdata;    // 12-bit 0 to 4095 sample


// Initialize SysTick interrupts to trigger at 30 Hz, 33 ms
// period = 2666666
void SysTick_Init(unsigned long period){
	NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = period-1;// reload value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000;
	// priority 1
  NVIC_ST_CTRL_R = 0x0007; // enable,core clock, and interrupts

}
// executes every 33 ms, collects a sample, converts and stores in mailbox
void SysTick_Handler(void){ 
	unsigned long sample;
	GPIO_PORTF_DATA_R ^= 0x02;
	GPIO_PORTF_DATA_R ^= 0x02;
	sample = ADC0_In();
	Player_Position = Convert(sample);
	Flag = 1; // new data is ready
	GPIO_PORTF_DATA_R ^= 0x02;
}

void GameEngine_Init(void){
	ADC0_Init();
	SysTick_Init(2666666);
	
}
unsigned long Convert(unsigned long sample){ 
	unsigned long result;
	//unsigned long A=500;
	//unsigned long A=367;
	//unsigned long A=470;
	//unsigned long B=150;
	unsigned long A= 16;
	unsigned long B = 1;
	if (sample < 20) {
		return 0;
	} else if (sample > 4075) {
		return 65;
	} else {
		result = ((A*sample)>>10)+B;
		return result;
	}
}
