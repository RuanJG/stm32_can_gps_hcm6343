/* -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set expandtab shiftwidth=4 tabstop=4: */
/**
 * \file
 * <PRE>
 * MODP_B64 - High performance base64 encoder/decoder
 * Version 1.3 -- 17-Mar-2006
 * http://modp.com/release/base64
 *
 * Copyright &copy; 2005, 2006  Nick Galbreath -- nickg [at] modp [dot] com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *   Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 *   Neither the name of the modp.com nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This is the standard "new" BSD license:
 * http://www.opensource.org/licenses/bsd-license.php
 * </PRE>
 */

/* public header */
#include "base64.h"

/*
 * If you are ripping this out of the library, comment out the next
 * line and uncomment the next lines as approrpiate
 */
//#include "config.h"

/* if on motoral, sun, ibm; uncomment this */
/* #define WORDS_BIGENDIAN 1 */
/* else for Intel, Amd; uncomment this */
/* #undef WORDS_BIGENDIAN */

#include "base64_data.h"

#define BADCHAR 0x01FFFFFF

/**
 * you can control if we use padding by commenting out this
 * next line.  However, I highly recommend you use padding and not
 * using it should only be for compatability with a 3rd party.
 * Also, 'no padding' is not tested!
 */
#define DOPAD 1

/*
 * if we aren't doing padding
 * set the pad character to NULL
 */
#ifndef DOPAD
#undef CHARPAD
#define CHARPAD '\0'
#endif

int base64_encode(char* dest, const char* str, int len)
{
    int i;
    uint8_t* p = (uint8_t*) dest;

    /* unsigned here is important! */
    uint8_t t1, t2, t3;

    for (i = 0; i < len - 2; i += 3) {
        t1 = str[i]; t2 = str[i+1]; t3 = str[i+2];
        *p++ = e0[t1];
        *p++ = e1[((t1 & 0x03) << 4) | ((t2 >> 4) & 0x0F)];
        *p++ = e1[((t2 & 0x0F) << 2) | ((t3 >> 6) & 0x03)];
        *p++ = e2[t3];
    }

    switch (len - i) {
    case 0:
        break;
    case 1:
        t1 = str[i];
        *p++ = e0[t1];
        *p++ = e1[(t1 & 0x03) << 4];
        *p++ = CHARPAD;
        *p++ = CHARPAD;
        break;
    default: /* case 2 */
        t1 = str[i]; t2 = str[i+1];
        *p++ = e0[t1];
        *p++ = e1[((t1 & 0x03) << 4) | ((t2 >> 4) & 0x0F)];
        *p++ = e2[(t2 & 0x0F) << 2];
        *p++ = CHARPAD;
    }

    *p = '\0';
    return p - (uint8_t*)dest;
}

#ifdef WORDS_BIGENDIAN   /* BIG ENDIAN -- SUN / IBM / MOTOROLA */
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXxx
int base64_decode(char* dest, const char* src, int len)
{
	base64_decode_ext(dest, src, 0, len);
}

int base64_decode_ext(char* dest, const char* src, int offset, int len)
{
    if (len == 0) return 0;

#ifdef DOPAD
    /* if padding is used, then the message must be at least
       4 chars and be a multiple of 4.
       there can be at most 2 pad chars at the end */
    if (len < 4 || (len % 4 != 0)) return -1;
    if (src[len-1] == CHARPAD) {
        len--;
        if (src[len -1] == CHARPAD) {
            len--;
        }
    }
#endif  /* DOPAD */

    int i;
    int leftover = len % 4;
    int chunks = (leftover == 0) ? len / 4 - 1 : len /4;

    uint8_t* p = (uint8_t*) dest;
    uint32_t x = 0;
    uint32_t* destInt = (uint32_t*) p;
    uint32_t* srcInt = (uint32_t*) src;
    uint32_t y = *srcInt++;
    for (i = 0; i < chunks; ++i) {
        x = d0[y >> 24 & 0xff] | d1[y >> 16 & 0xff] |
            d2[y >> 8 & 0xff] | d3[y & 0xff];

        if (x >= BADCHAR)  return -1;
        *destInt = x << 8;
        p += 3;
        destInt = (uint32_t*)p;
        y = *srcInt++;
    }

    switch (leftover) {
    case 0:
        x = d0[y >> 24 & 0xff] | d1[y >> 16 & 0xff] |
            d2[y >>  8 & 0xff] | d3[y & 0xff];
        if (x >= BADCHAR)  return -1;
        *p++ = ((uint8_t*)&x)[1];
        *p++ = ((uint8_t*)&x)[2];
        *p = ((uint8_t*)&x)[3];
        return (chunks+1)*3;
    case 1:
        x = d3[y >> 24];
        *p =  (uint8_t)x;
        break;
    case 2:
        x = d3[y >> 24] *64 + d3[(y >> 16) & 0xff];
        *p =  (uint8_t)(x >> 4);
        break;
    default:  /* case 3 */
        x = (d3[y >> 24] *64 + d3[(y >> 16) & 0xff])*64 +
            d3[(y >> 8) & 0xff];
        *p++ = (uint8_t) (x >> 10);
        *p = (uint8_t) (x >> 2);
        break;
    }

    if (x >= BADCHAR) return -1;
    return 3*chunks + (6*leftover)/8;
}
#endif
#else /* LITTLE  ENDIAN -- INTEL AND FRIENDS */

int base64_decode(char* dest, const char* src, int len)
{
    int i;
    int leftover = len % 4;
    int chunks = (leftover == 0) ? len / 4 - 1 : len /4;

    uint8_t* p = (uint8_t*)dest;
    uint32_t x = 0;
    uint32_t* destInt = (uint32_t*) p;
    uint32_t* srcInt = (uint32_t*) src;
    uint32_t y = *srcInt++;
	
    if (len == 0) return 0;

#ifdef DOPAD
    /*
     * if padding is used, then the message must be at least
     * 4 chars and be a multiple of 4
     */
    if (len < 4 || (len % 4 != 0)) return -1; /* error */
    /* there can be at most 2 pad chars at the end */
    if (src[len-1] == CHARPAD) {
        len--;
        if (src[len -1] == CHARPAD) {
            len--;
        }
    }
#endif

    for (i = 0; i < chunks; ++i) {
        x = d0[y & 0xff] |
            d1[(y >> 8) & 0xff] |
            d2[(y >> 16) & 0xff] |
            d3[(y >> 24) & 0xff];

        if (x >= BADCHAR) return -1;
        *destInt = x ;
        p += 3;
        destInt = (uint32_t*)p;
        y = *srcInt++;}


    switch (leftover) {
    case 0:
        x = d0[y & 0xff] |
            d1[(y >> 8) & 0xff] |
            d2[(y >> 16) & 0xff] |
            d3[(y >> 24) & 0xff];

        if (x >= BADCHAR) return -1;
        *p++ =  ((uint8_t*)(&x))[0];
        *p++ =  ((uint8_t*)(&x))[1];
        *p =    ((uint8_t*)(&x))[2];
        return (chunks+1)*3;
        //break;
    case 1:  /* with padding this is an impossible case */
        x = d0[y & 0xff];
        *p = *((uint8_t*)(&x)); // i.e. first char/byte in int
        break;
    case 2: // * case 2, 1  output byte */
        x = d0[y & 0xff] | d1[y >> 8 & 0xff];
        *p = *((uint8_t*)(&x)); // i.e. first char
        break;
    default: /* case 3, 2 output bytes */
        x = d0[y & 0xff] |
            d1[y >> 8 & 0xff ] |
            d2[y >> 16 & 0xff];  /* 0x3c */
        *p++ =  ((uint8_t*)(&x))[0];
        *p =  ((uint8_t*)(&x))[1];
        break;
    }

    if (x >= BADCHAR) return -1;

    return 3*chunks + (6*leftover)/8;
}

#endif  /* if bigendian / else / endif */

const char BASE_CODE[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";  
	  
//编码，参数：要编码的字符串指针，解码后存放的位置（编码字串长度的4/3），要编码的字符串长度 ->返回结果长度  
int fnBase64Encode(char *lpString, char *lpBuffer, int sLen)   
{   
	register int vLen = 0;  //寄存器局部变量，提速  
	
	while(sLen > 0)      //处理整个字符串  
	{   
		*lpBuffer++ = BASE_CODE[(lpString[0] >> 2 ) & 0x3F];  //右移两位，与00111111是防止溢出，自加  
		if(sLen > 2) //够3个字符  
		{   
			*lpBuffer++ = BASE_CODE[((lpString[0] & 3) << 4) | (lpString[1] >> 4)];  
			*lpBuffer++ = BASE_CODE[((lpString[1] & 0xF) << 2) | (lpString[2] >> 6)];  
			*lpBuffer++ = BASE_CODE[lpString[2] & 0x3F];  
		}
		else  
		{   
			switch(sLen)    //追加“=”  
			{   
				case 1:  
					*lpBuffer ++ = BASE_CODE[(lpString[0] & 3) << 4 ];  
					*lpBuffer ++ = '=';  
					*lpBuffer ++ = '=';  
					break;  
				case 2:  
					*lpBuffer ++ = BASE_CODE[((lpString[0] & 3) << 4) | (lpString[1] >> 4)];  
					*lpBuffer ++ = BASE_CODE[((lpString[1] & 0x0F) << 2) | (lpString[2] >> 6)];  
					*lpBuffer ++ = '=';  
					break;  
			}  
		}  
		lpString += 3;  
		sLen -= 3;  
		vLen +=4;  
	}  
	*lpBuffer = 0;  
	return vLen;  
}  
  

//子函数 - 取密文的索引  
char GetCharIndex(char c) //内联函数可以省去函数调用过程，提速  
{   
	if((c >= 'A') && (c <= 'Z'))  
	{   
		return c - 'A';  
	}
	else if((c >= 'a') && (c <= 'z'))  
	{   
		return c - 'a' + 26;  
	}else if((c >= '0') && (c <= '9'))  
	{   
		return c - '0' + 52;  
	}else if(c == '+')  
	{   
		return 62;  
	}else if(c == '/')  
	{   
		return 63;  
	}else if(c == '=')  
	{   
		return 0;  
	}  
	return 0;  
}  
  
//解码，参数：结果，密文，密文长度  
int fnBase64Decode(char *lpString, char *lpSrc, int sLen)   //解码函数  
{   
	static char lpCode[4];  
	register int vLen = 0;  
	if(sLen % 4)        //Base64编码长度必定是4的倍数，包括'='  
	{   
		lpString[0] = '\0';  
		return -1;  
	}  
	while(sLen > 4)      //不足四个字符，忽略  
	{   
		lpCode[0] = GetCharIndex(lpSrc[0]);  
		lpCode[1] = GetCharIndex(lpSrc[1]);  
		lpCode[2] = GetCharIndex(lpSrc[2]);  
		lpCode[3] = GetCharIndex(lpSrc[3]);  

		*lpString++ = (lpCode[0] << 2) | (lpCode[1] >> 4);  
		*lpString++ = (lpCode[1] << 4) | (lpCode[2] >> 2);  
		*lpString++ = (lpCode[2] << 6) | (lpCode[3]);  

		lpSrc += 4;  
		sLen -= 4;  
		vLen += 3; 
	}  
	if(sLen == 4)
	{
		lpCode[0] = GetCharIndex(lpSrc[0]);  
		lpCode[1] = GetCharIndex(lpSrc[1]);  
		lpCode[2] = GetCharIndex(lpSrc[2]);  
		lpCode[3] = GetCharIndex(lpSrc[3]);  
		
		*lpString++ = (lpCode[0] << 2) | (lpCode[1] >> 4);  
		*lpString++ = (lpCode[1] << 4) | (lpCode[2] >> 2);  
		*lpString++ = (lpCode[2] << 6) | (lpCode[3]);  

		if(lpSrc[2] == '=')
		{
			vLen += 1; 
		}
		else if(lpSrc[3] == '=')
		{
			vLen += 2; 
		}
		else 
		{
			vLen += 3; 
		}
	}
	return vLen;
}
