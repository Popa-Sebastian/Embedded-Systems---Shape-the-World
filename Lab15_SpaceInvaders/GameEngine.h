//Header file for Game Engine

// Global Variables
unsigned long Flag;       // 1 means valid Distance, 0 means Distance is empty
unsigned long Player_Position;   // player position on x-axis 0-84

// Declarations
void SysTick_Init(unsigned long);
void SysTick_Handler(void);
unsigned long Convert(unsigned long);
void GameEngine_Init(void);
