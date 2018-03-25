#include "code.h"
#include "para.h"
#include "lcd.h"
#include "relay.h"
#include "eepdata.h"
#include "flag.h"
#include "pincfg.h"
#include "dac.h"

Uint32	vp;
Uint16	encoder_cal_value[4];
Uint16	temp[4];// ;	temp[3][4]
float	data[4];
Uint16	encoder_pre_data[4];

void encoder_init()
{
    GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef   SPI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	/*
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_5|GPIO_Pin_7; 	
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; 	
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
	*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; 	
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7; 	
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; 	
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
	SPI_Cmd(SPI1,DISABLE);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;	
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1,&SPI_InitStructure);
	SPI_Cmd(SPI1, ENABLE);
	 
	EN_DA_SCK = 1;
	delayus(1);
	EN_CS = 1;
	//DA_CS = 0;     //DA 的信号反向
	//encoder_cal();
}
/*
Uint8 fb_fw_read(){
    
    return true;
}

Uint8 fb_tq_read(){

    return false;
}
*/
void tkencoder_read_data(Uint8 *data)
{
    
	EN_CS = 0;
	delayus(100);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1,0);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	data[0] = SPI_I2S_ReceiveData(SPI1);
	delayus(100);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1,0);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	data[1] = SPI_I2S_ReceiveData(SPI1);
	delayus(100);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1,0);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	data[2] = SPI_I2S_ReceiveData(SPI1);
	delayus(100);
	//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	//SPI_I2S_SendData(SPI1,0x1234);
	//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	//data[3] = SPI_I2S_ReceiveData(SPI1);
	EN_CS = 1;
	delayus(100);
	
	
	
}

Uint8 en_read_data(){ 
	

	Uint8	data[4];
	Uint8	tmp;
	
	
	tkencoder_read_data(data);
	
	tmp = data[0]^data[1]^data[2];//^data[3];
	if((tmp)||((data[0]==0)&&(data[1]==0))){
		return false;
	}
	vp = (data[0]<<8) | data[1];
	return true;
	     
}

Uint32 encoder_get_data()
{
    Int32 wrong_times = 3;
	
	while(wrong_times>0){
		if(en_read_data()==true){
		    return vp;
		}else{
			wrong_times--;		
		}
	}
	return _CodeVP;
}

void get_code_vp(){
    
	Uint32 res,res1;	

	encoder_init();
	delayus(100);
	res = encoder_get_data();
	res1 = circle_to_line(res);
	/*
	if((res < _CodeVP-1)||(res > _CodeVP+1)){
		_CodeVP = res;
		_L_CodeVP = circle_to_line(_CodeVP);		
	}*/
	if((res1 < _L_CodeVP-1)||(res1 > _L_CodeVP+1)){
		_CodeVP = res;
		_L_CodeVP = res1;		
	}
#if(FB_POSITION==1)
	dac_init();
	//delayus(100);
	//dac_set_data(_NJDA|0x9000);
	delayus(100);
	dac_set_data((_VPDA&0xfff)|0x1000);
	//dac_set_data(0x1555);
	#if(FB_TORQUE==1)
		delayus(100);
		dac_set_data(_NJDA|0x9000);
	#endif
#elif(FB_TORQUE==1)
	dac_init();	
	delayus(100);
	dac_set_data(_NJDA|0x9000);
#endif
	
		
}

Uint32 circle_to_line(Uint32 vp){
    Uint32 cld,bs,lz;
    
    eedata_read(_CL_Dir,cld);
    eedata_read(_Clock_Decrease ,bs);
    eedata_read(_L_Zero,lz);
    if(((cld==false)&&(bs==false))||
       ((cld!=false)&&(bs!=false))){
            return (vp - lz);
    }else{
        return (lz - vp);
    }  

}

Uint32 alu_dbd(){
    Uint32 id,ll;
    
    eedata_read(_IC_Dbd,id);
    eedata_read(_Limit_Length,ll);
    return ((Uint32)id*ll/1000 + 1);
}
void alu_ic_code(){
    Uint32 dl,dh;
    Uint32 lvp,hvp;
    Uint32 cll,opl;
    float res;
    
    _Back_Flag = false;
    eedata_read(_IC_Low_VP,dl);
    eedata_read(_IC_High_VP,dh);
    if(dh<dl){
        hvp = dl;
        lvp = dh;
    }else{
        hvp = dh;
        lvp = dl;
    }
    eedata_read(_L_CL_Limit,cll);
    eedata_read(_L_OP_Limit,opl);
    eedata_read(_IC_Low,dl);
    eedata_read(_IC_High,dh);
    if(_IC[10]>=dh){
        res = hvp;
    }else if(_IC[10]>=dl){
        res = 1.0*(hvp-lvp)*(_IC[10]-dl)/(dh-dl)+lvp;
    }else if(_IC[10]>=dl/2){
        res = lvp;
    }else{
        lcd_dis_clr_alarm();
        lcd_dis_alarm_salos();
        _strAlarmFlag |= _SignLostedFlag;
        monitor_release_dummy();
        eedata_read(_POSALS,dl);
        if(dl==0x03){
            _IC_Code = cll;
        }else if(dl==0x01){
            _IC_Code = opl;
        }else if(dl==0x07){
            eedata_read(_LOSPOS,dl);
            _IC_Code = (Uint32)(opl-cll)*dl/100+cll;
        }else{
            _Back_Flag = true;
        }
        return;
    }
    hvp = (Uint32)(res*(opl-cll)/100.0) + cll;
    _strAlarmFlag &= ~_SignLostedFlag;
    _IC_Code = (Uint32)hvp;
    
}

void alu_dp_code(){
    Uint32 min,max;
    
    eedata_read(_POSMIN_Code,min);
    eedata_read(_POSMAX_Code,max);
    if(max>min){
        _DP_Code = (Uint32)_DP_ACTPOS*(max-min)/0xff+min;
    }else{
        _DP_Code = min - (Uint32)_DP_ACTPOS*(min-max)/0xff;
    }
}


Uint32 alu_dis_ic(){
    Uint32 low,high,res;
    
    eedata_read(_IC_Low,low);
    eedata_read(_IC_High,high);
    if(_IC[10]>=low){
        res = 160*((Uint32)(_IC[10]-low))/(high-low) + 40;
    }else{
        res = 40 - 160*((Uint32)(low-_IC[10]))/(high-low);
    }
    return res;
}

Uint32 alu_dis_position_back(){
    
    Uint32 low,high,res;
    
    eedata_read(_Pos_BackL,low);
    eedata_read(_Pos_BackH,high);
    if(_Menu330Count>=low){
        res = 1600*((Uint32)(_Menu330Count-low))/(high-low) + 400;
    }else{
        res = 400 - 1600*((Uint32)(low-_Menu330Count))/(high-low);
    }
    return res;
}

Uint32 alu_dis_tor_back(){
    
    Uint32 low,high,res;
    
    eedata_read(_Tor_BackL,low);
    eedata_read(_Tor_BackH,high);
    if(_Menu331Count>=low){
        res = 1600*((Uint32)(_Menu331Count-low))/(high-low) + 400;
    }else{
        res = 400 - 1600*((Uint32)(low-_Menu331Count))/(high-low);
    }
    return res;
}

void cal_zero(Uint32 cll, Uint32 opl){
    
    Uint32 cld,bs,res;

    eedata_read(_CL_Dir,cld);
    eedata_read(_Clock_Decrease ,bs);

    if(((cld==false)&&(bs==false))||
       ((cld!=false)&&(bs!=false))){  
        //对应于，编码器顺时针减小，关闭方向为顺时针，或者相反。
		res = cll - opl;
        res>>=1;
        if((opl>=cll)&&(res<=cll)){
            
            res = cll - res;
        }else{
            res += opl;
        }      
    }else{
        res = opl - cll;
        res>>=1;
        if((cll>=opl)&&(res<=opl)){
                res = opl - res;
        }else{
            res += cll;
        }
    } 
    eedata_dwrite(_L_Zero,res);
}

Uint8 cal_limit(Uint32 cll, Uint32 opl, Uint32 lz){
    Uint32  mul;
    Uint32  sub,res;
    
    
    cal_zero(cll,opl);
    opl = circle_to_line(opl);
    cll = circle_to_line(cll);
    sub = opl-cll;
    if(sub>(430650-512)){
        eedata_dwrite(_L_Zero,lz);
        lcd_dis_clr_alarm();
        lcd_dis_alarm_lengtherror();
        delayms(400);
        lcd_dis_clr_alarm();
        delayms(200);
        lcd_dis_alarm_lengtherror();
        delayms(400);
        lcd_dis_clr_alarm();
        return E_ERR;
    }else{
        eedata_dwrite(_Limit_Length,sub);
        eedata_dwrite(_L_OP_Limit,opl);
        eedata_dwrite(_L_CL_Limit,cll);
        eedata_read(_POSMIN,res);
        mul = (Uint32)res * sub;
        res = mul/100;
        res += cll;
        eedata_dwrite(_POSMIN_Code,res);
        eedata_read(_POSMAX,res);
        mul = (Uint32)res * sub;
        res = mul/100;
        res += cll;
        eedata_dwrite(_POSMAX_Code,res);
        
    }
    return E_OK;
}

void cal_limit_length(){
    Uint32  lz,opl,cll;
    
    if(((_SetOPLimitFlag==true)&&(_SetCLLimitFlag!=true))||
       ((_SetOPLimitFlag!=true)&&(_SetCLLimitFlag==true))){
        eedata_read(_L_Zero,lz);
        eedata_read(_OP_Limit,opl);
        eedata_read(_CL_Limit,cll);
        cal_limit(cll,opl,lz);
    }
        
    _SetOPLimitFlag = false;
    _SetCLLimitFlag = false;
//	encoder_cal_flag = false;
    relay_position_judge();
    rush_relays();
}

Uint8 cal_length(Uint32 low, Uint32 high){    
    Uint32 cld,bs,res;
    
    if((_SetOPLimitFlag==true)||(_SetCLLimitFlag==true)){

        eedata_read(_CL_Dir,cld);
        eedata_read(_Clock_Decrease ,bs);
        if(((cld==false)&&(bs==false))||
           ((cld!=false)&&(bs!=false))){   
            res = low - high;
        }else{
            res = high - low;
        } 
        if(_Length_Check_Flag==true){
            if(res<=200){
                _Length_Error_Flag = true;
                return E_ERR;
            }
        }else{
            if((res>=200)&&(res<=400)){
                _Length_Check_Flag = true;
            }
        }
    }
    _Length_Error_Flag = false;
    return E_OK;
}
