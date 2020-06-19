/*	20190730：	
串口注册	int rt_hw_usart_init(void)  drv_uart.c 已修改默认只开uart1
串口中断其他标志位的清除处理	__HAL_UART_CLEAR_OREFLAG(&uart->huart)
*/
#include <rtthread.h>
#include <rtdef.h>

#include "app_gpio_dido.h"
#include "app_button.h"
#include "app_serial.h"
#include "app_eeprom_24xx.h"
#include "inc_controller.h"
#include "tmc429.h"
 
 
#define N_O_RS232							 2  

#define UART2_THREAD_PRIORITY 18
#define UART3_THREAD_PRIORITY 19

#define uart_rx_len_max 			30

int 		motorPosition[N_O_MOTORS];

static uint8_t 			alarmCh[N_O_RS232]={8,8};
static uint8_t 			whichAxis[N_O_RS232]={0,0};
static uint16_t 		inputAlarmCnt[N_O_RS232]={0,0};
static int 					next_posititon[N_O_RS232]={0,0};			//uint32 负位置会出现错误
static uint8_t 			DataUselessCnt[N_O_RS232]={0,0};			//第一個字符串有可能不全

static uint8_t 			MotorSpeedIsZeroCnt[N_O_RS232]={0,0};
/**********************************************************************************************************************/
static struct 			rt_semaphore rx_sem[N_O_RS232];	

static rt_device_t 	serialRS232[N_O_RS232];

static char 			uart_rx_buff[2][uart_rx_len_max];
static uint8_t	 	uart_rx_len_index[2]={0,0};

/**********************************************************************************************************************/

static int SerialApp_thread_init(void);
/**********************************************************************************************************************/
void  openSerial_rx_int(uint8_t whichSerial)
{
  rt_device_control(serialRS232[whichSerial], RT_DEVICE_CTRL_SET_INT, NULL);	DataUselessCnt[whichSerial]=1;	
}
void  closeSerial_rx_int(uint8_t whichSerial)
{
	rt_device_control(serialRS232[whichSerial], RT_DEVICE_CTRL_CLR_INT, NULL);	
}
int printdata(int argc, char **argv)
{
	static uint8_t Serial_ThreadInited=0;
	uint8_t whichSerial=128;
	
	if(Serial_ThreadInited==0)
	{
		Serial_ThreadInited=1;
		SerialApp_thread_init();
	}
	
	if (argc ==1)
	{
		whichSerial=255;
//		openSerial_rx_int(2);	MotorSpeedIsZeroCnt[0]=0;
//		openSerial_rx_int(3); MotorSpeedIsZeroCnt[1]=0;
	}	
	else if (argc ==2)
	{
		if 			(!strcmp(argv[1], "com1")) 	{whichSerial=0;}//openSerial_rx_int(2);MotorSpeedIsZeroCnt[0]=0;}
		else if (!strcmp(argv[1], "com2")) 	{whichSerial=1;}//openSerial_rx_int(3);MotorSpeedIsZeroCnt[1]=0;}	
    else if (!strcmp(argv[1], "stop")) 
		{
			closeSerial_rx_int(2);closeSerial_rx_int(3);
			rt_kprintf("stop printing the pressure value ok\n");		
		}	
	}
	if(whichSerial!=0)
	{
		if(g_tParam.Project_ID==OQC_FLEX || g_tParam.Project_ID==BUTTON_ROAD)
		{
			whichAxis[whichSerial]=AXIS_Z;			next_posititon[whichSerial]=Read429Int(SPI_DEV0_TMC429,IDX_XTARGET|(whichAxis[whichSerial]<<5));	
			MotorSpeedIsZeroCnt[whichSerial]=0;	openSerial_rx_int(whichSerial);
		}	
		else if(g_tParam.Project_ID==EOK_POWE_2AXIS)
		{
			whichAxis[0]=AXIS_Y;
			whichAxis[1]=AXIS_Y;
			if(whichSerial==0)	
			{
				next_posititon[0]=Read429Int(SPI_DEV0_TMC429,IDX_XTARGET|(whichAxis[0]<<5));	
				MotorSpeedIsZeroCnt[0]=0;openSerial_rx_int(2);
			}
			if(whichSerial==1)	
			{
				next_posititon[1]=Read429Int(SPI_DEV1_TMC429,IDX_XTARGET|(whichAxis[1]<<5));	
				MotorSpeedIsZeroCnt[1]=0;openSerial_rx_int(3);
			}
			else
			{
				next_posititon[0]=Read429Int(SPI_DEV0_TMC429,IDX_XTARGET|(whichAxis[0]<<5));	
				MotorSpeedIsZeroCnt[0]=0;openSerial_rx_int(2);
				next_posititon[1]=Read429Int(SPI_DEV1_TMC429,IDX_XTARGET|(whichAxis[1]<<5));	
				MotorSpeedIsZeroCnt[1]=0;openSerial_rx_int(3);			
			}
		}
	}
	return 0;
}
static void serial_data_processing(uint8_t whichSerial)
{
	if(whichSerial==0)	TMC429_DEV=SPI_DEV0_TMC429;
	if(whichSerial==1)	TMC429_DEV=SPI_DEV1_TMC429;
	
	if(Read429Short(TMC429_DEV,IDX_VACTUAL|(whichAxis[whichSerial]<<5)) ==0 )				//读取信息的时候如果被其他中断调用（比如一直UART1中断命令），则SPI读取值出问题
	{
		if(MotorSpeedIsZeroCnt[whichSerial]++ > 1)
		{
			MotorSpeedIsZeroCnt[whichSerial]=0;
			closeSerial_rx_int(whichSerial);
			rt_kprintf("motor[%d] is stop and stop printing data\n>>",whichAxis[whichSerial]);
			
		}
	}
	else 
	{
		MotorSpeedIsZeroCnt[whichSerial]=0;
		if(DataUselessCnt[whichSerial]<2)	DataUselessCnt[whichSerial]++;
		else 
		{			
			motorPosition[whichAxis[whichSerial]]=Read429Int(TMC429_DEV,IDX_XACTUAL|(whichAxis[whichSerial]<<5));
			if(next_posititon[whichSerial] > motorPosition[whichAxis[whichSerial]] && buttonRESETpressed==FALSE)
			{
				if(inputGet(alarmCh[whichSerial])==0) inputAlarmCnt[whichSerial]++; else inputAlarmCnt[whichSerial]=0;
			
				if(inputAlarmCnt[whichSerial]>=3) {HardStop(TMC429_DEV,whichAxis[whichSerial]);inputAlarmCnt[whichSerial]=0;}
			}
			if(whichSerial==0)	rt_kprintf("P[%d]=%d,Press%s\n",whichAxis[whichSerial],motorPosition[whichAxis[whichSerial]],&uart_rx_buff[whichSerial]);			
			if(whichSerial==1)	rt_kprintf("P[%d]=%d,Press%s\n",whichAxis[whichSerial]+3,motorPosition[whichAxis[whichSerial]],&uart_rx_buff[whichSerial]);			
			
		}
		uart_rx_len_index[whichSerial]=0; 
		memset(uart_rx_buff[whichSerial], '\0', uart_rx_len_max);
		
	}		
}
/**********************************************************************************************************************/

static char*	uart_name[N_O_RS232]={"uart2","uart3"};
static char*	rx_sem_name[N_O_RS232]={"rx2_sem","rx3_sem"};

static rt_err_t uart2_input(rt_device_t dev, rt_size_t size)
{
	rt_sem_release(&rx_sem[0]);
	return RT_EOK;
}
static rt_err_t uart3_input(rt_device_t dev, rt_size_t size)
{
	rt_sem_release(&rx_sem[1]);
	return RT_EOK;
}
static void serial2_thread_entry(void *parameter)
{
	char ch;
	while (1)
	{
		while (rt_device_read(serialRS232[0], -1, &ch, 1) != 1)
		{
				rt_sem_take(&rx_sem[0], RT_WAITING_FOREVER);
		}	
		rt_enter_critical();
		if(ch==0x0d || ch==0x0a) 
		{
			if(uart_rx_len_index[0]!=0)
			serial_data_processing(0);
		}
		else
		{
			if(ch!=0x0d && ch!=0x0a) 
			{
				uart_rx_buff[0][uart_rx_len_index[0]]=ch;
				uart_rx_len_index[0]++;
			}
			if(uart_rx_len_index[0] > (uart_rx_len_max-1) ) uart_rx_len_index[0]=0; 
		}		
		rt_exit_critical();								
	}
}
static void serial3_thread_entry(void *parameter)
{
	char ch;
	while (1)
	{
		while (rt_device_read(serialRS232[1], -1, &ch, 1) != 1)
		{
				rt_sem_take(&rx_sem[1], RT_WAITING_FOREVER);
		}	
		rt_enter_critical();
		if(ch==0x0d || ch==0x0a) 
		{
			if(uart_rx_len_index[0]!=0)
			serial_data_processing(1);
		}
		else
		{
			if(ch!=0x0d && ch!=0x0a) 
			{
				uart_rx_buff[1][uart_rx_len_index[1]]=ch;
				uart_rx_len_index[1]++;
			}
			if(uart_rx_len_index[1] > (uart_rx_len_max-1) ) uart_rx_len_index[1]=0; 
		}		
		rt_exit_critical();								
	}
}
//默认注册初始化2个串口设备,具体中断接收按需求配置
static int SerialApp_thread_init(void)
{
	uint8_t i;
	
	for(i=0;i<N_O_RS232;i++)
	{
		memset(uart_rx_buff[i], '\0', uart_rx_len_max);
		serialRS232[i] = rt_device_find(uart_name[i]);
		if (!serialRS232[i])
		{
				rt_kprintf("find %s failed!\n",uart_name[i]);
				return RT_ERROR;
		}			
		rt_sem_init(&rx_sem[i], rx_sem_name[i], 0, RT_IPC_FLAG_FIFO);	

		rt_device_open(serialRS232[i], RT_DEVICE_FLAG_INT_RX);		

		rt_device_control(serialRS232[i], RT_DEVICE_CTRL_CLR_INT, NULL);	
		
		if(i==0)
		{
			rt_device_set_rx_indicate(serialRS232[i], uart2_input);
		  rt_thread_t thread = rt_thread_create(uart_name[i], serial2_thread_entry, RT_NULL, 1024, UART2_THREAD_PRIORITY, 20);
			if (thread != RT_NULL)	rt_thread_startup(thread);
			else	
			{
				rt_kprintf("rt_thread_create %s failed!\n",uart_name[i]);
				return RT_ERROR;	
			}
		}
			
		if(i==1)
		{
			rt_device_set_rx_indicate(serialRS232[i], uart3_input);
		  rt_thread_t thread = rt_thread_create(uart_name[i], serial3_thread_entry, RT_NULL, 1024, UART3_THREAD_PRIORITY, 20);
			if (thread != RT_NULL)	rt_thread_startup(thread);
			else	
			{
				rt_kprintf("rt_thread_create %s failed!\n",uart_name[i]);
				return RT_ERROR;	
			}
		}		
		//MX_USART2_UART_Init();	MX_USART3_UART_Init();
	}
	rt_kprintf("SerialApp_thread_init...ok\n");
	return	RT_EOK;
}
/******************************************finsh***********************************************************************/
//MSH_CMD_EXPORT(printdata, auto print position and press);
/*********************************************************************************************************************/



