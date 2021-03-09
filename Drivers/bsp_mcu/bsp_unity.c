#include "bsp_unity.h"	


#if 1
	#define DBG_ENABLE	0
	#if 	( DBG_ENABLE )
	#define DBG_TRACE		rt_kprintf
	#else
	#define DBG_TRACE(...)		
#endif

#endif


uint8_t data_invert_order(uint8_t data)
{
	uint8_t t,out_data=0;

	uint8_t byte=data;
	
	for(t=0;t<8;t++)	
	{
		out_data+=((byte & 0x80)>>(7-t));
			byte<<=1;
	}	
	return out_data;
}

void MakeLowercase( char *str)		
{
	while(*str!='\0')
	{
			if( ((*str)>='A')&&((*str)<='Z') )
			{
					*str=*str+32;
			}
			str++;
	}
}
void MakeLowercaseTo( char *strScr, char *strDir)
{
	while(*strScr!='\0')
	{
			if( ((*strScr)>='A')&&((*strScr)<='Z') )
			{
					*strDir=*strScr+32;
			}
			else *strDir=*strScr;
			DBG_TRACE("strScr=%c,strDir=%c\n",*strScr,*strDir);
			strScr++;
			strDir++;
	}
//	*strDir='\0';
}
//


