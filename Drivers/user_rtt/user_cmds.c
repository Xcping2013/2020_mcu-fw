#include <rthw.h>
#include "msh.h"
#include "finsh.h"
#include "shell.h"

#include "user_rtt.h"

#include "inc_controller.h"	

int UserMsh_Commands(char *cmd, rt_size_t length)
{	
	//if(Alma_QT1_Commands(cmd)==0) return 0;
	msh_exec(cmd,length);
}	

int UserFinsh_Commands(struct finsh_parser *parser, const char *line)
{
	
	//if(Alma_QT1_Commands(line)==0) return 0;
	
	
#ifndef FINSH_USING_MSH_ONLY
	finsh_run_line(parser, line);
#endif
	
	
}
