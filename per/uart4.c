#include "stm32f10x.h"
#include "uart4.h"
#include "led.h"
#include "rs485.h"
#include "flash.h"

#define UART4_RCV_SIZE		20
#define UART4_DR_Base		((uint32_t)UART4_BASE + 0x04)
#define UART4_BRATE			9600


Uart4_Cfg uart4_cfg;

extern uint16_t MB_HoldReg[];
extern uint8_t rs485_send_buff[];

uint8_t uart4_rcv_buff[UART4_RCV_SIZE];

void uart4_rcv_dma_init()
{
    DMA_InitTypeDef DMA_InitStructure;

	DMA_DeInit(DMA2_Channel3);
	DMA_InitStructure.DMA_PeripheralBaseAddr = UART4_DR_Base;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uart4_rcv_buff;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = UART4_RCV_SIZE;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA2_Channel3, &DMA_InitStructure);

	DMA_Cmd(DMA2_Channel3, ENABLE);   
}

void uart4_send_dma_init(uint8_t length)
{
    DMA_InitTypeDef DMA_InitStructure;

	DMA_DeInit(DMA2_Channel5);
	DMA_InitStructure.DMA_PeripheralBaseAddr = UART4_DR_Base;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)rs485_send_buff;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = length;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA2_Channel5, &DMA_InitStructure);

	DMA_Cmd(DMA2_Channel5, ENABLE);   
}

void uart4_send_dma_disable()
{
	DMA_Cmd(DMA2_Channel5, DISABLE);   
}

void uart4_para_init()
{
	USART_InitTypeDef USART_InitStructure;
	uint32_t bt;
	uint16_t pt,st,wl;
	
	switch(uart4_cfg.com_brate){
		case 0:
			bt = 4800;
			break;
		case 1: 
			bt = 9600;
			break;
		case 2: 
			bt = 19200;
			break;
		case 3:
			bt = 38400;
			break;
		case 4: 
			bt = 57600;
			break;
		case 5: 
			bt = 115200;
			break;
		default: 
			bt = 9600;
			break;
			
	}
	switch(uart4_cfg.com_parity){
		case 1: 
			pt = USART_Parity_Even;
			wl = USART_WordLength_9b;
			break;
		case 2: 
			pt = USART_Parity_Odd;
			wl = USART_WordLength_9b;
			break;
		default: 
			pt = USART_Parity_No;
			wl = USART_WordLength_8b;
			break;
	}
	switch(uart4_cfg.com_stop){
		case 1: 
			st = USART_StopBits_1;
			break;
		case 2: 
			st = USART_StopBits_2;
			break;
		default: 
			st = USART_StopBits_1;
			break;
	}
	
	USART_InitStructure.USART_BaudRate = bt;
	USART_InitStructure.USART_WordLength = wl;
	USART_InitStructure.USART_StopBits = st;
	USART_InitStructure.USART_Parity = pt;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	USART_Init(UART4, &USART_InitStructure);
}

void uart4_port_init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2,ENABLE); 
	
	/* Configure UART4 Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);  
	
	/* Configure UART4 Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	uart4_para_init();
	
	USART_GetITStatus(UART4, USART_IT_IDLE);
	USART_ReceiveData(UART4);
	uart4_rcv_dma_init();
	
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	/* Enable the Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel4_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//idle interrupt enable
	USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);
	DMA_ClearFlag(DMA2_FLAG_TE5);
	DMA_ClearFlag(DMA2_FLAG_TC5);
	DMA_ITConfig(DMA2_Channel5,DMA_IT_TC|DMA_IT_TE,ENABLE);
	
	USART_DMACmd(UART4, USART_DMAReq_Rx, ENABLE); 
#ifdef	USE_SEND_DMA
	USART_DMACmd(UART4, USART_DMAReq_Tx, ENABLE); 
#endif
	USART_Cmd(UART4, ENABLE);
}



void uart4_init()
{
	
	FLASH_ReadMoreData(FLASH_START_ADDRESS,(uint16_t *)&uart4_cfg,sizeof(uart4_cfg));
	if(uart4_cfg.saved == 0x5a){
		MB_HoldReg[1] = uart4_cfg.com_brate;
		MB_HoldReg[2] = uart4_cfg.com_parity;
		MB_HoldReg[3] = uart4_cfg.com_stop;
	}else{
		uart4_cfg.saved = 0x5a;
		uart4_cfg.com_brate = 1;
		uart4_cfg.com_parity = 0;
		uart4_cfg.com_stop = 1;
		FLASH_WriteMoreData(FLASH_START_ADDRESS,(uint16_t *)&uart4_cfg,sizeof(uart4_cfg));
	}
	uart4_port_init();
}
//uint8_t teststr[] = {0x31,0x32,0x33,0x34,0x35};
void uart4_pdu_process(uint8_t *buff,uint16_t length)
{
    //uart4_send(buff,length);
	LED_LINK_BLINK;
	modbus_pdu_process(buff,length);    
}

void uart4_receive()
{
    uint16_t final_byte; 

	//clear idle it pending bit
	USART_ReceiveData(UART4);

	final_byte = (UART4_RCV_SIZE - DMA_GetCurrDataCounter(DMA2_Channel3));
	

	// disauart4 and re-configure the DMA to receive bytes 
    //DMA_DeInit(DMA2_Channel3);

	uart4_pdu_process(uart4_rcv_buff,final_byte);

  	uart4_rcv_dma_init();
}

void uart4_send(uint8_t* buf,uint16_t size )
{
#ifdef	USE_SEND_DMA
	uart4_send_dma_init(size);
#else	
   	uint16_t i;
	 
    for(i=0; i<size; i++)   
    {   
		while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);   		
		
		USART_SendData(UART4, *buf++); 	
	}  
	while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET); 
	rs485_rx_enable();
#endif
}

