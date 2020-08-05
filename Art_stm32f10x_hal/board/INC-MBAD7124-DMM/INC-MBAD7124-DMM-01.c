#include "platform_drivers.h"
#include "INC-MBAD7124-DMM-01.h"

#if 1
	#define DBG_ENABLE	1
	#if 	( DBG_ENABLE )
	#define DBG_TRACE		rt_kprintf
	#else
	#define DBG_TRACE(...)		
#endif

#endif

/*			CMDS

dmm get data DMM_FUNC_DCV_100mV
dmm get offset DMM_FUNC_DCV_100mV
dmm get gain DMM_FUNC_DCV_100mV 
dmm set gain DMM_FUNC_DCV_100mV 1.1118 
dmm set offset DMM_FUNC_DCV_100mV -5.589

dmm get data DMM_FUNC_DCV_10V
dmm get offset DMM_FUNC_DCV_10V
dmm get gain DMM_FUNC_DCV_10V 
dmm set gain DMM_FUNC_DCV_10V 	1.0646
dmm set offset DMM_FUNC_DCV_10V -0.0056

dmm get data DMM_FUNC_DCI_100mA
dmm get offset DMM_FUNC_DCI_100mA
dmm get gain DMM_FUNC_DCI_100mA 
dmm set gain DMM_FUNC_DCI_100mA 	1.0593
dmm set offset DMM_FUNC_DCI_100mA -0.1496

dmm get data DMM_FUNC_DCI_500mA
dmm get offset DMM_FUNC_DCI_500mA
dmm get gain DMM_FUNC_DCI_500mA 
dmm set gain DMM_FUNC_DCI_500mA 	1.0601
dmm set offset DMM_FUNC_DCI_500mA -0.1904

dmm get data DMM_FUNC_OHM_10R_4W
dmm get offset DMM_FUNC_OHM_10R_4W
dmm get gain DMM_FUNC_OHM_10R_4W 
dmm set gain DMM_FUNC_OHM_10R_4W 	1.0701
dmm set offset DMM_FUNC_OHM_10R_4W 0.4664



*/

char version_string[]="INC-MBAD7124-DMM Ver1.0.0";

uint8_t LED_PIN=PB_4;

at24cxx_t at24c256=
{
	{0},
	{PB_6,PB_5},	
	0xA0,	
};

static uint32_t StreamDataCNT=0;
static rt_uint8_t StreamDataStack[ 512 ];
static struct	rt_thread StreamDataThread;
//
int StreamData_thread_init(void);

#if 1
SPI_HandleTypeDef hspi1;
static void MX_SPI1_Init(void);
/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
//AD7124 MODE0 | MODE3
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */
	
  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;	//72/128=562.5KHZ
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hspi->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspInit 0 */

  /* USER CODE END SPI1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**SPI1 GPIO Configuration    
    PB5     ------> SPI1_SCK
    PB6     ------> SPI1_MISO
    PB7     ------> SPI1_MOSI 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		
    /* SPI1 interrupt Init */
//    HAL_NVIC_SetPriority(SPI1_IRQn, 5, 0);
//    HAL_NVIC_EnableIRQ(SPI1_IRQn);
  /* USER CODE BEGIN SPI1_MspInit 1 */

  /* USER CODE END SPI1_MspInit 1 */
  }
}
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{

  if(hspi->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspDeInit 0 */

  /* USER CODE END SPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();
  
    /**SPI1 GPIO Configuration    
    PB3     ------> SPI1_SCK
    PB4     ------> SPI1_MISO
    PB5     ------> SPI1_MOSI 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);

    /* SPI1 interrupt DeInit */
    //HAL_NVIC_DisableIRQ(SPI1_IRQn);
  /* USER CODE BEGIN SPI1_MspDeInit 1 */

  /* USER CODE END SPI1_MspDeInit 1 */
  }
}
#endif
//

int main(void)
{

    rt_pin_mode(LED_PIN, PIN_MODE_OUTPUT);
		rt_pin_write(LED_PIN, PIN_HIGH);
    while (1)
    {
        rt_pin_write(LED_PIN, PIN_HIGH);
        rt_thread_mdelay(1000);
        rt_pin_write(LED_PIN, PIN_LOW);
        rt_thread_mdelay(1000);
    }
    return RT_EOK;
}
//MB1616DEV6 DEV0			SPI1	PB4-6		AD7124->614KHZ
#define ad7124_cs_pin		PA_4
#define ad7124_sync_pin	PB_0
#define ad7124_psw_pin	PB_1

void ad7124_spi_gpio_init(void)
{
	pinMode(ad7124_cs_pin,PIN_MODE_OUTPUT);	
	pinMode(ad7124_sync_pin,PIN_MODE_OUTPUT);
	pinMode(ad7124_psw_pin,PIN_MODE_OUTPUT);
	pinSet(ad7124_sync_pin);
	pinSet(ad7124_cs_pin);
	MX_SPI1_Init();	
}
void ad7124_spi_readwrite(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size,uint32_t Timeout)
{
		rt_pin_write(ad7124_cs_pin,0);
		
		HAL_SPI_TransmitReceive(&hspi1, pTxData, pRxData, Size,Timeout);
		
		rt_pin_write(ad7124_cs_pin,1);
}
/****************************************mbkbcn1  init  all**********************************************************/
int mbad7124_hw_init(void)
{		
	//board.c	rcc_set
	rt_kprintf("\n%s build at %s %s\n\n", version_string, __TIME__, __DATE__);
			
	delay_ms(10);
	
	at24cxx_hw_init();
		
	dmm_init();
	
	rt_kprintf("controller init......[ok]\n");

	rt_kprintf("\nyou can type help to list commands and all commands should end with \\r\\n\n");
	
	rt_kprintf(">>");
	
	StreamData_thread_init();
	
  return 0;
}

/*
*/
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
		if(StreamDataCNT )
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
INIT_APP_EXPORT(mbad7124_hw_init);






//


//


