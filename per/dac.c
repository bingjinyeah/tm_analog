#include "stm32f10x.h"
#include "dac.h"

void dac_init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	DAC_InitTypeDef	  DAC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE ); 
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//DAC�����������֮��������ΪGPIO_Mode_AIN;��Ϊʹ��DAC��
	//gpio���Զ���DAC����������Ϊ������Ա������ĸ��š�
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 	
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_4); 
	
  	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None; 
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;  
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable ; 
    DAC_Init(DAC_Channel_1,&DAC_InitStructure);
      
    DAC_Cmd(DAC_Channel_2,ENABLE);
    DAC_SetChannel2Data(DAC_Align_12b_R,0);
}
/*
    DAC�����ѹ0~2.5V,��Ӧ��AOͨ�����0~25mA��
    DAC�ο���ѹ2.5V���ú������������λΪmV
*/
void dac_set_vol(uint16_t vol)
{
	float temp = vol;  
	
    temp /= 1000;  
    temp = temp*4095/2.5;  
    DAC_SetChannel2Data(DAC_Align_12b_R,(uint16_t)temp);
}
