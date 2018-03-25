#ifndef LED_H
#define LED_H

#include "pincfg.h"

#define  LED_RUN_ON		LED_RUN = 0;
#define  LED_LINK_ON	LED_LINK = 0;

#define  LED_RUN_OFF	LED_RUN = 1;
#define  LED_LINK_OFF	LED_LINK = 1;

#define  LED_LINK_BLINK (LED_LINK = !LED_LINK);

extern void led_init(void);

#endif
