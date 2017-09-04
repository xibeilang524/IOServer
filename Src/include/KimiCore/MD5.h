#ifndef __KIMI_KERNEL_MD5_H__
#define __KIMI_KERNEL_MD5_H__

#include <string>

_KIMI_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////
// MD5 加密算法
#ifndef POINTER
typedef unsigned char *POINTER; 
#endif // POINTER

#ifndef UINT2
typedef unsigned short int UINT2; 
#endif // UINT2

#ifndef UINT4
typedef unsigned long int UINT4; 
#endif // UINT4

typedef struct
{
	UINT4 state[4]; 
	UINT4 count[2]; 
	unsigned char buffer[64]; 
} MD5_CTX; 

KIMI_CORE_API(void) MD5Init(MD5_CTX *); 
KIMI_CORE_API(void) MD5Update(MD5_CTX *, unsigned char *, unsigned int); 
KIMI_CORE_API(void) MD5Final(unsigned char [16], MD5_CTX *); 

// 将字符串编码为16个字节的二进制码
KIMI_CORE_API(void) MD5_Encode(unsigned char *inBuf, unsigned int nsize, unsigned char outBuf[16]);

// 将由MD5_Encode函数生成的16个字节的二进制码转换成32个字节的字符串(第个字节转换为两位)
KIMI_CORE_API(void) MD5_ToString(unsigned char inBuf[16], char outBuf[33]);

// MD5_ToString() 函数的互逆函数
KIMI_CORE_API(void) MD5_ToCode(char inBuf[33], unsigned char outBuf[16]);

//MD5加密变成32字节,lpszTxt
//KIMI_CORE_API(BOOL) MakeMD5Txt32(const char *lpszTxt,  string &szMD5);

//MD5加密变成32字节,lpszTxt
//KIMI_CORE_API(BOOL) MakeMD5Txt32W(const wchar_t *lpszTxt, wstring &szMD5);


_KIMI_NAMESPACE_END
///////////////////////////////////////////////////////////////////////////

#endif // end of __KIMI_KERNEL_MD5_H__