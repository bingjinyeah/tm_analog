#ifndef MODBUS_DRIVER_H
#define MODBUS_DRIVER_H

#include "stm32f10x.h"

#define _ModServer

#define true				0xff
#define false				0x00
	
#define MB_BROADCAST_ADDRESS  0x00     //广播地址 
#define MB_FRAME_SIZE         65       
#define MB_BUFFER_SIZE	 (MB_FRAME_SIZE+5)*2  //ASCII码时，数据长度*2
#define MB_MAX_REPEAT  5     //最大重发次数

#define MB_COILS_AMOUNT           8                //线圈数量
#define MB_DI_AMOUNT              8                //离散量输入数量
#define MB_IR_AMOUNT              2					//输入寄存器
#define MB_HR_AMOUNT              16  			//保持寄存器数量

   

typedef struct{
	uint8_t	ComPort;             //不发送  
	uint8_t	Address;
	uint8_t	Function;
	uint8_t	Data[MB_FRAME_SIZE];
	uint16_t	Length;               //不发送    data区字节长度
	uint16_t	CRCResult;
}MB_FRAME;

extern uint8_t MB_DRIVER_GetCoils(uint16_t address);
extern uint8_t MB_DRIVER_GetDisIn(uint16_t address);
extern void MB_DRIVER_SetCoils(uint16_t address,uint16_t data);

extern void MB_DRIVER_ReadCoils(MB_FRAME *pframe);
extern void MB_DRIVER_ReadDisIn(MB_FRAME *pframe);
extern void MB_DRIVER_ReadHoldReg(MB_FRAME *pframe);
extern void MB_DRIVER_ReadInReg(MB_FRAME *pframe);
extern void MB_DRIVER_WriteSingleCoil(MB_FRAME *pframe);
extern void MB_DRIVER_WriteSingleHoldReg(MB_FRAME *pframe);
extern void MB_DRIVER_ReadExStatus(MB_FRAME *pframe);
extern void MB_DRIVER_WriteMoreCoils(MB_FRAME *pframe);
extern void MB_DRIVER_WriteMoreReg(MB_FRAME *pframe);
extern void MB_DRIVER_ReadSlaveID(MB_FRAME *pframe);
extern void MB_DRIVER_ReadDeviceID(MB_FRAME *pframe);
extern void MB_DRIVER_BackData(MB_FRAME *pframe);
extern void MB_DRIVER_ReadDiagnose(MB_FRAME *pframe);
extern void MB_DRIVER_SendData(MB_FRAME* pframe);


#endif 
 
 
 
 
 
 
 
 
