
#ifndef _IAP_H
#define _IAP_H


#include "stm32f10x_conf.h"
#include "stm32f10x.h"
#include "system.h"



//*******************  may need to configure these paramter

/*
#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_CL) || defined (STM32F10X_XL)
  #define FLASH_PAGE_SIZE    ((uint16_t)0x800)
#else
  #define FLASH_PAGE_SIZE    ((uint16_t)0x400)
#endif
*/
#define FLASH_PAGE_SIZE    ((uint16_t)0x400)


// Define Application Address Area */
// iap firmware : 0x8000000   0x4000                         ; 0x4000/1024 = 16 = 16kB         
// app firmware : 0x8004000   (0x10000 - 0x4000) = 0xC000
#define ApplicationOffset 0x4000
#define ApplicationAddress  (0x08000000 | ApplicationOffset)

// ********************************************************************************************************************


typedef void(*iapFunction)(void);


//****************** remote program potocal
//id
#define PACKGET_START_ID 1 //[id start]data{[0]}
#define PACKGET_ACK_ID 2 //[id ack]data{[ACK_OK/ACK_FALSE/ACK/RESTART][error code]}
#define PACKGET_DATA_ID 3 //[id data] data{[seq][]...[]}
#define PACKGET_END_ID 4 //[id end] data{[stop/jump]}
//data
#define PACKGET_ACK_OK 1
#define PACKGET_ACK_FALSE 2
#define PACKGET_ACK_RESTART 3
#define PACKGET_END_JUMP 1
#define PACKGET_END_STOP 2
#define PACKGET_MAX_DATA_SEQ 200  // new_seq = (last_seq+1)% PACKGET_MAX_DATA_SEQ
// ack error code
#define PACKGET_ACK_FALSE_PROGRAM_ERROR 21 // the send data process stop
#define PACKGET_ACK_FALSE_ERASE_ERROR 22 // the send data process stop
#define PACKGET_ACK_FALSE_SEQ_FALSE 23 // the send data process will restart
// if has ack false , the flash process shuld be restart




#endif //_IAP_H
