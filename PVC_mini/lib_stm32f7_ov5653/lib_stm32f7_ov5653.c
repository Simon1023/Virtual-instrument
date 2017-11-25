#include <lib_stm32f7_ov5653.h>
#include "DCMI_OV5653_INITTABLE.h"
#include <stdlib.h>
#include "stm32f7xx_hal.h"

extern I2C_HandleTypeDef hi2c3;
uint32_t DMA_FRAME_BUFFER = (640*480/4);

//#pragma arm section rwdata = "SDRAM1", zidata = "SDRAM1"
//unsigned char OrgImgBuf[480][640];	// cap_rows = 240 , cap_cols = 320
//unsigned char OrgImgBuf[307200];		
//#pragma arm section rwdata
unsigned char OrgImgBuf[307200];

void OV5653_Reset(void) 
{
	uint8_t sccb_wrdata[] = {0x30, 0x08, 0x80};		
	HAL_I2C_Master_Transmit(&hi2c3, 0x6C, sccb_wrdata, 3, 100);	 
	HAL_Delay(20);
}

void OV5653_InitConfig(void)
{
	int i;
	HAL_Delay(20);
	uint8_t * reg_index = &init_reg[0][0];	
	for(i = 0; i < INIT_REG_NUM; i++)
	{ 
		HAL_I2C_Master_Transmit(&hi2c3, 0x6C, reg_index+(i*3), 3, 100);	 
		HAL_Delay(1);
	}	
	
//	/*TEST START*/
//	uint8_t AAA[INIT_REG_NUM];
//	uint8_t *BBB = AAA;
//	for(i = 0; i < INIT_REG_NUM; i++)
//	{ 
//		HAL_I2C_Master_Transmit(&hi2c3, 0x6C, reg_index+(i*3), 2, 100);		
//		HAL_I2C_Master_Receive(&hi2c3, 0x6C, BBB+i, 1, 100);	
//		HAL_Delay(1);
//	}			
//	/*TEST END*/
}
