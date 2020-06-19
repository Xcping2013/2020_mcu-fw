
#include "app_gpio_dido.h"	
#include "app_eeprom_24xx.h"

#include "tmc429_motion_cmd.h"
#include "user_msh.h"	

#if 0
#define printf rt_kprintf
#endif

//char version_string[]="ver3.6.1";

void user_show_version(void)
{
	rt_show_version();

	rt_kprintf("\nFW-MB1616dDEV6-RTT-%d-%d %s build at %s %s\n\n", STM32_FLASH_SIZE/1024, STM32_SRAM_SIZE, version_string, __TIME__, __DATE__);

}
static void PrintCommandlist_older(void)
{
//	rt_kprintf("ver1 commands:\n");
	rt_kprintf("sv-------------------get software version\n");
	rt_kprintf("hv-------------------get hardware version\n");
	rt_kprintf("fsn------------------get fixture serial number\n");	
	rt_kprintf("fsn[]----------------set fixture serial number\n");	
	rt_kprintf("rom[]----------------read roms\n");
	rt_kprintf("rom[][]--------------write roms\n");
	rt_kprintf("save info xx yyyyy---write datas\n");
	rt_kprintf("read info xx---------read datas\n");
	rt_kprintf("int[]----------------get the input status\n");
	rt_kprintf("out[][]--------------set the output status\n");
	
	rt_kprintf("ParamSave ProjectType OKOA\n");		
	rt_kprintf("ParamSave ProjectType OKOA_LED\n");	
//	rt_kprintf("ParamSave ProjectType BX88\n");
	rt_kprintf("\n");
//	rt_kprintf("type[os]\n");
//	rt_kprintf("type[bqt1]\n");
//	rt_kprintf("type[heat]\n");
	
//	rt_kprintf("Please send command as above end with \\r\\n\n");
}
static void	Printhelplist(void)
{
	
	rt_kprintf("    gpio help:\n");
	rt_kprintf("    button help:\n");
	rt_kprintf("    eeprom help:\n");
	rt_kprintf("    uart3 help:\n");
	
	rt_kprintf("    motor help:\n");
	rt_kprintf("    motor get help:\n");
	rt_kprintf("    motor set help:\n");	
}
static void	PrintCommandlist(void)
{
		//ºÍMSHÍ³Ò»
		PrintCommandlist_older();	
//		rt_kprintf("ver2 commands:\n");
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
//	  rt_kprintf("\ncommands help\n");
//		Printhelplist();
}

uint8_t  ProcessCommand(char  *string)
{
	uint8_t result = REPLY_OK;
		
	if(!strcmp("help",string))	
	{
		rt_kprintf("\n");
		
		PrintCommandlist();
		
	}
	else if(!Command_analysis_dido(string))						{;}
////	else if(!Command_analysis_button(string))			{;}
////	else 
	else if(!Command_analysis_eeprom(string))			{;}
	//else if(!Command_analysis_uart3(string))			{;}
	
	else if(!Command_analysis_motor(string))			{;}
	else if(!Command_analysis_motor_get(string))	{;}
	else if(!Command_analysis_motor_set(string))	{;}
		
	else if(!strcmp("version",string))
	{
		user_show_version();
	}
	else if(!strcmp("reboot",string))
	{
		rt_kprintf("\n");
		SCB_AIRCR = SCB_RESET_VALUE;		
	}
	else if(!strcmp("command_Fset",string))
	{
		rt_kprintf("\n");
		cmd_mshMode_enter();
	}
	else if(!strcmp("board_debug",string))
	{
		rt_kprintf("\n");
		board_debug();
	}
	else result = REPLY_CHKERR;
	return result;
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










