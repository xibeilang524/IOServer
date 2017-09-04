/*****************************************************************
* CopyRight By 西安热工研究院有限公司
* All rights reserved.
* 文件名称: CDrvFactory.cpp 101规约接口驱动实现文件
* 摘    要：按需求实现101规约。
* 当前版本：1.0
* 作    者：ZCQ
* 创建日期：2013.06.11
*****************************************************************/
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "101Drv.h"
#include "MyDrvObj.h"
//#include "General\DebugUtils.h>

#include <math.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////
//实现静态对象类工厂
CDrvObj* CDrvFactory::NewDrv()
{
	return new CMyDrvObj();
}

DRVRET CDrvFactory::GetDriverInfo(LPDRVINFO lpDrvInfo)
{
	memset(lpDrvInfo, 0, sizeof(T_DRVINFO));
	strcpy_s(lpDrvInfo->sname, "101规约接口驱动");
	strcpy_s(lpDrvInfo->sdes, "CopyRight By 西安热工研究院有限公司");
	strcpy_s(lpDrvInfo->sver, "2.0.0.0"); //程序版本
	lpDrvInfo->dwver = 0x0200;//规范版本
	lpDrvInfo->dwres = 0;
	return DRERR_OK;
}

//////////////////////////////////////////////////////////////////////
// CMyDrvObj class
//////////////////////////////////////////////////////////////////////

CMyDrvObj::CMyDrvObj()
{

	// 设置配置串行化存储时的标志ID，最好不同驱动之间设置不同值以示区分
	SetSerializeID(0x10000007);

	// 设置通讯协议类型，如果支持多协议，则可以用 | 的或操作连接多协议声明：
	// SetComTypes(COM_TYPE_UDP | COM_TYPE_TCP);
	// 然后在CreateProtocol()接口中根据用户选择的协议来创建协议处理类对象实例;
	SetComTypes(COM_TYPE_TCP |COM_TYPE_COM );
	EnabledTestConnect();
	CConfigInfo::m_szHelpFile = _T("101接口驱动.chm");
	CConfigInfo::m_szParamHintText = _T("设备地址;。例如：1;");

	CTagItem::SetCSVFormatName(_T("JinLing_101Drv"));
	SetConnectionFailedStartup(TRUE);
}

CMyDrvObj::~CMyDrvObj()
{

}

BOOL CMyDrvObj::TestConnect(CConfigInfo &config)
{
	return CDDCDrvObj::TestConnect(config);
}

DRVRET CMyDrvObj::GetErrMsg(CString& szMsg, DRVRET dwcode)
{	
	if (dwcode < 100)
		return CDrvObj::GetErrMsg(szMsg, dwcode);

	//>100是驱动自定义的，如果没有自定义错误信息，可不用实现该方法

	switch (dwcode)
	{
	case 1001:
		szMsg = _T("XXX错误");
		break;
	case 1002:
		szMsg = _T("XXX错误");
		break;
	default:
		return DRERR_NOERRCODE;
	}

	return DRERR_OK;
}

// 由CreateProtocol()创建的协议处理类实例不需要显式删除，
// 驱动管理对象会在适当时间自动删除
CDDCDrvProtocol	*CMyDrvObj::CreateProtocol()
{
	
	CDDCDrvProtocol *pObj = NULL;

	if (m_config.m_wComType  == COM_TYPE_TCP )
	{
		pObj = new CMyTCPClientProtocol();
		AddInfoFmt("创建TCP/IP 连接方式！");
	}
	else
	{
		pObj = new CMySerialProtocol();
		AddInfoFmt("创建COM串口连接方式！");
	}

	
	if (pObj)
	InitTagIdMap();

	return pObj;
}

//////////////////////////////////////////////////////////////////////
// CMySerialProtocol 协议处理类实现
//////////////////////////////////////////////////////////////////////
CMySerialProtocol::CMySerialProtocol()
{
	m_nLinkStateMachine = S_INIT;
	m_lStartTime = 0;
	m_nAddr      = 1;
	m_pBuffer = new BYTE[TCP_BUFFER_SIZE];
}

CMySerialProtocol::~CMySerialProtocol()
{
	delete[] m_pBuffer;
}

DRVRET CMySerialProtocol::WriteVar(LPDRREC lpval)
{
	return DRERR_NOTSUPPORT;
}

/**************************************************************
// 解析过程
****************************************************************/
void CMySerialProtocol::DoRun()
{
	int nUpdateSec = m_pDrvObj->m_config.m_nUpdateSec/1000;
	long lEndTime = time(NULL);
	if (m_lStartTime != 0)
	{		
		if (lEndTime - m_lStartTime < nUpdateSec)
		{
			Sleep(10);
			return;
		}
	}

	if (!IsConnect())
	{
		if (!Connect())
			return;
	}

	m_nLinkStateMachine = S_RECIEVE_DATA;

	switch(m_nLinkStateMachine)
	{
	case S_INIT:                                    // 初始状态
	case S_LINK_REQ:                                // 请求远方链路{10 49 01 4A 16 }
		if (DoSend(S_LINK_REQ)){
			SetLinkStateMachine(S_LINK_OK);       
		}
		else
			SetLinkStateMachine(S_INIT);
		break;
	case S_LINK_OK:                                 // 确认远方链路{10 0B 01 0C 16 }
		if (DoRead(S_LINK_OK)){
			SetLinkStateMachine(S_CALL_SYS_TIME);
		}
		else
			SetLinkStateMachine(S_INIT); 
		break;
	case S_RESET_REQ:                                // 请求远方链路{10 49 01 4A 16 }
		if (DoSend(S_RESET_REQ)){
			SetLinkStateMachine(S_RESET_OK);       
		}
		else
			SetLinkStateMachine(S_INIT);
		break;
	case S_RESET_OK:                                 // 确认远方链路{10 0B 01 0C 16 }
		if (DoRead(S_RESET_OK)){
			SetLinkStateMachine(S_CALL_SYS_TIME);
		}
		else
			SetLinkStateMachine(S_INIT); 
		break;
	case S_CALL_SYS_TIME:                                 // 确认远方链路{10 0B 01 0C 16 }
		if (DoSend(S_CALL_SYS_TIME)){
			SetLinkStateMachine(S_SYS_TIME_OK);
		}
		else
			SetLinkStateMachine(S_INIT); 
		break;
	case S_SYS_TIME_OK:      
		if (!DoRead(S_SYS_TIME_OK))
		{
			m_lStartTime = ::time(NULL);
		}
		SetLinkStateMachine(S_CALL_DATA_SECOND);
		break;
	case S_CALL_DATA_SECOND:     // 确认远方链路{10 0B 01 0C 16 }
		if (DoSend(S_CALL_DATA_SECOND))
		{
			SetLinkStateMachine(S_RECIEVE_DATA);
		}
		else
			SetLinkStateMachine(S_INIT);
		break;
	case S_CALL_DATA_WHOLE:                         // 总召
		if (DoSend(S_CALL_DATA_WHOLE))
		{
			SetLinkStateMachine(S_RECIEVE_DATA); 
		}
		break;
	case S_RECIEVE_DATA:                            // 接收数据
		DoRead(S_RECIEVE_DATA);
		SetLinkStateMachine(S_INIT); 
		break;
	default:
		break;
	}
}


/**************************************************
*功    能：计算校验和
*参    数：BYTE *pData 数据
int iLen    左边起始地址（从1开始）
int iRight  右边起始地址
*返 回 值： 检查和
*创 建 者：ZCQ
*创建日期：2013.06.12
**************************************************/
int CMySerialProtocol::CheckSum(BYTE *pData,int iLen,int iLeft,int iRight)
{
	if ( NULL == pData || iLeft < 1 || iRight < 1)
	{
		return -1;
	}

	if (iLen < max(iLeft,iRight) || iLen < 3)
	{
		return -1;
	}

	int iTemp = 0;
	for (int i = iLeft-1; i< iLen - iRight+1;i++)
	{
		iTemp += pData[i];
	}
	iTemp =  LOBYTE(iTemp);

	return iTemp;
}

/**************************************************
*功    能：检查字节的某为0还是1
*参    数：BYTE byData 数据
BYTE byPos  位置（首地址为0）
*返 回 值：0还是1 
*创 建 者：ZCQ
*创建日期：2013.06.12
**************************************************/
int CMySerialProtocol::ParseByte(BYTE byData,BYTE byPos)
{
	float fRadix = 2,fIndex = byPos;
	BYTE byResult = (BYTE)pow(fRadix,fIndex);

	if (byData & byResult)
	{
		return 1;
	}
	return 0;
}

/**************************************************
*功    能：帧检查（包括包标识、校验和、结束符、长度等）
*参    数：BYTE *pBuf  数据
int iLen    长度
char cCheckType 检查类型
*返 回 值：TRUE : 成功；FALSE： 失败
*创 建 者：ZCQ
*创建日期：2013.06.12
**************************************************/
bool CMySerialProtocol::FrmBaseCheck(BYTE *pBuf, int iLen,char cCheckType)
{
	if ( NULL == pBuf || iLen < FRM_LENGTH_FIX)
	{
		return false;
	}

	if ( S_LINK_OK == cCheckType || S_RESET_OK  == cCheckType )
	{
		if (pBuf[0] != 0x10 ||  pBuf[3]!=CheckSum(pBuf,iLen,2,3) ||pBuf[4] != 0x16)
		{
			return false;
		}
	}
	else if ( S_CALL_SYS_TIME == cCheckType)
	{
		if (iLen < FRM_LENGTH_TIME)
		{
			return FALSE;
		}

		if (pBuf[0] != 0x68 || pBuf[3] != 0x68 || pBuf[iLen-2] != CheckSum(pBuf,iLen,5,3) ||pBuf[iLen -1] != 0x16)
		{
			return false;
		}
	}
	else if ( S_CALL_DATA_WHOLE == cCheckType || S_CALL_DATA_SECOND == cCheckType ||  S_CALL_DATA_FIRST == cCheckType ||  S_RECIEVE_DATA == cCheckType ) 
	{
		int iAckCount = pBuf[1] + 6;
		if (iAckCount < iLen)
		{
			return FALSE;
		}

		if (pBuf[0] != 0x68 || pBuf[3] != 0x68 || pBuf[iAckCount-2] != CheckSum(pBuf,iAckCount,5,3)||pBuf[iAckCount -1] != 0x16)
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}


BOOL CMySerialProtocol::DoRead(int iTypeCode)
{
	int iSize = ReadData(m_pBuffer, TCP_BUFFER_SIZE);
	if (iSize <= 0)
	{
		Sleep(10);
		return FALSE;
	}

	if(!FrmBaseCheck(m_pBuffer,iSize,iTypeCode))
	{
		m_pDrvObj->AddError("接收数据帧校验错误！");
		return FALSE;
	}

	if ( S_LINK_OK == iTypeCode )
	{
		if (m_pBuffer[1] != 0x0b )
		{
			return FALSE;
		}
	}
	else if ( S_RESET_OK == iTypeCode)
	{
		if ( m_pBuffer[1] != 0x00 )
		{
			return FALSE;
		}
	}
	else if ( S_SYS_TIME_OK == iTypeCode)
	{
		//  PN是否正确(PN =1 不允许校时) ;不允许校时，采用本地时标
		if (1 ==ParseByte(m_pBuffer[8],6))
		{
			return FALSE;
		}

		// 获取从站时间，七个八位位组二进制时间(CP56Time2a)
		int iMilliSec = m_pBuffer[12] + (m_pBuffer[13] & 0x11);
		int nSecond = (m_pBuffer[13] >> 2);
		int nMin  = m_pBuffer[14];
		int nHour = m_pBuffer[15];
		int nDay = 0;
		BOOL bWeek = FALSE;
		if (bWeek)
		{
			nDay = (m_pBuffer[16] & 0x1F);
		}
		else
		{
			nDay = m_pBuffer[16];
		}
		int nMonth = m_pBuffer[17];
		int nYear  = m_pBuffer[18]+2000;
		COleDateTime sTime;
		sTime.SetDateTime(nYear, nMonth, nDay, nHour, nMin, nSecond);

		SYSTEMTIME systime;
		VariantTimeToSystemTime(sTime, &systime);
		CTime tm(systime);
		m_lStartTime  = tm.GetTime();
	}
	else if ( S_RECIEVE_DATA == iTypeCode)
	{
		DATAHEADER *pDATAHEADER = (LPDATAHEADER)m_pBuffer; 
		// 数据响应请求帧FUN = 8 并且从站回复肯定确认 PN = 0 
		if ( (pDATAHEADER->btCtrl & 0x0F) == 8 && 0 == ParseByte(pDATAHEADER->btCot,6))
		{
			//  遥信点处理流程( TI = 1 不带时标的单点信息)
			if (pDATAHEADER->btType == 1)
			{
				if (!HandleDigitPoint((BYTE*)m_pBuffer,iSize,m_lStartTime))
				{
					m_pDrvObj->AddError(_T("遥信点处理失败"));
					return FALSE;
				}
			}
			// 遥测点处理流程 (TI = 9测量值, 规一化值)
			else if (pDATAHEADER->btType == 9)
			{
				if (!HandleAnologPoint((BYTE*)m_pBuffer,iSize,m_lStartTime))
				{
					m_pDrvObj->AddError(_T("遥信点处理失败"));
					return  FALSE;
				}
			}
			// 视图刷新
			m_pDrvObj->FlushValue();
		}
	}
	return TRUE;
}

BOOL CMySerialProtocol::DoSend(int iTypeCode)
{
	int iMsgLenth = 0;
	BYTE byCS     = 0;
	BYTE pData[FRM_LENGTH_MAX] = {0};

	if (iTypeCode == S_LINK_REQ )
	{//请求远方链路{10 49 01 4A 16 }
		byCS = LOBYTE(0x49 + m_nAddr);
		pData[0] = 0x10;
		pData[1] = 0x49;
		pData[2] = m_nAddr;
		pData[3] = byCS;
		pData[4] = 0x16;
		iMsgLenth   = FRM_LENGTH_FIX;
	}
	else if ( iTypeCode == S_RESET_REQ )
	{//复位远方链路{10 40 01 41 16 }
		byCS = LOBYTE(0x40 + m_nAddr);
		pData[0] = 0x10;
		pData[1] = 0x40;
		pData[2] = m_nAddr;
		pData[3] = byCS;
		pData[4] = 0x16;
		iMsgLenth   = FRM_LENGTH_FIX;
	}
	else if ( iTypeCode == S_CALL_DATA_FIRST )
	{//复位远方链路{10 40 01 41 16 }
		byCS = LOBYTE(0x7A + m_nAddr);
		pData[0] = 0x10;
		pData[1] = 0x7A;
		pData[2] = m_nAddr;
		pData[3] = byCS;
		pData[4] = 0x16;
		iMsgLenth   = FRM_LENGTH_FIX;
	}
	else if ( iTypeCode == S_CALL_SYS_TIME )
	{//复位远方链路{0x68,0x0F,0x0F,0x68,0x53,01,0x67,0x01,0x06,0x01,00,00,00,00,00,00,0x24,0x0B,0x05,nCS,0x16};
		byCS = LOBYTE(0x53+ m_nAddr +0x67+0x01+0x06+0x01+0x00+0x00+0x00+0x00+0x00+0x00+0x24+0x0B+0x05);
		BYTE buf[FRM_LENGTH_TIME] ={0x68,0x0F,0x0F,0x68,0x53,m_nAddr,0x67,0x01,0x06,0x01,00,00,00,00,00,00,0x24,0x0B,0x05,byCS,0x16};
		memcpy(pData,buf,FRM_LENGTH_TIME);
		iMsgLenth   = FRM_LENGTH_TIME;
	}
	else if ( iTypeCode == S_CALL_DATA_SECOND )
	{//二级用户数据{10 40 01 41 16 }
		byCS = LOBYTE(0x7B + m_nAddr);
		pData[0] = 0x10;
		pData[1] = 0x7B;
		pData[2] = m_nAddr;
		pData[3] = byCS;
		pData[4] = 0x16;
		iMsgLenth   = FRM_LENGTH_FIX;
	}
	else if ( iTypeCode == S_CALL_DATA_WHOLE )
	{// 总召  {68 09 09 68 88 01 64 01 0a 01 00 00 14 0d 16}
		byCS = LOBYTE(0x73+m_nAddr+0x64+0x01+0x06+0x01+0x00+0x00+0x14);
		BYTE buf[FRM_LENGTH_TIME] = {0x68,0x09,0x09,0x68,0x73,m_nAddr,0x64,0x01,0x06,0x01,0x00,0x00,0x14,byCS,0x16};
		memcpy(pData,buf,FRM_LENGTH_TIME);
		iMsgLenth   = FRM_LENGTH_TIME;
	}
	else
	{
		return FALSE;
	}

	// 发送数据
	int nsize = SendData(pData,iMsgLenth); 
	if (0 == nsize)
		return FALSE;

	return TRUE;
}

void CMySerialProtocol::GetAddress()
{
	CString sParam = m_config.m_szTcpUdpParam;
	sParam.Trim();
	int nFind = sParam.Find(';');
	//  获取设备地址
	CString szt;
	if ((nFind > 0) || (!sParam.IsEmpty()))
	{
		szt = sParam.Left(nFind);
		m_nAddr  = atoi(szt);
		if (m_nAddr < 0 || m_nAddr > 256)
		{
			m_nAddr = 1;
		}
	}
}

/**************************************************
*功    能：处理数字量测点 （遥信）
*参    数：long lTime 时标
*返 回 值：无
*创 建 者：ZCQ
*创建日期：2013.06.12
**************************************************/
bool CMySerialProtocol::HandleDigitPoint(BYTE *bufRev,int nSize,long lTime)
{

	BYTE *p  = bufRev;
	LPDATAHEADER pDataHeader = (LPDATAHEADER)p;

	//  SQ_TRUE = 1 地址连续
	if ( SQ_TRUE ==ParseByte(pDataHeader->btVSQ,7))
	{
		// 获取信息体的起始地址，即标签的ID
		BYTE btVal[2];
		ADDR *pADDR = (LPADDR)(p + sizeof(DATAHEADER));
		unsigned short usAddr= pADDR->shAddr;

		// 获取信息体的个数
		int iItemCount = pDataHeader->btVSQ & 0x7F;

		// 获取信息体数据区地址
		LPDIGIT_AVALUE pValues = (LPDIGIT_AVALUE)(p + sizeof(DATAHEADER)+ sizeof(ADDR)) ;
		CTagItem *pTag = NULL;
		CString strID;

		// 循环获取数据的值
		for ( int k = 0; k < iItemCount; k++)
		{
			// 通过ID获取标签
			strID.Format(_T("%d"), usAddr + k - DIGIT_START_INDEX);
			pTag = m_pDrvObj->FindByID(strID);

			// 判断标签是否存在
			if (pTag)
			{
				// 带品质描述词的单点信息(SIQ)，求其当前值NT（次高位）
				float fValue = (float)ParseByte(pValues[k].bySIQ,1);

				if (m_pDrvObj->m_config.m_bLocalTime)
				{ 		
					m_pDrvObj->AddValueByID(strID, ::time(NULL), QA_OK, (float)fValue);
				}
				else
				{
					m_pDrvObj->AddValueByID(strID,lTime, QA_OK, (float)fValue);
				}
			}
		}
	}
	else // SQ = 0 地址不连续 (没有报文暂不处理)
	{
		// 获取信息体的个数
		int iItemCount = pDataHeader->btVSQ & 0x7F;

		// 获取信息体数据区地址
		LPDIGIT_ADDR_AVALUE pValues = (LPDIGIT_ADDR_AVALUE)(p + sizeof(DATAHEADER)) ;
		CTagItem *pTag = NULL;
		CString strID;

		// 循环获取数据的值
		for ( int k = 0; k < iItemCount; k++)
		{
			// 通过ID获取标签
			strID.Format(_T("%d"),pValues[k].byID);
			pTag = m_pDrvObj->FindByID(strID);

			// 判断标签是否存在
			if (pTag)
			{
				// 带品质描述词的单点信息(SIQ)，求其当前值NT（次高位）
				float fValue = (float)ParseByte(pValues[k].bySIQ,1);

				if (m_pDrvObj->m_config.m_bLocalTime)
				{ 		
					m_pDrvObj->AddValueByID(strID, ::time(NULL), QA_OK, (float)fValue);
				}
				else
				{
					m_pDrvObj->AddValueByID(strID,lTime, QA_OK, (float)fValue);
				}
			}
		}
	}
	return true;
}

/**************************************************
*功    能：处理模拟量测点 （遥测）
*参    数：long lTime 时标
*返 回 值：无
*创 建 者：ZCQ
*创建日期：2013.06.12
**************************************************/
bool CMySerialProtocol::HandleAnologPoint(BYTE *bufRev,int nSize,long lTime)
{
	BYTE *p  = bufRev;
	LPDATAHEADER pDataHeader = (LPDATAHEADER)p;

	//  SQ_TRUE = 1 地址连续
	if ( SQ_TRUE == ParseByte(pDataHeader->btVSQ,7))
	{
		// 获取信息体的地址，即标签的ID
		BYTE btVal[2];
		ADDR *pADDR = (LPADDR)(p + sizeof(DATAHEADER));
		unsigned short usAddr= pADDR->shAddr;

		// 获取信息体的个数
		int iItemCount = pDataHeader->btVSQ & 0x7F;

		// 获取信息体数据区地址
		LPANALOG_AVALUE pValues = (LPANALOG_AVALUE)(p + sizeof(DATAHEADER)+ sizeof(ADDR)) ;
		CTagItem *pTag = NULL;
		CString  strID;

		// 循环获取数据的值
		for ( int k = 0; k < iItemCount; k++)
		{
			// 通过ID获取标签
			strID.Format(_T("%d"), usAddr + k - ANALOG_START_INDEX);
			pTag = m_pDrvObj->FindByID(strID);

			// 判断标签是否存在
			if (pTag)
			{
				// 计算浮点数的值
				BYTE byValue[2];
				byValue[0] = pValues[k].btLowAddr;
				byValue[1] = pValues[k].btHighAddr;
				short fValue= *((short*)byValue);

				if (m_pDrvObj->m_config.m_bLocalTime)
				{ 		
					m_pDrvObj->AddValueByID(strID, ::time(NULL), QA_OK, (float)fValue);
				}
				else
				{
					m_pDrvObj->AddValueByID(strID,lTime, QA_OK, (float)fValue);
				}
			}
		}
	}
	else // SQ = 0 地址不连续
	{
		// 获取信息体的个数
		int iItemCount = pDataHeader->btVSQ & 0x7F;

		// 获取信息体数据区地址
		LPANALOG_ADDR_AVALUE pValues = (LPANALOG_ADDR_AVALUE)(p + sizeof(DATAHEADER)) ;
		CTagItem *pTag = NULL;
		CString strID;

		// 循环获取数据的值
		for ( int k = 0; k < iItemCount; k++)
		{
			// 通过ID获取标签
			strID.Format(_T("%d"), m_objBuffer.SwitchBytes(pValues[k].shID,FALSE));

			pTag = m_pDrvObj->FindByID(strID);
			// 判断标签是否存在
			if (pTag)
			{
				// 计算浮点数的值
				BYTE byValue[2];
				byValue[0] = pValues[k].btLowAddr;
				byValue[1] = pValues[k].btHighAddr;
				short fValue= *((short*)byValue);

				if (m_pDrvObj->m_config.m_bLocalTime)
				{ 		
					m_pDrvObj->AddValueByID(strID,::time(NULL), QA_OK, (float)fValue);
				}
				else
				{
					m_pDrvObj->AddValueByID(strID,lTime, QA_OK, (float)fValue);
				}
			}
		}
	}
	return true;
}


/////////////////////////////////////////////////////////////////////*****************************************************************/
// CMyTCPClientProtocol 协议处理类实现
//////////////////////////////////////////////////////////////////////
CMyTCPClientProtocol::CMyTCPClientProtocol()
{
	m_nLinkStateMachine = S_INIT;
	m_lStartTime = 0;
	m_nAddr      = 1;
	m_pBuffer = new BYTE[TCP_BUFFER_SIZE];
}

CMyTCPClientProtocol::~CMyTCPClientProtocol()
{
	delete[] m_pBuffer;
}

DRVRET CMyTCPClientProtocol::WriteVar(LPDRREC lpval)
{
	return DRERR_NOTSUPPORT;
}

/**************************************************************
// 解析过程
****************************************************************/
void CMyTCPClientProtocol::DoRun()
{
	int nUpdateSec = m_pDrvObj->m_config.m_nUpdateSec/1000;
	long lEndTime = time(NULL);
	if (m_lStartTime != 0)
	{		
		if (lEndTime - m_lStartTime < nUpdateSec)
		{
			Sleep(10);
			return;
		}
	}

	if (!IsConnect())
	{
		if (!Connect())
			return;
	}

	m_nLinkStateMachine = S_RECIEVE_DATA;

	switch(m_nLinkStateMachine)
	{
	case S_INIT:                                    // 初始状态
	case S_LINK_REQ:                                // 请求远方链路{10 49 01 4A 16 }
		if (DoSend(S_LINK_REQ)){
			SetLinkStateMachine(S_LINK_OK);       
		}
		else
			SetLinkStateMachine(S_INIT);
		break;
	case S_LINK_OK:                                 // 确认远方链路{10 0B 01 0C 16 }
		if (DoRead(S_LINK_OK)){
			SetLinkStateMachine(S_RESET_REQ);
		}
		else
			SetLinkStateMachine(S_INIT); 
		break;
	case S_RESET_REQ:                                // 请求远方链路{10 49 01 4A 16 }
		if (DoSend(S_RESET_REQ)){
			SetLinkStateMachine(S_RESET_OK);       
		}
		else
			SetLinkStateMachine(S_INIT);
		break;
	case S_RESET_OK:                                 // 确认远方链路{10 0B 01 0C 16 }
		if (DoRead(S_RESET_OK)){
			SetLinkStateMachine(S_CALL_SYS_TIME);
		}
		else
			SetLinkStateMachine(S_INIT); 
		break;
	case S_CALL_SYS_TIME:                                 // 确认远方链路{10 0B 01 0C 16 }
		if (DoSend(S_CALL_SYS_TIME)){
			SetLinkStateMachine(S_SYS_TIME_OK);
		}
		else
			SetLinkStateMachine(S_INIT); 
		break;
	case S_SYS_TIME_OK:      
		if (!DoRead(S_SYS_TIME_OK))
		{
			m_lStartTime = ::time(NULL);
		}
		SetLinkStateMachine(S_CALL_DATA_SECOND);
		break;
	case S_CALL_DATA_SECOND:     // 确认远方链路{10 0B 01 0C 16 }
		if (DoSend(S_CALL_DATA_SECOND))
		{
			SetLinkStateMachine(S_RECIEVE_DATA);
		}
		else
			SetLinkStateMachine(S_INIT);
		break;
	case S_CALL_DATA_WHOLE:                         // 总召
		if (DoSend(S_CALL_DATA_WHOLE))
		{
			SetLinkStateMachine(S_RECIEVE_DATA); 
		}
		break;
	case S_RECIEVE_DATA:                            // 接收数据
		DoRead(S_RECIEVE_DATA);
		SetLinkStateMachine(S_INIT); 
		break;
	default:
		break;
	}
}


/**************************************************
*功    能：计算校验和
*参    数：BYTE *pData 数据
int iLen    左边起始地址（从1开始）
int iRight  右边起始地址
*返 回 值： 检查和
*创 建 者：ZCQ
*创建日期：2013.06.12
**************************************************/
int CMyTCPClientProtocol::CheckSum(BYTE *pData,int iLen,int iLeft,int iRight)
{
	if ( NULL == pData || iLeft < 1 || iRight < 1)
	{
		return -1;
	}

	if (iLen < max(iLeft,iRight) || iLen < 3)
	{
		return -1;
	}

	int iTemp = 0;
	for (int i = iLeft-1; i< iLen - iRight+1;i++)
	{
		iTemp += pData[i];
	}
	iTemp =  LOBYTE(iTemp);

	return iTemp;
}

/**************************************************
*功    能：检查字节的某为0还是1
*参    数：BYTE byData 数据
BYTE byPos  位置（首地址为0）
*返 回 值：0还是1 
*创 建 者：ZCQ
*创建日期：2013.06.12
**************************************************/
int CMyTCPClientProtocol::ParseByte(BYTE byData,BYTE byPos)
{
	float fRadix = 2,fIndex = byPos;
	BYTE byResult = (BYTE)pow(fRadix,fIndex);

	if (byData & byResult)
	{
		return 1;
	}
	return 0;
}

/**************************************************
*功    能：帧检查（包括包标识、校验和、结束符、长度等）
*参    数：BYTE *pBuf  数据
int iLen    长度
char cCheckType 检查类型
*返 回 值：TRUE : 成功；FALSE： 失败
*创 建 者：ZCQ
*创建日期：2013.06.12
**************************************************/
bool CMyTCPClientProtocol::FrmBaseCheck(BYTE *pBuf, int iLen,char cCheckType)
{
	if ( NULL == pBuf || iLen < FRM_LENGTH_FIX)
	{
		return false;
	}

	if ( S_LINK_OK == cCheckType || S_RESET_OK  == cCheckType )
	{
		if (pBuf[0] != 0x10 ||  pBuf[3]!=CheckSum(pBuf,iLen,2,3) ||pBuf[4] != 0x16)
		{
			return false;
		}
	}
	else if ( S_CALL_SYS_TIME == cCheckType)
	{
		if (iLen < FRM_LENGTH_TIME)
		{
			return FALSE;
		}

		if (pBuf[0] != 0x68 || pBuf[3] != 0x68 || pBuf[iLen-2] != CheckSum(pBuf,iLen,5,3) ||pBuf[iLen -1] != 0x16)
		{
			return false;
		}
	}
	else if ( S_CALL_DATA_WHOLE == cCheckType || S_CALL_DATA_SECOND == cCheckType ||  S_CALL_DATA_FIRST == cCheckType ||  S_RECIEVE_DATA == cCheckType ) 
	{
		int iAckCount = pBuf[1] +6;
		if (iAckCount < iLen)
		{
			return FALSE;
		}

		if (pBuf[0] != 0x68 || pBuf[3] != 0x68 || pBuf[iAckCount-2] != CheckSum(pBuf,iAckCount,5,3)||pBuf[iAckCount -1] != 0x16)
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}


BOOL CMyTCPClientProtocol::DoRead(int iTypeCode)
{
	int iSize = ReadData(m_pBuffer, TCP_BUFFER_SIZE);
	if (iSize <= 0)
	{
		Sleep(10);
		return FALSE;
	}

	if(!FrmBaseCheck(m_pBuffer,iSize,iTypeCode))
	{
		m_pDrvObj->AddError("接收数据帧校验错误！");
		return FALSE;
	}

	if ( S_LINK_OK == iTypeCode )
	{
		if (m_pBuffer[1] != 0x0b )
		{
			return FALSE;
		}
	}
	else if ( S_RESET_OK == iTypeCode)
	{
		if ( m_pBuffer[1] != 0x00 )
		{
			return FALSE;
		}
	}
	else if ( S_SYS_TIME_OK == iTypeCode)
	{
		//  PN是否正确(PN =1 不允许校时) ;不允许校时，采用本地时标
		if (1 ==ParseByte(m_pBuffer[8],6))
		{
			return FALSE;
		}

		// 获取从站时间，七个八位位组二进制时间(CP56Time2a)
		int iMilliSec = m_pBuffer[12] + (m_pBuffer[13] & 0x11);
		int nSecond = (m_pBuffer[13] >> 2);
		int nMin  = m_pBuffer[14];
		int nHour = m_pBuffer[15];
		int nDay = 0;
		BOOL bWeek = FALSE;
		if (bWeek)
		{
			nDay = (m_pBuffer[16] & 0x1F);
		}
		else
		{
			nDay = m_pBuffer[16];
		}
		int nMonth = m_pBuffer[17];
		int nYear  = m_pBuffer[18]+2000;
		COleDateTime sTime;
		sTime.SetDateTime(nYear, nMonth, nDay, nHour, nMin, nSecond);

		SYSTEMTIME systime;
		VariantTimeToSystemTime(sTime, &systime);
		CTime tm(systime);
		m_lStartTime  = tm.GetTime();
	}
	else if ( S_RECIEVE_DATA == iTypeCode)
	{
		DATAHEADER *pDATAHEADER = (LPDATAHEADER)m_pBuffer; 
		// 数据响应请求帧FUN = 8 并且从站回复肯定确认 PN = 0 
		if ( (pDATAHEADER->btCtrl & 0x0F) == 8 && 0 == ParseByte(pDATAHEADER->btCot,6))
		{
			//  遥信点处理流程( TI = 1 不带时标的单点信息)
			if (pDATAHEADER->btType == 1)
			{
				if (!HandleDigitPoint((BYTE*)m_pBuffer,iSize,m_lStartTime))
				{
					m_pDrvObj->AddError(_T("遥信点处理失败"));
					return FALSE;
				}
			}
			// 遥测点处理流程 (TI = 9测量值, 规一化值)
			else if (pDATAHEADER->btType == 9)
			{
				if (!HandleAnologPoint((BYTE*)m_pBuffer,iSize,m_lStartTime))
				{
					m_pDrvObj->AddError(_T("遥信点处理失败"));
					return  FALSE;
				}
			}
			// 视图刷新
			m_pDrvObj->FlushValue();
		}
	}
	return TRUE;
}

BOOL CMyTCPClientProtocol::DoSend(int iTypeCode)
{
	int iMsgLenth = 0;
	BYTE byCS     = 0;
	BYTE pData[FRM_LENGTH_MAX] = {0};

	if (iTypeCode == S_LINK_REQ )
	{//请求远方链路{10 49 01 4A 16 }
		byCS = LOBYTE(0x49 + m_nAddr);
		pData[0] = 0x10;
		pData[1] = 0x49;
		pData[2] = m_nAddr;
		pData[3] = byCS;
		pData[4] = 0x16;
		iMsgLenth   = FRM_LENGTH_FIX;
	}
	else if ( iTypeCode == S_RESET_REQ )
	{//复位远方链路{10 40 01 41 16 }
		byCS = LOBYTE(0x40 + m_nAddr);
		pData[0] = 0x10;
		pData[1] = 0x40;
		pData[2] = m_nAddr;
		pData[3] = byCS;
		pData[4] = 0x16;
		iMsgLenth   = FRM_LENGTH_FIX;
	}
	else if ( iTypeCode == S_CALL_DATA_FIRST )
	{//复位远方链路{10 40 01 41 16 }
		byCS = LOBYTE(0x7A + m_nAddr);
		pData[0] = 0x10;
		pData[1] = 0x7A;
		pData[2] = m_nAddr;
		pData[3] = byCS;
		pData[4] = 0x16;
		iMsgLenth   = FRM_LENGTH_FIX;
	}
	else if ( iTypeCode == S_CALL_SYS_TIME )
	{//复位远方链路{0x68,0x0F,0x0F,0x68,0x53,01,0x67,0x01,0x06,0x01,00,00,00,00,00,00,0x24,0x0B,0x05,nCS,0x16};
		byCS = LOBYTE(0x53+ m_nAddr +0x67+0x01+0x06+0x01+0x00+0x00+0x00+0x00+0x00+0x00+0x24+0x0B+0x05);
		BYTE buf[FRM_LENGTH_TIME] ={0x68,0x0F,0x0F,0x68,0x53,m_nAddr,0x67,0x01,0x06,0x01,00,00,00,00,00,00,0x24,0x0B,0x05,byCS,0x16};
        memcpy(pData,buf,FRM_LENGTH_TIME);
		iMsgLenth   = FRM_LENGTH_TIME;
	}
	else if ( iTypeCode == S_CALL_DATA_SECOND )
	{//二级用户数据{10 40 01 41 16 }
		byCS = LOBYTE(0x7B + m_nAddr);
		pData[0] = 0x10;
		pData[1] = 0x7B;
		pData[2] = m_nAddr;
		pData[3] = byCS;
		pData[4] = 0x16;
		iMsgLenth   = FRM_LENGTH_FIX;
	}
	else if ( iTypeCode == S_CALL_DATA_WHOLE )
	{// 总召  {68 09 09 68 88 01 64 01 0a 01 00 00 14 0d 16}
		byCS = LOBYTE(0x73+m_nAddr+0x64+0x01+0x06+0x01+0x00+0x00+0x14);
		BYTE buf[FRM_LENGTH_TIME] = {0x68,0x09,0x09,0x68,0x73,m_nAddr,0x64,0x01,0x06,0x01,0x00,0x00,0x14,byCS,0x16};
		memcpy(pData,buf,FRM_LENGTH_TIME);
		iMsgLenth   = FRM_LENGTH_TIME;
	}
	else
	{
		return FALSE;
	}

	// 发送数据
	int nsize = SendData(pData,iMsgLenth); 
	if (0 == nsize)
		return FALSE;

	return TRUE;
}

void CMyTCPClientProtocol::GetAddress()
{
	CString sParam = m_config.m_szTcpUdpParam;
	sParam.Trim();
	int nFind = sParam.Find(';');
	//  获取设备地址
	CString szt;
	if ((nFind > 0) || (!sParam.IsEmpty()))
	{
		szt = sParam.Left(nFind);
		m_nAddr  = atoi(szt);
		if (m_nAddr < 0 || m_nAddr > 256)
		{
			m_nAddr = 1;
		}
	}
}

/**************************************************
*功    能：处理数字量测点 （遥信）
*参    数：long lTime 时标
*返 回 值：无
*创 建 者：ZCQ
*创建日期：2013.06.12
**************************************************/
bool CMyTCPClientProtocol::HandleDigitPoint(BYTE *bufRev,int nSize,long lTime)
{

	BYTE *p  = bufRev;
	LPDATAHEADER pDataHeader = (LPDATAHEADER)p;

	//  SQ_TRUE = 1 地址连续
	if ( SQ_TRUE ==ParseByte(pDataHeader->btVSQ,7))
	{
		// 获取信息体的起始地址，即标签的ID
		BYTE btVal[2];
		ADDR *pADDR = (LPADDR)(p + sizeof(DATAHEADER));
		unsigned short usAddr= pADDR->shAddr;

		// 获取信息体的个数
		int iItemCount = pDataHeader->btVSQ & 0x7F;

		// 获取信息体数据区地址
		LPDIGIT_AVALUE pValues = (LPDIGIT_AVALUE)(p + sizeof(DATAHEADER)+ sizeof(ADDR)) ;
		CTagItem *pTag = NULL;
		CString strID;

		// 循环获取数据的值
		for ( int k = 0; k < iItemCount; k++)
		{
			// 通过ID获取标签
			strID.Format(_T("%d"), usAddr + k - DIGIT_START_INDEX);
			pTag = m_pDrvObj->FindByID(strID);

			// 判断标签是否存在
			if (pTag)
			{
				// 带品质描述词的单点信息(SIQ)，求其当前值NT（次高位）
				float fValue = (float)ParseByte(pValues[k].bySIQ,1);

				if (m_pDrvObj->m_config.m_bLocalTime)
				{ 		
					m_pDrvObj->AddValueByID(strID, 0, QA_OK, (float)fValue);
				}
				else
				{
					m_pDrvObj->AddValueByID(strID,lTime, QA_OK, (float)fValue);
				}
			}
		}
	}
	else // SQ = 0 地址不连续 (没有报文暂不处理)
	{
		// 获取信息体的个数
		int iItemCount = pDataHeader->btVSQ & 0x7F;

		// 获取信息体数据区地址
		LPDIGIT_ADDR_AVALUE pValues = (LPDIGIT_ADDR_AVALUE)(p + sizeof(DATAHEADER)) ;
		CTagItem *pTag = NULL;
		CString strID;

		// 循环获取数据的值
		for ( int k = 0; k < iItemCount; k++)
		{
			// 通过ID获取标签
			strID.Format(_T("%d"),pValues[k].byID);
			pTag = m_pDrvObj->FindByID(strID);

			// 判断标签是否存在
			if (pTag)
			{
				// 带品质描述词的单点信息(SIQ)，求其当前值NT（次高位）
				float fValue = (float)ParseByte(pValues[k].bySIQ,1);

				if (m_pDrvObj->m_config.m_bLocalTime)
				{ 		
					m_pDrvObj->AddValueByID(strID, 0, QA_OK, (float)fValue);
				}
				else
				{
					m_pDrvObj->AddValueByID(strID,lTime, QA_OK, (float)fValue);
				}
			}
		}
	}
	return true;
}

/**************************************************
*功    能：处理模拟量测点 （遥测）
*参    数：long lTime 时标
*返 回 值：无
*创 建 者：ZCQ
*创建日期：2013.06.12
**************************************************/
bool CMyTCPClientProtocol::HandleAnologPoint(BYTE *bufRev,int nSize,long lTime)
{
	BYTE *p  = bufRev;
	LPDATAHEADER pDataHeader = (LPDATAHEADER)p;

	//  SQ_TRUE = 1 地址连续
	if ( SQ_TRUE == ParseByte(pDataHeader->btVSQ,7))
	{
		// 获取信息体的地址，即标签的ID
		BYTE btVal[2];
		ADDR *pADDR = (LPADDR)(p + sizeof(DATAHEADER));
		unsigned short usAddr= pADDR->shAddr;

		// 获取信息体的个数
		int iItemCount = pDataHeader->btVSQ & 0x7F;

		// 获取信息体数据区地址
		LPANALOG_AVALUE pValues = (LPANALOG_AVALUE)(p + sizeof(DATAHEADER)+ sizeof(ADDR)) ;
		CTagItem *pTag = NULL;
		CString  strID;

		// 循环获取数据的值
		for ( int k = 0; k < iItemCount; k++)
		{
			// 通过ID获取标签
			strID.Format(_T("%d"), usAddr + k - ANALOG_START_INDEX);
			pTag = m_pDrvObj->FindByID(strID);

			// 判断标签是否存在
			if (pTag)
			{
				// 计算浮点数的值
				BYTE byValue[2];
				byValue[0] = pValues[k].btLowAddr;
				byValue[1] = pValues[k].btHighAddr;
				short fValue= *((short*)byValue);

				if (m_pDrvObj->m_config.m_bLocalTime)
				{ 		
					m_pDrvObj->AddValueByID(strID, ::time(NULL), QA_OK, (float)fValue);
				}
				else
				{
					m_pDrvObj->AddValueByID(strID,lTime, QA_OK, (float)fValue);
				}
			}
		}
	}
	else // SQ = 0 地址不连续
	{
		// 获取信息体的个数
		int iItemCount = pDataHeader->btVSQ & 0x7F;

		// 获取信息体数据区地址
		LPANALOG_ADDR_AVALUE pValues = (LPANALOG_ADDR_AVALUE)(p + sizeof(DATAHEADER)) ;
		CTagItem *pTag = NULL;
		CString strID;

		// 循环获取数据的值
		for ( int k = 0; k < iItemCount; k++)
		{
			strID.Format(_T("%d"), pValues[k].shID);
			// 通过ID获取标签
			pTag = m_pDrvObj->FindByID(strID);
			// 判断标签是否存在
			if (pTag)
			{
				// 计算浮点数的值
				BYTE byValue[2];
				byValue[0] = pValues[k].btLowAddr;
				byValue[1] = pValues[k].btHighAddr;
				short fValue= *((short*)byValue);

				if (m_pDrvObj->m_config.m_bLocalTime)
				{ 		
					m_pDrvObj->AddValueByID(strID,::time(NULL), QA_OK, (float)fValue);
				}
				else
				{
					m_pDrvObj->AddValueByID(strID,lTime, QA_OK, (float)fValue);
				}
				m_pDrvObj->AddDebugFmt(_T("遥测点 ： %s ,值: %d"), strID,fValue);
			}
		}
	}
	return true;
}

#ifdef DEBUG_2
int CMySerialProtocol::TestMsg(BYTE *byDataType)
{

	int iCount = 48;
	BYTE bufTest[] = {0x68,0x25,0x00,0x68,0x0a,0x12,0x10,0x14,0x00,0x00,0x0c,0x1c,0x0a,0xdd,0x07,0x00,0x01,0x01,0x01,0x00,0x00,0x01,0x01,0x02,0xa4,0xca,0x22,0x00,0x00,0x02,0x03,0xac,0x26,0x00,0x00,0x02,0x03,0x04,0x04,0x44,0x16,0x00,0x00,0x04,0xc3,0x64,0x16};
	memcpy(byDataType,bufTest,iCount);
	return iCount;
}
#endif