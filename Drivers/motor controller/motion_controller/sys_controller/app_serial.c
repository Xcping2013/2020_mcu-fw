/*

20190730：

rt_hw_serial_register(&serial3, "uart3",RT_DEVICE_FLAG_RDONLY | RT_DEVICE_FLAG_INT_RX,uart);

uart_isr UART_FLAG_ORE等的清除

		if (__HAL_UART_GET_FLAG(&(uart->huart), UART_FLAG_ORE) != RESET)
		{
				__HAL_UART_CLEAR_OREFLAG(&uart->huart);
		}
		if (__HAL_UART_GET_FLAG(&(uart->huart), UART_FLAG_NE) != RESET)
		{
				__HAL_UART_CLEAR_NEFLAG(&uart->huart);
		}
		if (__HAL_UART_GET_FLAG(&(uart->huart), UART_FLAG_FE) != RESET)
		{
				__HAL_UART_CLEAR_FEFLAG(&uart->huart);
		}
		if (__HAL_UART_GET_FLAG(&(uart->huart), UART_FLAG_PE) != RESET)
		{
				__HAL_UART_CLEAR_PEFLAG(&uart->huart);
		}
*/
#include "app_sys_control.h"



static uint8_t whichAxis=0;
static uint16_t input8Cnt=0;
static int next_posititon=0;	//uint32 负位置会出现错误
/**********************************************************************************************************************/
static struct 				rt_semaphore rx_sem;	
static uint8_t 				DataRemoveCnt=0;					//第一個字符串有可能不全
static uint8_t 				AxisSpeedKeepZeroCnt=0;

volatile uint8_t 			SerialOpened=0;
static rt_device_t 		serial3;

#define UART3_THREAD_PRIORITY 19
#define uart_rx_len_max 			30
static char 			uart_rx_buff[uart_rx_len_max];
static uint8_t	 	uart_rx_len_index=0;
struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT; 
/**********************************************************************************************************************/
static void serial_thread_entry(void *parameter);
static void serial3_data_processing(void);
/**********************************************************************************************************************/
void  openSerial(void)
{
	rt_device_control(serial3, RT_DEVICE_CTRL_SET_INT, NULL);	
	DataRemoveCnt=1;	
	SerialOpened=TRUE; 
}
void  closeSerial(void)
{
	rt_device_control(serial3, RT_DEVICE_CTRL_CLR_INT, NULL);	
}
int printdata(int argc, char **argv)
{
	if (argc !=1)
	{
		CMD_TRACE("Please input: printdata\n");
		return RT_ERROR;
	}	
	AxisSpeedKeepZeroCnt=0;
	if(g_tParam.Project_ID==OQC_FLEX || g_tParam.Project_ID==BUTTON_ROAD)
	{
		whichAxis=AXIS_Z;
	}	
	else if(g_tParam.Project_ID==EOK_POWE_2AXIS)
	{
		whichAxis=AXIS_Y;
	}
	next_posititon=Read429Int(IDX_XTARGET|(whichAxis<<5));	
	openSerial();
	return 0;
}
int printdata_disable(int argc, char **argv)
{
	if (argc !=2)
	{
		CMD_TRACE("Please input: stop printdata\n");
		return RT_ERROR;
	}	
	if (!strcmp(argv[1], "printdata"))
	{
		closeSerial();
		printf("stop printing the pressure value ok\n");		
	}
	return 0;
}
static void serial3_data_processing(void)
{
	if(Read429Short(IDX_VACTUAL|(whichAxis<<5)) ==0 )				//读取信息的时候如果被其他中断调用（比如一直UART1中断命令），则SPI读取值出问题
	{
		if(AxisSpeedKeepZeroCnt++ > 1)
		{
			AxisSpeedKeepZeroCnt=0;
			closeSerial();
			rt_kprintf("motor[%d] is stop and stop printing data\n>>",whichAxis);
			
		}
	}
	else 
	{
		AxisSpeedKeepZeroCnt=0;
		if(DataRemoveCnt<2)	DataRemoveCnt++;
		else 
		{			
			motorPosition[whichAxis]=Read429Int(IDX_XACTUAL|(whichAxis<<5));
			if(next_posititon > motorPosition[whichAxis] && buttonRESETpressed==FALSE)
			{
				if(INPUT8==0) input8Cnt++; else input8Cnt=0;
			
				if(input8Cnt>=3) {HardStop(whichAxis);input8Cnt=0;}
			}
			rt_kprintf("P[%d]=%d,Press%s\n",whichAxis,motorPosition[whichAxis],&uart_rx_buff);			
			
		}
		uart_rx_len_index=0; 
		memset(uart_rx_buff, '\0', uart_rx_len_max);
		
	}		
}
/**********************************************************************************************************************/



	static char 			uart_name[]="uart3";

	static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
	{
		rt_sem_release(&rx_sem);
		return RT_EOK;
	}
	static void serial_thread_entry(void *parameter)
	{
			char ch;
			while (1)
			{
					while (rt_device_read(serial3, -1, &ch, 1) != 1)
					{
							rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
					}	
					rt_enter_critical();
					if(ch==0x0d || ch==0x0a) 
					{
						if(uart_rx_len_index!=0)
						serial3_data_processing();
					}
					else
					{
						if(ch!=0x0d && ch!=0x0a) 
						{
							uart_rx_buff[uart_rx_len_index]=ch;
							uart_rx_len_index++;
						}
						if(uart_rx_len_index > (uart_rx_len_max-1) ) uart_rx_len_index=0; 
					}		
					rt_exit_critical();								
			}
	}
//
	static int uart3_thread_init(void)
	{
			rt_err_t ret = RT_EOK;
		
			memset(uart_rx_buff, '\0', uart_rx_len_max);
		
			serial3 = rt_device_find(uart_name);
		
			if (!serial3)
			{
					rt_kprintf("find %s failed!\n",uart_name);
					return RT_ERROR;
			}

			rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);	

			rt_device_open(serial3, RT_DEVICE_FLAG_INT_RX);		

			rt_device_control(serial3, RT_DEVICE_CTRL_CLR_INT, NULL);	
			
			rt_device_set_rx_indicate(serial3, uart_input);				

			//MX_USART3_UART_Init();
			
			rt_thread_t thread = rt_thread_create("serial3", serial_thread_entry, RT_NULL, 1024, UART3_THREAD_PRIORITY, 20);
			
			
			if (thread != RT_NULL)
			{
					rt_thread_startup(thread);
			}
			else
			{
					ret = RT_ERROR;
			}
			rt_kprintf("uart3_thread_init()...ok\n");
			return ret;			
	}

/******************************************finsh***********************************************************************/
//INIT_APP_EXPORT(uart3_thread_init);
MSH_CMD_EXPORT(printdata, auto print position and press);
/*********************************************************************************************************************/



