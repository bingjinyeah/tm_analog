#include "stm32f10x.h"
#include "timer.h"
#include "led.h"
#include "adc.h"
#include "dac.h"
#include "uart4.h"
#include "rs485.h"
#include "flash.h"



extern uint16_t MB_HoldReg[];
extern uint16_t MB_InReg[];
extern uint16_t	ADC_Value[2];
uint16_t ao_out_value = 0x00;
extern Uart4_Cfg uart4_cfg;

uint8_t changed = 0;

int main(void) 
{    
	
	timer_init();
	led_init();
	adc_init();
	dac_init();
	rs485_init();
    while(1){
		MB_InReg[0] =  (uint16_t)(2500*ADC_Value[0]/4095.0);
		MB_InReg[1] =  (uint16_t)(2500*ADC_Value[1]/4095.0);
		//MB_InReg[0] =  (uint16_t)(3.05*ADC_Value[0]);
		//MB_InReg[1] =  (uint16_t)(3.05*ADC_Value[1]);
		if(ao_out_value != MB_HoldReg[0]){
			ao_out_value = MB_HoldReg[0];
			dac_set_vol(ao_out_value);
		}
		if((uart4_cfg.com_brate != MB_HoldReg[1])||(uart4_cfg.com_parity != MB_HoldReg[2])
			||(uart4_cfg.com_stop != MB_HoldReg[3])){
			uart4_cfg.com_brate = MB_HoldReg[1];
			uart4_cfg.com_parity = MB_HoldReg[2];
			uart4_cfg.com_stop = MB_HoldReg[3];
			uart4_para_init();
			changed = 1;	
		}
		if(changed){
			FLASH_WriteMoreData(FLASH_START_ADDRESS,(uint16_t *)&uart4_cfg,sizeof(uart4_cfg));
			changed = 0;
		}
	}
	    
}
