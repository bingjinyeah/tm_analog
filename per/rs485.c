#include "stm32f10x.h"
#include "uart4.h"
#include <string.h>
#include "modbus_driver.h"
#include "pincfg.h"
#include "crc.h"
#include "rs485.h"

MB_FRAME mb_rcv_frame;
uint8_t  rs485_send_buff[MB_BUFFER_SIZE];
uint8_t  mb_local_address;

void rs485_tx_enable()
{
	RS485_DE = 1;
	RS485_nRE = 1;
}

void rs485_rx_enable()
{
	RS485_DE = 0;
	RS485_nRE = 0;
}

void get_local_address()
{
	uint8_t addr = 000;
	
	addr |= GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_9);
	addr <<= 1;
	addr |= GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_0);
	addr <<= 1;
	addr |= GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_1);
	addr <<= 1;
	addr |= GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_2);
	addr <<= 1;
	addr |= GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_3);
	addr <<= 1;
	addr |= GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_4);
	addr <<= 1;
	addr |= GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_5);
	addr <<= 1;
	addr |= GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_6);
	mb_local_address = addr;
}

void rs485_init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);
	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);  
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 |GPIO_Pin_2 | GPIO_Pin_3
								| GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
  
	
	get_local_address();
	rs485_rx_enable();
	uart4_init();
}

void rs485_send(uint8_t* buf,uint16_t size )
{
	if(buf==NULL)
		return;
	memcpy(rs485_send_buff,buf,size);
	rs485_tx_enable();
	uart4_send(rs485_send_buff,size);
}

void rs485_tx_complete()
{
	uart4_send_dma_disable();
	rs485_rx_enable();
}

void modbus_pdu_process(uint8_t *pdata,uint16_t length)
{
	uint8_t   index;		     
	uint8_t   i;
	uint16_t  buffer_length;
	uint16_t  crc_result;
	if ((length <= 3) || (length > MB_BUFFER_SIZE / 2)) {
		return;
	}
	buffer_length = length;
	index = 0;
	//mb_rcv_frame.ComPort = UART_COM4;
	mb_rcv_frame.Address = pdata[index++];
	mb_rcv_frame.Function = pdata[index++];
	mb_rcv_frame.Length = buffer_length - 4;
			
	for (i = 0; i < mb_rcv_frame.Length; i++)
	{
		mb_rcv_frame.Data[i] = pdata[index++];
	}  
	*((uint8_t*)&mb_rcv_frame.CRCResult+1) = pdata[index++];
	*((uint8_t*)&mb_rcv_frame.CRCResult) = pdata[index++];
	crc_result = CRC16(pdata, buffer_length - 2);
	index = 0;
	if (crc_result != mb_rcv_frame.CRCResult){
		return;
	}
	if ((mb_rcv_frame.Address != mb_local_address) 
		&&(mb_rcv_frame.Address != MB_BROADCAST_ADDRESS)) {
		return;
	}
    
	switch(mb_rcv_frame.Function)
	{	
		//case 1: MB_DRIVER_ReadCoils(&mb_rcv_frame); break;
		case 3: 
			MB_DRIVER_ReadHoldReg(&mb_rcv_frame);
			break;	
		case 4: 
			MB_DRIVER_ReadInReg(&mb_rcv_frame);
			break;
		case 6: 
			MB_DRIVER_WriteSingleHoldReg(&mb_rcv_frame); 
			break;
			//case 16: MB_DRIVER_WriteMoreReg(&mb_rcv_frame); break;     //16:??????
		default:
			break;
	}
}
