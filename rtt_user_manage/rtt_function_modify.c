
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








