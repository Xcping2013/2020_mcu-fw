#include "bsp_mcu_gpio.h"
#include "mpm.h"

#define UART5_THREAD_PRIORITY 16
#define uart_rx_len_max 100
static char uart_rx_buff[uart_rx_len_max];
static uint8_t uart_rx_len_index=0;
/**********************************************************************************************************************/
static rt_device_t serial;
static char uart_name[]="uart5"; 
static struct rt_semaphore rx_sem;	

static uint8_t realy_pin_inited=0;
//struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT; 
/**********************************************************************************************************************/
int dut_tsi_stream_thread_init(void);
static void dut_tsi_thread_entry(void *parameter);
void  dut_tsi_rx_enable(void);
/**********************************************************************************************************************/
void realyPinInit(void)
{
	if(realy_pin_inited==0)
	{
		rt_pin_mode(DUT_5V_EN_PIN,PIN_MODE_OUTPUT);
		rt_pin_write(DUT_5V_EN_PIN, PIN_LOW);
		rt_pin_mode(DUT_UART_SEL_PIN,PIN_MODE_OUTPUT);
		rt_pin_write(DUT_UART_SEL_PIN, PIN_HIGH);
		
		dut_tsi_rx_enable();
		
		realy_pin_inited=1;
	}
	
}

void  dut_tsi_rx_enable(void)
{
	rt_device_control(serial, RT_DEVICE_CTRL_SET_INT, NULL);	
}

void  dut_tsi_rx_disable(void)
{
	rt_device_control(serial, RT_DEVICE_CTRL_CLR_INT, NULL);	
}


int dut(int argc, char **argv)
{	
	//realyPinInit();
	//pinSet(DUT_5V_EN_PIN);
	pinSet(DUT_UART_SEL_PIN);
	if (argc ==1)
	{
		rt_kprintf("usage:\n");
		rt_kprintf("dut rx_enable:\n");
		rt_kprintf("dut rx_disable:\n");
		rt_kprintf("dut tx <cmds>\n");
		return RT_ERROR;
	}	
	if (argc ==2)
	{
		if 			(!strcmp(argv[1], "rx_enable")) 	dut_tsi_rx_enable();
		else if (!strcmp(argv[1], "rx_disable")) 	dut_tsi_rx_disable();

		rt_kprintf("dut rx set ok\n");
		return RT_EOK;
	}	
	if (argc > 2)
	{
		if(!strcmp(argv[1], "tx")) 
		{
			rt_device_write(serial, 0, argv[2],strlen(argv[2]));	
			rt_device_write(serial, 0, "\n",1);			
			//rt_kprintf("dut tx %s len=%d ok\n",argv[2],strlen(argv[2]));
			return RT_EOK;
		}	
		if((!strcmp(argv[1], "5v")) &&(!strcmp(argv[2], "on")))		
		{pinSet(DUT_5V_EN_PIN); rt_kprintf("DUT_5V_EN_PIN=1\n");return RT_EOK;}
		
		if((!strcmp(argv[1], "5v")) &&(!strcmp(argv[2], "off")))	
		{pinReset(DUT_5V_EN_PIN);rt_kprintf("DUT_5V_EN_PIN=0\n");return RT_EOK;}
		
		return RT_ERROR;
	}		
	return RT_EOK;
}

//
int tsi(int argc, char **argv)
{
	//pinReset(DUT_5V_EN_PIN);
	pinReset(DUT_UART_SEL_PIN);
	if (argc ==1)
	{
		rt_kprintf("usage:\n");
		rt_kprintf("tsi rx_enable:\n");
		rt_kprintf("tsi rx_disable:\n");
		rt_kprintf("tsi tx <cmds>\n");
		return RT_ERROR;
	}	
	if (argc ==2)
	{
		if 			(!strcmp(argv[1], "rx_enable")) 	dut_tsi_rx_enable();
		else if (!strcmp(argv[1], "rx_disable")) 	dut_tsi_rx_disable();

		rt_kprintf("tsi rx set ok\n");
		return RT_EOK;
	}	
	if (argc > 2)
	{
		if(!strcmp(argv[1], "tx")) 
		{
			rt_device_write(serial, 0, argv[2],strlen(argv[2]));	
			rt_device_write(serial, 0, "\r\n",2);			
			//rt_kprintf("tsi tx %s len=%d ok\n",argv[2],strlen(argv[2]));
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
static void dut_tsi_thread_entry(void *parameter)
{
    char ch;
	
	  pinMode(DUT_5V_EN_PIN, PIN_MODE_OUTPUT);  
	
		pinReset(DUT_5V_EN_PIN);
	
		realyPinInit();
	
    while (1)
    {	
        while (rt_device_read(serial, -1, &ch, 1) != 1)
        {
            rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
        }	
				rt_enter_critical();
				
				if(ch=='\r' || ch=='\n') 
				{
					if(uart_rx_len_index!=0)
					{
						rt_kprintf("dut|tsi rx data:%s\n",uart_rx_buff);		
						uart_rx_len_index=0; 
						memset(uart_rx_buff, '\0', uart_rx_len_max);					
					}
				}
				else
				{
					//if(ch!=0x0a && ch!=0x0d) 
					{
						uart_rx_buff[uart_rx_len_index]=ch;
						uart_rx_len_index++;
					}
					if(uart_rx_len_index > uart_rx_len_max) uart_rx_len_index=0; 
				}	
				rt_exit_critical();				
    }
}
//
int dut_tsi_stream_thread_init(void)
{
    rt_err_t ret = RT_EOK;
	
		memset(uart_rx_buff, '\0', uart_rx_len_max);
    serial = rt_device_find(uart_name);
	
    if (!serial)
    {
        rt_kprintf("find %s failed!\n",uart_name);
        return RT_ERROR;
    }

    rt_sem_init(&rx_sem, "rx5_sem", 0, RT_IPC_FLAG_FIFO);	

		rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);		
		
		rt_device_control(serial, RT_DEVICE_CTRL_CLR_INT, NULL);	
	
		rt_device_set_rx_indicate(serial, uart_input);				
		
    rt_thread_t thread = rt_thread_create("serial", dut_tsi_thread_entry, RT_NULL, 1024, UART5_THREAD_PRIORITY, 100);
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

INIT_APP_EXPORT(dut_tsi_stream_thread_init);

MSH_CMD_EXPORT(dut, ...);
MSH_CMD_EXPORT(tsi, ...);
















