#include "stm32f10x.h"

#define FLASH_SIZE 256         

#if FLASH_SIZE<256
  #define SECTOR_SIZE           1024 
#else 
  #define SECTOR_SIZE           2048
#endif

void FLASH_WriteMoreData(uint32_t startAddress,uint16_t *writeData,uint16_t countToWrite)
{
	uint32_t	offsetAddress;
	uint32_t	sectorPosition;
	uint32_t	sectorStartAddress;
	uint16_t	dataIndex;
	
	if(startAddress<FLASH_BASE||((startAddress+countToWrite*2)>=(FLASH_BASE+1024*FLASH_SIZE))){
		return;
	}
	FLASH_Unlock(); 
	offsetAddress = startAddress - FLASH_BASE; 
	sectorPosition = offsetAddress / SECTOR_SIZE; 

	sectorStartAddress = sectorPosition*SECTOR_SIZE + FLASH_BASE; 

	FLASH_ErasePage(sectorStartAddress);

	for(dataIndex=0;dataIndex<countToWrite;dataIndex++){
		FLASH_ProgramHalfWord(startAddress+dataIndex*2,writeData[dataIndex]);
	}

	FLASH_Lock();
}

void FLASH_ReadMoreData(uint32_t startAddress,uint16_t *writeData,uint16_t countToWrite)
{
	uint16_t	dataIndex;
	
	if(startAddress<FLASH_BASE||((startAddress+countToWrite*2)>=(FLASH_BASE+1024*FLASH_SIZE))){
		return;
	}
	//FLASH_Unlock(); 

	for(dataIndex=0;dataIndex<countToWrite;dataIndex++){
		//FLASH_ProgramHalfWord(startAddress+dataIndex*2,writeData[dataIndex]);
		writeData[dataIndex] =*((__IO uint16_t*)(startAddress+dataIndex*2));
	}

	//FLASH_Lock();
}
