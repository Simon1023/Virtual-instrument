#ifndef __is42s16160g_6bl_H
#define __is42s16160g_6bl_H
#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "stm32f7xx_hal.h"

extern void IS42S16160G_6BL_Init_Sequence(SDRAM_HandleTypeDef *hsdram);


#define SDRAM_BANK_ADDR ((uint32_t)0xC0000000)













#ifdef __cplusplus
}
#endif
#endif
