#include "stm32f10x.h"
#include "global.h"
#include  <ctype.h>
#include  <string.h>
#include  <stdio.h>	

#include "base64\base64.h"

void ExecuteCommand(u8 *commamnd, int commamnd_len);

typedef enum _RFSessionExpectStates
{
        ExpectByteIdle,
        ExpectByte0D,
        ExpectByte0A,

        ExpectByteFE,
        ExpectByteFEFE,
}RFSessionExpectStates;
		
typedef enum _RFSessionReceivedStates
{
	ReceivedByteIdle,
	
	ReceivedByte0,
	ReceivedByte1,
	ReceivedByte2,
	ReceivedByte3,
	ReceivedByte4,
	ReceivedByte5,
	ReceivedByte6,
	ReceivedByte7,
	ReceivedByte8,
	ReceivedByte9,
	ReceivedByte11,
	ReceivedByte12,
	ReceivedByte13,
			  
	ReceivedByteFA,
	//ReceivedByteFB,
	ReceivedByteFC,
	ReceivedByteFD,

	ReceivedByteFAFA,
	//ReceivedByteFBFB,
	ReceivedByteFCFC,
	ReceivedByteFDFD,

}RFSessionReceivedStates;

static RFSessionExpectStates		expectState 	= ExpectByteIdle;
static RFSessionReceivedStates 	receivedState = ReceivedByteIdle;

#define RECEIVED_BUFFER_LEN 120
		
static u8 receivedBuffer[RECEIVED_BUFFER_LEN];
static u8 decodedBuffer[160];

static int receivedBufferCount = 0;
static int decodedBufferCount;

void ResetReceivedState(void)
{
	expectState 	= ExpectByteIdle;
	receivedState = ReceivedByteIdle;
	receivedBufferCount = 0;
}

//int ledcount = 0;
//int ledEnable = 0;

void OnReceivedOk(void)
{
	switch(receivedState)
	{
    case ReceivedByteFAFA:
    //case ReceivedByteFBFB:
    case ReceivedByteFCFC:
    case ReceivedByteFDFD:
			//解码
			//decodedBufferCount = base64_decode((char *)decodedBuffer, (char *)receivedBuffer + 2, receivedBufferCount - 4);
			decodedBufferCount = fnBase64Decode((char *)decodedBuffer, (char *)receivedBuffer + 2, receivedBufferCount - 4);
		
		/*
			if((++ledcount%10) == 0)
			{
					if(ledEnable)
					{
						ALERT_LED_SET;
						ledEnable = 0;
					}
					else
					{
						ALERT_LED_RESET;
						ledEnable = 1;
					}
			}
			*/
		
			//执行命令
			ExecuteCommand(decodedBuffer, decodedBufferCount);
			break;			
		default:
			break;
	}
	//重置
	ResetReceivedState();
}

void OnReceivedError(void)
{
	ResetReceivedState();
}

void OnReceivedByteIdle(u8 c)
{
	receivedBuffer[receivedBufferCount++] = c;
	switch(c)
	{
		case 0xFA:
			receivedState = ReceivedByteFA;
			break;
		//case 0xFB:
		//	receivedState = ReceivedByteFB;
		//	break;
		case 0xFC:
			receivedState = ReceivedByteFC;
			break;
		case 0xFD:
			receivedState = ReceivedByteFD;
			break;
		default:
			OnReceivedError();
			break;
	}
}

void OnReceivedByteFA(u8 c)
{
	receivedBuffer[receivedBufferCount++] = c;
	switch(c)
	{
		case 0xFA:
			receivedState = ReceivedByteFAFA;
			expectState = ExpectByteFE;
			break;
		default:
			OnReceivedError();
			break;
	}
}

/*
void OnReceivedByteFB(u8 c)
{
	receivedBuffer[receivedBufferCount++] = c;
	switch(c)
	{
		case 0xFA:
			receivedState = ReceivedByteFBFB;
			expectState = ExpectByteFE;
			break;
		default:
			OnReceivedError();
			break;
	}
}
*/
void OnReceivedByteFC(u8 c)
{
	receivedBuffer[receivedBufferCount++] = c;
	switch(c)
	{
		case 0xFC:
			receivedState = ReceivedByteFCFC;
			expectState = ExpectByteFE;
			break;
		default:
			OnReceivedError();
			break;
	}
}

void OnReceivedByteFD(u8 c)
{
	receivedBuffer[receivedBufferCount++] = c;
	switch(c)
	{
		case 0xFD:
			receivedState = ReceivedByteFDFD;
			expectState = ExpectByteFE;
			break;
		default:
			OnReceivedError();
			break;
	}
}

bool IsValidBase64Byte(u8 c)
{
	if((c >= 'A') && (c <= 'Z'))  
	{   
		return TRUE;  
	}
	else if((c >= 'a') && (c <= 'z'))  
	{   
		return TRUE;  
	}else if((c >= '0') && (c <= '9'))  
	{   
		return TRUE;  
	}else if(c == '+')  
	{   
		return TRUE;  
	}else if(c == '/')  
	{   
		return TRUE;  
	}else if(c == '=')  
	{   
		return TRUE;  
	}  
	return FALSE;  
}

void OnExpectByteFE(u8 c)
{
	receivedBuffer[receivedBufferCount++] = c;
	switch(expectState)
	{
		case ExpectByteFE:
			switch(c)
			{
				case 0xFE:
					expectState = ExpectByteFEFE;
					break;
				case 0xFA:
				case 0xFB:
				case 0xFC:
				case 0xFD:
					//重置
					ResetReceivedState();
					OnReceivedByteIdle(c);
					break;
				default:
					//if(IsValidBase64Byte(c) != TRUE)
					//{
					//	ResetReceivedState();
					//}
					break;
			}
			
			//if(receivedBufferCount >= sizeof(receivedBuffer)){RECEIVED_BUFFER_LEN
			if(receivedBufferCount >= RECEIVED_BUFFER_LEN){
				OnReceivedError();	
			}
			break;
		case ExpectByteFEFE:
			if(c != 0xFE){
				OnReceivedError();	
			}
			else {
				OnReceivedOk();
			}
			break;
		default:
			OnReceivedError();	
			break;
	}
}

void OnReceivedCommandByte(u8 c)
{
	switch(receivedState)
	{
		case ReceivedByteIdle:
			OnReceivedByteIdle(c);
			break;
		case ReceivedByteFA:
			OnReceivedByteFA(c);
			break;
		//case ReceivedByteFB:
		//	OnReceivedByteFB(c);
		//	break;
		case ReceivedByteFC:
			OnReceivedByteFC(c);
			break;
		case ReceivedByteFD:
			OnReceivedByteFD(c);
			break;
    case ReceivedByteFAFA:
    //case ReceivedByteFBFB:
    case ReceivedByteFCFC:
    case ReceivedByteFDFD:
			OnExpectByteFE(c);
			break;			
		default:
			break;
	}
}	
