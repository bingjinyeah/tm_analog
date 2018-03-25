#include "rtc.h"
calendar_obj calendar; 
calendar_obj calendar_tmp; 
//ƽ�����·����ڱ�
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
    if(pyear%4==0){//�������ܱ�4����
        if(pyear%100==0){
            if(pyear%400==0){
				return 1;//�����00��β����Ҫ�ܱ�400����
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
*����ʱ��
*�������ʱ��ת��Ϊ����
*��1970��1��1��Ϊ��׼
*1970~2099Ϊ�Ϸ����
����ֵ��0���ɹ�������������
*/
u8 RTC_Set(u16 year,u8 mon,u8 day,u8 hour,u8 min,u8 sec)
{
    u16 t;
    u32 secCount=0;
    if(year<1970||year>2099)
        return 1;
    for(t=1970;t<year;t++)    //��������ݵ��������
    {
        if(Is_Leap_Year(t))//����
            secCount+=31622400;//�����������
        else
            secCount+=31536000;    
    }
    mon-=1;//�ȼ���һ���¼�����������Ϊ���»�û�й��꣬����һ����
    for(t=0;t<mon;t++)
    {
        secCount+=(u32)mon_table[t]*86400;//�·ݵ��������
        if(Is_Leap_Year(year)&&t==1)
            secCount+=86400;//���꣬2�·�����һ���������
    }
    
    secCount+=(u32)(day-1)*86400;//��ǰ�����������������
    secCount+=(u32)hour*3600;//Сʱ������
    secCount+=(u32)min*60;//����������
    secCount+=sec;
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR    | RCC_APB1Periph_BKP,ENABLE);
//    PWR_BackupAccessCmd(ENABLE);
    RTC_SetCounter(secCount);//����RTC��������ֵ
    RTC_WaitForLastTask();    
    RTC_Get();
    return 0;
}

/*
�õ���ǰ��ʱ����
�ɹ�����0�����󷵻�����
*/
u8 RTC_Get(void)
{
	static u16 dayCount=0;
	u32 secCount=0;
	u32 tmp=0;
	u16 tmp1=0;

	secCount = RTC_GetCounter();
	tmp = secCount/86400;//�õ�����
	if(dayCount!=tmp){//����һ��
		dayCount = tmp;
		tmp1=1970;//��1970�꿪ʼ
		while(tmp>=365){
			if(Is_Leap_Year(tmp1)){//������
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
		calendar.month=tmp1+1;//�õ��·ݣ�+1����Ϊ�·ݴ�1�¿�ʼ��tmp1=0��ʾһ��
		calendar.day=tmp+1;    //�õ����ڣ�tmp��ʾ������������������ʵ������Ӧ+1
	}
	tmp = secCount%86400;
	calendar.hour = tmp/3600;
	calendar.min = (tmp%3600)/60;
	calendar.sec = (tmp%3600)%60;
	return 0;
}
/*
RTCʱ���жϣ�ÿ�봥��һ��
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
