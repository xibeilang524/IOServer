#include "StdAfx.h"
#include "HeartThread.h"



CHeartThread::CHeartThread()
{

}
CHeartThread::~CHeartThread()
{

}

//报文为SYSTEMTIME 结构，后面跟这16字节的CRC32 共20字节

int CHeartThread::DoOneJob(CJobItem* pjob)
{
	ASSERT(m_pconpool != NULL);
	BYTE *psend = NULL;
	int  nsend = 0;

	int nsize = 0;
	BYTE *pb = pjob->m_bin.GetBuf(nsize);
	if((pb == NULL) ||(nsize != 36)) //错误报文
	{
		gGetDriveMgr()->m_nbakddcconnum = 0; //置备DDC无连接
		m_frmsend.MakeFrm(OD_DDC_ERROR,0,0,OD_DDC_ERROR,(BYTE*)NULL,0);
		psend = m_frmsend.GetFrm(&nsend);
		m_pconpool->WriteSocket(pjob->m_dwid,psend,nsend,1000);
		return 0;
	}
	DWORD dwcrc32 = 0,*pdw = (DWORD*)(pb + 32);
	switch(pb[8])
	{
	case OD_DDC_HEART: //心跳命令
		gGetDriveMgr()->m_lHeartTime = (long)::time(NULL);
		dwcrc32 = Utility::GetCRC32(pb+16,16);
		if(dwcrc32 != *pdw)
		{
			gGetDriveMgr()->m_nbakddcconnum = 0;//置备DDC无连接
			m_frmsend.MakeFrm(OD_DDC_ERROR,0,0,OD_DDC_ERROR,(BYTE*)NULL,0);
		}
		else 
		{
			gGetDriveMgr()->m_nbakddcconnum = *((long*)(pb + 16));//备DDC的已连接个数
			//这里判断是否将工作交与
			if((::gGetDriveMgr()->DDSConnectedNum()  < 1) && (gGetDriveMgr()->m_nbakddcconnum > 0))
				m_frmsend.MakeFrm(OD_DDC_HEART,0,0,0xFE,(BYTE*)NULL,0); //请备DDC工作
			else
				m_frmsend.MakeFrm(OD_DDC_HEART,0,0,0x00,(BYTE*)NULL,0); //请备DDC休息			
		}
		break;
	default:
		m_frmsend.MakeFrm(OD_DDC_ERROR,0,0,OD_DDC_ERROR,(BYTE*)NULL,0);
		break;
	}
	psend = m_frmsend.GetFrm(&nsend);
	m_pconpool->WriteSocket(pjob->m_dwid,psend,nsend,1000);
	return 0;
}

