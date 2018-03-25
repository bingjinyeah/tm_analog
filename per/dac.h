#ifndef DAC_H
#define DAC_H
#include "stm32f10x.h"

extern void dac_init(void);
extern void dac_set_vol(uint16_t data);
#endif
