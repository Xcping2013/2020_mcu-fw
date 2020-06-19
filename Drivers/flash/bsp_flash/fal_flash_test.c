/*	FAL (Flash Abstraction Layer) Flash 抽象层
	对 Flash 及基于 Flash 的分区进行管理、操作的抽象层，对上层统一了 Flash 及 分区操作的 API

1. 定义 Flash 设备 ： 包括读写擦除等操作函数

	 fal_flash_stm32f2_port.c || drv_flash_f1.c	|| fal_flash_sfud_port.c
	 
	 const struct fal_flash_dev stm32f2_onchip_flash = { "stm32_onchip", 0x08000000, 1024*1024, 128*1024, {init, read, write, erase} };
	 
	 如果没有 init 初始化过程，可以置空
	 
2. 定义 Flash 设备表	

	fal_cfg.h	-->	FAL_FLASH_DEV_TABLE

3. 配置分区 : 分区名	Flash设备名	偏移地址	大小	说明

	drv_flash_f1.c	fal_cfg.h		fal.c
		
stm32f2_onchip_flash	---		stm32f_onchip_flash
														BSP_USING_ON_CHIP_FLASH	STM32_FLASH_START_ADRESS...	LOG_E	LOG_D
stm32_onchip					---		onchip_flash
		
fal_init();
		
*/

#include <rtthread.h>
#include <stdint.h>
#include <rtdevice.h>

int fal_flash_onchip_app(void)
{
	/*
drv_flash_f1.c	onchip_flash	单独操作     easyFlash    FAL	 文件系统
										spi_flash              easyFlash    FAL  文件系统	
	*/
    /* user app entry */
    return 0;
}

int fal_flash_sfud_app(void)
{
    /* user app entry */
    return 0;
}



