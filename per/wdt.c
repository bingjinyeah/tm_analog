#include "wdt.h"

void enable_wdt(){
    //_SWDTEN = 1;
}

void disable_wdt(){
    //_SWDTEN = 0;
}

void clr_wdt(){
    //ClrWdt();
    //CLR_WDT_Tris = 0;
    //Nop();
    //CLR_WDT_Write = 1;
    //delayus(10);
    //CLR_WDT_Write = 0;
}

