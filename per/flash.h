#ifndef	FLASH_H
#define	FLASH_H
#include "stm32f10x.h"

extern void FLASH_WriteMoreData(uint32_t startAddress,uint16_t *writeData,uint16_t countToWrite);
extern void FLASH_ReadMoreData(uint32_t startAddress,uint16_t *writeData,uint16_t countToWrite);
#endif
