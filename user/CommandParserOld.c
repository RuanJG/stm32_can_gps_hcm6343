#include "stm32f10x.h"
#include "global.h"
#include  <ctype.h>
#include  <string.h>
#include  <stdio.h>	

//static u8 lastc = 0;

//#define  MAX_SIZE   20                                      //  缓冲区最大限制长度
//static char RxBuf[1 + MAX_SIZE];                                   //  接收缓冲区
//static int BufP = 0;                                               //  缓冲区位置变量
//bool RxEndFlag = FALSE;                                 	//  接收结束标志

#define COMMAND_BUFFER_SIZE 30
static u8 command_buffer[COMMAND_BUFFER_SIZE];
static int command_buffer_count = 0;

void CommandPaserOld(u8 c)
{
		//if(c == '\n' && lastc == '\r')
		if (c == '\r')
		{										//<CR><LF>
			//CommandLength = CommandIndex; 						
			//CommandIndex = 0;								//接收结束标志置位
			
			//执行命令
			ExecuteCommand(command_buffer, command_buffer_count);	
			
			command_buffer_count = 0;//重置缓冲区
		}
		else if (c == 253)
		{
			Arm9Started = TRUE;
		}
		else
		{
			if(command_buffer_count >= COMMAND_BUFFER_SIZE)
			{
				command_buffer_count = 0;//重置缓冲区
			}
			//Command[CommandIndex] = c;				 	//command data
			//CommandIndex++;
			command_buffer[command_buffer_count++] = c;
		}
		//lastc = c;
}
