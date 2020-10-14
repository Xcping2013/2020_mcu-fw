
#include "mpm.h"
#include "dfs_private.h"

static int rt_hw_spi4_tfcard(void)
{
    __HAL_RCC_GPIOE_CLK_ENABLE();

    rt_hw_spi_device_attach("spi4", "spi10", GPIOE, GPIO_PIN_4);

    return msd_init("sd0", "spi10");
}


static void elmfatfs_dfs_mount(void)
{
	dfs_mount("sd0", "/", "elm", 0, 0);
}

void sd_card_test(void)
{
	FILE *recvdata_p0;

	char RxBuf_P0[1024];
	
	static uint8_t sd_card_inited=0;
	
	if(rt_pin_read(SD_DETECT_PIN)==0)
	{

		if(sd_card_inited==0)
		{
			sd_card_inited=1;
			
			rt_hw_spi4_tfcard();

			elmfatfs_dfs_mount();

//			recvdata_p0 = fopen("recvdata_p0.csv", "a+");
//			if (recvdata_p0 != RT_NULL)
//			{
//					fputs((char *)RxBuf_P0, recvdata_p0);
//					fputs("\n", recvdata_p0);
//					fclose(recvdata_p0);
//			}
			
		}
		rt_kprintf("List information about the FILEs\n");	
		cmd_ls(1,(char**)"");
		
	}
	else rt_kprintf("sd_card no found!!!\n");	
}


void sdin(void)
{
 rt_pin_mode(SD_DETECT_PIN,PIN_MODE_INPUT_PULLUP);
	
 rt_kprintf ("sd_detect pin=%d\n",rt_pin_read(SD_DETECT_PIN));	
	
}

//INIT_DEVICE_EXPORT(sdcard_init);

MSH_CMD_EXPORT(sd_card_test,detect sd_card and list files);
//MSH_CMD_EXPORT(sdin, ...);
/*
//ls		cat recvdata_p0.csv
//mkfs sd0  				  # sd0 为块设备名称，该命令会默认格式化 sd0 为 elm-FAT 文件系统
//mkfs -t elm sd0             # 使用 -t 参数指定文件系统类型为 elm-FAT 文件系统


ls		显示文件和目录的信息
cd		进入指定目录
cp		复制文件
rm		删除文件或目录
mv		将文件移动位置或改名
echo	将指定内容写入指定文件，当文件存在时，就写入该文件，当文件不存在时就新创建一个文件并写入
cat		展示文件的内容
pwd		打印出当前目录地址
mkdir	创建文件夹
mkfs	格式化文件系统


mkdir rt-thread            				  # 创建 rt-thread 文件夹
rm rt-thread                     		  # 删除 rt-thread 文件夹
rm hello.txt                    		  # 删除 hello.txt 文件
ls                          			  # 查看目录信息如下
echo "hello rt-thread!!!"                 # 将字符串输出到标准输出
echo "hello rt-thread!!!" hello.txt       # 将字符串出输出到 hello.txt 文件


*/





