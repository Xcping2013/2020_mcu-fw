#include "dmm.h"
#include <stdlib.h>
#include "ad7124.h"
#include "ad7124_regs.h"
#include "cal.h"

/*		DMM采集板子调试使用步骤：

DMM模块增加EEPROM用于保存校准参数

1.先进行内部零点和PGA校准：不接外部信号
  (优化内码，一条命令处理全部)
	
	更新：按下内部校准按键，校准并保存校准数据后复位
	
    DMM INT_CAL FULL DCI_1A
    DMM INT_CAL ZERO DCI_1A
    DMM INT_CAL FULL OHM_10R_4W
    DMM INT_CAL ZERO OHM_10R_4W
    AD7124_CAL_SAVE

2.进行信号校准 注意电流只能采集一个方向  

  更新：外接固定电流电阻信号，按下信号校准按键，校准后保存参数
	
	input 100 || 285mA: 		DMM SetGain BySignal DCI_1A 285
	input 150mOHM:  DMM SetGain BySignal OHM_10R_4W 150
	DMM SaveGainOffset
	
3.批量的时候 是否可以保存相同的K值

4.现场没有数字电源的时候如何进行校准, 或者保证校准一次后所有模块无需再校准
	
*/


#define OHM_10mA_Out_PIN 	PC_6	

static int  dmm_fun_type	= DMM_FUNC_UNKNOW;

int CS_10R(int argc, char **argv)
{
	if(argc==2)
	{
		uint8_t cs_pin_inited=0;
		if(cs_pin_inited==0)
		{
			cs_pin_inited=1;
			pinMode(OHM_10mA_Out_PIN,GPIO_MODE_OUTPUT_OD);
		}
		if (!strcmp(argv[1], "off"))
		{
			pinSet(OHM_10mA_Out_PIN);
			rt_kprintf("select 10r\n"); 
			return RT_EOK	; 				
		}
		if (!strcmp(argv[1], "on"))
		{
			pinReset(OHM_10mA_Out_PIN);
			rt_kprintf("deselect 10r\n"); 
			return RT_EOK	; 				
		}		
	}
	return RT_EOK	; 		
}
MSH_CMD_EXPORT(CS_10R,...);

void AD7124ChannelConfigToReg(void)
{
	ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_Channel_0]);
	ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_Channel_1]);
	ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_Channel_2]);
	ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_Channel_3]);

	ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_Config_0]);	
	ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_Config_1]);
	ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_Config_2]);
	ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_Config_3]);

#if defined (USING_EX_CURRENT_SOURCE)
	
#else
	ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_IOCon1]);	
#endif
	
}

/* 
		AIN3、AIN4=U	CH0
		AIN6、AIN7=I	CH1	CH3
		AIN1、AIN2=R	CH2
		Bipolar		23BIT
		uBipolar	24BIT
		ad7124_regsW[AD7124_Filter_0].value=0x0070;
		ad7124_regsW[AD7124_Offset_0].value=0x0070;				
		ad7124_regsW[AD7124_Gain_0].value=0x0070;			
*/
void ADCChannelConvert( int _Func )
{
	static uint8_t ad7124_inited=0;
	if(ad7124_inited==0)	
	{
		ad7124_init(); 
		pinMode(OHM_10mA_Out_PIN,PIN_MODE_OUTPUT);
		pinReset(OHM_10mA_Out_PIN);
		ad7124_inited=1;
		
	}
	if( dmm_fun_type != _Func )
	{
		dmm_fun_type = _Func;

		switch( _Func )
		{
			//IN3 IN4 CH0
			case DMM_FUNC_DCV_100mV:	//0.1/10*128=1.28V	PGA=128;			
				
					 ad7124_regsW[AD7124_Channel_0].value=0x8064;	//AIN3、AIN4
					 ad7124_regsW[AD7124_Channel_1].value=0x10c7;	//DISABLE
					 ad7124_regsW[AD7124_Channel_2].value=0x2022;	//DISABLE	
					 ad7124_regsW[AD7124_Channel_3].value=0x30c7; //DISABLE		
			
					 ad7124_regsW[AD7124_Config_0].value=0x0077;	//PGA=128;		
					 //ad7124_regsW[AD7124_Config_0].value=0x0877;	//PGA=128;		
				break;
			case DMM_FUNC_DCV_1V:			//1/10*16=1.6V		  PGA=16;
				
					 ad7124_regsW[AD7124_Channel_0].value=0x8064;	
					 ad7124_regsW[AD7124_Channel_1].value=0x10c7;	
					 ad7124_regsW[AD7124_Channel_2].value=0x2022;	
					 ad7124_regsW[AD7124_Channel_3].value=0x30c7;	
			
					 ad7124_regsW[AD7124_Config_0].value=0x0074;	
						
				break;
			case DMM_FUNC_DCV_10V:		//10/10*2=2v				PGA=2
				
					 ad7124_regsW[AD7124_Channel_0].value=0x8064;	
					 ad7124_regsW[AD7124_Channel_1].value=0x10c7;	
					 ad7124_regsW[AD7124_Channel_2].value=0x2022;	
				   ad7124_regsW[AD7124_Channel_3].value=0x30c7;	
			
					 ad7124_regsW[AD7124_Config_0].value=0x0071;	
			
					 ad7124_regsW[AD7124_IOCon1].value=0x0000;		
					
				break;
			case DMM_FUNC_DCV_25V:		//25/10*1=2.5V			PGA=1
				
					 ad7124_regsW[AD7124_Channel_0].value=0x8064;	
					 ad7124_regsW[AD7124_Channel_1].value=0x10c7;	
					 ad7124_regsW[AD7124_Channel_2].value=0x2022;	
				   ad7124_regsW[AD7124_Channel_3].value=0x30c7;	
			
					 ad7124_regsW[AD7124_Config_0].value=0x0070;	
							
				break;
			
			case DMM_FUNC_DCI_100uA:	
				break;
			case DMM_FUNC_DCI_1mA:		
				break;
			//IN6 IN7 CH3
			case DMM_FUNC_DCI_100mA:	//0.15ohm *0.1 *128=1.92V		PGA=128
				
					 ad7124_regsW[AD7124_Channel_0].value=0x0064;										
					 ad7124_regsW[AD7124_Channel_1].value=0x10c7;													
				   ad7124_regsW[AD7124_Channel_2].value=0x2022;		
					 ad7124_regsW[AD7124_Channel_3].value=0xB0c7;	
			
					 ad7124_regsW[AD7124_Config_3].value=0x077;				

#if defined (USING_EX_CURRENT_SOURCE)	
					pinReset(OHM_10mA_Out_PIN);
#else		
					ad7124_regsW[AD7124_IOCon1].value=0x0000;			
#endif				
				break;
			//IN6 IN7 CH1
			case DMM_FUNC_DCI_1A:			//0.15ohm *1 *16=2.4V				PGA=16
					 ad7124_regsW[AD7124_Channel_0].value=0x0064;										
					 ad7124_regsW[AD7124_Channel_1].value=0x90c7;													
				   ad7124_regsW[AD7124_Channel_2].value=0x2022;														
					 ad7124_regsW[AD7124_Channel_3].value=0x30c7;		
			
					 ad7124_regsW[AD7124_Config_1].value=0x074;
			
#if defined (USING_EX_CURRENT_SOURCE)	
					pinReset(OHM_10mA_Out_PIN);
#else		
					ad7124_regsW[AD7124_IOCon1].value=0x0000;			
#endif	
				break;
			//IN1 IN2 CH2
			case DMM_FUNC_OHM_10R_4W:	//2ohm *0.010 *128=2.56V		PGA=128
						
					ad7124_regsW[AD7124_Channel_0].value=0x0064;
					ad7124_regsW[AD7124_Channel_1].value=0x10c7;		
					ad7124_regsW[AD7124_Channel_2].value=0xA022;	//1010000000100010	AIN1	AIN2		
					ad7124_regsW[AD7124_Channel_3].value=0x30c7;	
			
					ad7124_regsW[AD7124_Config_2].value=0x077;		//1*0.001*128=0.128V	
			
#if defined (USING_EX_CURRENT_SOURCE)	
					pinSet(OHM_10mA_Out_PIN);
#else		
					ad7124_regsW[AD7124_IOCon1].value=0x0600;			//IOUT0=1000uA		
#endif
						
				break;	
			case DMM_FUNC_OHM_1K_4W:	//1000ohm *0.001 *2=2V			PGA=2
				
					ad7124_regsW[AD7124_Config_2].value=0x0071;		
			
					ad7124_regsW[AD7124_Channel_0].value=0x0064;
					ad7124_regsW[AD7124_Channel_1].value=0x00c7;		
					ad7124_regsW[AD7124_Channel_2].value=0xA022;	//1010000000100010	AIN1	AIN2		

#if defined (USING_EX_CURRENT_SOURCE)	
					pinSet(OHM_10mA_Out_PIN);
#else		
					ad7124_regsW[AD7124_IOCon1].value=0x0600;			//IOUT0=1000uA		
#endif		
			
				break;
			case DMM_FUNC_OHM_10K_4W:	//10kohm *0.0001 *2=2V			PGA=2
				
					ad7124_regsW[AD7124_Config_2].value=0x0071;		
			
					ad7124_regsW[AD7124_Channel_0].value=0x0064;
					ad7124_regsW[AD7124_Channel_1].value=0x00c7;		
					ad7124_regsW[AD7124_Channel_2].value=0xA022;	//1010000000100010	AIN1	AIN2		
			
#if defined (USING_EX_CURRENT_SOURCE)	
					pinSet(OHM_10mA_Out_PIN);
#else		
					ad7124_regsW[AD7124_IOCon1].value=0x0600;			//IOUT0=1000uA		
#endif	
			
				break;
			default:
				break;
		}
		AD7124ChannelConfigToReg();
//		delay_ms(10);
//		AD7124_ReadAverData(ad7124_handler);
//		AD7124_ReadAverData(ad7124_handler);
//		AD7124_ReadAverData(ad7124_handler);
	}
}
//
void ADCChannelConfigToRam( int _Func )
{
	
}
void print_dmm_value( int _Func )
{
	ADCChannelConvert(_Func);
	if(_Func==DMM_FUNC_DCV_100mV || _Func==DMM_FUNC_DCV_1V||_Func==DMM_FUNC_DCV_10V || _Func==DMM_FUNC_DCV_25V)
	{
#if defined (USING_GAIN_AD_VALUE)	
		int32_t ADvalue;
		double DMM_Votage;
		uint8_t voltageStr[10];
				
		ADvalue=AD7124_ReadADAverValue(ad7124_handler);	
		
		DMM_Votage=((ADvalue-CTable[_Func])*KTable[_Func])/1000;
		
		sprintf(voltageStr,"%.3lf",DMM_Votage);
		AD7124_TRACE("Voltage=%s mV\n",voltageStr);
#else		
		double voltage;
		uint8_t voltageStr[10];		
		voltage=AD7124_ReadAverData(ad7124_handler);	
		sprintf(voltageStr,"%.3lf",(voltage*KTable[_Func]-CTable[_Func])/1000);
//		if(voltage<436041)
//		sprintf(voltageStr,"%.3lf",(voltage*KTable[DMM_FUNC_DCI_100mA]-CTable[DMM_FUNC_DCI_1A])/1000);
//		else sprintf(voltageStr,"%.3lf",(voltage*KTable[DMM_FUNC_DCI_1A]-CTable[DMM_FUNC_DCI_1A])/1000);
		AD7124_TRACE("Current=%s mA\n",voltageStr);			
#endif
	}
	else if(_Func==DMM_FUNC_DCI_1A || _Func==DMM_FUNC_DCI_100uA || _Func==DMM_FUNC_DCI_1mA||_Func==DMM_FUNC_DCI_100mA )
	{
#if defined (USING_GAIN_AD_VALUE)	
		int32_t ADvalue;
		double DMM_Votage;
		uint8_t voltageStr[10];
		
		uint8_t voltageStr1[10];
		uint8_t voltageStr2[10];
		
		ADvalue=AD7124_ReadADAverValue(ad7124_handler);	
		
		DMM_Votage=((ADvalue-CTable[_Func])*KTable[_Func])/1000;
		
//		DMM_Votage=((ADvalue-CTable[_Func])*KTable[_Func])/1000+1;
			
//		sprintf(voltageStr1,"%.3lf",CTable[_Func]);
//		sprintf(voltageStr2,"%.3lf",KTable[_Func]);
//		AD7124_TRACE("ADvalue=%d CTable=%s KTable=%s\n",ADvalue,voltageStr1,voltageStr2);
		sprintf(voltageStr,"%.3lf",DMM_Votage);
		AD7124_TRACE("Current=%s mA\n",voltageStr);
#else		
		double voltage;
		uint8_t voltageStr[10];		
		voltage=AD7124_ReadAverData(ad7124_handler);	
		sprintf(voltageStr,"%.3lf",(voltage*KTable[_Func]-CTable[_Func])/1000);
//		if(voltage<436041)
//		sprintf(voltageStr,"%.3lf",(voltage*KTable[DMM_FUNC_DCI_100mA]-CTable[DMM_FUNC_DCI_1A])/1000);
//		else sprintf(voltageStr,"%.3lf",(voltage*KTable[DMM_FUNC_DCI_1A]-CTable[DMM_FUNC_DCI_1A])/1000);
		AD7124_TRACE("Current=%s mA\n",voltageStr);			
#endif
	}	
	else if(_Func==DMM_FUNC_OHM_10R_4W ) // || _Func==DMM_FUNC_OHM_1K_4W||_Func==DMM_FUNC_OHM_10K_4W )
	{
#if defined (USING_GAIN_AD_VALUE)	
		int32_t ADvalue;
		double DMM_Votage;
		uint8_t voltageStr[10];
				
		ADvalue=AD7124_ReadADAverValue(ad7124_handler);	
		
		DMM_Votage=((ADvalue-CTable[_Func])*KTable[_Func])/1000;
		
		sprintf(voltageStr,"%.1lf",DMM_Votage);
		AD7124_TRACE("resistor=%s mOHM\n",voltageStr);		
#else		
		double voltage;
		uint8_t voltageStr[10];
		voltage=AD7124_ReadAverData(ad7124_handler)*0.001;	
		//if(voltage>CTable[_Func])
		sprintf(voltageStr,"%.1lf",(voltage*KTable[_Func]));//-CTable[_Func]/1000));		
		
		AD7124_TRACE("resistor=%s mOHM\n",voltageStr);		
#endif
		
	}	
}
void ad7124_getSetK(void)
{
	 ad7124_regsW[AD7124_Channel_0].value=0x8064;	
	 ad7124_regsW[AD7124_Channel_1].value=0x00c7;	
	 ad7124_regsW[AD7124_Channel_2].value=0x0022;	
	 ad7124_regsW[AD7124_Config_0].value=0x0070;	
	 AD7124ChannelConfigToReg();
	 AD7124_ReadAverData(ad7124_handler);
	 AD7124_ReadAverData(ad7124_handler);
	 AD7124_ReadAverData(ad7124_handler);
	 double voltage=AD7124_ReadAverData(ad7124_handler);
	 if(voltage!=0) 
	 {
		 voltage=2500000/voltage;
		 KTable[DMM_FUNC_DCV_100mV]=voltage/128;
		 KTable[DMM_FUNC_DCV_1V]=voltage/16;
		 KTable[DMM_FUNC_DCV_10V]=voltage/2;
		 KTable[DMM_FUNC_DCV_25V]=voltage; 
	 }
	 ad7124_regsW[AD7124_Channel_0].value=0x0064;										
	 ad7124_regsW[AD7124_Channel_1].value=0x90c7;													
	 ad7124_regsW[AD7124_Channel_2].value=0x0022;												
	 ad7124_regsW[AD7124_Config_1].value=0x0077;	
	 AD7124ChannelConfigToReg();
	 AD7124_ReadAverData(ad7124_handler);
	 AD7124_ReadAverData(ad7124_handler);
	 AD7124_ReadAverData(ad7124_handler); 
	 voltage=AD7124_ReadAverData(ad7124_handler);
	 if(voltage!=0) 
	 {
		 voltage=375000/voltage;
		 KTable[DMM_FUNC_DCI_100mA]=voltage;
		 KTable[DMM_FUNC_DCI_1A]=voltage/8;
	 }		
	  pinMode(PC_6,GPIO_MODE_OUTPUT_OD);
	  pinReset(PC_6);
		ad7124_regsW[AD7124_IOCon1].value=0x0700;			//IOUT0=1000uA
		ad7124_regsW[AD7124_Config_2].value=0x0077;		//1*0.001*128=0.128V
		ad7124_regsW[AD7124_Channel_0].value=0x0064;
		ad7124_regsW[AD7124_Channel_1].value=0x00c7;		
		ad7124_regsW[AD7124_Channel_2].value=0xA022;	//1010000000100010	AIN1	AIN2								
	  AD7124ChannelConfigToReg();
	  AD7124_ReadAverData(ad7124_handler);
	  AD7124_ReadAverData(ad7124_handler);
	  AD7124_ReadAverData(ad7124_handler); 
	  voltage=AD7124_ReadAverData(ad7124_handler);
	  if(voltage!=0) 
	  {
			/*
			voltage=10ohm *0.001 *128=k*10ohm *0.001 *2
						 =
			*/
		  voltage=10000/voltage;		
		  KTable[DMM_FUNC_OHM_10R_4W]=voltage;
		  KTable[DMM_FUNC_OHM_1K_4W]=voltage/64;
			KTable[DMM_FUNC_OHM_10K_4W]=voltage/640;
	  }	
		pinReset(PC_6);		
		Cal_RamToFlash();
}


#define  CAL_ZERO	0
#define  CAL_FULL	1

void AD7124_SYS_CALIBRATION( uint8_t mode, int _Func )
{
//	static uint8_t sys_full_cal_done=0;
	
	if( ( _Func == DMM_FUNC_DCV_10V )|| (_Func==DMM_FUNC_DCI_1A)|| (_Func==DMM_FUNC_DCI_100mA) || (_Func==DMM_FUNC_OHM_10R_4W))
	{
		ADCChannelConvert(_Func);
		
		AD7124ChannelConfigToReg();
		
		if(mode==CAL_FULL)
		{
			//REFin   f_P f_cal  clk_in
			//  1 		1 1 0 0 00 00
			ad7124_regsW[AD7124_ADC_CTRL_REG].value=0x1A0;		
		}
		else if(mode==CAL_ZERO)
		{
			ad7124_regsW[AD7124_ADC_CTRL_REG].value=0x19C;
		}
		ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_ADC_CTRL_REG]);
	}
	delay_ms(100);
	ad7124_regsW[AD7124_ADC_CTRL_REG].value=0x180;	//FULL POWER+CON_CONV
	ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_ADC_CTRL_REG]);
}
void AD7124_INT_CALIBRATION( uint8_t mode, int _Func )
{
	if( ( _Func == DMM_FUNC_DCV_10V )|| (_Func==DMM_FUNC_DCI_1A) || (_Func==DMM_FUNC_DCI_100mA) || (_Func==DMM_FUNC_OHM_10R_4W))
	{
		ADCChannelConvert(_Func);
		//if(_Func== DMM_FUNC_OHM_10R_4W)
		
#if defined (USING_EX_CURRENT_SOURCE)	
		
		pinReset(OHM_10mA_Out_PIN);
#else		
	  ad7124_regsW[AD7124_IOCon1].value=0x0000;			
#endif	

		delay_ms(100);

		AD7124ChannelConfigToReg();
		if(mode==CAL_FULL)
		{
			//LowPower	CLKin	con_conv	VREFin		SET1
			ad7124_regsW[AD7124_ADC_CTRL_REG].value=0x0100;	
			if(_Func== DMM_FUNC_DCV_10V)
			{
				ad7124_regsW[AD7124_Offset_0].value=0x800000;		
				ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_Offset_0]);
			}
			if(_Func== DMM_FUNC_DCI_1A)
			{
				ad7124_regsW[AD7124_Offset_1].value=0x800000;		
				ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_Offset_1]);
			}	
			if(_Func== DMM_FUNC_DCI_100mA)
			{
				ad7124_regsW[AD7124_Offset_3].value=0x800000;		
				ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_Offset_3]);
			}	
			if(_Func== DMM_FUNC_OHM_10R_4W)
			{
				ad7124_regsW[AD7124_Offset_2].value=0x800000;		
				ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_Offset_2]);
			}					
			ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_ADC_CTRL_REG]);
			
			delay_ms(2);
			//SET1+IntFULL-scale cal
			ad7124_regsW[AD7124_ADC_CTRL_REG].value=0x0118;	
		}
		else if(mode==CAL_ZERO)
		{
			ad7124_regsW[AD7124_ADC_CTRL_REG].value=0x0114;
		}
		ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_ADC_CTRL_REG]);
	}
	delay_ms(100);
	
	ad7124_regsW[AD7124_ADC_CTRL_REG].value=0x180;	//FULL POWER+CON_CONV
	ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_ADC_CTRL_REG]);

#if defined (USING_EX_CURRENT_SOURCE)	
		
		pinSet(OHM_10mA_Out_PIN);
#else		
	  ad7124_regsW[AD7124_IOCon1].value=0x00600;			//IOUT=OFF
		ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_IOCon1]);	
	
#endif		


}

void ad7124_inter_calibration(void)
{
	AD7124_INT_CALIBRATION(CAL_FULL, DMM_FUNC_DCV_10V);
	AD7124_INT_CALIBRATION(CAL_ZERO, DMM_FUNC_DCV_10V);

	AD7124_INT_CALIBRATION(CAL_FULL, DMM_FUNC_DCI_1A);
	AD7124_INT_CALIBRATION(CAL_ZERO, DMM_FUNC_DCI_1A);
	
	AD7124_INT_CALIBRATION(CAL_FULL, DMM_FUNC_OHM_10R_4W);
	AD7124_INT_CALIBRATION(CAL_ZERO, DMM_FUNC_OHM_10R_4W);
	
	AD7124_CALIBRATION_RamToEeprom();
	
}

#include "bsp_eeprom_24xx.h"

typedef struct
{
	//uint32_t ParamVer;
	
	uint32_t Offset[4];	//12B
	uint32_t Gain[4];		//12B
	
	//F32 KTABLE[4]			//16B
	//F32 CTABLE[4]			//16B
	//									32+24=56B
}
AD7124_PARAM_T;

AD7124_PARAM_T AD7124_CAL_PARAM;

void Reg_IdentifyRam(void)
{
	unsigned int i= 0;

	for(i=0;i<4;i++)
	{
		AD7124_CAL_PARAM.Offset[i]=ad7124_regsW[AD7124_Offset_0+i].value;
		AD7124_CAL_PARAM.Gain[i]=ad7124_regsW[AD7124_Gain_0+i].value;
	}	
}
//128=256*64  256=512*64byte 为了2芯片兼容 使用前256页就好
//数据存储	0~55其他用途 56~255 用户使用
void AD7124_CALIBRATION_RamToEeprom(void)
{
	uint8_t i;
	int32_t ret;
	for (i = AD7124_Status; (i < AD7124_REG_NO) && !(ret < 0); i++)
	{
			ret = ad7124_read_register(ad7124_handler, &ad7124_regsR[i]);
			AD7124_TRACE("ad7124_regs[%d]=0x%x\n",i,ad7124_regsR[i].value);
	}
	for(i=0;i<4;i++)
	{
		AD7124_CAL_PARAM.Offset[i]=ad7124_regsR[AD7124_Offset_0+i].value;
		AD7124_CAL_PARAM.Gain[i]=ad7124_regsR[AD7124_Gain_0+i].value;
	}
	at24cxx.write(at24c256,55*64+1,(uint8_t *)&AD7124_CAL_PARAM,sizeof(AD7124_CAL_PARAM));	
}
void AD7124_CALIBRATION_EepromToRam(void)
{
	uint8_t i;
	int32_t ret;
	at24cxx.read(at24c256,55*64+1,(uint8_t *)&AD7124_CAL_PARAM,sizeof(AD7124_CAL_PARAM));	
	for(i=0;i<4;i++)
	{
		ad7124_regsW[AD7124_Offset_0+i].value=AD7124_CAL_PARAM.Offset[i];
		
		ad7124_regsW[AD7124_Gain_0+i].value=AD7124_CAL_PARAM.Gain[i];
		
//		ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_Offset_0+i]);
//		ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_Gain_0+i]);
	}
//	for (i = AD7124_Status; (i < AD7124_REG_NO) && !(ret < 0); i++)
//	{
//			ret = ad7124_read_register(ad7124_handler, &ad7124_regsR[i]);
//			AD7124_TRACE("ad7124_regs[%d]=0x%x\n",i,ad7124_regsR[i].value);
//	}

}
MSH_CMD_EXPORT_ALIAS(AD7124_CALIBRATION_RamToEeprom,AD7124_CAL_SAVE,...);
MSH_CMD_EXPORT_ALIAS(AD7124_CALIBRATION_EepromToRam,AD7124_CAL_READ,...);
//


