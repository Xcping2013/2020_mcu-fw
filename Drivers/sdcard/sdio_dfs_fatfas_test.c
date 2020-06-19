/*	
1.bsp\stm32\...  --> env: scons --dist

2.board\CubeMX_Config\sdio set\generate

3.board\Kconfig :config BSP_USING_WDT
    config BSP_USING_SDIO
            bool "Enable SDIO BUS"
            select RT_USING_SDIO
            default    n
4.env: menuconfig    Enable SDIO BUS
5.DFS elm-chan fatfs
6.scons --target=mdk5
7.asm-download-debug

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
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include <dfs_posix.h>

int sdio_fatfs_init(void)
{
   struct rt_device *sd_dev = RT_NULL;

		while(1)
		{
			
			sd_dev = rt_device_find("sd0");
			
			if (sd_dev!=RT_NULL)
			{
        if (dfs_mount("sd0", "/", "elm", 0, 0) == 0)
        {
            rt_kprintf("sd card mount to / success!\n");
        }
        else
        {
            rt_kprintf("sd card mount to / failed!\n");
        }
				break;
			}
			rt_thread_delay(50);
		}
    return RT_EOK;
}

INIT_APP_EXPORT(sdio_fatfs_init);


