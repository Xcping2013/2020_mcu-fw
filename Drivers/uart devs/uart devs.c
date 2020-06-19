#include "mpm.h"

#define UART_THREAD_PRIORITY 16

#define UART_RX_LEN_MAX 100
static char uart_rx_buff[UART_RX_LEN_MAX];
static uint8_t uart_rx_len_index=0;
/**********************************************************************************************************************/
static rt_device_t serial;
static struct rt_semaphore rx_sem;	
static char uart_name[]="uart6"; 
static char rx_sem_name[]="rx_sem"; 
//struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT; 
/**********************************************************************************************************************/
int uart_dev1_stream_thread_init(void);
static void uart_dev1_thread_entry(void *parameter);
/**********************************************************************************************************************/
void  uart_dev1_rx_enable(void)
{
	rt_device_control(serial, RT_DEVICE_CTRL_SET_INT, NULL);	
}

void  uart_dev1_rx_disable(void)
{
	rt_device_control(serial, RT_DEVICE_CTRL_CLR_INT, NULL);	
}

int uart_dev1(int argc, char **argv)
{
	static uint8_t uart_dev1_inited=0;
	
  if(uart_dev1_inited==0)
	{
		uart_dev1_inited=1;
	}
	if (argc ==1)
	{
		rt_kprintf("usage:\n");
		rt_kprintf("uart_dev1 rx_enable:\n");
		rt_kprintf("uart_dev1 rx_disable:\n");
		rt_kprintf("uart_dev1 tx <cmds>\n");
		return RT_ERROR;
	}	
	if (argc ==2)
	{
		if 			(!strcmp(argv[1], "rx_enable")) 	uart_dev1_rx_enable();
		else if (!strcmp(argv[1], "rx_disable")) 	uart_dev1_rx_disable();

		rt_kprintf("uart_dev1 rx set ok\n");
		return RT_EOK;
	}	
	if (argc > 2)
	{
		if(!strcmp(argv[1], "tx")) 
		{
			rt_device_write(serial, 0, argv[2],strlen(argv[2]));	
			rt_device_write(serial, 0, "\n",1);			
			//rt_kprintf("uart_dev1 tx %s len=%d ok\n",argv[2],strlen(argv[2]));
			return RT_EOK;
		}	
		return RT_ERROR;
	}		
	return RT_EOK;
}
/**********************************************************************************************************************/
static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
	rt_sem_release(&rx_sem);
	return RT_EOK;
}
static void uart_dev1_thread_entry(void *parameter)
{
    char ch;
    while (1)
    {	
        while (rt_device_read(serial, -1, &ch, 1) != 1)
        {
            rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
        }	
				//rt_enter_critical();
				
				if(ch=='\r' || ch=='\n') 
				{
					if(uart_rx_len_index!=0)
					{
						rt_kprintf("uart_dev1 rx data:%s\n",uart_rx_buff);		
						uart_rx_len_index=0; 
						memset(uart_rx_buff, '\0', UART_RX_LEN_MAX);					
					}
				}
				else
				{
					//if(ch!=0x0a && ch!=0x0d) 
					{
						uart_rx_buff[uart_rx_len_index]=ch;
						uart_rx_len_index++;
					}
					if(uart_rx_len_index > UART_RX_LEN_MAX) uart_rx_len_index=0; 
				}	
				//rt_exit_critical();				
    }
}
//
int uart_dev1_stream_thread_init(void)
{
    rt_err_t ret = RT_EOK;
	
		memset(uart_rx_buff, '\0', UART_RX_LEN_MAX);
    serial = rt_device_find(uart_name);
	
    if (!serial)
    {
        rt_kprintf("find %s failed!\n",uart_name);
        return RT_ERROR;
    }

    rt_sem_init(&rx_sem, &rx_sem_name, 0, RT_IPC_FLAG_FIFO);	

		rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);		
		
		rt_device_control(serial, RT_DEVICE_CTRL_CLR_INT, NULL);	
	
		rt_device_set_rx_indicate(serial, uart_input);				
		
    rt_thread_t thread = rt_thread_create("serial", uart_dev1_thread_entry, RT_NULL, 1024, UART6_THREAD_PRIORITY, 100);
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
			
				rt_device_control(serial, RT_DEVICE_CTRL_SET_INT, NULL);	
    }
    else
    {
        ret = RT_ERROR;
    }
    return ret;	
}

INIT_APP_EXPORT(uart_dev1_stream_thread_init);

MSH_CMD_EXPORT(uart_dev1, ...);

















