/*串行 (SPI) Flash 通用驱动库
An using JEDEC's SFDP standard serial (SPI) flash universal driver library | 使用 JEDEC SFDP 标准
支持 SPI/QSPI 接口		标准占用：RAM:0.2KB ROM:5.5KB	支持不同品牌及规格的 Flash
{"W25Q32BV", SFUD_MF_ID_WINBOND, 0x40, 0x16, 4L*1024L*1024L, SFUD_WM_PAGE_256B, 4096, 0x20}
 */
#include <rtthread.h>
#include <stdint.h>
#include <rtdevice.h>
#include "spi_flash.h"
#include "spi_flash_sfud.h"

#define SPI1_BUS_NAME                            "spi1"

#define SPI_W25QXX_DEVICE_NAME            			 "spi10"
#define W25Qxx_CS_PIN                             29

struct stm32_hw_spi_cs
{
    rt_uint32_t pin;
};

static struct rt_spi_device spi_dev_w25qxx;
static struct stm32_hw_spi_cs  w25qxx_cs;

static rt_spi_flash_device_t w25qxx;

static void w25qxx_spi_init(void)
{
		rt_err_t	res;
    w25qxx_cs.pin=29;
    rt_pin_mode(w25qxx_cs.pin, PIN_MODE_OUTPUT);

    res = rt_spi_bus_attach_device(&spi_dev_w25qxx, SPI_W25QXX_DEVICE_NAME, SPI1_BUS_NAME, (void *)&w25qxx_cs);
    
    w25qxx = rt_sfud_flash_probe(FAL_USING_NOR_FLASH_DEV_NAME, "spi10");    
  //dfs_mount("flash0", "/", "elm", 0, 0);
}
int sfud_spi_flash_app(void)
{
	w25qxx_spi_init();
    /* user app entry */
    return 0;
}

//INIT_APP_EXPORT(sfud_spi_flash_app);
/*

添加复位按键动作
添加项目选择参数配置
修改定时器中断：开关函数配套使用

*/

