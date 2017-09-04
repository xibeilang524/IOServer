// MyFrmParse.cpp: implementation of the CMyFrmParse class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "101Drv.h"
#include "MyFrmParse.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyFrmParse::CMyFrmParse()
{

}

CMyFrmParse::~CMyFrmParse()
{

}

//
BOOL CMyFrmParse::CheckCrc32(BYTE *pd)//按照16字节报头校验
{
	DWORD dwCRC32 = CCheckAlgorithm::TL_GetCRC32(pd,12);
	DWORD *pcrc = (DWORD *)(pd+12);
	if(*pcrc != dwCRC32)
		return FALSE;
	return TRUE;
}

/*
分包处理，根据实际情况改动分包算法

  返回值：
	0:没找到报头标志
	1:找到报头标志,长度不够无法验证报头
	2:报头校验完成,数据长度不够
	3:报文完成,
*/  
int CMyFrmParse::Parse()//分包,返回0，1，2，3
{
	int nsize = m_buf.GetDataSize();
	if(nsize < 4)//包头长度4
		return 0;
	BYTE *pc = m_buf.GetBuf();
	
	//无回朔扫描比较
	int np=0;//np指示向前移动字节数
	BOOL bFindHead=FALSE;
	
lpFindHeadFlag:
	
	while(np <= (nsize - 1))
	{
		if(pc[np] != 0x68){
			np++;	continue;
		}
		
		if(pc[np+3] != 0x68){
			np += 3;	continue;
		}
		bFindHead = TRUE;
		break;
	}
	if(!bFindHead)	//没找到报文头标志
	{
		m_buf.LeftMove(np);
		return 0;
	}
	if((np +4) > nsize)
	{
		m_buf.LeftMove(np);//长度不够
		return 1;
	}
	
// 	//验证CRC32
// 	if(!CheckCrc32(pc+np))
// 	{
// 		np++;
// 		goto lpFindHeadFlag;
// 	}
// 	//
	BYTE *pf = pc+np;
	const BYTE bFrameSize = *(pf+1);
	DWORD dSize = bFrameSize;
	DWORD *pdw = 4+&dSize+2;
	//	DWORD *pdw = (DWORD *)(pf+4);
	if(*pdw > (1024 * 1024))//MAX_FRM_LEN) //报文过长，可能是错误的报文
	{
		np++;
		goto lpFindHeadFlag;
	}
	m_nFrmLen = (int)(*pdw);//重置报文长度
	if((np + m_nFrmLen) <= nsize)
	{
		m_Frm.ClearData();//清空报文
		m_Frm.PutData(pc+np,m_nFrmLen);//复制报文
		m_buf.LeftMove(np + m_nFrmLen);//缓冲中剩下的字节左移
		m_nStatus = 3;
		return 3;
	}
	
	m_Frm.ClearData();//清空报文
	m_Frm.PutData(pc+np,nsize - np);//复制余下的字节
	m_buf.ClearData();//清空待处理缓冲
	m_nStatus = 2;
	return 2;//报头正确
}

// 字节序转换
short CMyFrmParse::SwitchBytes(short wBytes,BOOL bSwitchBytes)
{
	if (bSwitchBytes)
	{
		BYTE btVal[2];
		BYTE *p = (BYTE *)&wBytes;
		btVal[0] = p[1];
		btVal[1] = p[0];

		return *((WORD*)btVal);
	}
	else {
		return wBytes;
	}
}

float CMyFrmParse::SwitchShortToFloat(short wBytes,BOOL bSwitchBytes)
{
	BYTE btVal[2];
	BYTE *p = (BYTE *)&wBytes;

	if (bSwitchBytes)
	{
		btVal[0] = p[1];
		btVal[1] = p[0];
		return *((float*)btVal);
	}
	else {
		btVal[0] = p[0];
		btVal[1] = p[1];
		return *((float*)btVal);
	}
}

DWORD CMyFrmParse::SwitchBytes(DWORD wBytes,BOOL bSwitchBytes)
{
	if (bSwitchBytes)
	{
		BYTE btVal[4];
		BYTE *p = (BYTE *)&wBytes;
		btVal[0] = p[3];
		btVal[1] = p[2];
		btVal[2] = p[1];
		btVal[3] = p[0];
		return  *((DWORD*)btVal);
	}
	else {
		return wBytes;
	}
}

// 字节序转换
float CMyFrmParse::SwitchBytes(float fltBytes,BOOL bSwitchBytes)
{

	if (bSwitchBytes)
	{
		BYTE btVal[4];
		BYTE *p = (BYTE *)&fltBytes;
		btVal[0] = p[3];
		btVal[1] = p[2];
		btVal[2] = p[1];
		btVal[3] = p[0];
		return *((float*)btVal);
	}
	else {
		return fltBytes;
	}
}