#ifndef	_UART4_H
#define	_UART4_H

#include "stm32f10x.h"

#define	USE_SEND_DMA	1
#define	FLASH_START_ADDRESS		FLASH_BASE+1024*250

typedef struct{
	uint16_t saved;
	uint16_t  com_brate;		//0:4.8...1:9.6...2:19.2...3:38.4...4:57.6...5:115.2
	uint16_t  com_parity;	//0:no parity,1:even,2:odd
	uint16_t  com_stop;
}Uart4_Cfg;

extern void uart4_send(uint8_t* buf, uint16_t size );
extern void uart4_receive(void);
extern void uart4_init(void);
extern void uart4_send_dma_disable(void);
extern void uart4_para_init(void);

#endif
