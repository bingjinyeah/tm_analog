/* 
 * File:   timer.h
 * Author: Administrator
 *
 * Created on 2016?3?10?, ??2:45
 */

#ifndef TIMER_H
#define	TIMER_H
#define HZ_RUN		200
#define HZ_TIMER	500000
#define HZ_NOPOWER	1



extern void SysTickInit (uint32_t  cnts);
extern void SysTickDisable(void);
extern void timer_init(void);
extern void timer2_on(void);
extern void timer2_off(void);

#endif	/* TIMER_H */

