/*
	主要应用
		输入;输出;存储;光采集;电机运动
		
	主要完善: 

		速度档位的自动切换，pusle_div和ramp_div的自动跟随
		
*/

#include "cmds_dut_dummy.h"	
#include "app_eeprom_24xx.h"	
#include "main.h"
#include "msh.h"

#if 1 //B427 From B427 QT1 Commands.xls

int B427_QT1_Commands(char* string)
{
	//FINSH_ECHO_DISABLE_DEFAULT
	uint8_t ret=0;
	
	static uint8_t mode=3;
	
	if(!strncmp("[MSN]",string,5))
	{
		rt_kprintf("[MSN]<msn-DLC934300XTM6PK3S-ok>\n");
	}
	else if(!strncmp("[FSN-GWXZ875ZLKKT]",string,18))
	{
		rt_kprintf("[FSN-GWXZ875ZLKKT]<fsn-GWXZ875ZLKKT-ok>\n");
	}
	else if(!strncmp("[FSN]",string,5))
	{
		rt_kprintf("[FSN]<fsn-GWXZ875ZLKKT-ok>\n");
	}
	else if(!strncmp("[sleep-0]",string,9))
	{
		rt_kprintf("[sleep-0]<sleep-0>\n");
	}
	else if(!strncmp("[CBREAD-03]",string,9))
	{
		rt_kprintf("[CBREAD-03]<cbread-03:0000000000-0000-00-00-00-U>\n");
	}
	else if(!strncmp("[CBWRITE-03-0000000000000000000000000000000000000000-1566570831-0337-I]",string,71))
	{
		rt_kprintf("[CBWRITE-03-0000000000000000000000000000000000000000-1566570831-0337-I]<cbwrite-03:ok>\n");
	}
	else if(!strncmp("[TSETELPPA]",string,11))
	{
		rt_kprintf("[TSETELPPA]\n?\n");
	}
	else if(!strncmp("[axlint-r]",string,10))
	{
		rt_kprintf("[axlint-r]<axlint-1>\n");
	}
	else if(!strncmp("[axlint-0]",string,10))
	{
		rt_kprintf("[axlint-0]<axlint-0>\n");
	}
	else if(!strncmp("[SR]",string,4))
	{
		rt_kprintf("[SR]\nB427 HW EVT FW ver 0.5.2 Application\n");
	}
	else if(!strncmp("[AST]",string,5))
	{
		rt_kprintf("[AST]\nAccel Type: 0xFB\n<ast,+X:00004246,+Y:00001277,+Z:00002706,-X:-0004100,-Y:-0005259,-Z:-0002685-ok>\n");
	}
	else if(!strncmp("[TM-37-0]",string,9))
	{
		rt_kprintf("[TM-37-0]<tm-37-0-ok>\n");
	}
	else if(!strncmp("[MLOCK-1]",string,9))
	{
		rt_kprintf("[MLOCK-1]<mlock-1-ok>\n");
	}
	else if(!strncmp("[MSV]",string,5))
	{
		rt_kprintf("[MSV]<msv-0002040C-ok>\n");
	}
	else if(!strncmp("[IRRW-39-0000]",string,14))
	{
		rt_kprintf("[IRRW-39-0000]<irrw-39-0000-05935601-ok>\n");
	}
	
	else if(!strncmp("[eer-0000]",string,10))
	{
		rt_kprintf("[eer-0000]<eer-00000000-ABCDEF00-ok>\n");
	}	
	
	else if(!strncmp("[eec-0000-ABCDEF00]",string,19))
	{
		rt_kprintf("[eec-0000-ABCDEF00]<eec-00000000-ABCDEF00-ok>\n");
	}	

	else if(!strncmp("[eef]",string,5))
	{
		rt_kprintf("[eef]EEPROM Check Ok<eef>\n");
	}	
	
	else if(!strncmp("[BATMAN]",string,8))
	{
		rt_kprintf("[BATMAN]<batman-044%-V:3887-CS:02-L-000%-CS:00-V:0000-I:0000-0-R-000%-CS:00-V:0000-I:0000-0>\n");
	}		
	else if(!strncmp("[BUTTON]",string,8))
	{
		rt_kprintf("[BUTTON]<button-SW:1,HW:1>\n");
	}			
	
	else if(!strncmp("[HWREV]",string,7))
	{
		rt_kprintf("[HWREV]<hwrev-2>\n");
	}			
	
	else if(!strncmp("[BMU]",string,5))
	{
		rt_kprintf("[BMU]<bmu-11008-ok>\n");
	}			
		else if(!strncmp("[LID]",string,5))
	{
		rt_kprintf("[LID]<lid-SW:0,HW:0>\n");
	}	
	
		else if(!strncmp("[CMODE-6-6-1]",string,13))
	{
		
		rt_kprintf("[CMODE-6-6-1]<cmode-300-300-1-%d>\n",mode);
	}		
		else if(!strncmp("[CPG]",string,5))
	{
		rt_kprintf("[CPG]<cpg-0>\n");
	}	
		else if(!strncmp("[IOR-PB01]",string,10))
	{
		rt_kprintf("[IOR-PB01]<ior-PB01-0-ok>\n");
	}	
		else if(!strncmp("[BATTV]",string,7))
	{
		rt_kprintf("[BATTV]<battv-3914>\n");
	}	
		else if(!strncmp("[TORCH]",string,7))
	{
		rt_kprintf("[TORCH]<torch-C-mlbNTC:266-bmuNTC:259-bmuDie:248-mcuDie:226>\n");
	}		

			else if(!strncmp("[GG]",string,4))
	{
		rt_kprintf("[GG]<gg-v=3914, i=281, t=260, SOC=44, rawSOC=44>\n");
	}		

			else if(!strncmp("[AUTHD]",string,7))
	{
		rt_kprintf("[AUTHD]<authd-ok>\n");
	}	

				else if(!strncmp("[BRICKID]",string,9))
	{
		rt_kprintf("[BRICKID]<brickid-2>\n");
	}	


				else if(!strncmp("[PML-1]",string,7))
	{
		mode=1;
		rt_kprintf("[PML-1]<pml-1>\n");
	}	
	
	else if(!strncmp("[TM-42]",string,7))
	{
		rt_kprintf("[TM-42]<tm-42>\n");
	}	

	else if(!strncmp("[BMM-0]",string,7))
	{
		rt_kprintf("[BMM-0]<bmm-0>\n");
	}
	else if(!strncmp("[BOUT-1]",string,7))
	{
		rt_kprintf("[BOUT-1]<bout-1>\n");
	}
	else if(!strncmp("[BBUD-1-L]",string,10))
	{
		rt_kprintf("[BBUD-1-L]<bbud-1-L>\n");
	}
	else if(!strncmp("[BBUD-1-R]",string,10))
	{
		rt_kprintf("[BBUD-1-R]<bbud-1-R>\n");
	}
	else if(!strncmp("[BBUD-0-L]",string,10))
	{
		rt_kprintf("[BBUD-0-L]<bbud-0-L>\n");
	}
	else if(!strncmp("[BBUD-0-R]",string,10))
	{
		rt_kprintf("[BBUD-0-R]<bbud-0-R>\n");
	}	
		else if(!strncmp("[SYSTICK]",string,9))
	{
		rt_kprintf("[SYSTICK]<systick-00000013 sec>\n");
	}	
	
			else if(!strncmp("[BHR]",string,5))
	{
		rt_kprintf("[BHR]<bhr-B>\n");
	}	

			else if(!strncmp("[TUNNEL-L-0]",string,12))
	{
		rt_kprintf("[TUNNEL-L-0]<tunnel-L-0>\n[00004994] TUNNEL: OPEN\n");
	}		
	
			else if(!strncmp("ft version",string,10))
	{
		rt_kprintf("ft version\n> ft:ok 0 1 0 (Jun  4 2019, 22:31:25)\n]\n");
	}		

			else if(!strncmp("ft tunnel close",string,15))
	{
		rt_kprintf("ft tunnel close\n> ft:ok\n[00005703] TUNNEL: CLOSE\n");
	}		
	
	
			else if(!strncmp("[TUNNEL-R-0]",string,12))
	{
		rt_kprintf("[TUNNEL-R-0]<tunnel-R-0>\n[00004994] TUNNEL: OPEN\n");
	}		
	
			else if(!strncmp("[BLS]",string,5))
	{
		rt_kprintf("[BLS]<bls-1-1>\n");
	}		
	
				else if(!strncmp("[BLQ-1000]",string,10))
	{
		rt_kprintf("[BLQ-1000]\n<blq-0001000-ok-L:TX 0001105, RX 0001104, TXretry 0000000, RXerr 0000000, R:TX 0001099, RX 0001098, TXretry 0000000, RXerr 0000000>\n");
	}
			else if(!strncmp("[CMODE-?]",string,9))
	{
		rt_kprintf("[CMODE-?]<cmode-400-005-0-0>\n");
	}	

			else if(!strncmp("[CBNONCE]",string,9))
	{
		rt_kprintf("[CBNONCE]<cbnonce:6772423019AFEA581AF28A8E7E1FCC984F4CDD7F>\n");
	}	

			else if(!strncmp("[CBWRITE-03-1C02517AA68AFBEFB1FBA47E03CC6DC15C22DD4A-1566570976-0337-P]",string,71))
	{
		rt_kprintf("[CBWRITE-03-1C02517AA68AFBEFB1FBA47E03CC6DC15C22DD4A-1566570976-0337-P]<cbwrite-03:ok>\n");
	}	

	
	else if(!strncmp("[AXLN]",string,6))
	{
		rt_kprintf("[AXLN]\nODR: 125Hz\n\nX,Y,Z\n");
		rt_kprintf("69,-2118,-49\n68,-1986,-58\n70,-1977,-63\n69,-1984,-54\n71,-1987,-50\n");
		rt_kprintf("69,-1993,-55\n70,-1983,-65\n68,-1983,-50\n70,-1989,-56\n74,-1991,-78\n");
		rt_kprintf("63,-1986,-65\n58,-1990,-49\n76,-1985,-58\n75,-1984,-75\n61,-1987,-56\n");
		rt_kprintf("70,-1990,-60\n75,-1988,-60\n57,-1990,-66\n75,-1990,-73\n65,-1978,-64\n");		
		rt_kprintf("76,-1983,-59\n66,-1987,-65\n65,-1983,-54\n67,-1990,-64\n68,-1986,-66\n");
		rt_kprintf("69,-1988,-64\n73,-1990,-53\n63,-1987,-67\n69,-1990,-71\n66,-1993,-53\n");		
		rt_kprintf("62,-1992,-57\n73,-1985,-59\n68,-1982,-61\n66,-1982,-59\n70,-1982,-48\n");	
		rt_kprintf("71,-1983,-61\n66,-1978,-61\n66,-1979,-48\n69,-1984,-55\n68,-1985,-65\n");
		rt_kprintf("72,-1985,-58\n66,-1988,-50\n72,-1989,-67\n71,-1989,-64\n67,-1988,-53\n");
		rt_kprintf("69,-1991,-64\n71,-1991,-60\n73,-1988,-56\n66,-1989,-62\n66,-1982,-58\n");
		rt_kprintf("81,-1983,-79\n69,-1992,-56\n75,-1993,-58\n63,-1985,-51\n70,-1987,-68\n");
		rt_kprintf("77,-1986,-58\n60,-1983,-44\n70,-1988,-64\n74,-1984,-54\n70,-1978,-52\n");
		rt_kprintf("67,-1985,-41\n67,-1978,-56\n70,-1984,-52\n68,-1990,-41\n68,-1988,-62\n");
		rt_kprintf("73,-1982,-56\n72,-1986,-53\n64,-1984,-51\n72,-1991,-70\n67,-1982,-61\n");
		rt_kprintf("71,-1983,-58\n69,-1985,-65\n70,-1988,-63\n67,-1988,-54\n71,-1991,-54\n");
		rt_kprintf("74,-1986,-58\n71,-1985,-66\n71,-1984,-57\n66,-1982,-62\n66,-1988,-54\n");
		rt_kprintf("76,-1983,-59\n74,-1984,-56\n65,-1986,-55\n70,-1990,-60\n72,-1989,-59\n");
		rt_kprintf("68,-1988,-52\n63,-1988,-55\n67,-1984,-61\n63,-1984,-64\n70,-1983,-55\n");
		rt_kprintf("66,-1981,-63\n67,-1985,-61\n68,-1982,-66\n66,-1979,-59\n70,-1984,-65\n");
		rt_kprintf("72,-1985,-62\n69,-1986,-55\n62,-1989,-66\n73,-1986,-61\n69,-1986,-59\n");
		rt_kprintf("69,-1989,-60\n66,-1991,-63\n68,-1979,-63\n74,-1985,-53\n67,-1984,-56\n");
		rt_kprintf("70,-1984,-64\n72,-1988,-56\n70,-1987,-62\n71,-1979,-66\n74,-1989,-65\n");		
		rt_kprintf("68,-1991,-66\n69,-1985,-61\n70,-1985,-60\n70,-1987,-68\n70,-1989,-63\n");
		rt_kprintf("70,-1991,-63\n66,-1990,-63\n73,-1984,-59\n73,-1981,-59\n67,-1994,-62\n");
		rt_kprintf("73,-1990,-62\n72,-1984,-58\n68,-1992,-56\n66,-1990,-58\n72,-1983,-61\n");
		rt_kprintf("<axln-ok>\n");
	}
	else ret=1;
	return ret;
}

#endif

int DUT_Dummy_Commands(char* string)
{
	if(B427_QT1_Commands(string)==REPLY_OK) return 0;
}	

//




