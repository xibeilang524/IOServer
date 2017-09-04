// MyFrmParse.cpp: implementation of the CMyFrmParse class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyFrmParse.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyFrmParse::CMyFrmParse()
{

}

CMyFrmParse::~CMyFrmParse()
{

}

/*
分包处理，根据实际情况改动分包算法

  返回值：
	0:没找到报头标志
	1:找到报头标志,长度不够无法验证报头
	2:报头校验完成,数据长度不够
	3:报文完成,
  */
//////////////////////////////////////////////////////////////////////////
int CMyFrmParse::Parse()//分包,返回0，1，2，3
{
	int nsize=m_buf.GetDataSize();

	if (nsize == LEN_E5 )
	{
		return Parse_E5();
	}
	else if (nsize ==  LEN_FIX_FRM )
	{ 
        return Parse_Fix();
	} 
	else
	{
		return Parse_Vari();
	}
}

BYTE    CMyFrmParse::GetByte(BYTE btData, char cType)
{
	BYTE btRet = 0;
	if (cType == GET_BYTE_HIGH)
	{
		btRet = btData &0xF0;
		btRet = btRet>>4;
	}
	else
	{
		btRet = btData &0x0F;
	}
	return   btRet;
}

BYTE  CMyFrmParse::CalaCS(BYTE *pData, int iSize)
{
	BYTE byCS = 0;
	if (pData == NULL || iSize <= 0)
	{       return byCS;      }

	for (int i = 0 ; i < iSize; i++ )
	{
		byCS += pData[i];
	}

	return byCS;
}

int CMyFrmParse::Parse_E5() //分包,返回0，1，2，3
{
	int nsize=m_buf.GetDataSize();
	if(nsize < LEN_E5)//报头长度
		return 0;
	BYTE *pc = m_buf.GetBuf();

	m_nFrmLen = LEN_E5;
	if (pc[0] == 0xE5)
	{
		m_Frm.ClearData();//清空报文
		m_Frm.PutData(pc, m_nFrmLen);//复制报文
		m_buf.LeftMove(m_nFrmLen);//缓冲中剩下的字节左移
		m_nStatus = FRAME_STATUS_OK;
		return m_nStatus;
	}
	m_Frm.ClearData();//清空报文
	m_Frm.PutData(pc,nsize);//复制余下的字节
	m_buf.ClearData();//清空待处理缓冲
	m_nStatus = FRAME_STATUS_HEAD_NO_ENOUGH;
	return m_nStatus;//报头正确
}

int CMyFrmParse::Parse_Fix()//分包,返回0，1，2，3
{
	int nsize=m_buf.GetDataSize();
	if(nsize < LEN_FIX_FRM )//报头长度
		return 0;

	BYTE *pc = m_buf.GetBuf();
	//无回朔扫描比较
	int np=0;//np指示向前移动字节数
	BOOL bFindHead=FALSE;

lpFindHeadFlag:
	while(np <= (nsize - LEN_FIX_FRM))
	{
		if(pc[np] != 0x10){
			np++;	continue;}

		if(pc[np+5] != 0x16){
			np++;		continue;}

		bFindHead = TRUE;
		break;
	}

	if(!bFindHead)	{  //没找到报文头标志
		m_buf.LeftMove(np);
		return 0;   }

	if((np + LEN_FIX_FRM ) > nsize){
		m_buf.LeftMove(np);//长度不够
		return 1;  }

	//验证CS
	BYTE *pf = pc+np;
	FIXFRM *pFIXFRM = (LPFIXFRM)pf;

	BYTE byCS = CalaCS(&pFIXFRM->btCtrl, 3);
	if(byCS != pFIXFRM->pEndFrm.btCS){
		np++;
		goto lpFindHeadFlag;      }

	m_nFrmLen = LEN_FIX_FRM;//重置报文长度
	if ((np + m_nFrmLen) <= nsize){
		m_Frm.ClearData();//清空报文
		m_Frm.PutData(pc + np, m_nFrmLen);//复制报文
		m_buf.LeftMove(np + m_nFrmLen);//缓冲中剩下的字节左移

		m_nStatus = FRAME_STATUS_OK;
		return m_nStatus;}

	m_Frm.ClearData();//清空报文
	m_Frm.PutData(pc + np,nsize - np);//复制余下的字节
	m_buf.ClearData();//清空待处理缓冲
	m_nStatus = FRAME_STATUS_HEAD_NO_ENOUGH;
	return m_nStatus;//报头正确
}

int CMyFrmParse::Parse_Vari()//分包,返回0，1，2，3
{
	int nsize=m_buf.GetDataSize();

	int iMinMsgSize = sizeof(VARIHEADER) + sizeof(ENDFRM);
	if(nsize < iMinMsgSize )//报头长度
		return 0;
	BYTE *pc = m_buf.GetBuf();

	//无回朔扫描比较
	int np=0;//np指示向前移动字节数
	BOOL bFindHead=FALSE;

lpFindHeadFlag:
	while(np <= (nsize - LEN_FIX_FRM))
	{
		if(pc[np] != 0x68)  {
			np++;	continue;}

		if(pc[np+3] != 0x68) {
			np++;		continue;}

		bFindHead = TRUE;
		break;
	}

	if(!bFindHead){ 	//没找到报文头标志
		m_buf.LeftMove(np);
		return 0;   }

	if((np + iMinMsgSize ) > nsize)  {
		m_buf.LeftMove(np);//长度不够
		return 1;   }

	BYTE *pf = pc+np;
	VARIHEADER *pVARIHEADER = (LPVARIHEADER)pf;

	// 长度校验错误
	if (pVARIHEADER->btLen  != pVARIHEADER->btLen2) {
		np++;
		goto lpFindHeadFlag;  }

	int iFactLen = pVARIHEADER->btLen + 6;
	if((np + iFactLen ) > nsize)  {
		m_buf.LeftMove(np);   //实际长度不够
		return 1;   }

	//验证CS
	BYTE byCS = CalaCS(&pVARIHEADER->btCtrl, pVARIHEADER->btLen);
	ENDFRM *pENDFRM = (LPENDFRM)(pf +  pVARIHEADER->btLen +4);
	if(byCS != pENDFRM->btCS){
		np++;
		goto lpFindHeadFlag; }

	m_nFrmLen = iFactLen;   //重置报文长度
	if ((np + m_nFrmLen) <= nsize)
	{
		m_Frm.ClearData();//清空报文
		m_Frm.PutData(pc + np, m_nFrmLen);//复制报文
		m_buf.LeftMove(np + m_nFrmLen);//缓冲中剩下的字节左移

		m_nStatus = FRAME_STATUS_OK;
		return m_nStatus;
	}

	m_Frm.ClearData();//清空报文
	m_Frm.PutData(pc + np,nsize - np);//复制余下的字节
	m_buf.ClearData();//清空待处理缓冲
	m_nStatus = FRAME_STATUS_DATA_NO_ENOUGH;
	return m_nStatus;//报头正确
}
//////////////////////////////////////////////////////////////////////////

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

DWORD CMyFrmParse::SwitchBytes(BYTE * pBytes,BOOL bSwitchBytes)
{
	if ( pBytes == NULL )
	{
		return 0;
	}

	BYTE btVal[4];

	if (bSwitchBytes)
	{
		btVal[0] = pBytes[0];
		btVal[1] = pBytes[1];
		btVal[2] = pBytes[2];
		btVal[3] = pBytes[3];
	}
	else {
		btVal[0] = pBytes[3];
		btVal[1] = pBytes[2];
		btVal[2] = pBytes[1];
		btVal[3] = pBytes[0];
	}

	return *((DWORD*)btVal);
}
