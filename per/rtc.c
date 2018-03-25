#include "rtc.h"
calendar_obj calendar; 
calendar_obj calendar_tmp; 
//平均的月份日期表
const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};

void RTC_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void RTC_Init(void)
{
    
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR|RCC_APB1Periph_BKP,ENABLE);
    PWR_BackupAccessCmd(ENABLE);
	
	if(BKP_ReadBackupRegister(BKP_DR1)!=0x1017)
    {
		BKP_DeInit();
		//RCC_LSEConfig(RCC_LSE_ON);
		//while(!RCC_GetFlagStatus(RCC_FLAG_LSERDY));
		

		//RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div128);
		RCC_RTCCLKCmd(ENABLE);
		RTC_WaitForSynchro();             
		RTC_WaitForLastTask();
		//RTC_SetPrescaler(32768-1);
		RTC_SetPrescaler(62500-1); /* RTC period = RTCCLK/RTC_PR = (8MHz/128)/62500*/
		RTC_WaitForLastTask();
		RTC_Set(2017,11,11,11,11,11);
		BKP_WriteBackupRegister(BKP_DR1, 0x1017);
    }
    else
    {
        RTC_WaitForSynchro();             
		RTC_WaitForLastTask();
    }
    //RTC_NVIC_Configuration();
    RTC_Get();
}
u8 Is_Leap_Year(u16 pyear)
{
    if(pyear%4==0){//首先需能被4整除
        if(pyear%100==0){
            if(pyear%400==0){
				return 1;//如果以00结尾，还要能被400整除
            }else{
				return 0;
			}
        } else{
            return 1;
		}
    }else{
        return 0;
	}
}
/*
*设置时钟
*把输入的时钟转换为秒钟
*以1970年1月1日为基准
*1970~2099为合法年份
返回值：0，成功；其他，错误
*/
u8 RTC_Set(u16 year,u8 mon,u8 day,u8 hour,u8 min,u8 sec)
{
    u16 t;
    u32 secCount=0;
    if(year<1970||year>2099)
        return 1;
    for(t=1970;t<year;t++)    //把所有年份的秒钟相加
    {
        if(Is_Leap_Year(t))//闰年
            secCount+=31622400;//闰年的秒钟数
        else
            secCount+=31536000;    
    }
    mon-=1;//先减掉一个月计算秒数，因为本月还没有过完，不足一个月
    for(t=0;t<mon;t++)
    {
        secCount+=(u32)mon_table[t]*86400;//月份的秒数相加
        if(Is_Leap_Year(year)&&t==1)
            secCount+=86400;//闰年，2月份增加一天的秒钟数
    }
    
    secCount+=(u32)(day-1)*86400;//把前面天数的秒钟数相加
    secCount+=(u32)hour*3600;//小时秒钟数
    secCount+=(u32)min*60;//分钟秒钟数
    secCount+=sec;
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR    | RCC_APB1Periph_BKP,ENABLE);
//    PWR_BackupAccessCmd(ENABLE);
    RTC_SetCounter(secCount);//设置RTC计数器的值
    RTC_WaitForLastTask();    
    RTC_Get();
    return 0;
}

/*
得到当前的时间数
成功返回0，错误返回其他
*/
u8 RTC_Get(void)
{
	static u16 dayCount=0;
	u32 secCount=0;
	u32 tmp=0;
	u16 tmp1=0;

	secCount = RTC_GetCounter();
	tmp = secCount/86400;//得到天数
	if(dayCount!=tmp){//超过一天
		dayCount = tmp;
		tmp1=1970;//从1970年开始
		while(tmp>=365){
			if(Is_Leap_Year(tmp1)){//是闰年
				if(tmp>=366){    
					tmp-=366;
				}else{
					break;
				}
			}else{
				tmp-=365;
			}
			tmp1++;
		}
		calendar.year=tmp1;//????
		tmp1=0;
		while(tmp>=28){
			if(Is_Leap_Year(calendar.year)&&tmp1==1){
				if(tmp>=29){    
					tmp-=29;
				}else{
					break;
				}
			}else{
				if(tmp>=mon_table[tmp1])
					tmp-=mon_table[tmp1];
				else
					break;
			}
			tmp1++;
		}
		calendar.month=tmp1+1;//得到月份，+1是因为月份从1月开始，tmp1=0表示一月
		calendar.day=tmp+1;    //得到日期，tmp表示的是完整的天数，真实的日期应+1
	}
	tmp = secCount%86400;
	calendar.hour = tmp/3600;
	calendar.min = (tmp%3600)/60;
	calendar.sec = (tmp%3600)%60;
	return 0;
}
/*
RTC时钟中断，每秒触发一次
*/
void RTC_IRQHandler(void)
{         
    if (RTC_GetITStatus(RTC_IT_SEC) != RESET){                            
		RTC_Get();  
	}
    if(RTC_GetITStatus(RTC_IT_ALR)!= RESET){
        RTC_ClearITPendingBit(RTC_IT_ALR);       
	}                                                    
    RTC_ClearITPendingBit(RTC_IT_SEC|RTC_IT_OW);
    RTC_WaitForLastTask();                                                   
}
