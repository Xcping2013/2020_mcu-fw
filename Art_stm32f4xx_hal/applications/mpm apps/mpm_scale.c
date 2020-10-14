
#include "mpm.h"

#define UART3_THREAD_PRIORITY 16
#define uart_rx_len_max 100
static char uart_rx_buff[uart_rx_len_max];
static uint8_t uart_rx_len_index=0;
/**********************************************************************************************************************/
static rt_device_t serial;
static char uart_name[]="uart3"; 
static struct rt_semaphore rx_sem;	

//struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT; 
/**********************************************************************************************************************/
int scale_stream_thread_init(void);
static void scale_thread_entry(void *parameter);
/**********************************************************************************************************************/
void  scale_rx_enable(void)
{
	rt_device_control(serial, RT_DEVICE_CTRL_SET_INT, NULL);	
}

void  scale_rx_disable(void)
{
	rt_device_control(serial, RT_DEVICE_CTRL_CLR_INT, NULL);	
}


int scale(int argc, char **argv)
{
	static uint8_t scale_inited=0;
	
  if(scale_inited==0)
	{
		scale_inited=1;
		//scale_stream_thread_init();
	}
	if (argc ==1)
	{
		rt_kprintf("usage:\n");
		rt_kprintf("scale rx_enable:\n");
		rt_kprintf("scale rx_disable:\n");
		rt_kprintf("scale tx <cmds>\n");
		return RT_ERROR;
	}	
	if (argc ==2)
	{
		if 			(!strcmp(argv[1], "rx_enable")) 	scale_rx_enable();
		else if (!strcmp(argv[1], "rx_disable")) 	scale_rx_disable();

		rt_kprintf("mfc rx set ok\n");
		return RT_EOK;
	}	
	if (argc > 2)
	{
		if(!strcmp(argv[1], "tx")) 
		{
			rt_device_write(serial, 0, argv[2],strlen(argv[2]));	
			rt_device_write(serial, 0, "\n",1);			
			//rt_kprintf("scale tx %s len=%d ok\n",argv[2],strlen(argv[2]));
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
static void scale_thread_entry(void *parameter)
{
    char ch;
    while (1)
    {	
        while (rt_device_read(serial, -1, &ch, 1) != 1)
        {
            rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
        }	
//				rt_kprintf("%c",ch);	
				//rt_kprintf("mfc rx:%c\n",ch);	
				//rt_enter_critical();
				
				if(ch=='\r' || ch=='\n') 
				{
					if(uart_rx_len_index!=0)
					{
						rt_kprintf("scale rx data:%s\n",uart_rx_buff);		
						uart_rx_len_index=0; 
						memset(uart_rx_buff, '\0', uart_rx_len_max);					
					}
				}
				else
				{
					//if(ch!=0x0a && ch!=0x0d) 
					{
						uart_rx_buff[uart_rx_len_index]=ch;//-'0';
						//rt_kprintf("mfc rx:%c\n",ch);		
						uart_rx_len_index++;
					}
					if(uart_rx_len_index > uart_rx_len_max) uart_rx_len_index=0; 
				}	
				//rt_exit_critical();				
    }
}
//
int scale_stream_thread_init(void)
{
    rt_err_t ret = RT_EOK;
	
		memset(uart_rx_buff, '\0', uart_rx_len_max);
    serial = rt_device_find(uart_name);
	
    if (!serial)
    {
        rt_kprintf("find %s failed!\n",uart_name);
        return RT_ERROR;
    }

    rt_sem_init(&rx_sem, "rx3_sem", 0, RT_IPC_FLAG_FIFO);	

		rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);		
		
		rt_device_control(serial, RT_DEVICE_CTRL_CLR_INT, NULL);	
	
		rt_device_set_rx_indicate(serial, uart_input);				
		
    rt_thread_t thread = rt_thread_create("serial", scale_thread_entry, RT_NULL, 1024, UART3_THREAD_PRIORITY, 100);
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

INIT_APP_EXPORT(scale_stream_thread_init);

MSH_CMD_EXPORT(scale, ...);

















