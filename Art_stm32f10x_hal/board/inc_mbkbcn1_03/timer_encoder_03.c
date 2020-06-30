/*
����ABZ AB�໥�ӳ� 90�� ��������ת  ȡA��B���������½��ؿ��Խ���2��4��Ƶ		Z��λ��Ȧ���� ÿȦһ������	�ⲿIO�ж�
�ߣ�������ÿתһȦ�����������	�������� = �� x �ն������ 
TIME1 || TIME8 ����֪ͨ����A B�� ͨ�ö�ʱ���ֱ�A B�����

��ȡ�ı�����ֵ���ۻ����Ǳ�������ȡ������ ���ǵ�����б�����ɵ�Ӱ�죿
*/

#include "timer_encoder_03.h"

#if 1
	#define DBG_ENABLE	0
	#if 	( DBG_ENABLE )
	#define DBG_TRACE		rt_kprintf
	#else
	#define DBG_TRACE(...)		
#endif

#endif


/* 
������������������õ�Prescaler = 3 �� =��ӡ�õ��ı�����ֵ/4  ��ӡ�����������ڳ����������ƫ��账��
�ٶ������ú��� ��������õĲ����������� ��������쳣 ��������ֵ��
�ж�callback sys --> HAL_TIM_PeriodElapsedCallback
*/


int16_t OverflowCount=0;
__IO int32_t CaptureNumber=0;       // ���벶����

TIM_HandleTypeDef htim3;

//Timer CubeMx Hal init		Timer2_CH1 A		Timer2_CH2 B
#if 1								
/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 3;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 3;
  if (HAL_TIM_Encoder_Init(&htim3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}
/**
* @brief TIM_Encoder MSP Initialization
* This function configures the hardware resources used in this example
* @param htim_encoder: TIM_Encoder handle pointer
* @retval None
*/
void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef* htim_encoder)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(htim_encoder->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspInit 0 */

  /* USER CODE END TIM3_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_TIM3_CLK_ENABLE();
  
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**TIM3 GPIO Configuration    
    PB4     ------> TIM3_CH1
    PB5     ------> TIM3_CH2 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    __HAL_AFIO_REMAP_TIM3_PARTIAL();

  /* USER CODE BEGIN TIM3_MspInit 1 */
	  HAL_NVIC_SetPriority(TIM3_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
  /* USER CODE END TIM3_MspInit 1 */
  }

}

/**
* @brief TIM_Encoder MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param htim_encoder: TIM_Encoder handle pointer
* @retval None
*/
void HAL_TIM_Encoder_MspDeInit(TIM_HandleTypeDef* htim_encoder)
{
  if(htim_encoder->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspDeInit 0 */

  /* USER CODE END TIM3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM3_CLK_DISABLE();
  
    /**TIM3 GPIO Configuration    
    PB4     ------> TIM3_CH1
    PB5     ------> TIM3_CH2 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_4|GPIO_PIN_5);

  /* USER CODE BEGIN TIM3_MspDeInit 1 */
		HAL_NVIC_DisableIRQ(TIM3_IRQn);
  /* USER CODE END TIM3_MspDeInit 1 */
  }

}
void TIM3_IRQHandler(void)
{
 	rt_interrupt_enter();
  HAL_TIM_IRQHandler(&htim3);
	rt_interrupt_leave();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim3))
	{
    OverflowCount--;       //���¼������
	}
  else
	{
    OverflowCount++;       //���ϼ������
	}
}

#endif
//APP
void Timer_EncoderInit(void)
{
	MX_TIM3_Init();
	
	__HAL_TIM_SET_COUNTER(&htim3,0);
  __HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE);  				//��������жϱ�־λ
  __HAL_TIM_URS_ENABLE(&htim3);              				  //���������������Ų��������ж�
	
//	__HAL_TIM_DISABLE_IT(&htim3,TIM_IT_TRIGGER);
//	__HAL_TIM_DISABLE_IT(&htim3,TIM_IT_BREAK);
//	__HAL_TIM_DISABLE_IT(&htim3,TIM_IT_COM);
//	__HAL_TIM_DISABLE_IT(&htim3,TIM_IT_CC1);
//	__HAL_TIM_DISABLE_IT(&htim3,TIM_IT_CC2);
//	__HAL_TIM_DISABLE_IT(&htim3,TIM_IT_CC3);
//	__HAL_TIM_DISABLE_IT(&htim3,TIM_IT_CC4);

  __HAL_TIM_ENABLE_IT(&htim3,TIM_IT_UPDATE); 				  //ʹ�ܸ����ж�

	HAL_TIM_Encoder_Start_IT(&htim3, TIM_CHANNEL_ALL);
}
int encoder(int argc, char **argv)
{
    if (argc == 1)
    {	
			  rt_kprintf("Usage: \n");
			  rt_kprintf("encoder clear\n");
				rt_kprintf("encoder get\n");
        return RT_ERROR;
    }
    if (argc == 2)
    {	
			if (!strcmp(argv[1], "clear")) 
			{	
				HAL_TIM_Encoder_Stop_IT(&htim3, TIM_CHANNEL_ALL);
				OverflowCount=0;
				CaptureNumber=0;
				__HAL_TIM_SET_COUNTER(&htim3,0);
	
				__HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE);  				//��������жϱ�־λ
				__HAL_TIM_URS_ENABLE(&htim3);              				  //���������������Ų��������ж�
				__HAL_TIM_ENABLE_IT(&htim3,TIM_IT_UPDATE); 				  //ʹ�ܸ����ж�

				HAL_TIM_Encoder_Start_IT(&htim3, TIM_CHANNEL_ALL);
				
        return RT_EOK;	
			}
			if (!strcmp(argv[1], "get")) 
			{	
				CaptureNumber = OverflowCount*65535 + __HAL_TIM_GET_COUNTER(&htim3);
				rt_kprintf("encode CaptureNumber=%ld\n",CaptureNumber);
				
        return RT_EOK;	
			}
    }
		return RT_ERROR;
}
MSH_CMD_EXPORT(encoder,...);

//
//
//
/*********************************** Ƶ�ʲɼ�*����һ��C�ļ�*************************************************/
#if 	( 0 )

uint32_t               uwIC2Value1 = 0;
uint32_t               uwIC2Value2 = 0;
uint32_t               uwDiffCapture = 0;
/* Capture index */
uint16_t               uhCaptureIndex = 0;
/* Frequency Value */
uint32_t               uwFrequency = 0;
uint32_t               uwPulseCNT = 0;

void getFrequency(void)
{
	rt_kprintf("Frequency=%ld\n",uwFrequency*2);
	rt_kprintf("PulseCNT=%ld\n",uwPulseCNT);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
  {
		uwPulseCNT++;
		
    if(uhCaptureIndex == 0)
    {
      /* Get the 1st Input Capture value */
      uwIC2Value1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
      uhCaptureIndex = 1;
    }
    else if(uhCaptureIndex == 1)
    {
      /* Get the 2nd Input Capture value */
      uwIC2Value2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4); 

      /* Capture computation */
      if (uwIC2Value2 > uwIC2Value1)
      {
        uwDiffCapture = (uwIC2Value2 - uwIC2Value1); 
      }
      else if (uwIC2Value2 < uwIC2Value1)
      {
        /* 0xFFFF is max TIM2_CCRx value */
        uwDiffCapture = ((0xFFFF - uwIC2Value1) + uwIC2Value2) + 1;
      }
      else
      {
        /* If capture values are equal, we have reached the limit of frequency
           measures */
        Error_Handler();
      }
      /* Frequency computation: for this example TIMx (TIM2) is clocked by
         APB1Clk */      
      uwFrequency = HAL_RCC_GetPCLK1Freq() / uwDiffCapture;
      uhCaptureIndex = 0;
    }
  }
}


MSH_CMD_EXPORT(encode_capture_init,...);
MSH_CMD_EXPORT(getFrequency,...);

#endif


/*********************************** �������������**************************************************
���������ã�					200*8=1600	Pulse/round

TIM Encoder���ã�			UP-65536		Prescaler = 0
TIM Encoder��ȡֵ��		4000

���������������	
����42HSM08:					Prescaler = 0 1600 motor PulseOut = 4000 encoder in
											Prescaler = 3 1600 motor PulseOut = 1000 encoder in

���������������������  encode read = CaptureNumber*16/10

ʹ��TIM1��ȡ������ ֻ�ܵ��� ��������  TIME2 ����
*********************************************************************************************************/
//
//
//



