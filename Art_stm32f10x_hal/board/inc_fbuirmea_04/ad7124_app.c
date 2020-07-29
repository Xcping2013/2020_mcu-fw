/******************************************************************************/
/*		 ����     Ӧ�ü��	    ȫ��    
    һ�δ������
ready = (regs[AD7124_Status].value & AD7124_STATUS_REG_RDY) != 0;			BUG1	

  REG
STATUS	 			�� CH_ACTIVE   |  	RDYB 			| ERROR

ADC_Control   :  CLK_SEL		 |		MODE			|	POWER_MODE	|		REF_EN	|		DATA_STATUS		|		CONT_READ......

IO_CONTROL_1	:  IOUT0_CH_EN | IOUT1_CH_EN	|	IOUT0_SET		|	IOUT1_SET ......

IO_CONTROL_2	:  VBIASO-7_EN

REG_ID				:	 release ID
EERROR				:	
EERROR_EN			:	
MCLK_COUNT		:	

Channel_n			:		AINP			 |	 	AINN	 	 | Setup=SetupN |	Enable=TRUE
CONFIG_N			: 	PGA	 			 |	REF_SEL		 |	 AIN_BUF		|	 REF_BUF |	BURNOUT | BIPOLAR

FILTER_N			:		FS_SPS		 |   ......    |    REJ60		  | FILTER_MODE
OFFSET_N			:
GAIN					:
	
	У׼:
1. �ӼĴ�������ѡ��ADC_Control�Ĵ�����
2. ��ʹ��ͨ��0��
3. ѡ��Low Power(�͹���)ģʽ��
4. ִ���ڲ�������У׼��
a. ����ADC���ƼĴ�����Mode(ģʽ)λ��
b. ѡ���ڲ�������У׼��
c. ѡ��Ĵ������е�Gain0�Ĵ��������У׼�Ƿ���ִ�У�����ϵ���Ƿ�ı䡣
5. ִ���ڲ����ƽУ׼��
a. ����ADC���ƼĴ�����Mode(ģʽ)λ��
b. ѡ���ڲ����ƽУ׼��
c. ѡ��Ĵ������е�Oset0�Ĵ��������У׼�Ƿ���ִ�У�����ϵ���Ƿ�ı�
	
20200728��˼·���� Īÿ�α�д�����ϴ��޹�
*/
/******************************************************************************/
#include <stdlib.h>

#include "ad7124.h"
#include "ad7124_regs.h"

#include "ad7124_app.h"
#include "dmm.h"

#define INVALID_VAL -1 /* Invalid argument */
#define COMM_ERR    -2 /* Communication error on receive */
#define TIMEOUT     -3 /* A timeout has occured */

struct ad7124_dev ad7124_dev0;                      /* A new driver instance */
struct ad7124_dev *ad7124_handler0 = &ad7124_dev0; 	/* A driver handle to pass around */
enum ad7124_registers regNr;                       	/* Variable to iterate through registers */
long ADCtimeout = 1000;                             	/* Number of tries before a function times out */
long ret = 0;                                    /* Return value */

static uint8_t AD_PGA_Set=1;
static uint16_t AD_Iout_Set=1000;	//uA

/*---------------------------AD7124--BSP ---------------------------------------------------------------
*/
int32_t ad7124_setup(struct ad7124_dev *device) 
{
	enum ad7124_registers reg_nr;
	
	struct spi_init_param spi_param;
	
	if (!device)	return INVALID_VAL;

	ad7124_regs[AD7124_ADC_Control].value=0x0184;
	
	device->regs = ad7124_regs;
	
	device->spi_rdy_poll_cnt = 25000;
	
	/* Initialize the SPI communication. */
	ret = spi_init(&device->spi_desc, &spi_param);
	if (ret < 0)	return ret;

	/*  Reset the device interface.*/
	ret = ad7124_reset(device);
	if (ret < 0)	return ret;

	/* Update the device structure with power-on/reset settings */
	device->check_ready = 1;

	/*�ڲ�ʱ��;�ڲ���׼;����ת��;ȫ����ģʽ 
		0x0184 ����ת�� DataOnly
	  0x0584 ����ת�� Data+Status
		0x0180 ����ת��	DataOnly
		0x0580 ����ת�� Data+Status
	*/
	ad7124_regs[AD7124_ADC_Control].value=0x0184;
		
		
	/* Initialize registers AD7124_ADC_Control through AD7124_Filter_7. */	
	for(reg_nr = AD7124_Status; (reg_nr < AD7124_REG_NO) && !(ret < 0);//AD7124_Offset_0 AD7124_REG_NO
	    reg_nr++) {
		if (device->regs[reg_nr].rw == AD7124_RW) {
			ret = ad7124_write_register(device, device->regs[reg_nr]);
			if (ret < 0)
				break;
		}

		/* Get CRC State and device SPI interface settings */
		if (reg_nr == AD7124_Error_En) {
			ad7124_update_crcsetting(device);
			ad7124_update_dev_spi_settings(device);
		}
	}
	return ret;
}
void ad7124_init(void)
{
    /* Initialize AD7124 device. */		
		ret = ad7124_setup(&ad7124_dev0);
		
	  delay_ms(100);
    if (ret < 0)
    {
			AD7124_TRACE("AD7124 initialization failed,ret=%d\n",ret);
      /* AD7124 initialization failed, check the value of ret! */
    }
    else
    {
			AD7124_TRACE("AD7124 init ok\n");
      /* AD7124 initialization OK */
    } 
}
int32_t AD7124_ReadDataReg(struct ad7124_dev *device)
{
	struct ad7124_st_reg *regs;
	
	if(!device)	return INVALID_VAL;

	regs = device->regs;

/* Read the value of the Data Register */
	ad7124_read_register(device, &regs[AD7124_Data]);

//	ret = (sum/8388608.0-1)*2500000.0 +10.0;
//	retADValue = (sum*2500000.0)/16777216 ;
	
	return regs[AD7124_Data].value;
}
int32_t AD7124_GetAverADValue(struct ad7124_dev *device)
{
	int32_t sum=0;
	int8_t i=0;
	
	for(i=0;i<DMM.averTimes;i++)
	{
		ret = ad7124_wait_for_conv_ready(ad7124_handler0, ADCtimeout);
//		if (ret < 0)
//    {
//			AD7124_TRACE("ad7124 conv err,ret=%d\n",ret);
//    }
		sum += AD7124_ReadDataReg(device);
	}

	return sum/(DMM.averTimes*1.0);
}
double AD7124_AdValueToVotage(double ADvalue)	//�������޷���
{
	return  (ADvalue*2500000.0)/16777216 ;
}
/*---------------------------AD7124--APP ---------------------------------------------------------------
*/
//
void SetAD7124ToMatchFun( int _Func )
{
	static uint8_t dmm_fun_type_old=DMM_FUNC_UNKNOW;
	
	//AD7124_TRACE("SetAD7124ToMatchFun=%d,dmm_fun_type=%d\n",_Func,dmm_fun_type);
	
	if( dmm_fun_type_old != _Func )
	{
		dmm_fun_type_old = _Func;
		
		if(_Func==DMM_FUNC_DCV_100mV || _Func== DMM_FUNC_DCV_1V || _Func== DMM_FUNC_DCV_10V || _Func== DMM_FUNC_DCV_25V)
		{
			//	AD7124_TRACE("dmm_fun_type=%d\n",_Func);
			
			 ad7124_regs[AD7124_Channel_0].value=0x8064;	//AIN3+��AIN4- SET0 CH_EN
			 ad7124_regs[AD7124_Channel_1].value=0x10c7;	//AIN6+��AIN7- SET1	CH_DIS_EN
			 ad7124_regs[AD7124_Channel_2].value=0x2022;	//AIN1+��AIN2- SET2 CH_DIS_EN
			 ad7124_regs[AD7124_Channel_3].value=0x30c7;  //AIN6+��AIN7- SET3	CH_DIS_EN
			
			 ad7124_regs[AD7124_IOCon1].value=0x0000;			//Iout_OFF
		}
		else if(_Func==DMM_FUNC_DCI_100mA || _Func== DMM_FUNC_DCI_1A)
		{
			 ad7124_regs[AD7124_Channel_0].value=0x0064;	//AIN3+��AIN4- SET0 CH_DIS_EN						
			 ad7124_regs[AD7124_Channel_1].value=0x10c7;	//AIN6+��AIN7- SET1	CH_DIS_EN												
			 ad7124_regs[AD7124_Channel_2].value=0x2022;	//AIN1+��AIN2- SET2 CH_DIS_EN	
			 ad7124_regs[AD7124_Channel_3].value=0xB0c7;	//AIN6+��AIN7- SET3	CH_EN
			
			 ad7124_regs[AD7124_IOCon1].value=0x0000;			//Iout_OFF
		}
		else if(_Func==DMM_FUNC_OHM_10R_4W || _Func== DMM_FUNC_OHM_10K_4W)
		{
			ad7124_regs[AD7124_Channel_0].value=0x0064;	//AIN3+��AIN4- SET0 CH_DIS_EN	
			ad7124_regs[AD7124_Channel_1].value=0x10c7;	//AIN6+��AIN7- SET1	CH_DIS_EN			
			ad7124_regs[AD7124_Channel_2].value=0xA022;	//AIN1+��AIN2- SET2 CH_EN		
			ad7124_regs[AD7124_Channel_3].value=0x30c7;	//AIN6+��AIN7- SET3	CH_DIS_EN
	
			ad7124_regs[AD7124_Config_2].value=0x077;		//PGA=128 REF_INT BUF_ON �޼��� 0x877 ����
																										//1*0.001*128=0.128V	
//#if defined (USING_EX_CURRENT_SOURCE)	
//					//pinSet(OHM_10mA_Out_PIN);
//#else		
//			ad7124_regs[AD7124_IOCon1].value=0x0600;			//AIN0=IOUT0=1000uA			
//#endif
				
		}
		switch( _Func )//select PGA		//0x08xx-˫����		0x00xx-������
		{
			/*
			0.1/10*128=1.28V	PGA=128;		
			*/
			case DMM_FUNC_DCV_100mV:		 ad7124_regs[AD7124_Config_0].value=0x0077;	AD_PGA_Set=128;
				break;
			/*
			1/10*16=1.6V		  PGA=16;		
			*/
			case DMM_FUNC_DCV_1V:				ad7124_regs[AD7124_Config_0].value=0x0074;	AD_PGA_Set=16;						
				break;
			/*
			10/10*2=2v				PGA=2	
			*/
			case DMM_FUNC_DCV_10V:		  ad7124_regs[AD7124_Config_0].value=0x0071;	AD_PGA_Set=2;					
				break;
			/*
			25/10*1=2.5V			PGA=1	
			*/
			case DMM_FUNC_DCV_25V:		  ad7124_regs[AD7124_Config_0].value=0x0070;	AD_PGA_Set=1;							
				break;		
			/*
			0.15ohm *0.1 *128=1.92V		PGA=128
			*/
			case DMM_FUNC_DCI_100mA:	  ad7124_regs[AD7124_Config_3].value=0x077;		AD_PGA_Set=128;						
				break;
			/*
			0.15ohm *1 *16=2.4V				PGA=16
			*/
			case DMM_FUNC_DCI_1A:			
					ad7124_regs[AD7124_Config_1].value=0x074;					AD_PGA_Set=16;
					 ad7124_regs[AD7124_Channel_1].value=0x90c7;																									
					 ad7124_regs[AD7124_Channel_3].value=0x30c7;		
				break;
			/*
			10ohm *0.001 *128=1.28V		PGA=128
			*/
			case DMM_FUNC_OHM_10R_4W:	
					ad7124_regs[AD7124_Config_2].value=0x0077;		AD_PGA_Set=128;		AD_Iout_Set=1000;
					ad7124_regs[AD7124_IOCon1].value=0x0600;			//AIN0=IOUT0=1000uA		
				break;	
			/*
			10kohm *0.0001 *2=2V			PGA=2
			*/
			case DMM_FUNC_OHM_10K_4W:	
				ad7124_regs[AD7124_Config_2].value=0x0071;		AD_PGA_Set=2;		AD_Iout_Set=100;
				ad7124_regs[AD7124_IOCon1].value=0x0200;			//AIN0=IOUT0=100uA					
				break;
			default:
				break;
		}
		ad7124_write_register(ad7124_handler0,ad7124_regs[AD7124_Channel_0]);
		ad7124_write_register(ad7124_handler0,ad7124_regs[AD7124_Channel_1]);
		ad7124_write_register(ad7124_handler0,ad7124_regs[AD7124_Channel_2]);
		ad7124_write_register(ad7124_handler0,ad7124_regs[AD7124_Channel_3]);

		ad7124_write_register(ad7124_handler0,ad7124_regs[AD7124_Config_0]);	
		ad7124_write_register(ad7124_handler0,ad7124_regs[AD7124_Config_1]);
		ad7124_write_register(ad7124_handler0,ad7124_regs[AD7124_Config_2]);
		ad7124_write_register(ad7124_handler0,ad7124_regs[AD7124_Config_3]);
		ad7124_write_register(ad7124_handler0,ad7124_regs[AD7124_IOCon1]);	
		

		
		//AD7124_TRACE("ad7124_write_register over\n");
	}
			//����ת��OK��ת�����ģʽ����Ҫ�����ٿ���ת��
		ad7124_regs[AD7124_ADC_Control].value=0x0184;
		ad7124_write_register(ad7124_handler0,ad7124_regs[AD7124_ADC_Control]);	
}
void adc_hw_init(void)
{
	ad7124_init();
}
void adc_print_data( int _Func )
{
	double DMM_Votage;
	char StrAdcData[10];
	
	SetAD7124ToMatchFun(_Func);
	
	if( _Func > DMM_FUNC_UNKNOW && _Func < DMM_FUNC_COUNT )
	{	
		//DMM_Votage=((ADvalue-DMM.CTable[_Func])*DMM.KTable[_Func])/1000;
		
		DMM_Votage=AD7124_AdValueToVotage((AD7124_GetAverADValue(ad7124_handler0)));
	}
	if(_Func==DMM_FUNC_DCV_100mV || _Func==DMM_FUNC_DCV_1V||_Func==DMM_FUNC_DCV_10V || _Func==DMM_FUNC_DCV_25V)
	{
		sprintf(StrAdcData,"%.3lf",(DMM_Votage*10/AD_PGA_Set)*DMM.KTable[_Func]+DMM.CTable[_Func]);		
		AD7124_TRACE("Voltage=%s uV\n",StrAdcData);
	}
	else if(_Func==DMM_FUNC_DCI_1A || _Func==DMM_FUNC_DCI_100mA )
	{
		sprintf(StrAdcData,"%.3lf",(DMM_Votage/(AD_PGA_Set*SAMPLE_RES_MOHM))*DMM.KTable[_Func]+DMM.CTable[_Func]);		
		AD7124_TRACE("Current=%s mA\n",StrAdcData);			
	}	
	else if(_Func==DMM_FUNC_OHM_10R_4W || _Func==DMM_FUNC_OHM_10K_4W )
	{
		sprintf(StrAdcData,"%.3lf",(DMM_Votage/(AD_PGA_Set*AD_Iout_Set/1000))*DMM.KTable[_Func]+DMM.CTable[_Func]);		
		AD7124_TRACE("resistor=%s mOHM\n",StrAdcData);			
	}	
}
#if AD7124_TRACE_EN

struct ad7124_st_reg ad7124_regsR[AD7124_REG_NO] = {
	{0x00, 0,   1, 2}, /* AD7124_Status */
	{0x01, 0, 2, 1}, /* AD7124_ADC_Control */
	{0x02, 0, 3, 2}, /* AD7124_Data */
	{0x03, 0, 3, 1}, /* AD7124_IOCon1 */
	{0x04, 0, 2, 1}, /* AD7124_IOCon2 */
	{0x05, 0,   1, 2}, /* AD7124_ID */
	{0x06, 0, 3, 2}, /* AD7124_Error */
	{0x07, 0, 3, 1}, /* AD7124_Error_En */
	{0x08, 0,   1, 2}, /* AD7124_Mclk_Count */
	{0x09, 0, 2, 1}, /* AD7124_Channel_0 */
	{0x0A, 0, 2, 1}, /* AD7124_Channel_1 */
	{0x0B, 0, 2, 1}, /* AD7124_Channel_2 */
	{0x0C, 0, 2, 1}, /* AD7124_Channel_3 */
	{0x0D, 0, 2, 1}, /* AD7124_Channel_4 */
	{0x0E, 0, 2, 1}, /* AD7124_Channel_5 */
	{0x0F, 0, 2, 1}, /* AD7124_Channel_6 */
	{0x10, 0, 2, 1}, /* AD7124_Channel_7 */
	{0x11, 0, 2, 1}, /* AD7124_Channel_8 */
	{0x12, 0, 2, 1}, /* AD7124_Channel_9 */
	{0x13, 0, 2, 1}, /* AD7124_Channel_10 */
	{0x14, 0, 2, 1}, /* AD7124_Channel_11 */
	{0x15, 0, 2, 1}, /* AD7124_Channel_12 */
	{0x16, 0, 2, 1}, /* AD7124_Channel_13 */
	{0x17, 0, 2, 1}, /* AD7124_Channel_14 */
	{0x18, 0, 2, 1}, /* AD7124_Channel_15 */
	{0x19, 0, 2, 1}, /* AD7124_Config_0 */
	{0x1A, 0, 2, 1}, /* AD7124_Config_1 */
	{0x1B, 0, 2, 1}, /* AD7124_Config_2 */
	{0x1C, 0, 2, 1}, /* AD7124_Config_3 */
	{0x1D, 0, 2, 1}, /* AD7124_Config_4 */
	{0x1E, 0, 2, 1}, /* AD7124_Config_5 */
	{0x1F, 0, 2, 1}, /* AD7124_Config_6 */
	{0x20, 0, 2, 1}, /* AD7124_Config_7 */
	{0x21, 0, 3, 1}, /* AD7124_Filter_0 */
	{0x22, 0, 3, 1}, /* AD7124_Filter_1 */
	{0x23, 0, 3, 1}, /* AD7124_Filter_2 */
	{0x24, 0, 3, 1}, /* AD7124_Filter_3 */
	{0x25, 0, 3, 1}, /* AD7124_Filter_4 */
	{0x26, 0, 3, 1}, /* AD7124_Filter_5 */
	{0x27, 0, 3, 1}, /* AD7124_Filter_6 */
	{0x28, 0, 3, 1}, /* AD7124_Filter_7 */
	{0x29, 0, 3, 1}, /* AD7124_Offset_0 */
	{0x2A, 0, 3, 1}, /* AD7124_Offset_1 */
	{0x2B, 0, 3, 1}, /* AD7124_Offset_2 */
	{0x2C, 0, 3, 1}, /* AD7124_Offset_3 */
	{0x2D, 0, 3, 1}, /* AD7124_Offset_4 */
	{0x2E, 0, 3, 1}, /* AD7124_Offset_5 */
	{0x2F, 0, 3, 1}, /* AD7124_Offset_6 */
	{0x30, 0, 3, 1}, /* AD7124_Offset_7 */
	{0x31, 0, 3, 1}, /* AD7124_Gain_0 */
	{0x32, 0, 3, 1}, /* AD7124_Gain_1 */
	{0x33, 0, 3, 1}, /* AD7124_Gain_2 */
	{0x34, 0, 3, 1}, /* AD7124_Gain_3 */
	{0x35, 0, 3, 1}, /* AD7124_Gain_4 */
	{0x36, 0, 3, 1}, /* AD7124_Gain_5 */
	{0x37, 0, 3, 1}, /* AD7124_Gain_6 */
	{0x38, 0, 3, 1}, /* AD7124_Gain_7 */
};

int ad7124(int argc, char **argv)
{
	int result = RT_ERROR;
	
	ret=0;
	
	if (argc == 2 )
	{
		if (!strcmp(argv[1], "readRegs"))
		{
			for (regNr = AD7124_Status; (regNr < AD7124_REG_NO) && !(ret < 0); regNr++)
			{
					ret = ad7124_read_register(ad7124_handler0, &ad7124_regsR[regNr]);
					AD7124_TRACE("ad7124_regs[0x%x]=0x%x\n",regNr,ad7124_regsR[regNr].value);
			}
			return RT_EOK	; 
		}
		else	if (!strcmp(argv[1], "readData"))
		{
			//AD7124_AdValueToVotage(AD7124_GetAverADValue(ad7124_handler0));
			return RT_EOK	; 
		}
		else
		{		
			result =RT_ERROR;
		}     
	}
	if (argc == 3 )
	{
		if (!strcmp(argv[1], "readReg"))
		{
			uint8_t regno=atoi(argv[2]);	
			ad7124_read_register(ad7124_handler0,&ad7124_regsR[regno]);
			AD7124_TRACE("ad7124 register[%x]=0x%x\n",regno,ad7124_regsR[regno].value);
			return RT_EOK	; 				
		}
		else
		{		
			result =RT_ERROR;
		}   
	}
	if (argc == 4 )
	{
		if (!strcmp(argv[1], "writeReg"))
		{
			uint8_t regno=atoi(argv[2]);
			ad7124_regs[regno].value=atoi(argv[3]);
			ad7124_write_register(ad7124_handler0,ad7124_regs[regno]);
			AD7124_TRACE("ad7124 write register[%d]=%d OK\n",regno,ad7124_regs[regno].value);
			return RT_EOK	; 				
		}
		else
		{		
			result =RT_ERROR;
		}   
	}
	else if( result == RT_ERROR )
	{
		rt_kprintf("Usage: \n");
		
		rt_kprintf("ad7124  readReg  <AD7124_REG_NO:0~56>\n");
		rt_kprintf("ad7124  readRegs\n");
		rt_kprintf("ad7124  writeReg <AD7124_REG_NO> <AD7124_REG_VAL>\n");
	}
	return result;
}
MSH_CMD_EXPORT(ad7124, ...);

#endif

//

//












