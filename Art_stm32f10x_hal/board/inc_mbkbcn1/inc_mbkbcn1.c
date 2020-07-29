
#include "bsp_mcu_gpio.h"
#include "bsp_mcu_delay.h"
#include "app_eeprom_24xx.h"

#include "main.h"
#include "timer_encoder.h"
#include "bsp_12BitADC.h"

#if 1 //hal gpio define
#define INSH_ECHO_DISABLE_DEFAULT
#define SDA_Pin GPIO_PIN_14
#define SDA_GPIO_Port GPIOC
#define SCL_Pin GPIO_PIN_15
#define SCL_GPIO_Port GPIOC
#define SPI1_CS_Pin GPIO_PIN_11
#define SPI1_CS_GPIO_Port GPIOB
#define INTR_Pin GPIO_PIN_12
#define INTR_GPIO_Port GPIOB
#define TARGET_REACHED_Pin GPIO_PIN_13
#define TARGET_REACHED_GPIO_Port GPIOB
#define NFREEZE_Pin GPIO_PIN_14
#define NFREEZE_GPIO_Port GPIOB
#define LED_SYS_RUN_Pin GPIO_PIN_8
#define LED_SYS_RUN_GPIO_Port GPIOA
#define S7_Pin GPIO_PIN_15
#define S7_GPIO_Port GPIOA
#define S6_Pin GPIO_PIN_3
#define S6_GPIO_Port GPIOB
#define S5_Pin GPIO_PIN_4
#define S5_GPIO_Port GPIOB
#define S4_Pin GPIO_PIN_5
#define S4_GPIO_Port GPIOB
#define CS_C_Pin GPIO_PIN_6
#define CS_C_GPIO_Port GPIOB
#define CS_B_Pin GPIO_PIN_7
#define CS_B_GPIO_Port GPIOB
#define CS_A_Pin GPIO_PIN_9
#define CS_A_GPIO_Port GPIOB

#endif



//
char version_string[]="ver1.0.2";
/*
ver1.0.0 +bsp_12BitADC
ver1.0.1 +timer_encoder
ver1.0.2 S4~S7 set mode --- output
*/
at24cxx_t at24c256=
{
	{0},
	{PC_15,PC_14},	
	0xA0,	
};

uint8_t LED_PIN=PA_8;
uint8_t KeyChPinIN[4]={PB_5,PB_4,PB_3,PA_15};						//S4-S6
uint8_t KeyA0_7CSpin[4]={PB_9,PB_7,PB_6,PB_6};						//CS_A-CS_C
//
const float avg_factor = 0.8;
char strKeyVoltage[15]="";
char strKeyPress[15]="";
//
static uint32_t StreamDataCNT=0;
static rt_uint8_t StreamDataStack[ 512 ];
static struct	rt_thread StreamDataThread;
//
int StreamData_thread_init(void);
/****************************************MX_CUBE_HAL GPIO*******************************************************/
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, SDA_Pin|SCL_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, NFREEZE_Pin|CS_C_Pin|CS_B_Pin|CS_A_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED_SYS_RUN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : SDA_Pin SCL_Pin */
  GPIO_InitStruct.Pin = SDA_Pin|SCL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : SPI1_CS_Pin NFREEZE_Pin S6_Pin S5_Pin 
                           S4_Pin CS_C_Pin CS_B_Pin CS_A_Pin */
  GPIO_InitStruct.Pin = SPI1_CS_Pin|NFREEZE_Pin |CS_C_Pin|CS_B_Pin|CS_A_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : INTR_Pin TARGET_REACHED_Pin */
  GPIO_InitStruct.Pin = S4_Pin | S5_Pin | S6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOB, S4_Pin | S5_Pin | S6_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED_SYS_RUN_Pin S7_Pin */
  GPIO_InitStruct.Pin = LED_SYS_RUN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
  GPIO_InitStruct.Pin = S7_Pin ;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOA, S7_Pin, GPIO_PIN_RESET);

}
/****************************************MAIN---LED Blink*******************************************************/
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

/****************************************mbkbcn1  init  all**********************************************************/
int mbkbcn1_hw_init(void)
{		
	//board.c	rcc_set
	rt_kprintf("\nFW-MBKBCN1-RTT-%d-%d %s build at %s %s\n\n", STM32_FLASH_SIZE/1024, STM32_SRAM_SIZE, version_string, __TIME__, __DATE__);
			
	MX_GPIO_Init();
	at24cxx_hw_init();
	bsp_12BitADC_init();
	Timer_EncoderInit();
		
	rt_kprintf("controller init......[ok]\n");
	rt_kprintf("\n-------------------------------\n");
	rt_kprintf("\nCommands mode--msh:command and parameters are separated by spaces\n");
	rt_kprintf("-------------------------------\n");
	rt_kprintf("\nyou can type help to list commands and all commands should end with \\r\\n\n");

	StreamData_thread_init();
  return 0;
}



/****************************************PONY keyboard  app**********************************************************/
/*                            PONY keyboard MUX TABLE  from customer
S3	S2	S1	S0			MUX1	MUX2	MUX3	MUX4	MUX5	MUX6	MUX7	MUX8
6		5		4		3				A0		A1		A2		A3		A4		A5		A6		A7

L		L		L		L		0		K1		K15		K29		K43		K56		K68		K69		K70	
L 	L		L		H		1		K2		K16		K30		K44		K45		K57		K58		K71
L 	L		H		L		2		K3		K17		K31		K32		K46		K59		K72
L 	L		H		H		3		K4		K18		K19		K33		K47		K60		K61	
L 	H		L		L		4		K5		K20		K34		K48		K49		K62		K63		K74
L 	H		L		H		5		K6		K7		K21		K22		K35		K50		K64		K73
L 	H		H		L		6		K8		K23		K36		K51		K65		K75		K76		K77
L 	H		H		H		7		K9		K24		K37		K38		K39		K52		K53		K66
H 	L		L		L		8		K10		K11		K25		K26		K40		K54		K67		K78
H 	L		L		H		9		K12		K13		K14		K27		K28		K41		K42		K55
*/

void SetKeyChannel(uint8_t ch)
{
	switch(ch)
	{
		case 0: 		pinReset(KeyChPinIN[3]);	pinReset(KeyChPinIN[2]);	pinReset(KeyChPinIN[1]);		pinReset(KeyChPinIN[0]);									
			break;		//0000                                                          
		case 1:			pinReset(KeyChPinIN[3]);	pinReset(KeyChPinIN[2]);	pinReset(KeyChPinIN[1]);		pinSet(KeyChPinIN[0]);		
			break;		//0001		                                                        
		case 2:			pinReset(KeyChPinIN[3]);	pinReset(KeyChPinIN[2]);	pinSet(KeyChPinIN[1]);			pinReset(KeyChPinIN[0]);	
			break;		//0010                                                          
		case 3:			pinReset(KeyChPinIN[3]);	pinReset(KeyChPinIN[2]);	pinSet(KeyChPinIN[1]);			pinSet(KeyChPinIN[0]);		
			break;		//0011		                                                        
		case 4:			pinReset(KeyChPinIN[3]);	pinSet(KeyChPinIN[2]);		pinReset(KeyChPinIN[1]);		pinReset(KeyChPinIN[0]);	
			break;		//0100                                                          
		case 5:			pinReset(KeyChPinIN[3]);	pinSet(KeyChPinIN[2]);		pinReset(KeyChPinIN[1]);		pinSet(KeyChPinIN[0]);		
			break;		//0101                                                          
		case 6:			pinReset(KeyChPinIN[3]);	pinSet(KeyChPinIN[2]);		pinSet(KeyChPinIN[1]);			pinReset(KeyChPinIN[0]);	
			break;		//0110                                                         
		case 7:			pinReset(KeyChPinIN[3]);	pinSet(KeyChPinIN[2]);		pinSet(KeyChPinIN[1]);			pinSet(KeyChPinIN[0]);		
			break;		//0111                                                          
		case 8:			pinSet(KeyChPinIN[3]);		pinReset(KeyChPinIN[2]);	pinReset(KeyChPinIN[1]);		pinReset(KeyChPinIN[0]);	
			break;		//1000                                                        
		case 9:			pinSet(KeyChPinIN[3]);		pinReset(KeyChPinIN[2]);	pinReset(KeyChPinIN[1]);		pinSet(KeyChPinIN[0]);		
			break;		//1001
		default:
			break;
	}
}
void SetKeyA0_7(uint8_t ch)
{
	switch(ch)
	{
		case 0: 		pinReset(KeyA0_7CSpin[2]);	pinReset(KeyA0_7CSpin[1]);		pinReset(KeyA0_7CSpin[0]);	
			break;		//000                                                          
		case 1:			pinReset(KeyA0_7CSpin[2]);	pinReset(KeyA0_7CSpin[1]);		pinSet(KeyA0_7CSpin[0]);		
			break;		//001		                                                        
		case 2:			pinReset(KeyA0_7CSpin[2]);	pinSet(KeyA0_7CSpin[1]);			pinReset(KeyA0_7CSpin[0]);	
			break;		//010                                                          
		case 3:			pinReset(KeyA0_7CSpin[2]);	pinSet(KeyA0_7CSpin[1]);			pinSet(KeyA0_7CSpin[0]);		
			break;		//011		                                                        
		case 4:			pinSet(KeyA0_7CSpin[2]);		pinReset(KeyA0_7CSpin[1]);		pinReset(KeyA0_7CSpin[0]);	
			break;		//100                                                          
		case 5:			pinSet(KeyA0_7CSpin[2]);		pinReset(KeyA0_7CSpin[1]);		pinSet(KeyA0_7CSpin[0]);		
			break;		//101                                                          
		case 6:			pinSet(KeyA0_7CSpin[2]);		pinSet(KeyA0_7CSpin[1]);			pinReset(KeyA0_7CSpin[0]);	
			break;		//110                                                         
		case 7:			pinSet(KeyA0_7CSpin[2]);		pinSet(KeyA0_7CSpin[1]);			pinSet(KeyA0_7CSpin[0]);		
			break;		//111                                                          
                                                   

		default:
			break;
	}	
	
}
int getKeyData(int argc, char **argv)
{
	uint8_t result = REPLY_OK;
	
	if (argc ==4)
	{
		uint8_t 	KeyChannel=atoi(argv[1]);
		uint8_t 	KeyA0_7=atoi(argv[2]);
		
		StreamDataCNT=atoi(argv[3]);
				
		SetKeyChannel(KeyChannel);
		SetKeyA0_7(KeyA0_7);
	}
	else
	{
		rt_kprintf("Usage: \n");
		rt_kprintf("getKeyData <channel> <mux> <times>\n");
		rt_kprintf("    channel:0~9;mux:0~7\n");			
		result = REPLY_INVALID_CMD;
	}	
	return result;
}
/****************************************KEY Thread MSH*****************************************************/
static void StreamData_Thread_entry(void *parameter)
{
	static int time_start=0;
	static int time_stop=0;
	static uint8_t time_once=0;
	static float KeyVolt_AdcVal=0;
	
	static float KeyPress_AdcVal=0;
	
	while (1)
	{  
		if(StreamDataCNT)
		{			
				if(time_once==0)	
				{			
					//!> 按键切换到位后，等待电压稳定
					delay_us(100);
					//!> channel and mux is selected ADC数据是正确的固定通道 
					KeyVolt_AdcVal=GetADCReg_FilterVal(2,1);
					//预采数据 1ms
					for(uint8_t i=0;i<100;i++)
					{
						KeyVolt_AdcVal=KeyVolt_AdcVal * avg_factor + (1-avg_factor) * GetADCReg_FilterVal(2,1);
					}				
					time_once=1;time_start=HAL_GetTick();
					rt_kprintf("start time:%dms\n",HAL_GetTick());
				}				
				KeyVolt_AdcVal=KeyVolt_AdcVal * avg_factor + (1-avg_factor) * GetADCReg_FilterVal(2,1);
				
//				KeyPress_AdcVal=KeyPress_AdcVal * 0.2 + 0.8 * GetADCReg_FilterVal(1,1);
////				
				sprintf(strKeyPress,"%.3f",ConvertADC_to_mV(1,GetADCReg_FilterVal(1,1)));	

//				sprintf(strKeyPress,"%.3f",ConvertADC_to_mV(1,KeyPress_AdcVal));	
					
				sprintf(strKeyVoltage,"%.1f", ConvertADC_to_mV(2,KeyVolt_AdcVal));	
				
				CaptureNumber = OverflowCount*65535 + __HAL_TIM_GET_COUNTER(&htim2);						

				StreamDataCNT--;
				
				rt_kprintf("%sV,%smV,%dP\n",strKeyPress,strKeyVoltage,-CaptureNumber);				
				
				if(StreamDataCNT==0)	
				{
					time_once=0;
					time_stop=HAL_GetTick();
					
					rt_kprintf("stop time:%dms,spent:%dms\n",time_stop,abs(time_stop-time_start));
					rt_kprintf(">>");
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
                            "KeyData",
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
INIT_APP_EXPORT(mbkbcn1_hw_init);

MSH_CMD_EXPORT(getKeyData, ......);

/*

typedef struct
{
  UCHAR Type;        //!< type parameter
  union
  {
    uint16_t U16;      //!< value parameter as 32 bit integer
    UCHAR Byte[2];   //!< value parameter as 4 bytes
  } Value;           //!< value parameter
	
} Kb_Data_T;

Kb_Data_T  PressMV;
Kb_Data_T  VoltageMV;

UART_HandleTypeDef huart1;
//					HAL_UART_Transmit(&huart1,(uint8_t*)0x01,1,1000);
//					HAL_UART_Transmit(&huart1,(uint8_t*)0x02,1,1000);
//					HAL_UART_Transmit(&huart1,PressMV.Value.Byte,2,1000);
//					HAL_UART_Transmit(&huart1,VoltageMV.Value.Byte,2,1000);
//					HAL_UART_Transmit(&huart1,(uint8_t*)CaptureNumber,4,1000);
//					HAL_UART_Transmit(&huart1,(uint8_t*)0x0D,1,1000);

//				sprintf(strKeyVoltage,"%.3f",keyVolt_new);
							
//				PressMV.Value.U16=(uint16_t) (GetVol_Average(1,3)*1000);
//				VoltageMV.Value.U16=(uint16_t) (GetVol_Average(2,3)*1000);	
	

					//rt_kprintf("%sV,%sV,%ldP,%d\n",strKeyPress,strKeyVoltage,-CaptureNumber,HAL_GetTick());

//					for(uint8_t i=0;i<8;i++)
//					{
//						for(uint8_t j=0;j<30;j++)
//						{
//							GetVol_Avg_val(2,i);	
//						}
//					}

//		for(uint8_t i=0;i<8;i++)
//		{
//			//rt_kprintf("i: \n");
//			for(uint8_t j=0;j<30;j++)
//			{
//				//rt_kprintf("j: \n");
//				GetVol_Avg_val(2,i);	
//			}
//		}

//				KeyCurrentCH=pinRead(KeyChPinIN[0])+pinRead(KeyChPinIN[1])*2+pinRead(KeyChPinIN[2])*4+pinRead(KeyChPinIN[3])*8;
//				KeyChVolt[KeyCurrentCH]=GetVol_Average(2,1);
//				VoltageMV.Value.U16=(uint16_t) (KeyChVolt[KeyCurrentCH]*1000);	
//				sprintf(strKeyVoltage,"%.3f",KeyChVolt[KeyCHSelected]);			
//				rt_kprintf("KeyCurrentCH=%d\n",KeyCurrentCH);		
				
//				keyVolt_new=keyVolt_new*0.8+0.2*GetVol_Average(2,1);

//				sprintf(strKeyPress,"%.3f",GetVol_Average(1,1));	//new board		
//				sprintf(strKeyVoltage,"%.3f",GetVol_Average(2,1));
//					for(uint8_t i=0;i<8;i++)
//					{
//						//SetKeyChannel(i);
//						GetVol_Avg_val(2,i);	
//					}
					//SetKeyChannel(KeyCHSelected);
					
*/
//


