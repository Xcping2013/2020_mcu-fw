/******************************************************************************/
/*		 配置
ready = (regs[AD7124_Status].value &
AD7124_STATUS_REG_RDY) != 0;			BUG1	

1.Channel_n:	
	AINP
	AINM 
	Setup=SetupN	
	Enable=TRUE
2.CONFIG_N: 
  PGA_N=X;	
	AIN_BUFP & AIN_BUFM =ENABLED ;	
	BIPOLAR=DISABLE 
	REF_SEL= Interanl Reference
3.FILTER_N
	Filter = Sinc4
	FS_0 = 384
4.ADC_Control
	MODE = Continuous Conversion
	POWER_MODE = FULL POWER
	REF_EN = Enabled
5.IO_CONTROL_1:
	IOUT1 Channel Enable = AINN
	IOUT1 Select = xxxuA
	
	校准:
1. 从寄存器树中选择ADC_Control寄存器。
2. 仅使能通道0。
3. 选择Low Power(低功耗)模式。
4. 执行内部满量程校准。
a. 单击ADC控制寄存器的Mode(模式)位域。
b. 选择内部满量程校准。
c. 选择寄存器树中的Gain0寄存器，检查校准是否已执行，并且系数是否改变。
5. 执行内部零电平校准。
a. 单击ADC控制寄存器的Mode(模式)位域。
b. 选择内部零电平校准。
c. 选择寄存器树中的Oset0寄存器，检查校准是否已执行，并且系数是否改变

			//111 0000	unipolar  internal reference PGA=1		
//			ad7124_regsW[AD7124_Filter_0].value=0x0070;
//			ad7124_regsW[AD7124_Offset_0].value=0x0070;				
//			ad7124_regsW[AD7124_Gain_0].value=0x0070;	
*/
/******************************************************************************/
#include <stdlib.h>
#include "ad7124.h"
#include "ad7124_regs.h"

#include "dmm.h"
#include "cal.h"
/* Error codes */
#define INVALID_VAL -1 			/* Invalid argument */
#define COMM_ERR    -2 			/* Communication error on receive */
#define timeout     -3 			/* A ADCtimeout has occured */

struct ad7124_dev my_ad7124;                       /* A new driver instance */
struct ad7124_dev *ad7124_handler = &my_ad7124; 	 /* A driver handle to pass around */
enum ad7124_registers regNr;                       /* Variable to iterate through registers */
long ADCtimeout = 1000;                               /* Number of tries before a function times out */
long ret = 0;                                      /* Return value */
long sample;                                       /* Stores raw value read from the ADC */

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

struct ad7124_st_reg ad7124_regsW[] =
{
	{0x00, 0x00,   1, 2}, /* AD7124_Status */
	
	{0x01, 0x0184, 2, 1}, //内部时钟;内部基准;单步转换;全功率模式

	{0x02, 0x0000, 3, 2}, /* AD7124_Data */
	{0x03, 0x0000, 3, 1}, /* AD7124_IOCon1 */
	{0x04, 0x0000, 2, 1}, /* AD7124_IOCon2 */
	{0x05, 0x02,   1, 2}, /* AD7124_ID */
	
	{0x06, 0x0000, 3, 2}, /* AD7124_Error */
	
	{0x07, 0x0044, 3, 1}, /* AD7124_Error_En */
	
	{0x08, 0x00,   1, 2}, /* AD7124_Mclk_Count */
	
	{0x09, 0x0001, 2, 1}, /* AD7124_Channel_0 */
	{0x0A, 0x0085, 2, 1}, /* AD7124_Channel_1 */		
	
	{0x0B, 0x0001, 2, 1}, /* AD7124_Channel_2 */
	{0x0C, 0x0001, 2, 1}, /* AD7124_Channel_3 */
	{0x0D, 0x0001, 2, 1}, /* AD7124_Channel_4 */
	{0x0E, 0x0001, 2, 1}, /* AD7124_Channel_5 */
	{0x0F, 0x0001, 2, 1}, /* AD7124_Channel_6 */
	{0x10, 0x0001, 2, 1}, /* AD7124_Channel_7 */
	{0x11, 0x0001, 2, 1}, /* AD7124_Channel_8 */
	{0x12, 0x0001, 2, 1}, /* AD7124_Channel_9 */
	{0x13, 0x0001, 2, 1}, /* AD7124_Channel_10 */
	{0x14, 0x0001, 2, 1}, /* AD7124_Channel_11 */
	{0x15, 0x0001, 2, 1}, /* AD7124_Channel_12 */
	{0x16, 0x0001, 2, 1}, /* AD7124_Channel_13 */
	{0x17, 0x0001, 2, 1}, /* AD7124_Channel_14 */
	{0x18, 0x0001, 2, 1}, /* AD7124_Channel_15 */
	
	{0x19, 0x0870, 2, 1}, /* AD7124_Config_0 */	
	{0x1A, 0x0870, 2, 1}, /* AD7124_Config_1 */	
	
	{0x1B, 0x0860, 2, 1}, /* AD7124_Config_2 */	
	{0x1C, 0x0860, 2, 1}, /* AD7124_Config_3 */
	{0x1D, 0x0860, 2, 1}, /* AD7124_Config_4 */
	{0x1E, 0x0860, 2, 1}, /* AD7124_Config_5 */
	{0x1F, 0x0860, 2, 1}, /* AD7124_Config_6 */
	{0x20, 0x0860, 2, 1}, /* AD7124_Config_7 */
	
	{0x21, 0x060002, 3, 1}, /* AD7124_Filter_0 */
	{0x22, 0x060002, 3, 1}, /* AD7124_Filter_1 */
	
	{0x23, 0x060180, 3, 1}, /* AD7124_Filter_2 */
	{0x24, 0x060180, 3, 1}, /* AD7124_Filter_3 */
	{0x25, 0x060180, 3, 1}, /* AD7124_Filter_4 */
	{0x26, 0x060180, 3, 1}, /* AD7124_Filter_5 */
	{0x27, 0x060180, 3, 1}, /* AD7124_Filter_6 */
	{0x28, 0x060180, 3, 1}, /* AD7124_Filter_7 */
	
	
	{0x29, 0x800000, 3, 1}, /* AD7124_Offset_0 */
	{0x2A, 0x800000, 3, 1}, /* AD7124_Offset_1 */
	//{0x29, 0x800148, 3, 1}, /* AD7124_Offset_0 */
	//{0x2A, 0x7fb577, 3, 1}, /* AD7124_Offset_1 */		
	
	{0x2B, 0x800000, 3, 1}, /* AD7124_Offset_2 */
	{0x2C, 0x800000, 3, 1}, /* AD7124_Offset_3 */
	{0x2D, 0x800000, 3, 1}, /* AD7124_Offset_4 */
	{0x2E, 0x800000, 3, 1}, /* AD7124_Offset_5 */
	{0x2F, 0x800000, 3, 1}, /* AD7124_Offset_6 */
	{0x30, 0x800000, 3, 1}, /* AD7124_Offset_7 */
	
	{0x31, 0x500000, 3, 1}, /* AD7124_Gain_0 */
	{0x32, 0x500000, 3, 1}, /* AD7124_Gain_1 */
	//{0x31, 0x555901, 3, 1}, /* AD7124_Gain_0 */
	//{0x32, 0x697308, 3, 1}, /* AD7124_Gain_1 */			
	
	{0x33, 0x500000, 3, 1}, /* AD7124_Gain_2 */
	{0x34, 0x500000, 3, 1}, /* AD7124_Gain_3 */
	{0x35, 0x500000, 3, 1}, /* AD7124_Gain_4 */
	{0x36, 0x500000, 3, 1}, /* AD7124_Gain_5 */
	{0x37, 0x500000, 3, 1}, /* AD7124_Gain_6 */
	{0x38, 0x500000, 3, 1}, /* AD7124_Gain_7 */
};





struct ad7124_st_reg ad7124_regsW0[] =
{
	{0x00, 0x00,   1, 2}, /* AD7124_Status */
					//0x0140 mid	0x180 full
	{0x01, 0x0180, 2, 1}, /* AD7124_ADC_Control */	//1 0100 0000 REF_EN;POWER_MODE=FULL
//{0x01, 0x0000 | AD7124_ADC_CTRL_REG_POWER_MODE(2) | AD7124_ADC_CTRL_REG_MODE(0) | AD7124_ADC_CTRL_REG_DATA_STATUS | AD7124_ADC_CTRL_REG_REF_EN, 2, 1},	
	{0x02, 0x0000, 3, 2}, /* AD7124_Data */
	{0x03, 0x0000, 3, 1}, /* AD7124_IOCon1 */
	{0x04, 0x0000, 2, 1}, /* AD7124_IOCon2 */
	{0x05, 0x02,   1, 2}, /* AD7124_ID */
	{0x06, 0x0000, 3, 2}, /* AD7124_Error */
	{0x07, 0x0044, 3, 1}, /* AD7124_Error_En */
	{0x08, 0x00,   1, 2}, /* AD7124_Mclk_Count */
//	{0x09, 0x8210, 2, 1}, /* AD7124_Channel_0 */ //Enable=1	10000 = temperature senso
	{0x09, 0x8064, 2, 1}, /* AD7124_Channel_0 */	//1000000001100100	 Enable=1	00011 = AIN3
	
	{0x0A, 0x10c7, 2, 1}, /* AD7124_Channel_1 */	//IN6 IN7
	{0x0B, 0x2022, 2, 1}, /* AD7124_Channel_2 */
	{0x0C, 0x30c7, 2, 1}, /* AD7124_Channel_3 */
	{0x0D, 0x0001, 2, 1}, /* AD7124_Channel_4 */
	{0x0E, 0x0001, 2, 1}, /* AD7124_Channel_5 */
	{0x0F, 0x0001, 2, 1}, /* AD7124_Channel_6 */
	{0x10, 0x0001, 2, 1}, /* AD7124_Channel_7 */
	{0x11, 0x0001, 2, 1}, /* AD7124_Channel_8 */
	{0x12, 0x0001, 2, 1}, /* AD7124_Channel_9 */
	{0x13, 0x0001, 2, 1}, /* AD7124_Channel_10 */
	{0x14, 0x0001, 2, 1}, /* AD7124_Channel_11 */
	{0x15, 0x0001, 2, 1}, /* AD7124_Channel_12 */
	{0x16, 0x0001, 2, 1}, /* AD7124_Channel_13 */
	{0x17, 0x0001, 2, 1}, /* AD7124_Channel_14 */
	{0x18, 0x0001, 2, 1}, /* AD7124_Channel_15 */
	
	{0x19, 0x0070, 2, 1}, /* AD7124_Config_0 */	//111 0000 unipolar  internal reference PGA=1			BSP 
	{0x1A, 0x0074, 2, 1}, /* AD7124_Config_1 */	//111 0111	unipolar  internal reference PGA=128	BSP 0.38V
	{0x1B, 0x0075, 2, 1}, /* AD7124_Config_2 */	//111 0101	unipolar  internal reference PGA=32   BSP 1.05V
	{0x1C, 0x0077, 2, 1}, /* AD7124_Config_3 */
	{0x1D, 0x0870, 2, 1}, /* AD7124_Config_4 */
	{0x1E, 0x0870, 2, 1}, /* AD7124_Config_5 */
	{0x1F, 0x0870, 2, 1}, /* AD7124_Config_6 */
	{0x20, 0x0870, 2, 1}, /* AD7124_Config_7 */
	
	{0x21, 0x060180, 3, 1}, /* AD7124_Filter_0 */	//0x60020
	{0x22, 0x060180, 3, 1}, /* AD7124_Filter_1 */
	{0x23, 0x060180, 3, 1}, /* AD7124_Filter_2 */
	{0x24, 0x060180, 3, 1}, /* AD7124_Filter_3 */
	{0x25, 0x060180, 3, 1}, /* AD7124_Filter_4 */
	{0x26, 0x060180, 3, 1}, /* AD7124_Filter_5 */
	{0x27, 0x060180, 3, 1}, /* AD7124_Filter_6 */
	{0x28, 0x060180, 3, 1}, /* AD7124_Filter_7 */
	
	//{0x29, 0x800148, 3, 1}, /* AD7124_Offset_0 */
	
//SYS CAL
////ad7124_regs[42]=0x7fb577
//ad7124_regs[50]=0x697308	
	
	{0x29, 0x800000, 3, 1}, /* AD7124_Offset_0 */
	{0x2A, 0x800000, 3, 1}, /* AD7124_Offset_1 */
	//{0x2A, 0x7fb577, 3, 1}, /* AD7124_Offset_1 */		
	{0x2B, 0x800000, 3, 1}, /* AD7124_Offset_2 */
	{0x2C, 0x800000, 3, 1}, /* AD7124_Offset_3 */
	{0x2D, 0x800000, 3, 1}, /* AD7124_Offset_4 */
	{0x2E, 0x800000, 3, 1}, /* AD7124_Offset_5 */
	{0x2F, 0x800000, 3, 1}, /* AD7124_Offset_6 */
	{0x30, 0x800000, 3, 1}, /* AD7124_Offset_7 */
	
	
	//{0x31, 0x555901, 3, 1}, /* AD7124_Gain_0 */
	{0x31, 0x500000, 3, 1}, /* AD7124_Gain_0 */
	{0x32, 0x500000, 3, 1}, /* AD7124_Gain_1 */
	//{0x32, 0x697308, 3, 1}, /* AD7124_Gain_1 */			
	{0x33, 0x500000, 3, 1}, /* AD7124_Gain_2 */
	{0x34, 0x500000, 3, 1}, /* AD7124_Gain_3 */
	{0x35, 0x500000, 3, 1}, /* AD7124_Gain_4 */
	{0x36, 0x500000, 3, 1}, /* AD7124_Gain_5 */
	{0x37, 0x500000, 3, 1}, /* AD7124_Gain_6 */
	{0x38, 0x500000, 3, 1}, /* AD7124_Gain_7 */
};







int32_t ad7124_setup(struct ad7124_dev *device) 
{
	int32_t ret;
	enum ad7124_registers reg_nr;
	
	struct spi_init_param spi_param;
	
	if (!device)
		return INVALID_VAL;

	//device->regs = ad7124_regs;
	device->regs = ad7124_regsW;
	device->spi_rdy_poll_cnt = 25000;
	
	/* Initialize the SPI communication. */
	
	ret = spi_init(&device->spi_desc, &spi_param);
	if (ret < 0)
		return ret;

	/*  Reset the device interface.*/
	ret = ad7124_reset(device);
	if (ret < 0)
		return ret;

	/* Update the device structure with power-on/reset settings */
	device->check_ready = 1;

	/* Initialize registers AD7124_ADC_Control through AD7124_Filter_7. */	
	for(reg_nr = AD7124_Status; (reg_nr < AD7124_REG_NO) && !(ret < 0);//AD7124_Offset_0
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
double AD7124_ReadData(struct ad7124_dev *device)
{
	struct ad7124_st_reg *regs;
	int32_t sum =0;
	double  ret =0.0;

	if(!device)
		return INVALID_VAL;

	regs = device->regs;


/* Read the value of the Data Register */
	ad7124_read_register(device, &regs[AD7124_Data]);

	/* Get the read result */
	sum = regs[AD7124_Data].value;

	//ret = (sum/8388608.0-1)*2500000.0 +10.0;
	ret = (sum*2500000.0)/16777216 ;
	
	return ret;
	
}
double AD7124_ReadAverData(struct ad7124_dev *device)
{
	double sum=0;
	int8_t i=0;
	
	for(i=0;i<DMM_CAL_Table.filterTimes;i++)
	{
		ret = ad7124_wait_for_conv_ready(ad7124_handler, ADCtimeout);

		sum += AD7124_ReadData(device);
	}
	return sum/(DMM_CAL_Table.filterTimes*1.0);
//	return sum/10.0;
}
int32_t AD7124_ReadADAverValue(struct ad7124_dev *device)
{
	int8_t i=0;
	
	struct ad7124_st_reg *regs;
	int32_t sum =0;

	if(!device)
		return INVALID_VAL;

	regs = device->regs;
	
	for(i=0;i<DMM_CAL_Table.filterTimes;i++)
	{
		ret = ad7124_wait_for_conv_ready(ad7124_handler, ADCtimeout);
		
		ad7124_read_register(device, &regs[AD7124_Data]);

	/* Get the read result */
		sum += regs[AD7124_Data].value;

//		AD7124_TRACE("regs[AD7124_Data].value=%d\n",sum);
	}
	return sum/(DMM_CAL_Table.filterTimes);
}
double AD7124_AdValueToVotage(int32_t ADvalue)
{
	return  (ADvalue*2500000.0)/16777216 ;
}
void ad7124_readData(void)
{
    ret = ad7124_wait_for_conv_ready(ad7124_handler, ADCtimeout);
    if (ret < 0)
    {
			AD7124_TRACE("ad7124 conv err,ret=%d\n",ret);
    }
    ret = ad7124_read_data(ad7124_handler, &sample);
		AD7124_TRACE("ad7124 read AD =%d\n",sample);
		double voltage;
		uint8_t voltageStr[32];
		voltage=AD7124_ReadAverData(ad7124_handler);
		//AD7124_TRACE("voltageD=%f uV\n",voltage);
		sprintf(voltageStr,"%.6lf",voltage);							
		AD7124_TRACE("ad7124 read voltage=%s uV\n",voltageStr);		//RTT prinf float
		if (ret < 0)
    {
			AD7124_TRACE("ad7124_read_data_err,ret=%d\n",ret);
    }	
}
void AD7124_ReadVoltage(void)
{
		ret = ad7124_wait_for_conv_ready(ad7124_handler, ADCtimeout);
		ret = ad7124_read_data(ad7124_handler, &sample);
		AD7124_TRACE("sampleD=%d\n",sample);
		double voltage;
		uint8_t voltageStr[10];
		voltage=AD7124_ReadAverData(ad7124_handler);
		sprintf(voltageStr,"%lf",voltage);		
		//sprintf(voltageStr,"%lf",voltage);							//RTT prinf float
		AD7124_TRACE("voltageStr=%s uV\n",voltageStr);
}

void ad7124_init(void)
{
    /* Initialize AD7124 device. */

//		Cal_IdentifyRam();
//		Reg_IdentifyRam();
//		//Cal_FlashToRam();
//    
//	  if(at24cxx.readU8(at24c256,54*64)!='c')
//		{
//			at24cxx.writeU8(at24c256,54*64,'c');
//			DMM_CAL_KCTable_RamToEeprom();
//			AD7124_CALIBRATION_RamToEeprom();
//			AD7124_TRACE("first time save data to eeprom\n");
//			
//		}
//		DMM_CAL_KCTable_EepromToRam();
//		AD7124_CALIBRATION_EepromToRam();
	
		ret = ad7124_setup(&my_ad7124);
		
	  delay_ms(100);
    if (ret < 0)
    {
			AD7124_TRACE("AD7124 initialization failed,ret=%d\n",ret);
        /* AD7124 initialization failed, check the value of ret! */
    }
    else
    {
        /* AD7124 initialization OK */
    } 
}
//为了电阻的精度考虑,读取2次数据取平均,采样命令需要间隔0.1S
//电流精度OK可以增加SPS
int ad7124(int argc, char **argv)
{
	static uint8_t ad7124Inited=0;
	
	int result = RT_ERROR;

	if(ad7124Inited==0)	{ad7124Inited=1;	ad7124_init();}
	
	if (argc == 2 )
	{
		if (!strcmp(argv[1], "readRegs"))
		{
			for (regNr = AD7124_Status; (regNr < AD7124_REG_NO) && !(ret < 0); regNr++)
			{
					ret = ad7124_read_register(ad7124_handler, &ad7124_regsR[regNr]);
					AD7124_TRACE("ad7124_regs[%d]=0x%x\n",regNr,ad7124_regsR[regNr].value);
			}
			return RT_EOK	; 
		}
		else	if (!strcmp(argv[1], "readData"))
		{
			ad7124_readData();
			return RT_EOK	; 
		}
		else	if (!strcmp(argv[1], "DCV_100mV"))	
		{
			print_dmm_value(DMM_FUNC_DCV_100mV);
			return RT_EOK	; 
		}
		else	if (!strcmp(argv[1], "DCV_1V"))	
		{
			print_dmm_value(DMM_FUNC_DCV_1V);
			return RT_EOK	; 
		}
		else	if (!strcmp(argv[1], "DCV_10V"))	
		{
			print_dmm_value(DMM_FUNC_DCV_10V);
			return RT_EOK	; 
		}
		else	if (!strcmp(argv[1], "DCV_25V"))	
		{
			print_dmm_value(DMM_FUNC_DCV_25V);
			return RT_EOK	; 
		}
		else	if (!strcmp(argv[1], "DCI_100mA"))	
		{
			print_dmm_value(DMM_FUNC_DCI_100mA);
			return RT_EOK	; 
		}
		else	if (!strcmp(argv[1], "DCI_1A"))	
		{
			print_dmm_value(DMM_FUNC_DCI_1A);
			return RT_EOK	; 
		}
		else	if (!strcmp(argv[1], "OHM_10R_4W"))	
		{
			print_dmm_value(DMM_FUNC_OHM_10R_4W);
			return RT_EOK	; 
		}		
		else	if (!strcmp(argv[1], "OHM_1K_4W"))	
		{
			print_dmm_value(DMM_FUNC_OHM_1K_4W);
			return RT_EOK	; 
		}	
		else	if (!strcmp(argv[1], "OHM_10K_4W"))	
		{
			print_dmm_value(DMM_FUNC_OHM_10K_4W);
			return RT_EOK	; 
		}	
		else	if (!strcmp(argv[1], "SaveGainOffset"))	
		{
			DMM_CAL_KCTable_RamToEeprom();
			rt_kprintf("SaveGainOffset OK\n");
			return RT_EOK	; 
		}
		else	if (!strcmp(argv[1], "ReadGainOffset"))	
		{
			DMM_CAL_KCTable_EepromToRam();
			rt_kprintf("ReadGainOffset OK\n");
			return RT_EOK	; 
		}
		else	if (!strcmp(argv[1], "printGainOffset"))	
		{
			printGainOffset();
			return RT_EOK	; 
		}
		else	if (!strcmp(argv[1], "IC_CALIBRATE"))	
		{
			ad7124_inter_calibration();
			rt_kprintf("IC_CALIBRATE OK ... Start to reboot mcu...\n");
			//reboot();
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
			ad7124_read_register(ad7124_handler,&ad7124_regsR[regno]);
			AD7124_TRACE("ad7124 register[%x]=%x\n",regno,ad7124_regsR[regno].value);
			return RT_EOK	; 				
		}
		if (!strcmp(argv[1], "SetFilterTimes"))
		{
			DMM_CAL_Table.filterTimes=atoi(argv[2]);
			rt_kprintf("SetFilterTimes=%d OK\n",DMM_CAL_Table.filterTimes);
			return RT_EOK	; 
		}		
		else	if (!strcmp(argv[1], "CAL_DCV"))	
		{
			ADCChannelConvert(DMM_FUNC_DCV_10V);
			double inputValue= atoi(argv[2])*1000;	
			double getValue=AD7124_ReadAverData(ad7124_handler);
			if(inputValue==0)		CTable[DMM_FUNC_DCV_10V]=getValue;
			else if( getValue!=0)								KTable[DMM_FUNC_DCV_10V]=inputValue/getValue;
			return RT_EOK	; 
		}
		else	if (!strcmp(argv[1], "CAL_DCI"))	
		{
			ADCChannelConvert(DMM_FUNC_DCI_1A);
			double inputValue= atoi(argv[2])*1000;	
			double getValue=AD7124_ReadAverData(ad7124_handler);
			if(inputValue==0)			CTable[DMM_FUNC_DCI_1A]=getValue;//*15/100;
			else if( getValue!=0)	KTable[DMM_FUNC_DCI_1A]=inputValue/getValue;//(inputValue*15)/(getValue*100);
			return RT_EOK	; 
		}
		else	if (!strcmp(argv[1], "CAL_OHM"))	
		{
			ADCChannelConvert(DMM_FUNC_OHM_10R_4W);
			double inputValue= atoi(argv[2])*1000;	
			double getValue=AD7124_ReadAverData(ad7124_handler);
			if(inputValue==0)			CTable[DMM_FUNC_OHM_10R_4W]=getValue;
			else if( getValue!=0)	KTable[DMM_FUNC_OHM_10R_4W]=inputValue/getValue;
			return RT_EOK	; 
		}

		else
		{		
			result =RT_ERROR;
		}   
	}
	if (argc == 4 )
	{
		if (!strcmp(argv[1], "writeADReg"))
		{
			uint8_t regno=atoi(argv[2]);
			ad7124_regsW[regno].value=atoi(argv[3]);
			ad7124_write_register(ad7124_handler,ad7124_regsW[regno]);
			AD7124_TRACE("ad7124 write register[%d]=%d OK\n",regno,ad7124_regsW[regno].value);
			return RT_EOK	; 				
		}
		if (!strcmp(argv[1], "SYS_CAL"))
		{
			if (!strcmp(argv[2], "ZERO"))
			{
				if (!strcmp(argv[3], "DCV_10V"))
				{
					AD7124_SYS_CALIBRATION(0,DMM_FUNC_DCV_10V);		
					rt_kprintf("DMM SYS_CAL ZERO DCV_10V OK\n");					
				}
				if (!strcmp(argv[3], "DCI_1A"))
				{
					AD7124_SYS_CALIBRATION(0,DMM_FUNC_DCI_1A);			
					rt_kprintf("DMM SYS_CAL ZERO DCI_1A OK\n");							
				}
				if (!strcmp(argv[3], "DCI_100mA"))
				{
					AD7124_SYS_CALIBRATION(0,DMM_FUNC_DCI_100mA);			
					rt_kprintf("DMM SYS_CAL ZERO DCI_100mA OK\n");							
				}
				if (!strcmp(argv[3], "OHM_10R_4W"))
				{
					AD7124_SYS_CALIBRATION(0,DMM_FUNC_OHM_10R_4W);		
					rt_kprintf("DMM SYS_CAL ZERO OHM_10R_4W OK\n");	
				}
				return RT_EOK	;
			}
			if (!strcmp(argv[2], "FULL"))
			{
				if (!strcmp(argv[3], "DCV_10V"))
				{
					AD7124_SYS_CALIBRATION(1,DMM_FUNC_DCV_10V);		
					rt_kprintf("DMM SYS_CAL FULL DCV_10V OK\n");	
					
				}
				if (!strcmp(argv[3], "DCI_1A"))
				{
					AD7124_SYS_CALIBRATION(1,DMM_FUNC_DCI_1A);	
					rt_kprintf("DMM SYS_CAL FULL DCI_1A OK\n");						
				}
				if (!strcmp(argv[3], "DCI_100mA"))
				{
					AD7124_SYS_CALIBRATION(1,DMM_FUNC_DCI_100mA);			
					rt_kprintf("DMM SYS_CAL FULL DCI_100mA OK\n");							
				}
				if (!strcmp(argv[3], "OHM_10R_4W"))
				{
					AD7124_SYS_CALIBRATION(1,DMM_FUNC_OHM_10R_4W);
					rt_kprintf("DMM SYS_CAL FULL OHM_10R_4W OK\n");						
				}
				return RT_EOK	;
			}				
		}
		if (!strcmp(argv[1], "INT_CAL"))
		{
			if (!strcmp(argv[2], "ZERO"))
			{
				if (!strcmp(argv[3], "DCV_10V"))
				{
					AD7124_INT_CALIBRATION(0,DMM_FUNC_DCV_10V);		
					rt_kprintf("DMM INT_CAL ZERO DCV_10V OK\n");					
				}
				if (!strcmp(argv[3], "DCI_1A"))
				{
					AD7124_INT_CALIBRATION(0,DMM_FUNC_DCI_1A);			
					rt_kprintf("DMM INT_CAL ZERO DCI_1A OK\n");							
				}
				if (!strcmp(argv[3], "DCI_100mA"))
				{
					AD7124_INT_CALIBRATION(0,DMM_FUNC_DCI_100mA);			
					rt_kprintf("DMM INT_CAL ZERO DCI_100mA OK\n");							
				}
				if (!strcmp(argv[3], "OHM_10R_4W"))
				{
					AD7124_INT_CALIBRATION(0,DMM_FUNC_OHM_10R_4W);		
					rt_kprintf("DMM INT_CAL ZERO OHM_10R_4W OK\n");	
				}
				return RT_EOK	;
			}
			if (!strcmp(argv[2], "FULL"))
			{
				if (!strcmp(argv[3], "DCV_10V"))
				{
					AD7124_INT_CALIBRATION(1,DMM_FUNC_DCV_10V);		
					rt_kprintf("DMM INT_CAL FULL DCV_10V OK\n");	
					
				}
				if (!strcmp(argv[3], "DCI_1A"))
				{
					AD7124_INT_CALIBRATION(1,DMM_FUNC_DCI_1A);	
					rt_kprintf("DMM INT_CAL FULL DCI_1A OK\n");						
				}
				if (!strcmp(argv[3], "DCI_100mA"))
				{
					AD7124_INT_CALIBRATION(1,DMM_FUNC_DCI_100mA);	
					rt_kprintf("DMM INT_CAL FULL DCI_100mA OK\n");						
				}
				if (!strcmp(argv[3], "OHM_10R_4W"))
				{
					AD7124_INT_CALIBRATION(1,DMM_FUNC_OHM_10R_4W);
					rt_kprintf("DMM INT_CAL FULL OHM_10R_4W OK\n");						
				}
				return RT_EOK	;
			}				
		}
				
		else
		{		
			result =RT_ERROR;
		}   
	}
	
	if (argc == 5 )
	{
		if (!strcmp(argv[1], "SetGain"))
		{	
			int Type=DMM_FUNC_UNKNOW;
			
			if (!strcmp(argv[3], "DCV_10V"))	 	Type=DMM_FUNC_DCV_10V;
			if (!strcmp(argv[3], "DCI_100mA")) 	Type=DMM_FUNC_DCI_100mA;
			if (!strcmp(argv[3], "DCI_1A")) 	  Type=DMM_FUNC_DCI_1A;
			if (!strcmp(argv[3], "OHM_10R_4W")) Type=DMM_FUNC_OHM_10R_4W;
			
			if(Type!=DMM_FUNC_UNKNOW)					
			{
				if (!strcmp(argv[2], "BySignal"))	SetGainViaInputSignal(Type,atoi(argv[4]));
				if (!strcmp(argv[2], "ByType"))	  
				{
					KTable[Type]=atof(argv[4]);	
					//DMM_CAL_KCTable_RamToEeprom();
				}
				rt_kprintf("DMM SetGain OK\n");
			}
			return RT_EOK	;	
		}
		if (!strcmp(argv[1], "SetOffset"))
		{	
			int Type=DMM_FUNC_UNKNOW;
			
			if (!strcmp(argv[3], "DCV_10V"))	 	Type=DMM_FUNC_DCV_10V;
			if (!strcmp(argv[3], "DCI_100mA")) 	Type=DMM_FUNC_DCI_100mA;
			if (!strcmp(argv[3], "DCI_1A")) 	  Type=DMM_FUNC_DCI_1A;
			if (!strcmp(argv[3], "OHM_10R_4W")) Type=DMM_FUNC_OHM_10R_4W;
			
			if(Type!=DMM_FUNC_UNKNOW)					
			{
				if (!strcmp(argv[2], "BySignal"))	SetOffsetViaInputSignal(Type,atoi(argv[4]));
				if (!strcmp(argv[2], "ByType"))	  
				{
					CTable[Type]=atof(argv[4]);	
					//DMM_CAL_KCTable_RamToEeprom();
				}
				rt_kprintf("DMM SetGain OK\n");
			}
			return RT_EOK	;	
		}
	}
	else if( result == RT_ERROR )
	{
		rt_kprintf("Usage: \n");
		
//		rt_kprintf("DMM initAD\n");//                    \t-	setup ad7124\n");	
//		rt_kprintf("DMM readADRegs\n");//                \t- read all register\n");			
//		rt_kprintf("DMM readADData\n");//                 \t-	read current data\n");	
//		rt_kprintf("\n");	
//		rt_kprintf("DMM readADReg <regNum>\n");//         \t-	read the register\n");			
//		rt_kprintf("DMM writeADReg <regNum> <value>\n");//\t	-	write the register\n");	
		rt_kprintf("DMM SetFilterTimes\n");//                    \t-	measure voltage with range 10V\n");	
		rt_kprintf("DMM IC_CALIBRATE\n");//                    \t-	measure voltage with range 10V\n");	IC_CALIBRATE
		rt_kprintf("DMM DCV_10V\n");//                    \t-	measure voltage with range 10V\n");	
		rt_kprintf("DMM DCI_1A\n");//                     \t-	measure current with range 1A\n");
		rt_kprintf("DMM OHM_10R_4W \n");//                \t-	measure resistor with range 10OHM\n");				
	    rt_kprintf("\n");	
		rt_kprintf("DMM SaveGainOffset\n");//             \t-	\n");
		rt_kprintf("DMM ReadGainOffset\n");//             \t-	\n");		
		rt_kprintf("DMM printGainOffset\n");//            \t-	\n");
		rt_kprintf("\n");	
		rt_kprintf("DMM SetGain	BySignal DCV_10V | DCI_100mA | DCI_1A | OHM_10R_4W\n");//   \t-	\n");
		rt_kprintf("DMM SetOffset	BySignal DCV_10V | DCI_100mA | DCI_1A | OHM_10R_4W\n");//  \t-	\n");
		rt_kprintf("DMM SetGain	ByType DCV_10V | DCI_100mA | DCI_1A | OHM_10R_4W\n");//   \t-	\n");
		rt_kprintf("DMM SetOffset	ByType DCV_10V | DCI_100mA | DCI_1A | OHM_10R_4W\n");//  \t-	\n");

//	  rt_kprintf("\n");	
//		
//		rt_kprintf("DMM CAL_DCV XXX\n");//             	\t-	input xxx mV to cal\n");
//		rt_kprintf("DMM CAL_DCI XXX\n");//        	      \t-	input xxx mA to cal\n");
//		rt_kprintf("DMM CAL_OHM XXX\n");//        	      \t-	input xxx mA to cal\n");
//		
//		rt_kprintf("DMM SYS_CAL ZERO|FULL DCV_25V\n");//	  	\t-	\n");
//		rt_kprintf("DMM SYS_CAL ZERO|FULL DCI_1A\n");//	  	\t-	\n");	
//		rt_kprintf("DMM SYS_CAL ZERO|FULL OHM_10R_4W\n");//	\t-	\n");

//		rt_kprintf("DMM INT_CAL ZERO|FULL DCV_25V\n");//	  	\t-	\n");
//		rt_kprintf("DMM INT_CAL ZERO|FULL DCI_1A\n");//	  	\t-	\n");	
//		rt_kprintf("DMM INT_CAL ZERO|FULL OHM_10R_4W\n");//	\t-	\n");
	}
	return result;
}


MSH_CMD_EXPORT(ad7124, ...);
//MSH_CMD_EXPORT_ALIAS(ad7124,dmm,...);

/* 校准
int CAL_0(void)
{		
	//101010100
	ad7124_regsR[AD7124_ADC_CTRL_REG].value=0x0154;	
	ad7124_write_register(ad7124_handler,ad7124_regsR[AD7124_ADC_CTRL_REG]);
	delay_ms(10);
	AD7124_TRACE("ok\n");
//	ad7124_regsR[AD7124_ADC_CTRL_REG].value=0x0140;	
//	ad7124_write_register(ad7124_handler,ad7124_regsR[AD7124_ADC_CTRL_REG]);


	return 1;
}
int CAL_2V5(void)
{		
	//101011000
	ad7124_regsR[AD7124_ADC_CTRL_REG].value=0x0158;	
	ad7124_write_register(ad7124_handler,ad7124_regsR[AD7124_ADC_CTRL_REG]);
	delay_ms(10);
	AD7124_TRACE("ok\n");
//	ad7124_regsR[AD7124_ADC_CTRL_REG].value=0x0140;	
//	ad7124_write_register(ad7124_handler,ad7124_regsR[AD7124_ADC_CTRL_REG]);

	return 1;
}
MSH_CMD_EXPORT(CAL_0, ...);
MSH_CMD_EXPORT(CAL_2V5, ...);
*/


//

/*****************************************USER 20200529**************************************************/
/*************************************先快速开发 后扩展优化***效率*******************************************/
void DMM_ad7124ParmInit(void)
{
	ad7124_regsW[AD7124_Channel_0].value=0x0001;	//AIN0、AIN1
	ad7124_regsW[AD7124_Channel_1].value=0x1085;	//AIN4、AIN5
	
	ad7124_regsW[AD7124_Config_0].value=0x0871;		//PGA=2
	ad7124_regsW[AD7124_Config_1].value=0x0871;		//PGA=2
	
	ad7124_regsW[AD7124_Filter_0].value=0x60002;		//9600SPS
	ad7124_regsW[AD7124_Filter_1].value=0x60002;		//9600SPS
	
	ad7124_init();
	
}

int32_t DMM_GetData(uint8_t ch)
{	
	if(ch==1)	//压力变送器电压值采集
	{
		ad7124_regsW[AD7124_Channel_1].value=0x1085;	//DISABLE CH2
		ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_Channel_1]);
		
		ad7124_regsW[AD7124_Channel_0].value=0x8001;	//ENABLE CH1
		ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_Channel_0]);	
		
		ad7124_regsW[AD7124_ADC_Control].value=0x584;	//START CNV
		
		ad7124_wait_for_conv_ready(ad7124_handler, ADCtimeout);
		
		ad7124_read_register(ad7124_handler, &ad7124_regsR[AD7124_Data]);
		
		return ad7124_regsR[AD7124_Data].value;
		
	}
	else if(ch==2)	//按键电压值
	{
		ad7124_regsW[AD7124_Channel_0].value=0x1001;	//DISABLE CH1
		ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_Channel_0]);
		
		ad7124_regsW[AD7124_Channel_1].value=0x9085;	//ENABLE CH2
		ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_Channel_1]);	
		
		ad7124_regsW[AD7124_ADC_Control].value=0x584;	//START CNV
		
		ad7124_wait_for_conv_ready(ad7124_handler, ADCtimeout);
		
		ad7124_read_register(ad7124_handler, &ad7124_regsR[AD7124_Data]);
		
		return ad7124_regsR[AD7124_Data].value;
	}	
	return 0;
}
double DMM_GetAverVoltage(uint8_t ch,uint8_t times)
{
	long sum=0;
	uint8_t i=0;
	for(i=0;i<times;i++)
	{
		sum += DMM_GetData(ch);
	}
	return ((double)(sum)/times)*(2500000.0/16777216);
}




//




