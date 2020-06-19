
#include "adc.h"	

#define DATA_SAVED	'Y'
/**********************HAL ****************************/
ADC_HandleTypeDef hadc1;

void loadADC(void);

static void MX_ADC1_Init(void);

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config 
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */
	HAL_ADCEx_Calibration_Start(&hadc1);
  /* USER CODE END ADC1_Init 2 */

}

void User_ADC_Init(void)
{
	MX_ADC1_Init();
	loadADC();
}
uint16_t GetADCReg(ADC_HandleTypeDef* hadc,uint32_t ch)
{                  
		ADC_ChannelConfTypeDef ADC1_ChanConf;

	  ADC1_ChanConf.Channel=ch;                                   //通道
    ADC1_ChanConf.Rank=1;                                       //第1个序列，序列1
    ADC1_ChanConf.SamplingTime=ADC_SAMPLETIME_239CYCLES_5;      //采样时间               
    HAL_ADC_ConfigChannel(hadc,&ADC1_ChanConf);        					//通道配置
		
		delay_us(50);
		HAL_ADC_Start(hadc); 
		HAL_ADC_PollForConversion(hadc,50);  
	
		uint16_t ret=HAL_ADC_GetValue(hadc);
	
		rt_kprintf("adc=%d,%dmV,",ret,ret*3321/4096);	
		return ret;
		//return (uint16_t)HAL_ADC_GetValue(hadc);	  
	

}
uint16_t GetADCReg_Average(ADC_HandleTypeDef* hadc,uint32_t ch,uint8_t times)
{
	uint32_t temp_val=0;
	uint8_t t;
	for(t=0;t<times;t++)
	{
		temp_val+=GetADCReg(hadc,ch);
	}
	return temp_val/times;
} 
//

//
//typedef struct	
//{
//	uint8_t reload;
//	float K[8];
//	float B[8];
//}
//Voltage_KB_T;

Voltage_KB_T Voltage_KB={DATA_SAVED, {1,1,1,1,1,1,1,1},{0,0,0,0,0,0,0,0},};

float GetVol_Average(uint8_t Ainx, uint8_t times)
{
	u16 adcx;
	uint32_t ch;
	float temp, ret;
	
	if(Ainx>0 && Ainx<9)
	{
		switch(Ainx)
		{
			case 1:		ch=ADC_CHANNEL_0;
				break;	
			case 2:   ch=ADC_CHANNEL_1;	
				break;		
			case 3:   ch=ADC_CHANNEL_4;	
				break;		
			case 4:   ch=ADC_CHANNEL_5;	
				break;		
			case 5:   ch=ADC_CHANNEL_6;	
				break;		
			case 6:   ch=ADC_CHANNEL_7;	
				break;		
			case 7:   ch=ADC_CHANNEL_8;	
				break;		
			case 8:   ch=ADC_CHANNEL_9;	
				break;		
			default:
				break;
		}
		adcx=GetADCReg_Average(&hadc1,ch,times);
		temp= ((float)adcx*(3.321/4096));
	
		ret=temp*Voltage_KB.K[Ainx-1]+Voltage_KB.B[Ainx-1];
	}
	if(ret<0) ret=0;
	return ret;
}

void loadADC(void)
{
	
	Voltage_KB_T Voltage_KB1;

	at24cxx.read(at24c256 , (EEPROM_VAR_PAGE_ADDR+1)*64, (uint8_t *)&Voltage_KB1, sizeof(Voltage_KB_T));		

	if(Voltage_KB1.reload==DATA_SAVED)
	{
		for(uint8_t i=0;i<8;i++)
		{
			Voltage_KB.K[i]=Voltage_KB1.K[i];
			Voltage_KB.B[i]=Voltage_KB1.B[i];
		}
	}	
	else 
	{
		at24cxx.write(at24c256 , (EEPROM_VAR_PAGE_ADDR+1)*64, (uint8_t *)&Voltage_KB1, sizeof(Voltage_KB_T));			
	}
}
//
int adc(int argc, char **argv)
{
    if (argc == 1)
    {	
			  rt_kprintf("Usage: \n");
			  rt_kprintf("adc clear <ch>\n");
			  rt_kprintf("    ch:1~8\n");	
				rt_kprintf("adc set <ch> <k> <b>\n");	
			  rt_kprintf("    ch:1~8\n");
				rt_kprintf("adc getKB\n");
        return RT_ERROR;
    }
    if (argc == 2)
    {	
			if (!strcmp(argv[1], "getKB")) 
			{	
				char StrAdcKB[6]="";	
				for(uint8_t i=0;i<8;i++)
				{
					sprintf(StrAdcKB,"%.5f",Voltage_KB.K[i]);	rt_kprintf("K[%d]=%s\n",i,StrAdcKB);
					sprintf(StrAdcKB,"%.5f",Voltage_KB.B[i]);	rt_kprintf("B[%d]=%s\n",i,StrAdcKB);
				}				
        return RT_EOK;	
			}
    }
    if (argc == 3)
    {	
			if (!strcmp(argv[1], "clear")) 
			{	
				uint8_t ch=atoi(argv[2]);
				if(ch>0 && ch<9)
				{
					float temp;
					Voltage_KB.B[ch-1]=0;
					temp=-GetVol_Average(ch,3);
					Voltage_KB.B[ch-1]=temp;
					at24cxx.write(at24c256 , (EEPROM_VAR_PAGE_ADDR+1)*64, (uint8_t *)&Voltage_KB, sizeof(Voltage_KB_T));

				}
        return RT_EOK;	
			}
    }
    if (argc == 5)
    {	
			if (!strcmp(argv[1], "set")) 
			{	
				uint8_t ch=atoi(argv[2]);
				
				float k=atof(argv[3]);
				float b=atof(argv[4]);
				if(ch>0 && ch<9)
				{					
					Voltage_KB.K[ch-1]=k;
					Voltage_KB.B[ch-1]=b;
					at24cxx.write(at24c256 , (EEPROM_VAR_PAGE_ADDR+1)*64, (uint8_t *)&Voltage_KB, sizeof(Voltage_KB_T));
				}
			  rt_kprintf("adc set ok\n");	
        return RT_EOK;	
			}
    }
		return RT_ERROR;
}
//
MSH_CMD_EXPORT(adc, ......);

//


