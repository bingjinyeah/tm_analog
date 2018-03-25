#include "stm32f10x.h"

void delayms(uint16_t num)
{
    uint16_t c=2000;
    //clr_wdt();
    while(num--){
        while(c--){}
        c=2000;
    }
}

void delayus(uint16_t num)
{  
    uint32_t tmp = num*2;
	while(tmp--);
}

void delays(uint16_t num)
{
    num*=5;
    while(num--){
        delayms(200);
    }
}
