
//电机相关配置文件 #include "tmc429_interface.h"		tmc429_config.c

//

#if 0			//Main research direction

1. 全部板卡FW工程汇集在一个Keil工程下，每个文件夹下是一个完整系列文件，不同的应用编译不同的文件夹
   ---需要注意的是多个.c文件可以同名，使用屏蔽编译来选择
	 ---.h文件无法屏蔽，要么再增加一个独立的.h文件在共用的头文件中;要么直接在通用头文件中进行使用配置
			//#if board #else board #endif
	 
2. EEPROM通用配置： "app_eeprom_24xx.h"中 

		A:以下2条命令可以操作所有的EEPROM空间，SN命令存储在第一页
		EEPROM Read|WritePage
		EEPROM Read|Write
		B：其他read|savedata等需要注意地址是否会冲突，导致内容被覆盖



#endif


/********************************Version ChangeList  ****************************/
#if 0			//INC-MB1616DEV

FW:INC-MB1616DEV Ver1.0:

	 BSP_GPIO_PCA95XX								FB_RGB_ISL29125		
	 BSP_ADC_MCP3421								FB_CC_TLC5973
	 BSP_EEPROM_AT24C128						FB_MOTOR_TMC429


#endif

#if 0			//FB_RGB_ISL29125	


#endif

#if 0			//FB_MOTOR_TMC429


信号触发有效电平：

限位传感器未触发时为低电平，触发后为高电平 ---默认不接传感器=触发限位

原点触发有效电平：
uint8_t OriginSensorON[6]= {	LOW,	LOW,	LOW, 	LOW,	 LOW,	 LOW};

#endif



/********************************Commands msh   ******************************************
串口终端配置：

#define RT_DEVICE_FLAG_STREAM 0x040    
	流模式参数用于向串口终端输出字符串：当输出的字符是“\n” 时，自动在前面补一个 “\r” 做分行
//#define FINSH_ECHO_DISABLE_DEFAULT  
//	不返回输入的命令

命令应用
1.config
Baudrate:	115200	Data Bits:8		Parity: none	Stop Bits:1
2.PC发送字符串命令以\r\n结束，控制板返回命令以 \r\n>>结束 
3.命令列表可以发送help查看；命令格式：cmd 参数1 参数2 ...
4.连续发送多条命令时间间隔需大于20ms
5.通讯异常返回command not found的话，请重发命令。

6.命令中的<>内的数据为发送的参数 实际发送命令需要去除此符号
	命令中的[]内的数据为返回的参数 
	
help				命令列表打印
version			打印板卡信息
reboot			控制卡重启
*****************************************************************************************/

#if 0			//Commands msh GPIO

readinput <ch> 读取输入通道ch的状态

send1:		readinput 1					send2:		readinput 1 2 2 3	
receive1：in[1]=0						  receive2:	in[1]=0,in[2]=0,in[2]=0,in[3]=0

output on	<ch>	设置输出通道ch状态为1

send1：		output on 1					send2:		output on 1 2 2 3	
receive1：out[1]=1						receive2:	out[1]=1,out[2]=1,out[2]=1,out[3]=1

output off	<ch>	设置输出通道ch状态为0

send1：		output off 1				send2:		output off 1 2 2 3	
receive1：out[1]=0						receive2:	out[1]=0,out[2]=0,out[2]=0,out[3]=0

#endif

#if 0			//Commands msh EEPROM

savedata	<page>	<data>	在1~200存储位置写入数据,每页最大存储的长度为64字节，不支持数据带空格,需要把空格用其他字符替换

send：		savedata 1 this_data_is_for_test:position1=100;position2=200;
receive: 	savedata[1] ok

readdata	<page>	读取1~200存储位置的数据

send：		readdata 1
receive:	readdata[1]=this_data_is_for_test:position1=100;position2=200;

#endif

#if 0			//Commands msh MOTOR（axis_number=0~5 )

motor	stop	<axis_number>			电机停止运行并返回当前位置

send：		motor stop	1
receive:	motor[1] is stop and P[1]=0

motor  		set  speed	<axis_number>	<value>	设定电机每秒运行的脉冲数和驱动器的微步相关，可配置的最大速度为3814 /2=1907
send： 		motor set speed	1 105
receive:	set motor[1] speed=105 ok

motor	move	<axis_number>	<position>	电机相对运动到设定值

send：		motor move 1 1000
receive:	motor[1] is start to make relative motion

motor	get	position	<axis_number>			读取对应轴当前位置

send：		motor get position	1
receive:	motor[1] actual position=0	

motor	get	is_stop	<axis_number>				读取对应轴是否处于停止状态

send：		motor get is_stop	1
receive:	motor[1] stop=1	


motor	 golimit <motor number> <velocity>   ---motor search limit sensor

send：		motor	 golimit 0 1600   电子尾部看：顺时针找到正限位时停止
receive:	motor[0] is start go home by searching limit sensor
					---
					motor[0] golimit ok

send：		motor	 golimit 0 -1600  电子尾部看：逆时针找到负限位时停止
receive:	motor[0] is start go home by searching limit sensor
					---
					motor[0] golimit ok
					
motor	 gohome  <motor number> <velocity>   ---motor search limit then search the origin sensor

send：		motor	 gohome  0 1600   电子尾部看：顺时针先高速找到正限位再逆时针低速找原点
receive:	motor[0] is start go home by searching home sensor
					---
					motor[0] gohome ok

send：		motor	 gohome  0 -1600   电子尾部看：逆时针先找到负限位再顺时针找原点
receive:	motor[0] is start go home by searching home sensor
					---
					motor[0] gohome ok

#endif


