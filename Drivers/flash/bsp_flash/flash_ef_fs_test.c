//#define PKG_EASYFLASH_ERASE_GRAN 2048
//#define LFS_BLOCK_SIZE 2048*2
//#define FAL_EF_PART_NAME               "app"
//FAL_FLASH_DEV_TABLE
//FAL_PART_TABLE
#include <rtthread.h>
#include <stdint.h>
#include <rtdevice.h>

#include <rtconfig.h>
#include <board.h>
#include <fal.h>

#include "spi_flash.h"
#include "spi_flash_sfud.h"

#include <fal.h>
#include <dfs_fs.h>

#define SPI1_BUS_NAME                            "spi1"
#define SPI_W25QXX_DEVICE_NAME            			 "spi10"
#define W25Qxx_CS_PIN                             29

#define DBG_SECTION_NAME               "main"
#define DBG_LEVEL                      DBG_INFO
#include <rtdbg.h>
#define FS_PARTITION_NAME              "filesystem"//"app"    

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

int LittleFs_fal_flash_onchip(void)
{
    struct rt_device *mtd_dev = RT_NULL;

    fal_init();
		//need do fal probe app for fs
    mtd_dev = fal_mtd_nor_device_create(FS_PARTITION_NAME);
    if (!mtd_dev)
    {
        LOG_E("Can't create a mtd device on '%s' partition.", FS_PARTITION_NAME);
    }
    else
    {
        if (dfs_mount(FS_PARTITION_NAME, "/", "lfs", 0, 0) == 0)
        {
            LOG_I("Filesystem initialized!");
        }
        else
        {
            dfs_mkfs("lfs", FS_PARTITION_NAME);
            if (dfs_mount("filesystem", "/", "lfs", 0, 0) == 0)
            {
                LOG_I("Filesystem initialized!");
            }
            else
            {
                LOG_E("Failed to initialize filesystem!");
            }
        }
    }

//    while (1)
//    {
//       
//    }

    return RT_EOK;
}
/*	#define LFS_BLOCK_SIZE 2048 || 2048 *2

1.	SFUD || FLASH || FLASH_ONCHIP			FLASH驱动
2.	FAL																FLASH和分区管理	将分区创建成MTD API
3.	MTD																Memory Technology Device	FLASH的统一接口
4.	littlefs	
5.	DFS																Device File System 多种类型文件系统的统一操作接口
6.	APP

APP-->DFS API-->littlefs API--> MTD R|W -->FAL SFUD || ...

使用 ls 命令查看当前目录信息，运行结果如下所示：
msh />ls                          # 使用 ls 命令查看文件系统目录信息
Directory /:                      # 可以看到已经存在根目录 /

使用 echo 命令将输入的字符串输出到指定输出位置，运行结果如下所示：

msh />echo "hello RT-Thread!!!" hello.txt      # 将字符串出输出到 hello.txt 文件
msh />ls
Directory /:
hello.txt           18
msh />

使用 cat 命令查看文件内容，运行结果如下所示：

msh />cat hello.txt                     # 查看 hello.txt 文件的内容并输出
hello RT-Thread!!!

*/

/*	DFS 应用示例			FinSH 命令


FinSH 命令		描述
ls						显示文件和目录的信息
cd						进入指定目录
cp						复制文件
rm						删除文件或目录
mv						将文件移动位置或改名
echo					将指定内容写入指定文件，当文件存在时，就写入该文件，当文件不存在时就新创建一个文件并写入
cat						展示文件的内容
pwd						打印出当前目录地址
mkdir					创建文件夹
mkfs					格式化文件系统							mkfs sd0 		mkfs -t elm sd0	使用 -t 参数指定块文件系统类型为 elm-FAT 文件系统

mkdir rt-thread            								 # 创建 rt-thread 文件夹
echo "hello rt-thread!!!"              	   # 将字符串输出到标准输出
echo "hello rt-thread!!!" hello.txt     	 # 将字符串出输出到 hello.txt 文件
cat hello.txt                    					 # 查看 hello.txt 文件的内容并输出
rm rt-thread                      				 # 删除 rt-thread 文件夹
rm hello.txt                   					   # 删除 hello.txt 文件
msh /dir_test>cd ..                        # 切换到上级文件夹

open	write ||read	close
rename
stat
mkdir
opendir	readdir	closedir
opendir	readdir	telldir	seekdir	telldir	readdir	closedir

*/
int main(void)
{
    /* user app entry */
	w25qxx_spi_init();								//SFUD   				spi_flash_sfud.c
	LittleFs_fal_flash_onchip();			//HAL						fal_cfg.h					fal_flash_sfud_port.c
	easyflash_init();									//easyFlash			ef_fal_port.c
  return 0;
}

