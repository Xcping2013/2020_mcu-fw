/*
STM32 的FLASH模块由：主存储器 信息块 闪存存储器接口寄存器 等 3 部分组成。

主存储器:该部分用来存放代码和数据常数（如 const 类型的数据）
  对于大容量产品，其被划分为 256 页，每页 2K 字节。注意，小容量和中容量产品则每页只有 1K 字节。
  主存储器的起始地址就是 0X08000000， B0、 B1 都接 GND 的时候，就是从 0X08000000开始运行代码的

信息块:该部分分为 2 个小部分，其中启动程序代码，是用来存储 ST 自带的启动程序，用于串口下载代码
	当 B0 接 V3.3， B1 接 GND 的时候，运行的就是这部分代码。用户选择字节，则一般用于配置写保护、读保护等功能

闪存存储器接口寄存器:该部分用于控制闪存读写等，是整个闪存模块的控制机构

内置闪存模块可以在通用地址空间直接寻址，任何 32 位数据的读操作都能访问闪存模块的内容并得到相应的数据。

STM32 的闪存擦除分为两种：页擦除和整片擦除

*/

#ifndef __BSP_MCU_DELAY_H
#define __BSP_MCU_DELAY_H 

#include "bsp_defines.h"

//STM32F103VET:	V=100pins;E=512 Kbytes of flash memory

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Base address of the Flash sectors */
#define ADDR_FLASH_PAGE_0     ((uint32_t)0x08000000) /* Base @ of Page 0, 2 Kbytes */
#define ADDR_FLASH_PAGE_1     ((uint32_t)0x08000800) /* Base @ of Page 1, 2 Kbytes */
#define ADDR_FLASH_PAGE_2     ((uint32_t)0x08001000) /* Base @ of Page 2, 2 Kbytes */
#define ADDR_FLASH_PAGE_3     ((uint32_t)0x08001800) /* Base @ of Page 3, 2 Kbytes */
#define ADDR_FLASH_PAGE_4     ((uint32_t)0x08002000) /* Base @ of Page 4, 2 Kbytes */
#define ADDR_FLASH_PAGE_5    ((uint32_t)0x08002800)  /* Base @ of Page 5, 2 Kbytes */
#define ADDR_FLASH_PAGE_6    ((uint32_t)0x08003000) /* Base @ of Page 12, 2 Kbytes */

#define ADDR_FLASH_PAGE_127   ((uint32_t)0x0803F800) /* Base @ of Page 127, 2 Kbytes */

#define ADDR_FLASH_PAGE_255   ((uint32_t)0x0807F000) /* Base @ of Page 127, 2 Kbytes */


/* Exported functions ------------------------------------------------------- */

#endif
