#ifndef	_RS485_H
#define	_RS485_H

#include "stm32f10x.h"

void rs485_init(void);
void rs485_send(uint8_t* buf,uint16_t size );
void rs485_tx_complete(void);
void modbus_pdu_process(uint8_t *pdata,uint16_t length);

#endif
