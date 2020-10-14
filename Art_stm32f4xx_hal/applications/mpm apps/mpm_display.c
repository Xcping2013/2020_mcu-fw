
#include "mpm.h"
#include "mpm_display.h"

#define UART2_THREAD_PRIORITY 16
#define uart_rx_len_max 100
static char uart_rx_buff[uart_rx_len_max];
static uint8_t uart_rx_len_index=0;
/**********************************************************************************************************************/
static rt_device_t serial;
static char uart_name[]="uart2"; 
static struct rt_semaphore rx_sem;	

//struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT; 
/**********************************************************************************************************************/
int display_uart_thread_init(void);
static void display_thread_entry(void *parameter);
/**********************************************************************************************************************/
void  display_rx_enable(void)
{
	rt_device_control(serial, RT_DEVICE_CTRL_SET_INT, NULL);	
}

void  display_rx_disable(void)
{
	rt_device_control(serial, RT_DEVICE_CTRL_CLR_INT, NULL);	
}


int gfx(int argc, char **argv)
{
	static uint8_t display_inited=0;
	
  if(display_inited==0)
	{
		display_inited=1;
	}
	if (argc ==1)
	{
		rt_kprintf("usage:\n");
		
		rt_kprintf("gfx sys_GetModel:\n");
		rt_kprintf("gfx sys_GetVersion:\n");
		rt_kprintf("gfx sys_GetPmmC:\n");
		
		rt_kprintf("gfx gfx_Cls:\n");
		rt_kprintf("gfx gfx_reset:\n");
		rt_kprintf("gfx gfx_BGcolour 0-65535:\n");
		
//		rt_kprintf("gfx rx_enable:\n");
//		rt_kprintf("gfx rx_disable:\n");
//		rt_kprintf("gfx tx <cmds>\n");
		return RT_ERROR;
	}	
	if (argc ==2)
	{
		uint8_t txtem[4]={0xFF,0x42,0x00,0x10};

		if 			(!strcmp(argv[1], "rx_enable")) 	display_rx_enable();
		else if (!strcmp(argv[1], "rx_disable")) 	display_rx_disable();

		else if (!strcmp(argv[1], "sys_GetModel")) 	
		{
			txtem[0]=0x00;txtem[1]=0x1A;
			rt_device_write(serial, 0, txtem,2);	
		}
		else if (!strcmp(argv[1], "sys_GetVersion")) 	
		{
			txtem[0]=0x00;txtem[1]=0x1B;
			rt_device_write(serial, 0, txtem,2);	
		}
		else if (!strcmp(argv[1], "sys_GetPmmC")) 	
		{
			txtem[0]=0x00;txtem[1]=0x1C;
			rt_device_write(serial, 0, txtem,2);	
		}
		
		else if (!strcmp(argv[1], "gfx_Cls")) 	
		{
			txtem[0]=0xFF;txtem[1]=0x82;
			rt_device_write(serial, 0, txtem,2);	
		}
		else if (!strcmp(argv[1], "gfx_reset")) 	
		{
			pinReset(DISPLAY_RESET_PIN);
			rt_thread_mdelay(10);
			pinSet(DISPLAY_RESET_PIN);
		}
//		rt_kprintf("gfx rx set ok\n");
		return RT_EOK;
	}	
	
	if (argc == 3)
	{
		uint8_t txtem[4]={0xFF,0x42,0x00,0x10};
		if(!strcmp(argv[1], "tx")) 
		{
			rt_device_write(serial, 0, argv[2],strlen(argv[2]));	
			rt_device_write(serial, 0, "\n",1);			
			//rt_kprintf("display tx %s len=%d ok\n",argv[2],strlen(argv[2]));
			return RT_EOK;
		}	
		if(!strcmp(argv[1], "gfx_BGcolour")) 
		{
			txtem[0]=0xFF;	txtem[1]=0x48;
			txtem[2]=atoi(argv[2])>>8;
			txtem[3]=atoi(argv[2]);
			rt_device_write(serial, 0, txtem,4);	
			
			rt_thread_mdelay(10);
			txtem[0]=0xFF;txtem[1]=0x82;
			rt_device_write(serial, 0, txtem,2);	
			
			return RT_EOK;
		}	
		if(!strcmp(argv[1], "ScreenMode")) 
		{
			uint8_t txtem[4]={0xFF,0x42,0x00,0x10};
			txtem[3]=atoi(argv[2]);
			//he response will be 0x06, 0x03, 0x03  771 (0x03, 0x03)
			rt_device_write(serial, 0, txtem,4);	
			return RT_EOK;
		}	
		return RT_ERROR;
	}		
	if (argc >= 4)
	{
		if(!strcmp(argv[1], "send")) 
		{
			uint8_t i;
			uint8_t cmdTem[20]={0};
			for(i=0;i<argc-2;i++)
			{
				if(!strncmp(argv[2+i], "0x",2))
				{
					cmdTem[i]=atoi((uint8_t *)argv[2+i][2]);
				}
			}
			rt_device_write(serial, 0, (uint8_t *)cmdTem[0],argc-2);	
			rt_device_write(serial, 0, "\n",1);			
			//rt_kprintf("display tx %s len=%d ok\n",argv[2],strlen(argv[2]));
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
static void display_thread_entry(void *parameter)
{
    char ch;
		pinMode(DISPLAY_RESET_PIN,PIN_MODE_OUTPUT_OD);
		pinSet(DISPLAY_RESET_PIN);
    while (1)
    {	
        while (rt_device_read(serial, -1, &ch, 1) != 1)
        {
            rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
        }	
				rt_kprintf("gfx: 0x%x \n",ch);
//				rt_kprintf("%c",ch);	
				//rt_kprintf("mfc rx:%c\n",ch);	
				//rt_enter_critical();
#if 0				
				if(ch=='\r' || ch=='\n') 
				{
					if(uart_rx_len_index!=0)
					{
						rt_kprintf("gfx:%s\n",uart_rx_buff);		
						uart_rx_len_index=0; 
						memset(uart_rx_buff, '\0', uart_rx_len_max);					
					}
				}
				else
				{
					//if(ch!=0x0a && ch!=0x0d) 
					{
						uart_rx_buff[uart_rx_len_index]=ch;
						//rt_kprintf("mfc rx:%c\n",ch);		
						uart_rx_len_index++;
					}
					if(uart_rx_len_index > uart_rx_len_max) uart_rx_len_index=0; 
				}	
				//rt_exit_critical();	
#endif				
    }
}
//
int display_uart_thread_init(void)
{
    rt_err_t ret = RT_EOK;
	
		memset(uart_rx_buff, '\0', uart_rx_len_max);
    serial = rt_device_find(uart_name);
	
    if (!serial)
    {
        rt_kprintf("find %s failed!\n",uart_name);
        return RT_ERROR;
    }

    rt_sem_init(&rx_sem, "rx2_sem", 0, RT_IPC_FLAG_FIFO);	

		rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);		
		
		rt_device_control(serial, RT_DEVICE_CTRL_CLR_INT, NULL);	
	
		rt_device_set_rx_indicate(serial, uart_input);				
		
    rt_thread_t thread = rt_thread_create("serial", display_thread_entry, RT_NULL, 1024, UART2_THREAD_PRIORITY, 100);
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

/*
*/
INIT_APP_EXPORT(display_uart_thread_init);

void sys_GetPmmC(void)
{
	uint8_t txtem[2]={0x00,0x1C};
	//he response will be 0x06, 0x03, 0x03  771 (0x03, 0x03)
	rt_device_write(serial, 0, txtem,2);	
		
}
void gfx_Cls(void)
{
	uint8_t txtem[2]={0xFF,0x82};
	//he response will be 0x06, 0x03, 0x03  771 (0x03, 0x03)
	rt_device_write(serial, 0, txtem,2);	
		
}

void gfx_BGcolour(uint16_t colour)
{
	uint8_t txtem[4]={0xFF,0x48,0x00,0x10};
	
	txtem[2]=colour>>8;
	txtem[3]=colour;
	//he response will be 0x06, 0x03, 0x03  771 (0x03, 0x03)
	rt_device_write(serial, 0, txtem,4);	
		
}
int BG(int argc, char **argv)
{
	if (argc ==2)
	{
		gfx_BGcolour(atoi(argv[1]));
		return RT_EOK;
	}		
	
}
MSH_CMD_EXPORT(gfx, ...);
//MSH_CMD_EXPORT(sys_GetPmmC, ...);


//MSH_CMD_EXPORT(BG, ...);















