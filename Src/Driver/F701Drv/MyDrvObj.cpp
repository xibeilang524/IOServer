/*****************************************************************
* CopyRight By 西安升级科技
* All rights reserved.
* 文件名称: MyDrvObj.cpp  F701-c规约接口驱动实现文件
* 摘    要：F701-c规约接口驱动实现文件
* 当前版本：1.0
* 作    者：jyf
* 创建日期：2013.06.11
*****************************************************************/
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "F701Drv.h"
#include "MyDrvObj.h"
//#include "General\DebugUtils.h>

#include <math.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



typedef struct _weight_data
{
	int		iID;
	int		ltime;
	int		iQuality;
	float	fDefineValue;
	float	fTotalValue;
	float	fRealValue;
	int		iTotalNum;
	int		iUpper;
	char	chMechineName[50];
	char	chChenName[50];
	BYTE	btFlag;
}T_WeightData, *LPT_WeightData;

#define _DELEY_INTERVAL 300

/////////////////////////////////////////////////////////////////
//实现静态对象类工厂
CDrvObj* CDrvFactory::NewDrv()
{
	return new CMyDrvObj();
}

DRVRET CDrvFactory::GetDriverInfo(LPDRVINFO lpDrvInfo)
{
	memset(lpDrvInfo, 0, sizeof(T_DRVINFO));
	strcpy_s(lpDrvInfo->sname, "F701规约接口驱动");
	strcpy_s(lpDrvInfo->sdes, "CopyRight By 双人鱼科技");
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
	CConfigInfo::m_szHelpFile = _T("F701接口驱动.chm");
	CConfigInfo::m_szParamHintText = _T("设备地址;。例如：1;");

	CTagItem::SetCSVFormatName(_T("F701Drv"));
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
	pObj = new CMySerialProtocol();
	AddInfoFmt("创建COM串口连接方式！");
	
	if (pObj)
		InitTagIdMap();

	return pObj;
}

//////////////////////////////////////////////////////////////////////
// CMySerialProtocol 协议处理类实现
//////////////////////////////////////////////////////////////////////
CMySerialProtocol::CMySerialProtocol()
	:m_lLastCount(0)
	,m_szSendTag("")
	,m_lTimeSpan(0)
	,m_bNewConnected(TRUE)
{
	m_lStartTime = 0;
	m_nAddr      = 1;
	
	for (int i=0; i <3; i++)
	{
		m_lTimeSection[i] = 0;
	}
	m_vecLastValue.clear();

	if(m_pDrvObj)
		m_pDrvObj->m_config.m_szComParam = "0002|0001;C1.AI001;7:50:00|13:50:00|23:50:00;600";
}

CMySerialProtocol::~CMySerialProtocol()
{
	for (int i=0; i<m_vecLastValue.size(); i++)
	{
		T_LastValue *p = m_vecLastValue.at(i);
		delete p;
	}
	m_vecLastValue.clear();
}
int SplitString(const CString str, char split, CStringArray &strArray)
{
	strArray.RemoveAll();
	CString strTemp = str;
	int iIndex = 0;
	while (1)
	{
		iIndex = strTemp.Find(split);
		if(iIndex >= 0)
		{
			strArray.Add(strTemp.Left(iIndex));
			strTemp = strTemp.Right(strTemp.GetLength()-iIndex-1);
		}
		else
		{
			break;
		}
	}
	strArray.Add(strTemp);

	return strArray.GetSize();
}

BOOL CMySerialProtocol::Connect()
{
	CString strData = m_pDrvObj->m_config.m_szComParam;
	CStringArray strDatas;
	SplitString(strData, ';', strDatas);

	for (int i=0; i<m_vecLastValue.size(); i++)
	{
		T_LastValue *p = m_vecLastValue.at(i);
		delete p;
	}
	m_vecLastValue.clear();
	m_arrDevices.clear();
	if(strDatas.GetSize() >= 3)
	{
		CString szDevice = strDatas.GetAt(0);
		szDevice.Trim();
		CStringArray strDevices;
		SplitString(szDevice, '|', strDevices);
		string szDev;
		int nTolerance = 0;
		
		for (int i=0; i<strDevices.GetSize(); i++)
		{
			CString szTemp = strDevices.GetAt(i);
			szTemp.Trim();
			if(szTemp.IsEmpty())
				continue;
			int nPos = szTemp.Find(",");
			nTolerance = 2;
			szDev = szTemp;
			if (nPos > 0)
			{
				string szTolerance = szTemp.Right(szTemp.GetLength() - nPos -1);
				nTolerance = atoi(szTolerance.c_str());
				szDev = szTemp.Left(nPos);
				m_pDrvObj->AddDebugFmt("ID:%s, nTolerance:%d",szDev.c_str(), nTolerance);
			}
			m_arrDevices.push_back(szDev);
			m_mapTolerance.insert(::make_pair(szDev, nTolerance));
			T_LastValue *p = new T_LastValue;
			memset(p, 0, sizeof(T_LastValue));
			m_vecLastValue.push_back(p);
		}
		m_szSendTag = strDatas.GetAt(1);
		CString szSetTime = strDatas.GetAt(2);

		CStringArray strTimes;
		SplitString(szSetTime, '|', strTimes);
		if(strTimes.GetSize() < 3)
		{
			m_pDrvObj->AddInfo(_T("工作时间段数量设置错误，请重新设置"));
			return FALSE;
		}
		CString strTime;
		CTime tmNow = CTime::GetCurrentTime();
		CTime tmData(tmNow.GetYear(), tmNow.GetMonth(), tmNow.GetDay(), 0,0,0);
		for (int i=0; i<strTimes.GetSize() && i < 3; i++)
		{
			CString szTime = strTimes.GetAt(i);
			strTime.Format("%d-%d-%d %s", tmNow.GetYear(), tmNow.GetMonth(), tmNow.GetDay(), szTime);
			try
			{
				COleVariant vtime(strTime); 
				vtime.ChangeType(VT_DATE); 
				COleDateTime time4=vtime; 
				SYSTEMTIME systime; 
				VariantTimeToSystemTime(time4, &systime); 
				CTime tmSetTime(systime);
				
				m_lTimeSection[i] = systime.wHour * 3600 + systime.wMinute * 60;
				
			}
			catch (CException* e)
			{
				m_pDrvObj->AddInfo(_T("工作时间段设置错误，请重新设置"));
				return FALSE;
			}
		}

		CString strTimeSpan = strDatas.GetAt(3);
		string szTimeSpan = strTimeSpan;
		m_lTimeSpan = atol(szTimeSpan.c_str());

		if(strDatas.GetSize() > 4)
		{
			CString szTypes = strDatas.GetAt(4);

			CStringArray strTypes;
			SplitString(szTypes, '|', strTypes);
			m_vecTypes.clear();
			for (int i=0; i < strTypes.GetSize(); i++)
			{
				CString szType = strTypes.GetAt(i);
				long lType = atol((LPSTR)(LPCSTR)szType);
				m_vecTypes.push_back(lType);
			}
			if(m_vecTypes.size() < 2)
				m_pDrvObj->AddWarning(_T("请检查下采集类型设置参数"));
		}
	}
	else
	{
		m_pDrvObj->AddInfo(_T("设备参数和发送用标签为定义，请重新设置"));
		return FALSE;
	}

	return CSerialProtocol::Connect();
}


void CMySerialProtocol::Disconnect()
{
	for (int i=0; i<m_arrDevices.size(); i++)
	{
		string szDevice = m_arrDevices.at(i);
		m_pDrvObj->AddInfoFmt("关闭设备 %s", szDevice.c_str());
		StopDevice(szDevice);
	}
	for (int i=0; i<m_vecLastValue.size(); i++)
	{
		T_LastValue *p = m_vecLastValue.at(i);
		delete p;
	}
	m_vecLastValue.clear();
}
/**************************************************************
// 解析过程
****************************************************************/
void CMySerialProtocol::DoRun()
{
	int nUpdateSec = m_pDrvObj->m_config.m_nUpdateSec/1000;
	long lEndTime = (long)::time(NULL);
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
		{
			for (int i=0; i<m_arrDevices.size(); i++)
			{
				string szDevice = m_arrDevices.at(i);
				WriteDeviceFailed(szDevice, i);
			}
			return;
		}
	}


	for (int i=0; i<m_arrDevices.size(); i++)
	{
		string szDevice = m_arrDevices.at(i);
		StopDevice(szDevice);
		Sleep(100);
	}

	for (int i=0; i<m_arrDevices.size(); i++)
	{
		string szDevice = m_arrDevices.at(i);
		m_pDrvObj->AddInfoFmt("读取包装称 %s", szDevice.c_str());
		ReadDevice(szDevice, i);
		Sleep(_DELEY_INTERVAL);
		m_pDrvObj->FlushValue();
	}
	m_bNewConnected = FALSE;
}
BOOL CMySerialProtocol::ReadDevice(const string& szDevice, int iIndex)
{
	BOOL bStart = StartDevice(szDevice);

	if(!bStart)
	{
		WriteDeviceFailed(szDevice, iIndex);
		StopDevice(szDevice);
		m_pDrvObj->AddInfoFmt("包装称%s启动失败", szDevice.c_str());
		return FALSE;
	}
	//清空
	CTime tmNow = CTime::GetCurrentTime();

	//换班时间处理
	long lCount = 0;
	long lTime = tmNow.GetHour() * 3600 + tmNow.GetMinute() * 60 + tmNow.GetSecond();
	for (int i=0; i<3; i++)
	{
		long lSpan = lTime - m_lTimeSection[i];
		if(lSpan >0 && lSpan < m_lTimeSpan )
		{
			int nRet = GetCount(lCount);
			if(lCount != 0 && m_vecLastValue[iIndex]->bClearFlag != 1)
			{
				m_pDrvObj->AddDebugFmt("换班 袋数 %d ", lCount);
				ClearCount();
				Sleep(50);
				GetCount(lCount);
				if(lCount < m_vecLastValue[iIndex]->iLastCount)
				{
					m_vecLastValue[iIndex]->bClearFlag = 1;
					RecordClearFlag(szDevice, iIndex);
					m_pDrvObj->AddDebugFmt("换班 包装称 %s 清空计数器", szDevice.c_str());
				}

				StopDevice(szDevice);
				return TRUE;
			}
		}
	}
	m_vecLastValue[iIndex]->bClearFlag = 0;

	CString strID;
	CTagItem *pTag = NULL;

	long nType = 0;
	int nRetType = GetUpper(nType);
	if(nRetType == 0)
	{
		for (int i=0; i < m_vecTypes.size(); i++)
		{
			m_pDrvObj->AddInfoFmt("产品种类 newConnect<%d> m_vecTypes:<%d>, new:<%d> ",m_bNewConnected,  m_vecTypes.at(i),  nType);

			if(!m_bNewConnected 
				&& nType == m_vecTypes.at(i) 
				&& nType != m_vecLastValue[iIndex]->lLastUpper)
			{
				m_pDrvObj->AddInfoFmt("产品种类变化 old:<%d>, new:<%d> ",m_vecLastValue[iIndex]->lLastUpper,  nType);
				ClearCount();
				RecordClearFlag(szDevice, iIndex);
				StopDevice(szDevice);
				m_vecLastValue[iIndex]->lLastUpper = nType;
				return TRUE;
			}
			else if(m_bNewConnected)
			{
				m_vecLastValue[iIndex]->lLastUpper = nType;
				continue;
			}
		}
	}

	strID.Format(_T("ID_%s_TP"), szDevice.c_str());
	pTag = m_pDrvObj->FindByID(strID);
	if(!pTag)
		m_pDrvObj->AddDebugFmt("tag %s not exist", strID);
	else
		m_pDrvObj->AddValueByID(strID, ::time(NULL), 0, nType);

	int nRet = GetCount(lCount);
	if(lCount == 9999)
	{
		RecordClearFlag(szDevice, iIndex);
		ClearCount();
		m_pDrvObj->AddDebugFmt("包装称 %s 达到极限 清空计数器", szDevice.c_str());
		StopDevice(szDevice);
		return TRUE;
	}


	if(nRet != 0)
	{
		m_pDrvObj->AddDebugFmt("包装称 %s 获取数量错误，停止设备", szDevice.c_str());
		StopDevice(szDevice);
		return TRUE;
	}

	if(!m_bNewConnected && m_vecLastValue[iIndex]->iLastCount == lCount)
	{
		StopDevice(szDevice);
		return TRUE;
	}
	m_vecLastValue[iIndex]->iLastCount = lCount;

	strID.Format(_T("ID_%s_CNT"), szDevice.c_str());
	pTag = m_pDrvObj->FindByID(strID);
	if(!pTag)
		m_pDrvObj->AddDebugFmt("tag %s not exist", strID);
	else
	{	
		m_pDrvObj->AddDebugFmt("tag %s value<%d>", strID, lCount);
		m_pDrvObj->AddValueByID(strID, ::time(NULL), 0, lCount);
	}
	//读取毛重
	//float fToughValue = GetTough();
	//strID.Format(_T("ID_%s_MZ"), szDevice.c_str());
	//pTag = m_pDrvObj->FindByID(strID);
	//if(!pTag)
	//	m_pDrvObj->AddDebugFmt("tag %s not exist", strID);
	//else
	//	m_pDrvObj->AddValueByID(strID, ::time(NULL), 0, fToughValue);

	//读取净重
	float fSuttleValue = 0.0;
	//fSuttleValue = m_vecLastValue[iIndex]->fLastRealValue;;
	//nRet = GetSuttle(fSuttleValue);
	////float fSuttleValue = GetTotal();
	//if(nRet == 0)
	//{
	//	m_vecLastValue[iIndex]->fLastSuttleValue = fSuttleValue;

	//	strID.Format(_T("ID_%s_JZ"), szDevice.c_str());
	//	pTag = m_pDrvObj->FindByID(strID);
	//	if(!pTag)
	//		m_pDrvObj->AddDebugFmt("tag %s not exist", strID);
	//	else
	//		m_pDrvObj->AddValueByID(strID, ::time(NULL), 0, fSuttleValue);
	//}
	//else
	//{
	//	StopDevice(szDevice);
	//	return TRUE;
	//}
	float fTotalValue = m_vecLastValue[iIndex]->fLastTotalValue;
	nRet = GetTotal(fTotalValue);

	if(nRet == 0)
	{
		m_vecLastValue[iIndex]->fLastTotalValue = fTotalValue;

		strID.Format(_T("ID_%s_SJZL"), szDevice.c_str());
		pTag = m_pDrvObj->FindByID(strID);
		if(!pTag)
			m_pDrvObj->AddDebugFmt("tag %s not exist", strID);
		else
			m_pDrvObj->AddValueByID(strID, ::time(NULL), 0, fTotalValue);
	}
	else
	{
		StopDevice(szDevice);
		return TRUE;
	}

	////读取皮重
	//float fTareValue = GetTare();
	//strID.Format(_T("ID_%s_PZ"), szDevice.c_str());
	//pTag = m_pDrvObj->FindByID(strID);
	//if(!pTag)
	//	m_pDrvObj->AddDebugFmt("tag %s not exist", strID);
	//else
	//	m_pDrvObj->AddValueByID(strID, ::time(NULL), 0, fTareValue);

	long lSettingValue = m_vecLastValue[iIndex]->iLastSetValue;
	nRet = GetSetttingValue(m_szIndex.c_str(), lSettingValue);
	if(nRet == 0 )
	{
		strID.Format(_T("ID_%s_SD"), szDevice.c_str());
		pTag = m_pDrvObj->FindByID(strID);
		if(!pTag)
			m_pDrvObj->AddDebugFmt("tag %s not exist", strID);
		else
			m_pDrvObj->AddValueByID(strID, ::time(NULL), 0, lSettingValue);

		if(lSettingValue == 0)
		{
			StopDevice(szDevice);
			return TRUE;
		}
	}
	else
	{
		StopDevice(szDevice);
		return TRUE;
	}

	//在2公斤范围内波动，认为合理
	int nTolerance = 2;
	map<string,int>::iterator itor = m_mapTolerance.find(szDevice);
	if(itor != m_mapTolerance.end())
	{
		nTolerance = itor->second;
	}

	if(abs((m_vecLastValue[iIndex]->iLastSetValue - lSettingValue)) > nTolerance && !m_bNewConnected
		&& m_vecLastValue[iIndex]->iLastSetValue != 0)
	{
		RecordClearFlag(szDevice, iIndex);
		m_pDrvObj->AddDebugFmt("id:%s 设定值变化 上次:%d, 本次:%d, 误差:%d",
			szDevice.c_str(),
			m_vecLastValue[iIndex]->iLastSetValue,
			lSettingValue,
			nTolerance);
		m_vecLastValue[iIndex]->iLastSetValue = lSettingValue;
		ClearCount();

		StopDevice(szDevice);
		return TRUE;
	}

	if(nRet == 0)
	{
		m_vecLastValue[iIndex]->iLastSetValue = lSettingValue;
	}

	StopDevice(szDevice);

	string szTemp = m_szSendTag;

	if(m_szSendTag.IsEmpty())
	{
		m_pDrvObj->AddDebug("send tag is empty");
		return FALSE;
	}
	m_pDrvObj->AddDebugFmt("发送标签到sql  2 %s ", szTemp.c_str());

	T_WeightData tWeightData = {0};
	tWeightData.fRealValue = fSuttleValue;
	tWeightData.fDefineValue = lSettingValue;
	tWeightData.fTotalValue = fTotalValue;
	tWeightData.iUpper		= nType;
	tWeightData.iID = atoi(szDevice.c_str());
	tWeightData.iQuality = 0;
	tWeightData.iTotalNum = lCount;
	tWeightData.ltime = ::time(NULL);

	CString szLog;
	szLog.Format(_T("Device:%s, DS:%d,SJVal:%f,SDVal:%f,SJZL:%f,iUpper:%d"), 
		szDevice.c_str(), 
		tWeightData.iTotalNum,
		tWeightData.fRealValue,
		tWeightData.fDefineValue,
		tWeightData.fTotalValue, 
		tWeightData.iUpper);

	m_pDrvObj->AddDebug(szLog);

	m_pDrvObj->WriteStreamMessageCB(m_szSendTag, 0x101010, (BYTE *)(&tWeightData),sizeof(T_WeightData));

	return TRUE;
}

BOOL CMySerialProtocol::StartDevice(const string& szDevice)
{
	char chBuf[6] = {0};
	BYTE chReadBuf[9] = {0};
	
	chBuf[0] = 'S';
	memcpy_s(chBuf + 1, 4, szDevice.c_str(), 4);
	chBuf[5] = 0x0D;
	SendData((BYTE*)chBuf, 6);
	Sleep(_DELEY_INTERVAL);
	int nRet = ReadData(chReadBuf, 8);
	if(nRet >= 7)
	{
		char chID[5] = {0};
		memcpy_s(chID, 5, chReadBuf + 2, 4);
		if(strcmp(chID, szDevice.c_str()) == 0)
			return TRUE;
	}
	else if(nRet == 0)
	{
		SendData((BYTE*)chBuf, 6);
		Sleep(150);
		 nRet = ReadData(chReadBuf, 8);
		if(nRet >= 7)
		{
			char chID[5] = {0};
			memcpy_s(chID, 5, chReadBuf + 2, 4);
			if(strcmp(chID, szDevice.c_str()) == 0)
				return TRUE;
		}
	}
	
	return FALSE;
}

BOOL CMySerialProtocol::WriteDeviceFailed(const string& szDevice, int iIndex)
{
	T_WeightData tWeightData = {0};
	tWeightData.fRealValue = m_vecLastValue[iIndex]->fLastRealValue;
	tWeightData.fTotalValue = m_vecLastValue[iIndex]->fLastTotalValue;
	tWeightData.fDefineValue = m_vecLastValue[iIndex]->iLastSetValue;
	tWeightData.iID = atoi(szDevice.c_str());
	tWeightData.iQuality = -1;
	tWeightData.iTotalNum = m_vecLastValue[iIndex]->iLastCount;
	tWeightData.ltime = ::time(NULL);
	CString szLog;
	szLog.Format(_T("Device:%s, DS:%d,SJVal:%f,SDVal:%f,SJZL:%f"), 
		szDevice.c_str(), 
		tWeightData.iTotalNum,
		tWeightData.fRealValue,
		tWeightData.fDefineValue,
		tWeightData.fTotalValue);
	m_pDrvObj->AddDebug(szLog);

	m_pDrvObj->WriteStreamMessageCB(m_szSendTag, 0x101010, (BYTE *)(&tWeightData),sizeof(T_WeightData));
	return TRUE;
}
void CMySerialProtocol::StopDevice(const string& szDevice)
{
	char chBuf[6] = {0};
	BYTE chReadBuf[8] = {0};
	chBuf[0] = 'E';
	memcpy_s(chBuf + 1, 4, szDevice.c_str(), 4);
	chBuf[5] = 0x0D; // "cr"
	SendData((BYTE*)chBuf, 6);
	Sleep(_DELEY_INTERVAL);
	int nRet = ReadData(chReadBuf, 8);
	if(nRet >= 8)
	{
		char chID[5] = {0};
		memcpy_s(chID, 5, chReadBuf + 2, 4);
		if(strcmp(chID, szDevice.c_str()) == 0)
			return;
	}
	return ;
}

//毛重
int	CMySerialProtocol::GetTough(float& fValue)
{
	char chBuf[5] = {'R','A',0x0D};
	BYTE chReadBuf[11] = {0};
	SendData((BYTE*)chBuf, 3);
	Sleep(_DELEY_INTERVAL);
	int nRet = ReadData(chReadBuf, 11);
	if(nRet >= 8)
	{
		if(chReadBuf[0] == 'R' && chReadBuf[1] == 'A')
		{
			char chValue[7] = {0};
			memcpy_s(chValue, 7, chReadBuf + 2, 6);			
			fValue = atof(chValue);
			return 0;
		}
	}
	return -1;
}
//净重
int	CMySerialProtocol::GetSuttle(float& fValue)
{
	char chBuf[5] = {'R','B',0x0D};
	BYTE chReadBuf[11] = {0};
	SendData((BYTE*)chBuf, 3);
	Sleep(_DELEY_INTERVAL);
	int nRet = ReadData(chReadBuf, 11);
	if(nRet >= 8)
	{
		if(chReadBuf[0] == 'R' && chReadBuf[1] == 'B')
		{
			char chValue[7] = {0};
			memcpy_s(chValue, 7, chReadBuf + 2, 6);			
			fValue = atof(chValue);
			return 0;
		}
	}
	return -1;
}
//皮重
int   CMySerialProtocol::GetTare(float& fValue)
{
	char chBuf[5] = {'R','C',0x0D};
	BYTE chReadBuf[11] = {0};
	SendData((BYTE*)chBuf, 3);
	Sleep(_DELEY_INTERVAL);
	int nRet = ReadData(chReadBuf, 11);
	if(nRet >= 8)
	{
		if(chReadBuf[0] == 'R' && chReadBuf[1] == 'C')
		{
			char chValue[7] = {0};
			memcpy_s(chValue, 7, chReadBuf + 2, 6);			
			fValue = atof(chValue);
			return 0;
		}
	}
	return -1;
}

//状态1
BOOL  CMySerialProtocol::GetStatus1()
{
	char chBuf[5] = {'R','D',0x0D};
	BYTE chReadBuf[11] = {0};
	SendData((BYTE*)chBuf, 3);
	Sleep(_DELEY_INTERVAL);
	int nRet = ReadData(chReadBuf, 11);
	if(nRet >= 8)
	{
		if(chReadBuf[0] == 'R' && chReadBuf[1] == 'D')
		{
			if(chReadBuf[2] == '1' && chReadBuf[4] == '1')
				return TRUE;
		}
	}
	return FALSE;
}
//状态4
int  CMySerialProtocol::GetStatus4()
{
	char chBuf[5] = {'R','G',0x0D};
	BYTE chReadBuf[11] = {0};
	SendData((BYTE*)chBuf, 3);
	Sleep(_DELEY_INTERVAL);
	int nRet = ReadData(chReadBuf, 11);
	if(nRet != 11)
		return  -1;
	if(chReadBuf[0] != 'R' || chReadBuf[1] != 'G')
		return -1;
	return chReadBuf[4];
}

//累计次数
int  CMySerialProtocol::GetCount(long& iCount)
{
	char chBuf[] = {'R','H',0x0D};
	BYTE chReadBuf[11] = {0};
	char chCounts[6] = {0};
	SendData((BYTE*)chBuf, 3);
	Sleep(_DELEY_INTERVAL);
	int nRet = ReadData(chReadBuf, 11);
	if(nRet < 7)
		return  -1;
	if(chReadBuf[0] != 'R' || chReadBuf[1] != 'H')
		return -1;
	memcpy(chCounts, chReadBuf + 2, 4);
	iCount = atol(chCounts);

	return 0;
}

int CMySerialProtocol::GetTotal(float& fValue)
{
	char chBuf[] = {'R','I',0x0D};
	BYTE chReadBuf[15] = {0};
	char chCounts[11] = {0};
	SendData((BYTE*)chBuf, 3);
	Sleep(_DELEY_INTERVAL);
	int nRet = ReadData(chReadBuf, 15);
	if(nRet < 7)
		return  -1;
	if(chReadBuf[0] != 'R' || chReadBuf[1] != 'I')
		return -1;
	memcpy(chCounts, chReadBuf + 2, 10);
	fValue = atof(chCounts);
	return 0;
}


int CMySerialProtocol::GetUpper(long& iData)
{
	char chBuf[] = {'W','2','2',0x0D};
	BYTE chReadBuf[15] = {0};
	char chCounts[11] = {0};
	SendData((BYTE*)chBuf, 4);
	Sleep(_DELEY_INTERVAL);
	int nRet = ReadData(chReadBuf, 15);
	if(nRet < 7)
		return  -1;
	if(chReadBuf[0] != 'W' 
		|| chReadBuf[1] != '2' 
		|| chReadBuf[2] != '2')
		return -1;
	memcpy(chCounts, chReadBuf + 3, 5);
	iData = atol(chCounts);
	return 0;
}


int	CMySerialProtocol::ClearCount()
{
	BYTE chBuf[] = {'C','J',0x0D};
	SendData(chBuf, 3);
	long lCount = 0;
	int nRet =  GetCount(lCount);
	if(nRet == 0)
		return 1;
	if(nRet == -1)
		nRet =  GetCount(lCount);
	if(nRet != 0)
		return -1;
	return 1;
}


//设定值
int CMySerialProtocol::GetSetttingValue(const char* chNo, long& nValue)
{
	BYTE chBuf[] = {'W','1','2',0x0D};
	BYTE chReadBuf[11] = {0};
	char chCounts[6] = {0};
	SendData(chBuf, 4);
	Sleep(_DELEY_INTERVAL);
	int nRet = ReadData(chReadBuf, 11);
	if(nRet < 9)
		return  -1;
	if(chReadBuf[0] != 'W'
		|| '1' != chReadBuf[1] 
		||	'2' != chReadBuf[2] )
		return -1;
	memcpy(chCounts, chReadBuf + 3, 5);
	nValue = atol(chCounts);

	return 0;
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


int  CMySerialProtocol::RecordClearFlag(string szDevice, int iIndex)
{
	T_WeightData tWeightData = {0};
	tWeightData.fRealValue = m_vecLastValue[iIndex]->fLastRealValue;
	tWeightData.fTotalValue = m_vecLastValue[iIndex]->fLastTotalValue;
	tWeightData.fDefineValue = m_vecLastValue[iIndex]->iLastSetValue;
	tWeightData.iUpper		= m_vecLastValue[iIndex]->lLastUpper;
	tWeightData.iID = atoi(szDevice.c_str());
	tWeightData.iQuality = 0;
	tWeightData.iTotalNum = m_vecLastValue[iIndex]->iLastCount;
	tWeightData.ltime = ::time(NULL);
	tWeightData.btFlag = 1;
	CString szLog;
	szLog.Format(_T("Device:%s, DS:%d,SJVal:%f,SDVal:%f,SJZL:%f, flag=%d, iUp:%d"), 
		szDevice.c_str(), 
		tWeightData.iTotalNum,
		tWeightData.fRealValue,
		tWeightData.fDefineValue,
		tWeightData.fTotalValue,
		tWeightData.btFlag,
		tWeightData.iUpper);
	m_pDrvObj->AddDebug(szLog);

	m_pDrvObj->WriteStreamMessageCB(m_szSendTag, 0x101010, (BYTE *)(&tWeightData),sizeof(T_WeightData));

	return 0;
}