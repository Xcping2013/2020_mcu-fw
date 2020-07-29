#include "bsp_mcu_gpio.h"
#include "bsp_mcu_delay.h"
#include "app_eeprom_24xx.h"

#include "ADS1220.h"
#include "dmm.h"


#if 1
	#define DBG_ENABLE	1
	#if 	( DBG_ENABLE )
	#define DBG_TRACE		rt_kprintf
	#else
	#define DBG_TRACE(...)		
#endif

#endif

#define ADS1220_CS0_PIN  PA_4
#define ADS1220_RDRY_PIN PB_12
#define LED_PIN	PA_14

const float avg_factor = 0.8;

char version_string[]="FW:inc-mbads1220-4w-r ver1.0.0";

uint8_t RdcRang=DMM_FUNC_OHM_10R_4W;

static uint32_t StreamDataCNT=0;
static rt_uint8_t StreamDataStack[ 512 ];
static struct	rt_thread StreamDataThread;
//
int StreamData_thread_init(void);

at24cxx_t at24c256=
{
	{0},
	{PB_14,PB_13},	
	0xA0,	
};

int main(void)
{
    int count = 1;

    rt_pin_mode(LED_PIN, PIN_MODE_OUTPUT);
		rt_pin_write(LED_PIN, PIN_HIGH);
    while (count++)
    {
        rt_pin_write(LED_PIN, PIN_HIGH);
        rt_thread_mdelay(1000);
        rt_pin_write(LED_PIN, PIN_LOW);
        rt_thread_mdelay(1000);
    }
    return RT_EOK;
}
//
/****************************************mbkbcn1  init  all**********************************************************/
int mbads1220_4w_hw_init(void)
{		
	//board.c	rcc_set
	rt_kprintf("\n%s build at %s %s\n\n", version_string, __TIME__, __DATE__);
			
	at24cxx_hw_init();
	
	ADS1220_Init();
	
		
	rt_kprintf("controller init......[ok]\n");

	rt_kprintf("\nyou can type help to list commands and all commands should end with \\r\\n\n");
	
	rt_kprintf(">>");
	StreamData_thread_init();
  return 0;
}
#if 1	//CUBE SPI Init

SPI_HandleTypeDef hspi1;

void MX_SPI1_Init(void)
{
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;//SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}
void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspInit 0 */

  /* USER CODE END SPI1_MspInit 0 */
    /* SPI1 clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**SPI1 GPIO Configuration    
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN SPI1_MspInit 1 */

  /* USER CODE END SPI1_MspInit 1 */
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspDeInit 0 */

  /* USER CODE END SPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();
  
    /**SPI1 GPIO Configuration    
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);

  /* USER CODE BEGIN SPI1_MspDeInit 1 */

  /* USER CODE END SPI1_MspDeInit 1 */
  }

} 




#endif

/*

*/
#define PGA 	2                                     // Programmable Gain = 1
#define VREF 	2.048                                   // Internal reference of 2.048V
#define VFSR 	VREF/PGA             
#define FSR 	(((long int)1<<23)) 
#define LSB_Size (VFSR/FSR) 

uint8_t New_data_avialable;
//InterruptIn DRDY(PC_5);	//设置外部中断
void ext_int_DRDY(void);

void ADS1220_Init(void)
{
	MX_SPI1_Init();
	
	pinMode(ADS1220_CS0_PIN,PIN_MODE_OUTPUT);
	pinSet(ADS1220_CS0_PIN);
	
	pinMode(ADS1220_RDRY_PIN,PIN_MODE_INPUT);
	
	Cal_EEPROMToRam();
	
	delay_ms(10);
	//1R*1.5mA=1.5mV*128=192mV
	//AIN0-AIN1	ADS1220_GAIN_128	ADS1220_DR_20|ADS1220_CC	ADS1220_VREF_INT|ADS1220_REJECT_BOTH  ADS1220_IDAC1_AIN3
	ADS1220_SendResetCommand();
	delay_ms(10);
	ADS1220_Config();
	delay_ms(10);
	ADS1220_SendStartCommand();
	
}


void ADS1220_AssertCS( int fAssert)
{
   if (fAssert)	pinReset(ADS1220_CS0_PIN);
   else					pinSet(ADS1220_CS0_PIN);

      
}
uint8_t ADS1220_ReadWriteByte(unsigned char Value)
{
	uint8_t Result=0;
//	pinReset(ADS1220_CS0_PIN);
  HAL_SPI_TransmitReceive(&hspi1, &Value, &Result, 1,1000);
//	pinSet(ADS1220_CS0_PIN);
	return Result;
}
//
int ADS1220_ReadData(void)
{
  UCHAR Write[4], Read[4];
	
	int Result;
	
  Write[0]=0x00;//ADS1220_CMD_RDATA;
  Write[1]=0x00;
  Write[2]=0x00;
  Write[3]=0x00;
   
   // assert CS to start transfer
  ADS1220_AssertCS(1);

	HAL_SPI_TransmitReceive(&hspi1, Write, Read, 3,1000);     
   // get the conversion result
	ADS1220_AssertCS(0);

	DBG_TRACE("Read=0x%x,0x%x,0x%x,0x%x\n",Read[0],Read[1],Read[2],Read[3]);
  Result=(Read[0]<<16)|(Read[1]<<8)|(Read[2]);
	
  if(Result & 0x00800000) Result|=0xff000000;
  
	return Result;
}
void ext_int_DRDY(void)
{
  New_data_avialable = 1;
}
float GetADCReg_FilterVal(uint8_t times)
{
	uint32_t max,min,temp;
	
	uint32_t sum=0; 
	
	float aver=0.0;
	
	static uint8_t getOnce=0;

	if(	times>1)
	{
		for(uint8_t t=0;t<times+2;t++)
		{
			temp=ADS1220_ReadData();
			
			if(getOnce==0) {getOnce=1;max=min=temp;}
			
			if(max<temp) 	max=temp;
			if(min>temp) 	min=temp;
			
			sum=sum+temp;		
		}	
		aver=(float)(sum-max-min)/times;	
	}
	else 
	{
		aver=ADS1220_ReadData();
	}
	getOnce=0;
	return aver;
} 

float ConvertAdcToVolt(int adcVal)
{
   float Vout = 0;
   Vout = (float)(adcVal*LSB_Size*1000);    //In  mV
   return Vout;
}
int ReadRdc(int argc, char **argv)
{
	uint8_t result = REPLY_OK;
	
	if (argc ==1 )
	{
		//打印一个数据或者一直打印
		return RT_EOK;	
	}
		
	if (argc ==2)
	{
		//打印N个数据
		StreamDataCNT=atoi(argv[1]);
		return RT_EOK;			
	}
	if (argc ==3)
	{
		//选择设定的档位来采集 N个数据
		if (!strcmp(argv[1], "1R"))					RdcRang=DMM_FUNC_OHM_10R_4W;
		else if (!strcmp(argv[1], "100R"))	RdcRang=DMM_FUNC_OHM_10K_4W;
		else if (!strcmp(argv[1], "10K"))		RdcRang=DMM_FUNC_OHM_1M_4W;
		
		StreamDataCNT=atoi(argv[2]);	
		
		return RT_EOK;	
	}
	else
	{
		rt_kprintf("Usage: \n");
		rt_kprintf("ReadRdc\t\t---read data once\n");
		rt_kprintf("ReadRdc <times>\t\t---read data times\n");
		rt_kprintf("ReadRdc <rang> <times>---read data times base on rang\t\t\n");
		rt_kprintf("    rang:1R|100R|10K\n");			
		result = REPLY_INVALID_CMD;
	}	
	return result;
}

static void StreamData_Thread_entry(void *parameter)
{
	static int time_start=0;
	static int time_stop=0;
	static uint8_t time_once=0;
	
	float RdcCaledVal=0;
	
	char fAdcVal[10]="";
	
	uint16_t timeout=0; 
	
	while (1)
	{  
		if(StreamDataCNT && ( pinRead(ADS1220_RDRY_PIN)==0))
		{			
				if(time_once==0)	
				{			
					for(uint8_t i=0;i<100;i++)
					{
					}				
					time_once=1;time_start=HAL_GetTick();
					timeout=0;
					DBG_TRACE("start time:%dms\n",HAL_GetTick());
				}				
//				while(pinRead(ADS1220_RDRY_PIN)) 
//				{
//					timeout++;
//					if(timeout==2000)
//					{
//						break;
//					}
//				}
//				RdcCaledVal=ConvertAdcToVolt(GetADCReg_FilterVal(10)*1000)/1.5*DMM.KTable[DMM_FUNC_OHM_10R_4W]+DMM.CTable[DMM_FUNC_OHM_10R_4W];

//				RdcCaledVal=ConvertAdcToVolt(ADS1220_ReadData( ))*1000/1.5;

				RdcCaledVal=ConvertAdcToVolt(ADS1220_ReadData( ));

				
				sprintf(fAdcVal,"%.3f",RdcCaledVal);	rt_kprintf("%smV\n",fAdcVal);				
//				rt_kprintf("%smOHM\n",fAdcVal);	
				
				StreamDataCNT--;
								
				if(StreamDataCNT==0)	
				{
					time_once=0;
					time_stop=HAL_GetTick();
					
					DBG_TRACE("stop time:%dms,spent:%dms",time_stop,abs(time_stop-time_start));
					rt_kprintf("\n>>");
				}
				rt_hw_us_delay(10);	
		}
		else rt_thread_mdelay(5);		
	}		
}
int StreamData_thread_init(void)
{
    rt_err_t result;

    result = rt_thread_init(&StreamDataThread,
                            "getRes",
                            StreamData_Thread_entry,
                            RT_NULL,
                            (rt_uint8_t *)&StreamDataStack[0],
                            sizeof(StreamDataStack),
                            15,
                            20);
    if (result == RT_EOK)
    {
       rt_thread_startup(&StreamDataThread);
    }
    return 0;
}

//
INIT_APP_EXPORT(mbads1220_4w_hw_init);

MSH_CMD_EXPORT(ReadRdc, ......);




