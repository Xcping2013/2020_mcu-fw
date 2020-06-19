/*	#define LFS_BLOCK_SIZE 2048

1.	SFUN || FLASH || FLASH_ONCHIP			FLASH驱动
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
#include <rtthread.h>
#include <stdint.h>
#include <rtdevice.h>

#include <fal.h>
#include <dfs_fs.h>

#define DBG_SECTION_NAME               "main"
#define DBG_LEVEL                      DBG_INFO
#include <rtdbg.h>
#define FS_PARTITION_NAME              "filesystem"//"app"    

//int LittleFs_fal_flash_onchip_app(void)
int LittleFs_fal_flash_onchip_app(void)
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

//INIT_APP_EXPORT(LittleFs_fal_flash_onchip_app);


