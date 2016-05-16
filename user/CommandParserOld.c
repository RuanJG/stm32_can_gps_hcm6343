#include "stm32f10x.h"
#include "global.h"
#include  <ctype.h>
#include  <string.h>
#include  <stdio.h>	

//static u8 lastc = 0;

//#define  MAX_SIZE   20                                      //  ������������Ƴ���
//static char RxBuf[1 + MAX_SIZE];                                   //  ���ջ�����
//static int BufP = 0;                                               //  ������λ�ñ���
//bool RxEndFlag = FALSE;                                 	//  ���ս�����־

#define COMMAND_BUFFER_SIZE 30
static u8 command_buffer[COMMAND_BUFFER_SIZE];
static int command_buffer_count = 0;

void CommandPaserOld(u8 c)
{
		//if(c == '\n' && lastc == '\r')
		if (c == '\r')
		{										//<CR><LF>
			//CommandLength = CommandIndex; 						
			//CommandIndex = 0;								//���ս�����־��λ
			
			//ִ������
			ExecuteCommand(command_buffer, command_buffer_count);	
			
			command_buffer_count = 0;//���û�����
		}
		else if (c == 253)
		{
			Arm9Started = TRUE;
		}
		else
		{
			if(command_buffer_count >= COMMAND_BUFFER_SIZE)
			{
				command_buffer_count = 0;//���û�����
			}
			//Command[CommandIndex] = c;				 	//command data
			//CommandIndex++;
			command_buffer[command_buffer_count++] = c;
		}
		//lastc = c;
}
