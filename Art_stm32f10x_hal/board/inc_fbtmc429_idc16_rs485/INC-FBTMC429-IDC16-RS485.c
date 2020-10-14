#include <string.h>
#include <stdio.h>
#include "inc_dido.h"
#include "app_eeprom_24xx.h"
#include "bsp_mcu_gpio.h"
#include "INC-FBTMC429-IDC16-RS485.h"

//typedef struct	
//{																	
//	uint8_t saved;									// 1
//	char stationSet[20];
//	char snSet[20];
//}
//RS485_AUTO_T;

#if 1
	#define DBG_ENABLE	1
	#if 	( DBG_ENABLE )
	#define DBG_TRACE		rt_kprintf
	#else
	#define DBG_TRACE(...)		
#endif

#endif

#define UART3_THREAD_PRIORITY 19
#define uart_rx_len_max 100
#define RS485_TX_EN_PIN PD_12
/*
rt_hw_usart_init  	增加串口3配置相关
rtconfig.h					#define BSP_USING_UART3
stm32fxx_hal_mcp.c	HAL_UART_MspInit	HAL_UART_MspDeInit 添加串口3相关
*/

/**********************************************************************************************************************/
uint8_t DUT_detectPin={PE_2};

uint8_t positionNo=0;
char stationSet[20]="STATION01";
char snSet[20]="DND333200CJN90F1A";
//char snSet[20]="DNDZH08XN3Y9";
struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT; 

static char uart_name[]="uart3";

static char uart_rx_buff[100];
static char rx_buff_temp[100];

static uint8_t uart_rx_len_index=0;

static struct rt_semaphore rx_sem;	

static rt_device_t serial;

static void serial_thread_entry(void *parameter);
void Load_EEPROMToRam(void);
int uart3_stream_thread_init(void);
/**********************************************************************************************************************/
void fbtmc429_idc16_rs485_init(void)
{
	//rt_device_control(serial, RT_DEVICE_CTRL_SET_INT, NULL);	
	Load_EEPROMToRam();
	
	pinMode(RS485_TX_EN_PIN,PIN_MODE_OUTPUT);
	
	pinReset(RS485_TX_EN_PIN);
	
	uart3_stream_thread_init();
	
	DBG_TRACE("fbtmc429_idc16_rs485_init ok\n");
	
//	pinSet(RS485_TX_EN_PIN);
//	
//	rt_device_write(serial, 0, "fbtmc429_idc16_rs485_init ok", (sizeof("fbtmc429_idc16_rs485_init ok") - 1));
	
	pinReset(RS485_TX_EN_PIN);
	
	rt_device_control(serial, RT_DEVICE_CTRL_SET_INT, NULL);	
}
/**********************************************************************************************************************/
static void fbtmc429_idc16_rs485_app(void)
{
/*
$:N:X:SN:DUTOK:?  From RS485	
$:N:X:SN:DUTOK:?	RESP GET DUT OK AND SEND TO PC 
$:N:X:SN:DUTNG:?	RESP GET DUT NG AND SEND TO PC 
	
$:STATION01:1:DND333200CJN90F1A:DUTOK:?
	
N---STATION01->STATION02
X---1->6
SN--DND333200CJN90F1A | DNDZH08XN3Y9
	
how to respon to mac
	
C 库函数 char *strtok(char *str, const char *delim) 分解字符串 str 为一组字符串，delim 为分隔符
该函数返回被分解的第一个子字符串，如果没有可检索的字符串，则返回一个空指针
strtok在s中查找包含在delim中的字符并用NULL(’\0’)来替换
	
strcpy(dest, src);
	
char ustart[10]=" "; 
char stationNO[10]=" ";
char snNO[20]=" ";
*/
	char *token;

	char delims[] = ":";	
	
	DBG_TRACE("get rs485 data:%s\n",uart_rx_buff);
	
	strcpy(rx_buff_temp, uart_rx_buff);
	
	token = strtok(rx_buff_temp, delims);
	
	if(token != NULL && token[0]=='$')
	{
		token = strtok(NULL, delims); 
		
		if(token != NULL && !strcmp(token, stationSet))
		{
			token = strtok(NULL, delims);
			if(token != NULL)	positionNo= token[0]-'0';
			
			token = strtok(NULL, delims);
			
			if(token != NULL && !strcmp(token, snSet))
			{
				token = strtok(NULL, delims);
				if(token != NULL && !strcmp(token, "DUTOK"))
				{
					if(1)
					{
						uart_rx_buff[uart_rx_len_index-1-3]='N';
						uart_rx_buff[uart_rx_len_index-1-2]='G';
					}
					pinSet(RS485_TX_EN_PIN);
					//rt_device_write(serial, 0, uart_rx_buff, (sizeof(uart_rx_buff) - 1));
					rt_device_write(serial, 0, uart_rx_buff, (strlen(uart_rx_buff)));
				}		
			}
		}
		
	}	
	uart_rx_len_index=0; 
	memset(uart_rx_buff, '\0', uart_rx_len_max);
	
	pinReset(RS485_TX_EN_PIN);	
}
static rt_err_t uart3_input(rt_device_t dev, rt_size_t size)
{
	rt_sem_release(&rx_sem);
	
	return RT_EOK;
}
static void serial3_thread_entry(void *parameter)
{
    char ch;
    while (1)
    {
				
        while (rt_device_read(serial, -1, &ch, 1) != 1)
        {
            rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
        }	
				rt_enter_critical();
//				if(ch=='?') 
//				{
//					if(uart_rx_len_index!=0)
//					fbtmc429_idc16_rs485_app();
//				}
//				else
				{

					uart_rx_buff[uart_rx_len_index]=ch;
					uart_rx_len_index++;

					if(uart_rx_len_index > uart_rx_len_max)
					{
						uart_rx_len_index=0; 
					}
					if(ch=='?') 
					{
						if(uart_rx_len_index!=0)	uart_rx_buff[uart_rx_len_index]='\0';
						fbtmc429_idc16_rs485_app();
					}
				}	
				rt_exit_critical();				
    }
}
//
int uart3_stream_thread_init(void)
{
    rt_err_t ret = RT_EOK;
	
		memset(uart_rx_buff, '\0', uart_rx_len_max);
    serial = rt_device_find(uart_name);
	
    if (!serial)
    {
        rt_kprintf("find %s failed!\n",uart_name);
        return RT_ERROR;
    }

    rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);	

		rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);		
		
		rt_device_control(serial, RT_DEVICE_CTRL_CLR_INT, NULL);	
	
		rt_device_set_rx_indicate(serial, uart3_input);				

		//MX_USART3_UART_Init();
		
    rt_thread_t thread = rt_thread_create("rs485", serial3_thread_entry, RT_NULL, 1024, UART3_THREAD_PRIORITY, 100);
    
		if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
    else
    {
        ret = RT_ERROR;
    }
    return ret;	
}



//$:STATION01:2:DND333200CJN90F1A:DUTO:?

#define CAL_DATA_SAVED	'Y'
#define EEPROM_RS485_DATA_PAGE_ADDR		100

void Load_EEPROMToRam()
{
	uint8_t tempFlag;
	at24cxx.read(at24c256 , (EEPROM_RS485_DATA_PAGE_ADDR)*64,&tempFlag, 1);		

	if( tempFlag == CAL_DATA_SAVED )
	{
		at24cxx.read(at24c256 , (EEPROM_RS485_DATA_PAGE_ADDR)*64+1, (uint8_t *)stationSet, 20);
		at24cxx.read(at24c256 , (EEPROM_RS485_DATA_PAGE_ADDR)*64+32, (uint8_t *)snSet, 20);
		for(uint8_t i=0;i<20;i++)	
		{
			if(stationSet[i]>127) { stationSet[i]='\0';break;}
		}
		for(uint8_t i=0;i<20;i++)	
		{
			if(snSet[i]>127) { snSet[i]='\0';break;}
		}
	}
	else
	{
		tempFlag = CAL_DATA_SAVED;
		at24cxx.write(at24c256 , (EEPROM_RS485_DATA_PAGE_ADDR)*64, &tempFlag, 1);			
		at24cxx.write(at24c256 , (EEPROM_RS485_DATA_PAGE_ADDR)*64+1, (uint8_t *)stationSet, strlen(stationSet));		
		at24cxx.write(at24c256 , (EEPROM_RS485_DATA_PAGE_ADDR)*64+32, (uint8_t *)snSet, strlen(snSet));		
	}
}
int RS485(int argc, char **argv)
{
	int result = REPLY_OK;
	
	if		  (argc == 1 )  									
	{
		//字符串数据存储在data的第100页。如果应用中页数已经被使用,需要重新配置
		rt_kprintf("\n");
		rt_kprintf("RS485 ID\tget station id\n");
		rt_kprintf("RS485 ID xx\tset station id\n");
		rt_kprintf("RS485 SN\tget sn\n");
		rt_kprintf("RS485 SN xxx\tset sn\n");
	}
	else if (argc == 2 )					
	{
		if(!strcmp(argv[1], "ID"))
		{
			rt_kprintf("RS485 %s\n",stationSet);					
		}
		
		else	if (!strcmp(argv[1], "SN"))		
		{
			rt_kprintf("RS485 SN:%s\n",snSet);					
		}
	}	
	else if (argc == 3 )					
	{
		uint8_t datalen=strlen(argv[2]);
		if (!strcmp(argv[1], "ID"))
		{
			
			if(datalen<20)
			{
				at24cxx.write(at24c256 , (EEPROM_RS485_DATA_PAGE_ADDR)*64+1, (uint8_t *)argv[2], datalen);
				at24cxx.write(at24c256 , (EEPROM_RS485_DATA_PAGE_ADDR)*64+1+datalen, (uint8_t *)"\0\0",2);
				rt_kprintf("write stationID:%s ok\n",argv[2]);			
			}				
		}
		
		else	if (!strcmp(argv[1], "SN"))		
		{
			if(datalen<20)
			{
				at24cxx.write(at24c256 , (EEPROM_RS485_DATA_PAGE_ADDR)*64+32, (uint8_t *)argv[2], datalen);		
				at24cxx.write(at24c256 , (EEPROM_RS485_DATA_PAGE_ADDR)*64+32+datalen, (uint8_t *)"\0\0",2);	
				rt_kprintf("write SN:%s ok\n",argv[2]);			
			}
		}
	}	
	
	return result;	
}
INIT_APP_EXPORT(fbtmc429_idc16_rs485_init);
MSH_CMD_EXPORT(RS485, ......);
#if 0

void USART3_IRQHandler(void)
{
	rt_interrupt_enter();
	
	int ch;
	if ((__HAL_UART_GET_FLAG(&huart3, UART_FLAG_RXNE) != RESET) && (__HAL_UART_GET_IT_SOURCE(&huart3, UART_IT_RXNE) != RESET))
	{
		if (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_RXNE) != RESET)
		{
			
#if defined(USING_STM32F0_HAL) 
			ch = huart3.Instance->RDR & 0xff;
#else
			ch = huart3.Instance->DR & 0xff;
#endif
			if(ch==0x0d) 
			{
				rt_sem_release(&rx_sem);
			}
			else
			{
				USART_RX_BUF[USART_RX_STA&0X3FFF]=ch;
				USART_RX_STA++;
				if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0; 
			}		 
		}
		
    __HAL_UART_CLEAR_FLAG(&huart3, UART_FLAG_RXNE);
  }
	rt_interrupt_leave();
}


static void serial_thread_entry(void *parameter)
{
    char ch;
    while (1)
    {
        rt_sem_take(&rx_sem, RT_WAITING_FOREVER);

				u8 len=USART_RX_STA&0x3fff;	
				USART_RX_BUF[len]='\0';
									
				if(Read429Short(IDX_VACTUAL|(AXIS_Z<<5)) ==0 )				//读取信息的时候如果被其他中断调用（比如一直UART1中断命令），则SPI读取值出问题
				{
						if(AxisSpeedKeepZeroCnt++ > 1)
						{
							AxisSpeedKeepZeroCnt=0;
							closeSerial();
							rt_kprintf("motor[2] is stop and stop printing data\n>>");
						}					
				}
				else 
				{
					AxisSpeedKeepZeroCnt=0;
					if(DataRemoveCnt<2)	DataRemoveCnt++;
					else 
					{
						motorPosition[AXIS_Z]=Read429Int(IDX_XACTUAL|(AXIS_Z<<5));
						rt_kprintf("P[2]=%d,Press%s\n",motorPosition[AXIS_Z],&USART_RX_BUF);			
					}
					
					rt_pin_write(outputs_pin_num[7-1],PIN_HIGH);
					USART_RX_STA=0; 
					memset(USART_RX_BUF, '\0', USART_REC_LEN);
				}

    }
}
//
int uart_stream_thread_init(void)
{
    rt_err_t ret = RT_EOK;
		USART_RX_STA=0; 
		memset(USART_RX_BUF, '\0', USART_REC_LEN);

    rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);		

    rt_thread_t thread = rt_thread_create("uart3", serial_thread_entry, RT_NULL, 1024, 19, 20);
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
    else
    {
        ret = RT_ERROR;
    }
    return ret;	
}
#endif

#if (ENABLE_DEBUG_CODE)

void getUART3_Status(void)
{

	CMD_TRACE("SerialOpened=%d,UART_FLAG_RXNE=%d,UART_IT_RXNE=%d\n",\
	SerialOpened,__HAL_UART_GET_FLAG(&huart3, UART_FLAG_RXNE),__HAL_UART_GET_IT_SOURCE(&huart3, UART_IT_RXNE));//
}

void debug_exit(void *args)
{
	if(INPUT7==0)	 	
	{			
		getUART3_Status();
		MX_USART3_UART_Init();
		getUART3_Status();
		HAL_NVIC_DisableIRQ(USART3_IRQn);
	   __HAL_UART_DISABLE_IT(&huart3, UART_IT_RXNE);
//		list_thread();
//		list_sem();
//		list_event();
//	  list_mempool();
//	  list_timer();		
	} 
}
void InitIn7AsExti(void)
{
  rt_pin_attach_irq(inputs_pin_num[7-1], PIN_IRQ_MODE_FALLING, debug_exit, RT_NULL);
	rt_pin_irq_enable(inputs_pin_num[7-1], PIN_IRQ_ENABLE);
}
void uart3_reinit(void)
{
	//HAL_UART_AbortReceive_IT(&huart3);
	MX_SPI1_Init();
	MX_USART3_UART_Init();
}
MSH_CMD_EXPORT(openSerial,...);
MSH_CMD_EXPORT(closeSerial,...);
INIT_APP_EXPORT(InitIn7AsExti);
MSH_CMD_EXPORT(getUART3_Status,...);
MSH_CMD_EXPORT(uart3_reinit,...);
//

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t code_test_stack[ 1024 ];	//256

static struct rt_thread code_test_thread;

static void rt_hw_code_test_init(void)
{
    Write429Short(IDX_VMAX|(2<<5), MotorConfig[2].VMax);
  	SetAmaxAutoByspeed(2,MotorConfig[2].VMax);
	  
}

static void code_test_thread_entry(void *parameter)
{
		rt_hw_code_test_init();
    while (1)
    {
			 Write429Int(IDX_XTARGET|(2<<5), 3000);		 
			 Set429RampMode(2, RM_RAMP);
			 rt_thread_delay(RT_TICK_PER_SECOND / 50);
			 openSerial();
			 rt_thread_delay(RT_TICK_PER_SECOND);
			 OUTPUT8=1;
			 rt_thread_delay(RT_TICK_PER_SECOND/50);
			 OUTPUT8=0;
			 rt_thread_delay(RT_TICK_PER_SECOND/50);			
			 Write429Int(IDX_XTARGET|(2<<5), 6000);
			 Set429RampMode(2, RM_RAMP);
			 rt_thread_delay(RT_TICK_PER_SECOND / 50);
			 openSerial();
			 rt_thread_delay(RT_TICK_PER_SECOND);
			 OUTPUT8=1;
			 rt_thread_delay(RT_TICK_PER_SECOND/50);
			 OUTPUT8=0;
			 rt_thread_delay(RT_TICK_PER_SECOND/50);	
    }
}

int code_test_thread_init(void)
{
    rt_err_t result;

    /* init led thread */
    result = rt_thread_init(&code_test_thread,
                            "code_test",
                            code_test_thread_entry,
                            RT_NULL,
                            (rt_uint8_t *)&code_test_stack[0],
                            sizeof(code_test_stack),
                            20,
                            20);
    if (result == RT_EOK)
    {
       rt_thread_startup(&code_test_thread);
    }
    return 0;
}
//INIT_APP_EXPORT(code_test_thread_init);
#endif

/*		如果不使用RTT初始化 需进行调用配置  */
#if 0
UART_HandleTypeDef huart3;
static void MX_USART3_UART_Init(void);
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}
#endif
//


