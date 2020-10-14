#include "mpm.h"

/* MFC

mfc rx data: *@=@ ; A ;*@=A;

	Pressure 	Temp 			VolFlow 	Mass Flow 	Set Point 							Gas

SETPT-SOURCE-->Seral

A32000
A +014.66	  +025.49 	-0.0006 	-0.0006 	  +0.7905 	 +0000.507    Air TMF
 
A64000	FULL SCALE
A +014.66 +030.54 +0.0000 +0.0000 +5.0000 +0000.000    Air

*/


#define UART6_THREAD_PRIORITY 16
#define uart_rx_len_max 100
static char uart_rx_buff[uart_rx_len_max];
static uint8_t uart_rx_len_index=0;
/**********************************************************************************************************************/
static rt_device_t serial;
static char uart_name[]="uart6"; 
static struct rt_semaphore rx_sem;	

struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT; 
/**********************************************************************************************************************/
int mfc_stream_thread_init(void);
static void mfc_thread_entry(void *parameter);
/**********************************************************************************************************************/
void  mfc_rx_enable(void)
{
	rt_device_control(serial, RT_DEVICE_CTRL_SET_INT, NULL);	
}

void  mfc_rx_disable(void)
{
	rt_device_control(serial, RT_DEVICE_CTRL_CLR_INT, NULL);	
}


int mfc(int argc, char **argv)
{
	static uint8_t mfc_inited=0;
	
  if(mfc_inited==0)
	{
		mfc_inited=1;
		//mfc_stream_thread_init();
	}	
	if (argc ==2)
	{
		if 			(!strcmp(argv[1], "rx_enable")) 	mfc_rx_enable();
		else if (!strcmp(argv[1], "rx_disable")) 	mfc_rx_disable();

		rt_kprintf("mfc rx set ok\n");
		return RT_EOK;
	}	
	if (argc ==3)
	{
		if(!strcmp(argv[1], "tx")) 
		{
			rt_device_write(serial, 0, argv[2],strlen(argv[2]));	
			rt_device_write(serial, 0, "\r",1);	
			//rt_kprintf("mfc tx %s len=%d ok\n",argv[2],strlen(argv[2]));
			return RT_EOK;
		}	
		if(!strcmp(argv[1], "set")) 
		{
			uint16_t setValue=atoi(argv[2]);
			
			char Tsfdata[10]="";
			
			sprintf(Tsfdata,"A",(setValue/100.0)*64000);
			
			rt_device_write(serial, 0, Tsfdata,strlen(Tsfdata));	
			//rt_device_write(serial, 0, argv[2],strlen(argv[2]));	
			rt_device_write(serial, 0, "\r",1);	
			//rt_kprintf("mfc tx %s len=%d ok\n",argv[2],strlen(argv[2]));
			return RT_EOK;
		}	
		return RT_ERROR;
	}		
	else
	{
		rt_kprintf("usage:\n");
		
		rt_kprintf("mfc set n%:\n");
//		rt_kprintf("mfc rx_enable:\n");
//		rt_kprintf("mfc rx_disable:\n");
//		rt_kprintf("mfc tx <cmds>\n");
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
static void mfc_thread_entry(void *parameter)
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
						rt_kprintf("mfc rx data:%s\n",uart_rx_buff);		
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
int mfc_stream_thread_init(void)
{
    rt_err_t ret = RT_EOK;
	
		memset(uart_rx_buff, '\0', uart_rx_len_max);
    serial = rt_device_find(uart_name);
	
    if (!serial)
    {
        rt_kprintf("find %s failed!\n",uart_name);
        return RT_ERROR;
    }

    rt_sem_init(&rx_sem, "rx6_sem", 0, RT_IPC_FLAG_FIFO);	

		rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);		
		
		rt_device_control(serial, RT_DEVICE_CTRL_CLR_INT, NULL);	
	
		rt_device_set_rx_indicate(serial, uart_input);				
		
    rt_thread_t thread = rt_thread_create("serial", mfc_thread_entry, RT_NULL, 1024, UART6_THREAD_PRIORITY, 100);
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

void mfc_test(void)
{
	
	
	
}

INIT_APP_EXPORT(mfc_stream_thread_init);

MSH_CMD_EXPORT(mfc, ...);

















