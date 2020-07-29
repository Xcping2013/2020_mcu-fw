#include "platform_drivers.h"
#include "inc_fbuirmea_04.h"

#if 1
	#define DBG_ENABLE	1
	#if 	( DBG_ENABLE )
	#define DBG_TRACE		rt_kprintf
	#else
	#define DBG_TRACE(...)		
#endif

#endif


char version_string[]="ver1.0.0";

uint8_t LED_PIN=PB_2;

at24cxx_t at24c256=
{
	{0},
	{PC_14,PC_15},	
	0xA2,	
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
  
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**SPI1 GPIO Configuration    
    PB3     ------> SPI1_SCK
    PB4     ------> SPI1_MISO
    PB5     ------> SPI1_MOSI 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		__HAL_AFIO_REMAP_SPI1_ENABLE();
		
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
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5);

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
#define ad7124_cs_pin		PB_7
#define ad7124_sync_pin	PB_8
#define ad7124_psw_pin	PB_6

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


