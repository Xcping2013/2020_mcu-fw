


#include "tmc429_motion_cmd.h"
#include "inc_msh.h"	


#include "bsp_unity.h"	

#include "user_commands.h"	

#include "inc_dido.h"

#include "inc_mb1616dev6.h"
#include "dido_pca95xx.h"

#include "app_eeprom_24xx.h"


static void PrintCommandlist_User(void);
static void	PrintCommandlist(void);


/*UserMsh_Commands 

1. 先把命令小写后缓存到数组user_lowcase[1000] 中, 并加入结束符
2. 匹配Command_analysis_function命令则返回,否则继续对原始缓存命令进行msh_exe
3. 如果是help命令 Command_analysis和msh 都处理

4. 后续Ver01和Ver02命令可以依靠自动识别 进行自保存和调用

*/
char UserCommands[FINSH_CMD_SIZE+20]={0};	//shell->line_position <= FINSH_CMD_SIZE;

static void PrintCommandlist_User(void)
{
//	rt_kprintf("sv-------------------get software version\n");
//	rt_kprintf("hv-------------------get hardware version\n");
	rt_kprintf("fsn------------------get fixture serial number\n");	
	rt_kprintf("fsn[]----------------set fixture serial number\n");	
	rt_kprintf("rom[]----------------read roms\n");
	rt_kprintf("rom[][]--------------write roms\n");
	rt_kprintf("save info xx yyyyy---write datas\n");
	rt_kprintf("read info xx---------read datas\n");
	rt_kprintf("int[]----------------get the input status\n");
	rt_kprintf("out[][]--------------set the output status\n");
	rt_kprintf("\n");
}
static void	PrintCommandlist(void)
{
		//Ver01 
		PrintCommandlist_User();	
		//Ver02
		rt_kprintf("reboot                 - Reboot systerm\n");
	  rt_kprintf("version                - Show firmware version information\n");
		rt_kprintf("command_Fset           - Back to first set of commands\n");
	  rt_kprintf("board_debug            - Board function test\n");
	  rt_kprintf("\n");
	  rt_kprintf("readinput <var>        - Read the state of the input channle\n");
	  rt_kprintf("    var:1~16\n");
		rt_kprintf("output on <var>        - set the output channel on\n");
		rt_kprintf("output off <var>       - set the output channel off\n");
		rt_kprintf("    var:1~16\n");
	  rt_kprintf("savedata <var1> <var2> - Save data to page 1~200\n");
		rt_kprintf("    var1:1~200\n");
		rt_kprintf("    var2:string(max=64) without spaces\n");				
  	rt_kprintf("readdata <var>         - Read data from page 1~200\n");
		rt_kprintf("    var:1~200\n");
	  rt_kprintf("ParamSave <var1> <var2>- Save speical parameters\n");	
		rt_kprintf("    var1:type\n");
		rt_kprintf("    var2:data\n");
	
		rt_kprintf("\n");

}

uint8_t  Command_analysis_User(char *string)
{
	
#if 0		
	if(!strcmp("help",string))	{	rt_kprintf("\n");	PrintCommandlist();	}	//  user help		hide prevent mixup	
#endif 
	
	if(!strcmp("version",string))	{	user_show_version();	return REPLY_OK;}	

	if(Command_analysis_eeprom(string)==REPLY_OK)					{return REPLY_OK;}	
	if(Command_analysis_dido(string)==REPLY_OK)						{return REPLY_OK;}
	if(Command_analysis_eeprom(string)==REPLY_OK)					{return REPLY_OK;}
	
	//else if(!Command_analysis_button(string))			{return REPLY_OK;}
	//else if(!Command_analysis_uart3(string))			{return REPLY_OK;}
	
//	else if(!Command_analysis_motor(string))			{return REPLY_OK;}
//	else if(!Command_analysis_motor_get(string))	{return REPLY_OK;}
//	else if(!Command_analysis_motor_set(string))	{return REPLY_OK;}
//	else if(!strcmp("board_debug",string))
//	{
//		rt_kprintf("\n");
//		board_debug();
//	}
	return REPLY_CHKERR;
}

/*********************************************************************************************************/
/*********************************MB1616007 Commands************************************/
/*********************************************************************************************************/
#if 0
void MB1616007_Commands(char* string)
{
	if(!strcmp("input.",string))
	{
		
		
	}
	if(!strcmp("help",string))
	{
		rt_kprintf("input.readPin(indx)/()\n");
		rt_kprintf("output.writePin(indx, b)/(status)\n");
		rt_kprintf(" fs.readBetween(\"FileName\",\"Head\",\"Tail\")\n");		
	}
	
	
}

input commands:
{
	 input.help()
	 {
	 	+ok@input.help()
		input commands:
		 input.help()
		 input.readPin(indx)/()
		 {
		 	+ok@input.readpin(0x00000004)	input.readPin()
		 	+ok@input.readpin(02,1)
		 }
		 input.enableFalling(indx)/()
		 input.disableFalling(indx)/()
		 input.enableRaising(indx)/()
		 input.disableRaising(indx)/()
	 }
}
output commands:
{
	 output.readPin(indx)/()
	 {
	 	+ok@output.readpin(0x00000000)
	 	+ok@output.readpin(02,0)
	 }
	 output.writePin(indx, b)/(status)
	 {
	 	+ok@output.writepin(0x00000001)		output.writePin(0, 1)
	 }
	 output.togglePin(indx)/()
	 output.runPin(indx,ms)/(ms)
	 output.stopPin(indx,b)/()	
}
FS Commands:
{
	 fs.help()
	 fs.read("FileName")
	 fs.readBetween("FileName","Head","Tail")
	 fs.readRecord("FileName","Head","Tail","RecName")
	 fs.readRecord("FileName","RecName")
	 fs.writeRecord("FileName","Head","Tail","RecName","RecContent")
	 fs.writeRecord("FileName","RecName","RecContent")
	 fs.writeEnd("FileName","Content")
	 fs.writeLineEnd("FileName","Content")
	 fs.writeHead("FileName","Content")
	 fs.writeLineHead("FileName","Content")	
}
#endif
//










