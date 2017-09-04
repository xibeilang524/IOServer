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
int CMyFrmParse::Parse()//分包,返回0，1，2，3
{
	int nsize=m_buf.GetDataSize();
	if(nsize < 2)//报头长度2
		return 0;
	BYTE *pc = m_buf.GetBuf();
	
	//无回朔扫描比较
	int np=0;//np指示向前移动字节数
	BOOL bFindHead=FALSE;

lpFindHeadFlag:
	while(np <= (nsize - 2))
	{
		if(pc[np] != 0x68){
			np++;	continue;
		}
		if(pc[np+1] > 0xFD){
			np++;		continue;
		}
	
		bFindHead = TRUE;
		break;
	}
	if(!bFindHead)	//没找到报文头标志
	{
		m_buf.LeftMove(np);
		return 0;
	}
	if((np + 2) > nsize)
	{
		m_buf.LeftMove(np);//长度不够
		return 1;
	}

	//验证CRC32
/*	if(!CheckCrc32(pc+np))
	{
		np++;
		goto lpFindHeadFlag;
	}*/
	//
/*	BYTE *pf = pc+np;
	DWORD *pdw = (DWORD *)(pf);
	if(*pdw > (1024 * 1024))//MAX_FRM_LEN) //报文过长，可能是错误的报文
	{
		np++;
		goto lpFindHeadFlag;
	}
	m_nFrmLen = (int)(*pdw);//重置报文长度*/

	int ii = *(pc +2);
	int nFrameType = (ii & 0x03);
	if (nFrameType == 3) //U帧
	{
		m_nFrmLen = 6;
	}
	else
	{
		int nFrameType = (ii & 0x01);
		if (nFrameType == 0) //I帧
		{
			m_nFrmLen = 2 + *(pc +1);
		}
		else if (nFrameType == 1) //S帧
		{
			m_nFrmLen = 6;
		}
	}
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
