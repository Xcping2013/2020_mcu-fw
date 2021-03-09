
#include "inc_mbcooler.h"
#include "app_eeprom_24xx.h"

#include "tca6424a.h"	

ADC_HandleTypeDef hadc1;

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart1;		//config by rtt
UART_HandleTypeDef huart2;

#define TEMP_ADC_TAB_LEN 					71
#define EEPROM_Cooler_PAGE_ADDR		120 
#define CoolerSaved								'Y'

typedef struct	
{
	uint8_t saved;
	uint32_t degreecSaved;
	uint32_t timerSaved;
}
CoolerDataSaved_T;	

CoolerDataSaved_T CoolerDegreec_Time=
{
	CoolerSaved,
	250,
	300,
};

char version_string[]="Ver2.0";

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);


//
uint16_t ADC_GetNTCAdcVal(uint32_t ch);
uint16_t ADC_GetNTCAdcValAverage(uint32_t ch,uint8_t times);
void TIM_PWM_SetDigital(uint8_t pwm_ch, uint32_t DVal);
uint16_t getCoolerTemFromNTC(uint8_t channel);
void setCoolerTemFromPWM(uint8_t channel, uint16_t Degreec);
void SaveCoolerTemToEEPROM(uint8_t channel, uint16_t Degreec);
void getCoolerTemFromEEPROM(uint8_t channel);
int CoolerThreadInit(void);

/**
  * @brief  The application entry point.
  * @retval int
  */
int inc_mbcooler_init(void)
{

//  HAL_Init();

//  SystemClock_Config();

  MX_GPIO_Init();
	
	at24cxx_hw_init();
	
  MX_ADC1_Init();
	
	HAL_ADCEx_Calibration_Start(&hadc1);	
	
  MX_TIM3_Init();
  MX_TIM4_Init();
//  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
	
	rt_kprintf("\nFW-RTT-MB1616dDEV6 %s build at %s %s\n\n", version_string, __TIME__, __DATE__);

	getCoolerTemFromEEPROM(255);
	setCoolerTemFromPWM(255,CoolerDegreec_Time.degreecSaved);
	
	CoolerThreadInit();
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV8;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

#if 1																													//adc
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
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

//
#endif
#if 1																													//tim pwm
/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

//


#endif

#if 1																													//uart 
/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

#endif

#if 1																													//gpio
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_GREEN1_Pin|GPIO_RED1_Pin|LED0_Pin|RX_LED_Pin
                          |TX_LED_Pin|SCL_Pin|SDA_Pin|GPIO_RED2_Pin
                          |GPIO_YELLOW1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, DE_Pin|GPIO_OUT5_Pin|GPIO_FAN6_Pin|GPIO_FAN5_Pin
                          |GPIO_FAN4_Pin|GPIO_FAN3_Pin|GPIO_RED4_Pin|GPIO_YELLOW3_Pin
                          |GPIO_GREEN3_Pin|GPIO_RED3_Pin|GPIO_OUT1_Pin|GPIO_YELLOW2_Pin
                          |GPIO_GREEN2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_FAN2_Pin|GPIO_FAN1_Pin|GPIO_OUT4_Pin|GPIO_LOCK6_Pin
                          |GPIO_LOCK5_Pin|GPIO_LOCK4_Pin|GPIO_YELLOW5_Pin|GPIO_GREEN5_Pin
                          |GPIO_RED5_Pin|GPIO_OUT2_Pin|GPIO_YELLOW4_Pin|GPIO_GREEN4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_LOCK3_Pin|GPIO_LOCK2_Pin|GPIO_LOCK1_Pin|GPIO_OUT3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_YELLOW6_Pin|GPIO_GREEN6_Pin|GPIO_RED6_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : GPIO_GREEN1_Pin GPIO_RED1_Pin LED0_Pin RX_LED_Pin
                           TX_LED_Pin SCL_Pin SDA_Pin GPIO_RED2_Pin
                           GPIO_YELLOW1_Pin */
  GPIO_InitStruct.Pin = GPIO_GREEN1_Pin|GPIO_RED1_Pin|LED0_Pin|RX_LED_Pin
                          |TX_LED_Pin|SCL_Pin|SDA_Pin|GPIO_RED2_Pin
                          |GPIO_YELLOW1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : DUT_IN1_Pin DUT_IN2_Pin DUT_IN3_Pin A4_Pin
                           A3_Pin A2_Pin A1_Pin */
  GPIO_InitStruct.Pin = DUT_IN1_Pin|DUT_IN2_Pin|DUT_IN3_Pin|A4_Pin
                          |A3_Pin|A2_Pin|A1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : DUT_IN4_Pin DUT_IN5_Pin DUT_IN6_Pin LID_IN1_Pin
                           LID_IN2_Pin LID_IN3_Pin LID_IN4_Pin */
  GPIO_InitStruct.Pin = DUT_IN4_Pin|DUT_IN5_Pin|DUT_IN6_Pin|LID_IN1_Pin
                          |LID_IN2_Pin|LID_IN3_Pin|LID_IN4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LID_IN5_Pin LID_IN6_Pin */
  GPIO_InitStruct.Pin = LID_IN5_Pin|LID_IN6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : DET_Pin A0_Pin */
  GPIO_InitStruct.Pin = DET_Pin|A0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : DE_Pin GPIO_OUT5_Pin GPIO_FAN6_Pin GPIO_FAN5_Pin
                           GPIO_FAN4_Pin GPIO_FAN3_Pin GPIO_RED4_Pin GPIO_YELLOW3_Pin
                           GPIO_GREEN3_Pin GPIO_RED3_Pin GPIO_OUT1_Pin GPIO_YELLOW2_Pin
                           GPIO_GREEN2_Pin */
  GPIO_InitStruct.Pin = DE_Pin|GPIO_OUT5_Pin|GPIO_FAN6_Pin|GPIO_FAN5_Pin
                          |GPIO_FAN4_Pin|GPIO_FAN3_Pin|GPIO_RED4_Pin|GPIO_YELLOW3_Pin
                          |GPIO_GREEN3_Pin|GPIO_RED3_Pin|GPIO_OUT1_Pin|GPIO_YELLOW2_Pin
                          |GPIO_GREEN2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : GPIO_FAN2_Pin GPIO_FAN1_Pin GPIO_OUT4_Pin GPIO_LOCK6_Pin
                           GPIO_LOCK5_Pin GPIO_LOCK4_Pin GPIO_YELLOW5_Pin GPIO_GREEN5_Pin
                           GPIO_RED5_Pin GPIO_OUT2_Pin GPIO_YELLOW4_Pin GPIO_GREEN4_Pin */
  GPIO_InitStruct.Pin = GPIO_FAN2_Pin|GPIO_FAN1_Pin|GPIO_OUT4_Pin|GPIO_LOCK6_Pin
                          |GPIO_LOCK5_Pin|GPIO_LOCK4_Pin|GPIO_YELLOW5_Pin|GPIO_GREEN5_Pin
                          |GPIO_RED5_Pin|GPIO_OUT2_Pin|GPIO_YELLOW4_Pin|GPIO_GREEN4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : GPIO_LOCK3_Pin GPIO_LOCK2_Pin GPIO_LOCK1_Pin GPIO_OUT3_Pin */
  GPIO_InitStruct.Pin = GPIO_LOCK3_Pin|GPIO_LOCK2_Pin|GPIO_LOCK1_Pin|GPIO_OUT3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : GPIO_YELLOW6_Pin GPIO_GREEN6_Pin GPIO_RED6_Pin */
  GPIO_InitStruct.Pin = GPIO_YELLOW6_Pin|GPIO_GREEN6_Pin|GPIO_RED6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

#endif
#if 1 																												//stm32f1xx_hal_msp.c
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
                                        /**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  /* System interrupt init*/

  /** NOJTAG: JTAG-DP Disabled and SW-DP Enabled
  */
  __HAL_AFIO_REMAP_SWJ_NOJTAG();

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
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

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**ADC1 GPIO Configuration
    PA0-WKUP     ------> ADC1_IN0
    PA3     ------> ADC1_IN3
    PA4     ------> ADC1_IN4
    PA5     ------> ADC1_IN5
    PA6     ------> ADC1_IN6
    PA7     ------> ADC1_IN7
    PC4     ------> ADC1_IN14
    */
    GPIO_InitStruct.Pin = VDC_IN_Pin|VSNS5_Pin|VSNS4_Pin|VSNS3_Pin
                          |VSNS2_Pin|VSNS1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = VSNS0_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(VSNS0_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN ADC1_MspInit 1 */

  /* USER CODE END ADC1_MspInit 1 */
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
    PA0-WKUP     ------> ADC1_IN0
    PA3     ------> ADC1_IN3
    PA4     ------> ADC1_IN4
    PA5     ------> ADC1_IN5
    PA6     ------> ADC1_IN6
    PA7     ------> ADC1_IN7
    PC4     ------> ADC1_IN14
    */
    HAL_GPIO_DeInit(GPIOA, VDC_IN_Pin|VSNS5_Pin|VSNS4_Pin|VSNS3_Pin
                          |VSNS2_Pin|VSNS1_Pin);

    HAL_GPIO_DeInit(VSNS0_GPIO_Port, VSNS0_Pin);

  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }

}

/**
* @brief TIM_PWM MSP Initialization
* This function configures the hardware resources used in this example
* @param htim_pwm: TIM_PWM handle pointer
* @retval None
*/
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* htim_pwm)
{
  if(htim_pwm->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspInit 0 */

  /* USER CODE END TIM3_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_TIM3_CLK_ENABLE();
  /* USER CODE BEGIN TIM3_MspInit 1 */

  /* USER CODE END TIM3_MspInit 1 */
  }
  else if(htim_pwm->Instance==TIM4)
  {
  /* USER CODE BEGIN TIM4_MspInit 0 */

  /* USER CODE END TIM4_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_TIM4_CLK_ENABLE();
  /* USER CODE BEGIN TIM4_MspInit 1 */

  /* USER CODE END TIM4_MspInit 1 */
  }

}

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(htim->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspPostInit 0 */

  /* USER CODE END TIM3_MspPostInit 0 */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**TIM3 GPIO Configuration
    PC6     ------> TIM3_CH1
    PC7     ------> TIM3_CH2
    PC8     ------> TIM3_CH3
    PC9     ------> TIM3_CH4
    */
    GPIO_InitStruct.Pin = PWM2_Pin|PWM3_Pin|PWM4_Pin|PWM5_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    __HAL_AFIO_REMAP_TIM3_ENABLE();

  /* USER CODE BEGIN TIM3_MspPostInit 1 */

  /* USER CODE END TIM3_MspPostInit 1 */
  }
  else if(htim->Instance==TIM4)
  {
  /* USER CODE BEGIN TIM4_MspPostInit 0 */

  /* USER CODE END TIM4_MspPostInit 0 */

    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**TIM4 GPIO Configuration
    PD12     ------> TIM4_CH1
    PD13     ------> TIM4_CH2
    */
    GPIO_InitStruct.Pin = PWM0_Pin|PWM1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    __HAL_AFIO_REMAP_TIM4_ENABLE();

  /* USER CODE BEGIN TIM4_MspPostInit 1 */

  /* USER CODE END TIM4_MspPostInit 1 */
  }

}
/**
* @brief TIM_PWM MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param htim_pwm: TIM_PWM handle pointer
* @retval None
*/
void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef* htim_pwm)
{
  if(htim_pwm->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspDeInit 0 */

  /* USER CODE END TIM3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM3_CLK_DISABLE();
  /* USER CODE BEGIN TIM3_MspDeInit 1 */

  /* USER CODE END TIM3_MspDeInit 1 */
  }
  else if(htim_pwm->Instance==TIM4)
  {
  /* USER CODE BEGIN TIM4_MspDeInit 0 */

  /* USER CODE END TIM4_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM4_CLK_DISABLE();
  /* USER CODE BEGIN TIM4_MspDeInit 1 */

  /* USER CODE END TIM4_MspDeInit 1 */
  }

}

/**
* @brief UART MSP Initialization
* This function configures the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(huart->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(huart->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PD5     ------> USART2_TX
    PD6     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    __HAL_AFIO_REMAP_USART2_ENABLE();

  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }

}

/**
* @brief UART MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
  if(huart->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(huart->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PD5     ------> USART2_TX
    PD6     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_5|GPIO_PIN_6);

  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }

}

#endif

//void Error_Handler(void)
//{

//}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{

}
#endif


//

#if 1																													//cooler adc pwm 

#include "app_eeprom_24xx.h"
#include "bsp_mcu_delay.h"	

enum pwm_ch_n
{
	PWM_CH0=0,
	PWM_CH1,
	PWM_CH2,
	PWM_CH3,
	PWM_CH4,
	PWM_CH5,
};
enum adc_ch_n
{
	ADC_NTC0,
	ADC_NTC1,
	ADC_NTC2,
	ADC_NTC3,
	ADC_NTC4,
	ADC_NTC5,
};
const uint16_t NTC_TAB_T2ADC[TEMP_ADC_TAB_LEN][2] = {
	
0,3891,
5,3868,
10,3844,
15,3820,
20,3796,
25,3772,
30,3747,
35,3723,
40,3698,
45,3672,
50,3647,
55,3622,
60,3596,
65,3570,
70,3544,
75,3517,
80,3491,
85,3464,
90,3437,
95,3411,
100,3383,
105,3356,
110,3329,
115,3302,
120,3274,
125,3246,
130,3219,
135,3191,
140,3163,
145,3135,
150,3107,
155,3079,
160,3050,
165,3022,
170,2994,
175,2965,
180,2937,
185,2909,
190,2880,
195,2852,
200,2824,
205,2795,
210,2767,
215,2739,
220,2710,
225,2682,
230,2654,
235,2626,
240,2598,
245,2570,
250,2542,
255,2514,
260,2486,
265,2459,
270,2431,
275,2404,
280,2376,
285,2349,
290,2322,
295,2295,
300,2268,

305,2242,
310,2215,
315,2189,
320,2162,
325,2136,
330,2110,
335,2085,
340,2059,
345,2034,
350,2008,


};



uint16_t ADC_GetNTCAdcVal(uint32_t ch)
{
    ADC_ChannelConfTypeDef ADC1_ChanConf;
    
    ADC1_ChanConf.Channel=ch;                                   //通道
    ADC1_ChanConf.Rank=1;                                       //第1个序列，序列1
    ADC1_ChanConf.SamplingTime=ADC_SAMPLETIME_239CYCLES_5;      //采样时间               
    HAL_ADC_ConfigChannel(&hadc1,&ADC1_ChanConf);        //通道配置
	
    HAL_ADC_Start(&hadc1);                               //开启ADC
	
    HAL_ADC_PollForConversion(&hadc1,10);                //轮询转换
 
		return (uint16_t)HAL_ADC_GetValue(&hadc1);	        	//返回最近一次ADC1规则组的转换结果
}
uint16_t ADC_GetNTCAdcValAverage(uint32_t ch,uint8_t times)
{
	uint32_t temp_val=0;
	uint8_t t;
	for(t=0;t<times;t++)
	{
		temp_val+=ADC_GetNTCAdcVal(ch);
		delay_ms(5);
	}
	return temp_val/times;
}

void TIM_PWM_SetDigital(uint8_t pwm_ch, uint32_t DVal)
{
	switch(pwm_ch)
	{
		
		case PWM_CH0:  TIM4->CCR1=DVal; 
			break;
		case PWM_CH1:  TIM4->CCR2=DVal; 
			break;		
		case PWM_CH2:  TIM3->CCR1=DVal; 
			break;			
		case PWM_CH3:  TIM3->CCR2=DVal; 
			break;			
		case PWM_CH4:  TIM3->CCR3=DVal; 
			break;			
		case PWM_CH5:  TIM3->CCR4=DVal; 
			break;	
		default:
			break;
	}
}

uint16_t getCoolerTemFromNTC(uint8_t channel)
{
	uint32_t NTC_AdcChannel=0;
	uint32_t NTC_AdcVal=0;
	uint16_t NTC_Tem=0;
	float Adc_TFactor=0;
	
	uint8_t i;
	
	switch(channel)
	{
		case ADC_NTC0: NTC_AdcChannel=ADC_CHANNEL_14;
			break;
		case ADC_NTC1: NTC_AdcChannel=ADC_CHANNEL_7;
			break;		
		case ADC_NTC2: NTC_AdcChannel=ADC_CHANNEL_6;
			break;	
		case ADC_NTC3: NTC_AdcChannel=ADC_CHANNEL_5;
			break;		
		case ADC_NTC4: NTC_AdcChannel=ADC_CHANNEL_4;
			break;		
		case ADC_NTC5: NTC_AdcChannel=ADC_CHANNEL_3;
			break;	
		default:
			break;
	}
	NTC_AdcVal=ADC_GetNTCAdcValAverage(NTC_AdcChannel,10);
	
	if(NTC_AdcVal>NTC_TAB_T2ADC[0][1]) NTC_Tem=NTC_TAB_T2ADC[0][0];	
	
	else
	{
		for(i=1; i<TEMP_ADC_TAB_LEN ; i++)
		{
			if(NTC_AdcVal>NTC_TAB_T2ADC[i][1])
			{		
				break;
			}
		}
		if(i==TEMP_ADC_TAB_LEN)  NTC_Tem=NTC_TAB_T2ADC[TEMP_ADC_TAB_LEN-1][0];
		else 
		{
		//每个ADC值变化的度数
			Adc_TFactor=(NTC_TAB_T2ADC[i-1][1]-NTC_TAB_T2ADC[i][1])/(NTC_TAB_T2ADC[i][0]-NTC_TAB_T2ADC[i-1][0]);
			
			NTC_Tem=NTC_TAB_T2ADC[i-1][0]+Adc_TFactor*(NTC_TAB_T2ADC[i-1][1]-NTC_AdcVal);
		}
	}
	return NTC_Tem;
}
void setCoolerTemFromPWM(uint8_t channel, uint16_t Degreec)
{
	uint32_t NTC_AdcVal=0;
	float T_AdcFactor=0;
	uint8_t i;
	
	if(Degreec<NTC_TAB_T2ADC[0][0]) NTC_AdcVal=NTC_TAB_T2ADC[0][1];	
	
	for(i=1; i<TEMP_ADC_TAB_LEN ; i++)
	{
		if(Degreec<NTC_TAB_T2ADC[i][0])
		{		
			break;
		}
	}
	if(i==TEMP_ADC_TAB_LEN)  NTC_AdcVal=NTC_TAB_T2ADC[TEMP_ADC_TAB_LEN-1][1];
	else 
	{
	//每个温度变化的ADC值
		T_AdcFactor=(NTC_TAB_T2ADC[i][0]-NTC_TAB_T2ADC[i-1][0])/(NTC_TAB_T2ADC[i-1][1]-NTC_TAB_T2ADC[i][1]);
		
		NTC_AdcVal=NTC_TAB_T2ADC[i-1][1]-T_AdcFactor*(Degreec-NTC_TAB_T2ADC[i-1][0]);
	}	
	
	TIM_PWM_SetDigital(channel, NTC_AdcVal);
}
void SaveCoolerTemToEEPROM(uint8_t channel, uint16_t Degreec)
{
	CoolerDegreec_Time.degreecSaved=Degreec;
	at24cxx.write(at24c256 , (EEPROM_Cooler_PAGE_ADDR)*64, (uint8_t *)&CoolerDegreec_Time, sizeof(CoolerDataSaved_T));			
}
void getCoolerTemFromEEPROM(uint8_t channel)
{
	CoolerDataSaved_T CurrentDatas;
	
	at24cxx.read(at24c256 , (EEPROM_Cooler_PAGE_ADDR)*64, (uint8_t *)&CurrentDatas, sizeof(CoolerDataSaved_T));		
	if(CurrentDatas.saved==CoolerSaved)
	{
		CoolerDegreec_Time.degreecSaved=CurrentDatas.degreecSaved;
		CoolerDegreec_Time.timerSaved=CurrentDatas.timerSaved;
	}	
	else 
	{
		at24cxx.write(at24c256 , (EEPROM_Cooler_PAGE_ADDR)*64, (uint8_t *)&CoolerDegreec_Time, sizeof(CoolerDataSaved_T));			
	}		
}


//
#endif
//

#if 1																													//cooler msh

/********
1. 			cooler setDegreec ch0-5	val
	ex:
	send:	cooler setDegreec ch0 250
	rec:	cooler setDegreec ch0 250 ok\r\n

2. 			cooler setPowerOnDegreec ch0-5 val
	ex:
	send:	cooler setPowerOnDegreec 0 250
	rec:	cooler setPowerOnDegreec 0 250 ok\r\n

3. 			cooler getPowerOnDegreec ch0-5
	ex:
	send:	cooler getPowerOnDegreec ch0
	rec:	PowerOnDegreec ch0=250\r\n

4. 			cooler getCurrentDegreec ch0-5
	ex:
	send:	cooler getCurrentDegreec 0
	rec:	getCurrentDegreec ch0=250\r\n

5. 			cooler setTimer X_second
	ex:
	send:	cooler setTimer 10
	rec:	cooler setTimer 10 ok\r\n
	
*********/
// 

// cooler getdegree ch0-5
//
//
int cooler(int argc, char **argv)
{
  if (argc == 2 )		
	{
		//cooler getTimer
		if(!strcmp(argv[1], "getTimer"))
		{
				rt_kprintf("Timer=%d\n",CoolerDegreec_Time.timerSaved);							return RT_EOK;
		}	
		//cooler getPowerOnDegreec
		if(!strcmp(argv[1], "getPowerOnDegreec"))
		{
			getCoolerTemFromEEPROM(255);
			rt_kprintf("PowerOnDegreec=%d\n",CoolerDegreec_Time.degreecSaved);	return RT_EOK;			
		}	
	}
  if (argc == 3 )		
	{
		//cooler getCurrentDegreec ch0-5
		if(!strcmp(argv[1], "getCurrentDegreec"))
		{
			uint16_t CurrentChannel=atoi(argv[2]);
			if(CurrentChannel < 6	)
			{
				rt_kprintf("getCurrentDegreec ch%d=%d\n",CurrentChannel,getCoolerTemFromNTC(CurrentChannel));		
				return RT_EOK;
			}
			else 
			{		
				rt_kprintf("cooler's channel must be 0~5\n");
				return RT_ERROR;
			}			
		}	
		//cooler getPowerOnDegreec ch0~5
		if(!strcmp(argv[1], "getPowerOnDegreec"))
		{
			rt_kprintf("getPowerOnDegreec=%d\n",CoolerDegreec_Time.degreecSaved);		
		}
		//cooler setTimer second
		if(!strcmp(argv[1], "setTimer"))
		{
			uint16_t TimerSet=atoi(argv[2]);
			if(TimerSet > 0	)
			{
				CoolerDegreec_Time.timerSaved=TimerSet;
				at24cxx.write(at24c256 , (EEPROM_Cooler_PAGE_ADDR)*64, (uint8_t *)&CoolerDegreec_Time, sizeof(CoolerDataSaved_T));			

				rt_kprintf("cooler setTimer %d ok\n",TimerSet);		
				
				return RT_EOK;
			}
			else 
			{		
				rt_kprintf("cooler's set timer error\n");
				return RT_ERROR;
			}			
		}	
		//cooler setDegreec val
		if(!strcmp(argv[1], "setDegreec"))
		{
				uint16_t DegreecSet=atoi(argv[2]);
				setCoolerTemFromPWM(255,DegreecSet);
				rt_kprintf("setDegreec %d ok\n",DegreecSet);		
				return RT_EOK;
		}
		//cooler setPowerOnDegreec val
		if(!strcmp(argv[1], "setPowerOnDegreec"))
		{

				CoolerDegreec_Time.degreecSaved=atoi(argv[2]);;
				at24cxx.write(at24c256 , (EEPROM_Cooler_PAGE_ADDR)*64, (uint8_t *)&CoolerDegreec_Time, sizeof(CoolerDataSaved_T));			

				for(uint8_t i=0;i<6;i++)
				{
					setCoolerTemFromPWM(i,CoolerDegreec_Time.degreecSaved);	
				}			
				rt_kprintf("cooler setPowerOnDegreec %d ok\n",CoolerDegreec_Time.degreecSaved);		
				
				return RT_EOK;	
		}					
	}
	if (argc == 4 )	
	{
		//cooler setDegreec ch0-5	val
		if(!strcmp(argv[1], "setDegreec") && !strncmp(argv[2], "ch" , 2))
		{
			uint16_t CurrentChannel=atoi(&argv[2][2]);
			if(CurrentChannel < 6	)
			{
				uint16_t DegreecSet=atoi(argv[3]);
				setCoolerTemFromPWM(CurrentChannel,DegreecSet);
				rt_kprintf("setDegreec ch%d %d ok\n",CurrentChannel,DegreecSet);		
				return RT_EOK;
			}
			else 
			{		
				rt_kprintf("cooler's channel must be 0~5\n");
				return RT_ERROR;
			}			
		}	
		//cooler setPowerOnDegreec ch0-5 val	
		if(!strcmp(argv[1], "setPowerOnDegreec") && !strncmp(argv[2], "ch" , 2))
		{
			uint16_t CurrentChannel=atoi(&argv[2][2]);
			if(CurrentChannel < 6	)
			{
				uint16_t DegreecSet=atoi(argv[3]);
				SaveCoolerTemToEEPROM(CurrentChannel,DegreecSet);
				rt_kprintf("setPowerOnDegreec ch%d %d ok\n",CurrentChannel,DegreecSet);		
				return RT_EOK;
			}
			else 
			{		
				rt_kprintf("cooler's channel must be 0~5\n");
				return RT_ERROR;
			}			
		}					
	}

//	rt_kprintf("cooler setDegreec <ch0-5> <var>\n");
//	rt_kprintf("cooler setPowerOnDegreec <ch0-5> <var>\n");	
//	rt_kprintf("cooler getPowerOnDegreec <ch0-5>\n");	
	rt_kprintf("cooler Usage: \n");
	rt_kprintf("cooler getTimer\n");
	rt_kprintf("cooler getPowerOnDegreec\n");
	rt_kprintf("cooler getCurrentDegreec <ch0-5>\n");
	rt_kprintf("cooler setDegreec <var>\n");
	rt_kprintf("cooler setPowerOnDegreec <var>\n");

	rt_kprintf("cooler setTimer <var_second>\n");


	return RT_ERROR;
}


#endif

static struct rt_thread CoolerThread;

static rt_uint8_t CoolerStack[ 512 ];

static struct rt_thread CoolerThread;

/*

上电默认风扇打开 FAN_OUT_H,  电磁锁松开, LOCK_OUT_L
检测到放入DUT后,再检测到盖子传感器,电磁锁工作,锁住盖子,DUT开始定时制冷。
定时时间到后,如果温度也达到,电磁锁松开,盖子弹开,否则继续制冷到设定温度弹开盖子

取走DUT后,制冷结束,定时器重新赋值

水滴问题：不要一直工作,即取走DUT后,制冷结束

定时器开6个或者1个
定时器=1个时：定时器一直循环,比如一次定时10ms.时间到则,则采集TimerCntFlag=Start时,TimerCHCnt递减,到0时则则采集TimerCntFlag=Stop.
（定时器的验证,可1S闪烁LED或者串口打印）
在其他任务中操作,处理

*/


static void CoolerThread_entry(void *parameter)
{
	
	rt_thread_delay(100);

}
int CoolerThreadInit(void)
{
    rt_err_t result;

    result = rt_thread_init(&CoolerThread,
                            "CoolerApp",
                            CoolerThread_entry,
                            RT_NULL,
                            (rt_uint8_t *)&CoolerStack[0],
                            sizeof(CoolerStack),
                            15,
                            20);
    if (result == RT_EOK)
    {
       rt_thread_startup(&CoolerThread);
    }
    return 0;
}

INIT_APP_EXPORT(inc_mbcooler_init);	
MSH_CMD_EXPORT(cooler, ......);

//

#if 0		//定时器

#define HWTIMER_DEV_NAME   "timer0"     /* 定时器名称 */
#define HWTIMER_DEV_NAME   "timer0"     /* 定时器名称 */
/* 定时器超时回调函数 */
static rt_err_t timeout_cb(rt_device_t dev, rt_size_t size)
{
    rt_kprintf("tick is :%d !\n", rt_tick_get());

    return 0;
}

static int hwtimer_sample(int argc, char *argv[])
{
    rt_err_t ret = RT_EOK;
    rt_hwtimerval_t timeout_s;      /* 定时器超时值 */
    rt_device_t hw_dev = RT_NULL;   /* 定时器设备句柄 */
    rt_hwtimer_mode_t mode;         /* 定时器模式 */

    /* 查找定时器设备 */
    hw_dev = rt_device_find(HWTIMER_DEV_NAME);
    if (hw_dev == RT_NULL)
    {
        rt_kprintf("hwtimer sample run failed! can't find %s device!\n", HWTIMER_DEV_NAME);
        return RT_ERROR;
    }

    /* 以读写方式打开设备 */
    ret = rt_device_open(hw_dev, RT_DEVICE_OFLAG_RDWR);
    if (ret != RT_EOK)
    {
        rt_kprintf("open %s device failed!\n", HWTIMER_DEV_NAME);
        return ret;
    }

    /* 设置超时回调函数 */
    rt_device_set_rx_indicate(hw_dev, timeout_cb);

    /* 设置模式为周期性定时器 */
    mode = HWTIMER_MODE_PERIOD;
    ret = rt_device_control(hw_dev, HWTIMER_CTRL_MODE_SET, &mode);
    if (ret != RT_EOK)
    {
        rt_kprintf("set mode failed! ret is :%d\n", ret);
        return ret;
    }

    /* 设置定时器超时值为5s并启动定时器 */
    timeout_s.sec = 5;      /* 秒 */
    timeout_s.usec = 0;     /* 微秒 */

    if (rt_device_write(hw_dev, 0, &timeout_s, sizeof(timeout_s)) != sizeof(timeout_s))
    {
        rt_kprintf("set timeout value failed\n");
        return RT_ERROR;
    }

    /* 延时3500ms */
    rt_thread_mdelay(3500);

    /* 读取定时器当前值 */
    rt_device_read(hw_dev, 0, &timeout_s, sizeof(timeout_s));
    rt_kprintf("Read: Sec = %d, Usec = %d\n", timeout_s.sec, timeout_s.usec);

    return ret;
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(hwtimer_sample, hwtimer sample);



#endif


/* 定时器的控制块 */
static rt_timer_t timer0;
static rt_timer_t timer1;
static rt_timer_t timer2;
static rt_timer_t timer3;
static rt_timer_t timer4;
static rt_timer_t timer5;

static void timeout0(void *parameter)
{
    rt_kprintf("timer0 is timeout %d\n");
}
static void timeout1(void *parameter)
{
    rt_kprintf("timer1 is timeout %d\n");
}
static void timeout2(void *parameter)
{
    rt_kprintf("timer2 is timeout %d\n");
}
static void timeout3(void *parameter)
{
    rt_kprintf("timer3 is timeout %d\n");
}
static void timeout4(void *parameter)
{
    rt_kprintf("timer4 is timeout %d\n");
}
static void timeout5(void *parameter)
{
    rt_kprintf("timer5 is timeout %d\n");
}

int timer_sample(void)
{
   timer0 = rt_timer_create("timer0", timeout0, RT_NULL, 1*1000,  RT_TIMER_FLAG_ONE_SHOT);
	 timer1 = rt_timer_create("timer1", timeout1, RT_NULL, 2*1000,  RT_TIMER_FLAG_ONE_SHOT);
	 timer2 = rt_timer_create("timer2", timeout2, RT_NULL, 3*1000,  RT_TIMER_FLAG_ONE_SHOT);
	 timer3 = rt_timer_create("timer3", timeout3, RT_NULL, 4*1000,  RT_TIMER_FLAG_ONE_SHOT);
	 timer4 = rt_timer_create("timer4", timeout4, RT_NULL, 5*1000,  RT_TIMER_FLAG_ONE_SHOT);
	 timer5 = rt_timer_create("timer5", timeout5, RT_NULL, 6*1000,  RT_TIMER_FLAG_ONE_SHOT);
	
   if (timer0 != RT_NULL)	rt_timer_start(timer0);
	 if (timer1 != RT_NULL)	rt_timer_start(timer1);
	 if (timer2 != RT_NULL)	rt_timer_start(timer2);
	 if (timer3 != RT_NULL)	rt_timer_start(timer3);
	 if (timer4 != RT_NULL)	rt_timer_start(timer4);
	 if (timer5 != RT_NULL)	rt_timer_start(timer5);

   return 0;
}

/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(timer_sample, timer sample);









