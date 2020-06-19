/*
ef_fal_port.c			#define FAL_EF_PART_NAME                "parm"

fal_init();

easyflash_init();

ENV: Environment variables： 																									 	是否使能环境变量功能
Auto update ENV to latest default when current ENV version number is changed.：	是否启用环境变量自动更新功能。启动这个功能后，环境变量将在其版本号发生变化时自动更新。
Setting current ENV version number : 																						当前环境变量版本号
LOG: Save logs on flash：																												日志功能，可以将日志顺序保存至 Flash 中。还可以配合 EasyLogger 完成产品日志的掉电存储。
IAP: In Application Programming：																								IAP 在线升级功能，开启后将提供一些 IAP 功能里常用的 API 。
Erase minimum granularity：																											擦除的最小粒度，一般 SPI Flash 通常为 4KB，STM32F4 片内 Flash 通常为 128KB。
Write minimum granularity：																											写数据的最小粒度，一般 SPI Flash 通常为 1bit，STM32F4 片内 Flash 通常为 8bit，详见具体选项。
Start addr on flash or partition：																							EasyFlash 的整个存储区相对于 Flash 或者 分区 的偏移地址，视移植代码而定。
Enable debug log output：																												是否使能调试日志输出。开启后将会看到更多调试日志信息。
*/

/*
http://packages.rt-thread.org/itemDetail.html?package=EasyFlash
ENV 快速保存产品参数	ENV: Environment variables：
ROM: 6K bytes     RAM: 0.1K bytes

fal+easyflash
E:\rt-thread\bsp\stm32f10x-HAL\packages\fal-latest\samples\porting\fa_cfg.h
E:\rt-thread\bsp\stm32f10x-HAL\packages\EasyFlash-latest\ports\ef_fal_port.c

https://git.oschina.net/Armink/EasyFlash/repository/archive?ref=master

https://coding.net/u/armink/p/EasyFlash/git/archive/master

used:

EasyFlash-master\easyflash\port\ef_port.c
EasyFlash-master\demo\env\stm32f10x\rtt\components\easyflash\port\ef_port.c

reconfig.h				#define FAL_PART_TABLE_FLASH_DEV_NAME 	"onchip_flash"					

drv_flash_f1.c		const struct fal_flash_dev stm32_onchip_flash = { "onchip_flash", STM32_FLASH_START_ADRESS, STM32_FLASH_SIZE, FLASH_PAGE_SIZE, {NULL, fal_flash_read, fal_flash_write, fal_flash_erase} };
fal_cfg.h					FAL_PART_TABLE


*/
#include <rtthread.h>
#include <stdint.h>
#include <rtdevice.h>

int fal_easyflash_onchip_app(void)
{
    /* user app entry */
//	fal_init();

//	easyflash_init();
    return 0;
}

int fal_easyflash_sfud_app(void)
{
    /* user app entry */
    return 0;
}



