
#include "bsp_12BitADC.h"	
#include "app_eeprom_24xx.h"

#if 1
	#define DBG_ENABLE	0
	#if 	( DBG_ENABLE )
	#define DBG_TRACE		rt_kprintf
	#else
	#define DBG_TRACE(...)		
#endif

#endif

//
#define EEPROM_ADC_CAL_PAGE_ADDR	( 110 )			
#define EEPROM_ADC_CAL_PAGE_LEN	  ( 1+1 )										
		
//#define VREF	3291.0		

#define VREF	3000.0	
#define CAL_DATA_SAVED	30

/*************************************************************************************/
typedef struct	
{											// Bytes	45
	uint8_t saved;			//	1
	float zero;					//	4
	float gain[5];			//  4*5   20
	float offset[5];		//  4*5 	20
}
AdcCalibration_T;

AdcCalibration_T VoltageADC1_CAL={CAL_DATA_SAVED,0, {1,1,1,1,1},{0,0,0,0,0},};
AdcCalibration_T VoltageADC2_CAL={CAL_DATA_SAVED,0, {1,1,1,1,1},{0,0,0,0,0},};
/*************************************************************************************/
void LoadADC_CAL(void);

uint16_t GetADCRegVal(uint8_t ch);
uint16_t GetADCReg_AverVal(uint8_t ch, uint8_t times);

/*********HAL ******ADC_SAMPLETIME_71CYCLES_5 | ADC_SAMPLETIME_239CYCLES_5*********/
#if 1
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

static void MX_ADC1_Init(void);
static void MX_ADC2_Init(void);

static void MX_ADC1_Init(void)
{

  ADC_ChannelConfTypeDef sConfig = {0};

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
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */
	HAL_ADCEx_Calibration_Start(&hadc1);					 //У׼ADC
  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */
  /** Common config 
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_9;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */
	HAL_ADCEx_Calibration_Start(&hadc2);					 //У׼ADC
  /* USER CODE END ADC2_Init 2 */

}

/**
* @brief ADC MSP Initialization
* This function configures the hardware resources used in this example
* @param hadc: ADC handle pointer
* @retval None
*/
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hadc->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_ADC1_CLK_ENABLE();
  
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**ADC1 GPIO Configuration    
    PB0     ------> ADC1_IN8 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN ADC1_MspInit 1 */

  /* USER CODE END ADC1_MspInit 1 */
  }
  else if(hadc->Instance==ADC2)
  {
  /* USER CODE BEGIN ADC2_MspInit 0 */

  /* USER CODE END ADC2_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_ADC2_CLK_ENABLE();
  
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**ADC2 GPIO Configuration    
    PB1     ------> ADC2_IN9 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN ADC2_MspInit 1 */

  /* USER CODE END ADC2_MspInit 1 */
  }

}

/**
* @brief ADC MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hadc: ADC handle pointer
* @retval None
*/
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
  if(hadc->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC1_CLK_DISABLE();
  
    /**ADC1 GPIO Configuration    
    PB0     ------> ADC1_IN8 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0);

  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }
  else if(hadc->Instance==ADC2)
  {
  /* USER CODE BEGIN ADC2_MspDeInit 0 */

  /* USER CODE END ADC2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC2_CLK_DISABLE();
  
    /**ADC2 GPIO Configuration    
    PB1     ------> ADC2_IN9 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_1);

  /* USER CODE BEGIN ADC2_MspDeInit 1 */

  /* USER CODE END ADC2_MspDeInit 1 */
  }

}

#endif

/**********************USER  ADC BASIC****************************/
void LoadADC_CAL(void)
{
	
	AdcCalibration_T VoltageADC1_CAL1;
	AdcCalibration_T VoltageADC2_CAL1;
	
	at24cxx.read(at24c256 , (EEPROM_ADC_CAL_PAGE_ADDR)*64, (uint8_t *)&VoltageADC1_CAL1, sizeof(AdcCalibration_T));		
	at24cxx.read(at24c256 , (EEPROM_ADC_CAL_PAGE_ADDR+1)*64, (uint8_t *)&VoltageADC2_CAL1, sizeof(AdcCalibration_T));		
	if(VoltageADC1_CAL1.saved==CAL_DATA_SAVED)
	{
		for(uint8_t i=0;i<5;i++)
		{
			VoltageADC1_CAL.gain[i]=VoltageADC1_CAL1.gain[i];
			VoltageADC1_CAL.offset[i]=VoltageADC1_CAL1.offset[i];
		}
		VoltageADC1_CAL.zero=VoltageADC1_CAL1.zero;
	}	
	else 
	{
		at24cxx.write(at24c256 , (EEPROM_ADC_CAL_PAGE_ADDR)*64, (uint8_t *)&VoltageADC1_CAL, sizeof(AdcCalibration_T));			
	}
	if(VoltageADC2_CAL1.saved==CAL_DATA_SAVED)
	{
		for(uint8_t i=0;i<5;i++)
		{
			VoltageADC2_CAL.gain[i]=VoltageADC2_CAL1.gain[i];
			VoltageADC2_CAL.offset[i]=VoltageADC2_CAL1.offset[i];
		}
		VoltageADC2_CAL.zero=VoltageADC2_CAL1.zero;
	}	
	else 
	{
		at24cxx.write(at24c256 , (EEPROM_ADC_CAL_PAGE_ADDR+1)*64, (uint8_t *)&VoltageADC2_CAL, sizeof(AdcCalibration_T));			
	}	
}
void bsp_12BitADC_init(void)
{
	MX_ADC1_Init();
	MX_ADC2_Init();
	LoadADC_CAL();
}


uint16_t GetADCRegVal(uint8_t ch)
{   
	if(ch==1)
	{
		HAL_ADC_Start(&hadc1); 
		HAL_ADC_PollForConversion(&hadc1,20);       
		return (uint16_t)HAL_ADC_GetValue(&hadc1);	
	}
	else if(ch==2)
	{
		HAL_ADC_Start(&hadc2); 
		HAL_ADC_PollForConversion(&hadc2,20);       
		return (uint16_t)HAL_ADC_GetValue(&hadc2);		
	}
	return 0;
}
uint16_t GetADCReg_AverVal(uint8_t ch, uint8_t times)
{
	uint32_t	temp_val=0;
	uint8_t t;
	for(t=0;t<times;t++)
	{
		temp_val+=GetADCRegVal(ch);
	}
	return temp_val/times;
} 
float GetADCReg_FilterVal(uint8_t ch,uint8_t times)
{
	uint16_t max,min,temp;
	
	uint32_t sum=0; 
	
	float aver=0.0;
	
	static uint8_t getOnce=0;

	if(	times>1)
	{
		for(uint8_t t=0;t<times+2;t++)
		{
			temp=GetADCRegVal(ch);
			
			if(getOnce==0) {getOnce=1;max=min=temp;}
			
			if(max<temp) 	max=temp;
			if(min>temp) 	min=temp;
			
			sum=sum+temp;		
		}	
		aver=(float)(sum-max-min)/times;	
	}
	else 
	{
		aver=GetADCRegVal(ch);
	}
	getOnce=0;

	return aver;
} 
//
float ConvertADC_to_mV(uint8_t ch, float adcVal)
{
	float temp, ret;
	
	if(ch==1 ) 	
	{
		temp= adcVal*(VREF/1000/4096)*2;		
		ret=temp*VoltageADC1_CAL.gain[0]+VoltageADC1_CAL.offset[0];	
		if(ret<VoltageADC1_CAL.zero) ret=0;	
	}
	else if(ch==2) 			 
	{
		temp= adcVal*(VREF/4096);
		ret=temp*VoltageADC2_CAL.gain[0]+VoltageADC2_CAL.offset[0];		
		if(ret<VoltageADC2_CAL.zero) ret=0;
	}
	return ret;
}
/***********************************************************************************/
/***************************************key funnction***********************************/
/***********************************************************************************/
/***************************************msh commands***********************************/
int adc(int argc, char **argv)
{
    if (argc == 1)
    {	
			  rt_kprintf("Usage: \n");
			  rt_kprintf("adc zero <ch> <val>\n");
			  rt_kprintf("    ch:1~2\n");
				rt_kprintf("    val: when data < val ,set data=0\n");
				rt_kprintf("adc getCAL|getZero\n");
			
				rt_kprintf("adc set <ch> <k> <b>\n");	
			  rt_kprintf("    ch:1~2\n");
//				rt_kprintf("adc set <ch> <range> <k> <b>\n");
//				rt_kprintf("    range:1~5\n");
				
        return RT_ERROR;
    }
    if (argc == 2)
    {	
			if (!strcmp(argv[1], "getCAL")) 
			{	
				char StrAdcKB[6]="";	
				for(uint8_t i=0;i<5;i++)
				{
					sprintf(StrAdcKB,"%.5f",VoltageADC1_CAL.gain[i]);		rt_kprintf("K1[%d]=%s\n",i,StrAdcKB);
					sprintf(StrAdcKB,"%.5f",VoltageADC1_CAL.offset[i]);	rt_kprintf("B1[%d]=%s\n",i,StrAdcKB);
					sprintf(StrAdcKB,"%.5f",VoltageADC2_CAL.gain[i]);		rt_kprintf("K2[%d]=%s\n",i,StrAdcKB);
					sprintf(StrAdcKB,"%.5f",VoltageADC2_CAL.offset[i]);	rt_kprintf("B2[%d]=%s\n",i,StrAdcKB);
				}				
        return RT_EOK;	
			}
			if (!strcmp(argv[1], "getZero")) 
			{	
				char StrAdcKB[10]="";	

				sprintf(StrAdcKB,"%.5f",VoltageADC1_CAL.zero);	rt_kprintf("zero[1]=%s\n",StrAdcKB);
				sprintf(StrAdcKB,"%.5f",VoltageADC2_CAL.zero);	rt_kprintf("zero[2]=%s\n",StrAdcKB);
		
        return RT_EOK;	
			}
    }
		if (argc == 4)
		{
			if (!strcmp(argv[1], "zero"))
			{
				uint8_t ch=atoi(argv[2]);
				if(ch==1)
				{
					VoltageADC1_CAL.zero=atof(argv[3]);
					at24cxx.write(at24c256 , (EEPROM_ADC_CAL_PAGE_ADDR)*64, (uint8_t *)&VoltageADC1_CAL, sizeof(AdcCalibration_T));

				}
				else if(ch==2)
				{
					VoltageADC2_CAL.zero=atof(argv[3]);
					at24cxx.write(at24c256 , (EEPROM_ADC_CAL_PAGE_ADDR+1)*64, (uint8_t *)&VoltageADC2_CAL, sizeof(AdcCalibration_T));

				}			
			}
			 return RT_EOK;	
		}
    if (argc == 5)
    {	
			if (!strcmp(argv[1], "set")) 
			{	
				uint8_t ch=atoi(argv[2]);
				
				float k=atof(argv[3]);
				float b=atof(argv[4]);
				
				if(ch==1)
				{					
					VoltageADC1_CAL.gain[0]=k;
					VoltageADC1_CAL.offset[0]=b;
					at24cxx.write(at24c256 , (EEPROM_ADC_CAL_PAGE_ADDR)*64, (uint8_t *)&VoltageADC1_CAL, sizeof(AdcCalibration_T));
				}
				else if(ch==2)
				{
					VoltageADC2_CAL.gain[0]=k;
					VoltageADC2_CAL.offset[0]=b;	
					at24cxx.write(at24c256 , (EEPROM_ADC_CAL_PAGE_ADDR+1)*64, (uint8_t *)&VoltageADC2_CAL, sizeof(AdcCalibration_T));
				}
			  rt_kprintf("adc set cal ok\n");	
        return RT_EOK;	
			}
    }
    if (argc == 6)
    {	
			if (!strcmp(argv[1], "set")) 
			{	
				uint8_t ch=atoi(argv[2]);
				uint8_t range=atoi(argv[3])-1;
				
				float k=atof(argv[4]);
				float b=atof(argv[5]);
				
				if(ch==1)
				{					
					VoltageADC1_CAL.gain[range]=k;
					VoltageADC1_CAL.offset[range]=b;
					at24cxx.write(at24c256 , (EEPROM_ADC_CAL_PAGE_ADDR)*64, (uint8_t *)&VoltageADC1_CAL, sizeof(AdcCalibration_T));
				}
				else if(ch==2)
				{
					VoltageADC2_CAL.gain[range]=k;
					VoltageADC2_CAL.offset[range]=b;	
					at24cxx.write(at24c256 , (EEPROM_ADC_CAL_PAGE_ADDR+1)*64, (uint8_t *)&VoltageADC2_CAL, sizeof(AdcCalibration_T));
				}
			  rt_kprintf("adc set rang cal ok\n");	
        return RT_EOK;	
			}
    }
		return RT_ERROR;
}
//
MSH_CMD_EXPORT(adc, ......);

#if 	( DBG_ENABLE )

uint16_t readADC(uint8_t adcNo, uint8_t times)
{     
	for(uint8_t i=0;i<times;i++)
	{
		if(adcNo==1) 			
		rt_kprintf("adc%dREG=%d\n",adcNo,GetADCRegVal(1));
	
		if(adcNo==2) 			
		rt_kprintf("adc%dREG=%d\n",adcNo,GetADCRegVal(2);
	}
}

FINSH_FUNCTION_EXPORT(readADC, ......);

#endif
//


