
#include "bsp_defines.h"

#if 0
uint16_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
	while (BufferLength--)
  {
    if ((*pBuffer1) != *pBuffer2)
    {
      return BufferLength;
    }
    pBuffer1++;
    pBuffer2++;
  }

  return 0;
}

void xstrcat(char *str1,char *str2)
{
	int i,len1;
	for(i=0;str1[i]!='\0';i++);
	len1=i;
	for(i=0;str2[i]!='\0';i++)
	str1[i+len1]=str2[i];
}

//void reboot(void)
//{
//    rt_hw_cpu_reset();
//}

//MSH_CMD_EXPORT(reboot, cpu reboot);

#endif

/*		MSH_CMD_EXPORT
MSH_CMD_EXPORT(memcheck, check memory data);
MSH_CMD_EXPORT(memtrace, dump memory trace information);
MSH_CMD_EXPORT(version, show RT-Thread version information);
MSH_CMD_EXPORT(list_thread, list thread);
MSH_CMD_EXPORT(list_sem, list semaphore in system);
MSH_CMD_EXPORT(list_event, list event in system);
MSH_CMD_EXPORT(list_mutex, list mutex in system);
MSH_CMD_EXPORT(list_mailbox, list mail box in system);
MSH_CMD_EXPORT(list_msgqueue, list message queue in system);
MSH_CMD_EXPORT(list_memheap, list memory heap in system);
MSH_CMD_EXPORT(list_mempool, list memory pool in system);
MSH_CMD_EXPORT(list_timer, list timer in system);
MSH_CMD_EXPORT(list_device, list device in system);
*/

#if 0			//	rt_hw_usart_init

int rt_hw_usart_init(void)
{
    rt_size_t obj_num = sizeof(uart_obj) / sizeof(struct stm32_uart);
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
    rt_err_t result = 0;

    stm32_uart_get_dma_config();

    for (int i = 0; i < obj_num; i++)
    {
        uart_obj[i].config = &uart_config[i];
        uart_obj[i].serial.ops    = &stm32_uart_ops;
        uart_obj[i].serial.config = config;
        /* register UART device */
				if(uart_obj[i].config->Instance ==USART3)
				{
					result = rt_hw_serial_register(&uart_obj[i].serial, uart_obj[i].config->name,
															 RT_DEVICE_FLAG_RDONLY
															 | RT_DEVICE_FLAG_INT_RX
															 | uart_obj[i].uart_dma_flag
															 , &uart_obj[i]);
					RT_ASSERT(result == RT_EOK);
					
				}
				else
				{
					result = rt_hw_serial_register(&uart_obj[i].serial, uart_obj[i].config->name,
																				 RT_DEVICE_FLAG_RDWR
																				 | RT_DEVICE_FLAG_INT_RX
																				 | RT_DEVICE_FLAG_INT_TX
																				 | uart_obj[i].uart_dma_flag
																				 , &uart_obj[i]);
					RT_ASSERT(result == RT_EOK);
				}
    }

    return result;
}
#endif


//	  rt_kprintf("\nfirmware ver2.6 build at %s %s\n\n", __TIME__, __DATE__);

//UART
#if 0

MX_USARTx_UART_Init	
stm32_uart_ops
	stm32_configure
		HAL_UART_Init
			HAL_UART_MspInit
				HAL_UART_DeInit
					HAL_UART_MspDeInit

#endif








