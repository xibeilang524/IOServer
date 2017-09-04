/*****************************************************************
* CopyRight By 升级科技
* All rights reserved.
* 文件名称: CDrvFactory.h F701
* 摘    要：按需求实现F701规约。
* 当前版本：1.0
* 作    者：
* 创建日期：2017.05.01
*****************************************************************/
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYDRVOBJ_H__9D0CA9F6_F8AE_48C3_97F0_7623BFA62115__INCLUDED_)
#define AFX_MYDRVOBJ_H__9D0CA9F6_F8AE_48C3_97F0_7623BFA62115__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\SDK\ddclib.h"
#include "MyFrmParse.h"
#include <vector>
using namespace std;

typedef struct _t_lastvalue
{
	int iLastCount;
	int iLastSetValue;
	float fLastRealValue;
	float fLastSuttleValue;
	float fLastTotalValue;
	int	lLastUpper;
	BYTE bClearFlag;
}T_LastValue;



#pragma pack()

class CMyDrvObj : public CDDCDrvObj  
{
public:
	CMyDrvObj();
	virtual ~CMyDrvObj();
public:
	virtual BOOL TestConnect(CConfigInfo &config);

protected:
	virtual CDDCDrvProtocol	*CreateProtocol();
	virtual DRVRET GetErrMsg(CString& szMsg,DRVRET dwcode);
public:
};

//////////////////////////CMySerialProtocol//////////////////////////////////////////////////////
class CMySerialProtocol : public CSerialProtocol
{

public:
	CMySerialProtocol();
	virtual ~CMySerialProtocol();

protected:
	virtual BOOL	Connect();
	virtual void	Disconnect();
	virtual void	DoRun();
	int		CheckSum(BYTE *pData,int iLen,int iLeft,int iRight);

	BOOL	ReadDevice(const string& szDevice, int iIndex);
	BOOL	WriteDeviceFailed(const string& szDevice,int iIndex);
	BOOL	StartDevice(const string& szDevice);
	void	StopDevice(const string& szDevice);
	//毛重
	int	GetTough(float& fValue);
	//净重
	int	GetSuttle(float& fValue);
	//皮重
	int   GetTare(float& fValue);
	//状态1
	BOOL   GetStatus1();
	//状态4
	int   GetStatus4();
	//设定值
	int   GetSetttingValue(const char* chNo, long& nValue);
	//清除计数
	int		ClearCount();
	int		GetUpper(long& iData);
	//累计次数
	int   GetCount(long& nValue);
	//累计重量
	int   GetTotal(float& fValue);

	//记录清0 标志位
	int  RecordClearFlag(string szDevice, int iIndex);
	long    m_lStartTime;//开始时间
	int     m_nAddr;     //起始地址
	BOOL	m_bNewConnected;
	long	m_lLastCount;
private:
	vector<long>		m_vecTypes;
	vector<string>	m_arrDevices;
	map<string,int>		m_mapTolerance;
	string	szDevice;
	CString m_szSendTag;
	string	m_szIndex;
	long	m_lTimeSection[3];
	long	m_lTimeSpan;
	vector<T_LastValue*> m_vecLastValue;
};

#endif // !defined(AFX_MYDRVOBJ_H__9D0CA9F6_F8AE_48C3_97F0_7623BFA62115__INCLUDED_)
