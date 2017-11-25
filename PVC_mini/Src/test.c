#include "stm32f7xx_hal.h"
#include "is42s16160g-6bl.h"

void LEDs_TEST(void);
void USB_Test(void);
void SDRAM_Test(void);

extern SDRAM_HandleTypeDef hsdram1;
uint8_t testValue __attribute__((at(SDRAM_BANK_ADDR)));

void Board_Test(){
	uint8_t i;
//	/* TEST-1 : LEDs Test */
//	for(i=0; i<6; i++){
//		LEDs_TEST();
//		HAL_Delay(500);
//  }

//	/* TEST-2 : USB Test (VCP) */
//	USB_Test();

//	/* TEST-3 : SDRAM Test */
//	SDRAM_Test();
}

/* LEDs Test */
void LEDs_TEST(){
	HAL_GPIO_TogglePin(MB_LED0_GPIO_Port, MB_LED0_Pin);	
	//HAL_GPIO_TogglePin(MB_LED1_GPIO_Port, MB_LED1_Pin);	// does't work cause of layout issue.
	HAL_GPIO_TogglePin(MB_LED2_GPIO_Port, MB_LED2_Pin);	
	//HAL_GPIO_TogglePin(MB_LED3_GPIO_Port, MB_LED3_Pin);	// does't work cause of layout issue.
	HAL_GPIO_TogglePin(MB_LED4_GPIO_Port, MB_LED4_Pin);	
	//HAL_GPIO_TogglePin(MB_LED5_GPIO_Port, MB_LED5_Pin);	// does't work cause of layout issue.			
}

/* USB Test (VCP) */
void USB_Test(){
	uint8_t TestBuf[] = {'T', 'E', 'S', 'T'};		
	CDC_Transmit_HS(TestBuf, 4);	
}

/* SDRAM Test */
void SDRAM_Test(){

	
	

	testValue = 0xA5;
	
	
	
}

