// MyDrvObj.h: interface for the CMyDrvObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYDRVOBJ_H__9D0CA9F6_F8AE_48C3_97F0_7623BFA62115__INCLUDED_)
#define AFX_MYDRVOBJ_H__9D0CA9F6_F8AE_48C3_97F0_7623BFA62115__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\..\SDK\ddclib.h"
#include "MyFrmParse.h"
#include "Tool.h"

#define  BREAK_PERIOD  300
// #define  HAVE_ELECTR    

#pragma pack(1)
typedef struct tagAPCI 
{
	BYTE byHead[6];
}APCI, *LPAPCI;

typedef struct tagASDUHEAD
{
	BYTE byType;      //类型
	BYTE byWord;      //限定词
	BYTE byReason[2]; //传送原因
	BYTE byAddr[2];   //公共地址
}ASDUHEAD, *LPASDUHEAD;

//单点信息 含品质描述 不带时标 地址不连续
typedef struct tagIDENTPT1_0  
{
	BYTE byAddr[3]; //信息对象地址
	BYTE byVal;     //单点信息
}IDENTPT1_0, *LPIDENTPT1_0;

//单点信息 含品质描述 不带时标 地址连续
typedef struct tagIDENTPT1_1  
{
	BYTE byVal; //单点信息
}IDENTPT1_1, *LPIDENTPT1_1;

//双点信息 含品质描述 不带时标 地址不连续
typedef struct tagIDENTPT3_0
{
	BYTE byAddr[3]; //信息对象地址
	BYTE byVal;	    //双点信息
}IDENTPT3_0,*LPIDENTPT3_0;

//双点信息 含品质描述 不带时标 地址连续
typedef struct tagIDENTPT3_1
{
	BYTE byVal;	//双点信息
}IDENTPT3_1,*LPIDENTPT3_1;

//规一化值 不带时标 地址不连续
typedef struct tagIDENTPT9_0 
{
	BYTE byAddr[3]; //信息对象地址
	BYTE byVal[2];  //规一化值
	BYTE byQulity;  //品质描述
}IDENTPT9_0,*LPIDENTPT9_0;

//规一化值 不带时标 地址连续
typedef struct tagIDENTPT9_1
{
	BYTE byVal[2];  //规一化值
	BYTE byQulity;  //品质描述
}IDENTPT9_1,*LPIDENTPT9_1;

//标度化值 不带时标 地址不连续
typedef struct tagIDENTPT11_0
{
	BYTE byAddr[3]; //信息对象地址
 	BYTE byVal[2];  //标度化值
	BYTE byQulity;  //品质描述
}IDENTPT11_0,*LPIDENTPT11_0;

//标度化值 不带时标 地址连续
typedef struct tagIDENTPT11_1
{
	BYTE byVal[2];  //标度化值
	BYTE byQulity;  //品质描述
}IDENTPT11_1,*LPIDENTPT11_1;

//短浮点数 不带时标 地址不连续
typedef struct tagIDENTPT13_0
{
	BYTE byAddr[3]; //信息对象地址
	BYTE byVal[4];  //短浮点数
	BYTE byQulity;  //品质描述
}IDENTPT13_0,*LPIDENTPT13_0;

//短浮点数 不带时标 地址连续
typedef struct tagIDENTPT13_1
{
	BYTE byVal[4]; //短浮点数
	BYTE byQulity; //品质描述
}IDENTPT13_1,*LPIDENTPT13_1;

//不带品质描述的规一化值 不带时标 地址不连续
typedef struct tagIDENTPT21_0
{
	BYTE byAddr[3]; //信息对象地址
	BYTE byVal[2];  //不带品质描述的规一化值
}IDENTPT21_0,*LPIDENTPT21_0;

//不带品质描述的规一化值 不带时标 地址连续
typedef struct tagIDENTPT21_1
{
	BYTE byVal[2]; //不带品质描述的规一化值
}IDENTPT21_1,*LPIDENTPT21_1;

//单点信息 含品质描述 带时标 地址不连续
typedef struct tagIDENTPT30_0
{
	BYTE byAddr[3]; //信息对象地址
	BYTE byVal;     //单点信息
	BYTE byTime[7]; //时标
}IDENTPT30_0, *LPIDENTPT30_0;

//单点信息 含品质描述 带时标 地址连续
typedef struct tagIDENTPT30_1 
{
	BYTE byVal;		//单点信息 
	BYTE byTime[7]; //时标
}IDENTPT30_1, *LPIDENTPT30_1;

//双点信息 含品质描述 带时标 地址不连续
typedef struct tagIDENTPT31_0
{
	BYTE byAddr[3]; //信息对象地址
	BYTE byVal;	    //双点信息
	BYTE byTime[7]; //时标
}IDENTPT31_0,*LPIDENTPT31_0;

//双点信息 含品质描述 带时标 地址连续
typedef struct tagIDENTPT31_1
{
	BYTE byVal;	    //双点信息
	BYTE byTime[7]; //时标
}IDENTPT31_1,*LPIDENTPT31_1;

//规一化值 含品质描述 带时标 地址不连续
typedef struct tagIDENTPT34_0
{
	BYTE byAddr[3]; //信息对象地址
	BYTE byVal[2];  //规一化值
	BYTE byQulity;  //品质描述
	BYTE byTime[7]; //时标
}IDENTPT34_0,*LPIDENTPT34_0;

//规一化值 含品质描述 带时标 地址连续
typedef struct tagIDENTPT34_1
{
	BYTE byVal[2];  //规一化值
	BYTE byQulity;  //品质描述
	BYTE byTime[7]; //时标
}IDENTPT34_1,*LPIDENTPT34_1;

//标度化值 含品质描述 带时标 地址不连续
typedef struct tagIDENTPT35_0
{
	BYTE byAddr[3]; //信息对象地址
	BYTE byVal[2];  //标度化值
	BYTE byQulity;  //品质描述
	BYTE byTime[7]; //时标
}IDENTPT35_0,*LPIDENTPT35_0;

//标度化值 含品质描述 带时标 地址连续
typedef struct tagIDENTPT35_1
{
	BYTE byVal[2];  //标度化值
	BYTE byQulity;  //品质描述
	BYTE byTime[7]; //时标
}IDENTPT35_1,*LPIDENTPT35_1;

//短浮点数 含品质描述 带时标 地址不连续
typedef struct tagIDENTPT36_0
{
	BYTE byAddr[3]; //信息对象地址
	BYTE byVal[4];  //短浮点数
	BYTE byQulity;  //品质描述
	BYTE byTime[7]; //时标
}IDENTPT36_0,*LPIDENTPT36_0;

//短浮点数 含品质描述 带时标 地址连续
typedef struct tagIDENTPT36_1
{
	BYTE byVal[4];  //短浮点数
	BYTE byQulity;  //品质描述
	BYTE byTime[7]; //时标
}IDENTPT36_1,*LPIDENTPT36_1;

//整形量 含品质描述 带时标 地址不连续
typedef struct tagIDENTPT37_0
{
	BYTE byAddr[3]; //信息对象地址
	int iValue;  //整形量
	BYTE byQulity;  //品质描述
	BYTE byTime[7]; //时标
}IDENTPT37_0,*LPIDENTPT37_0;

//短浮点数 含品质描述 带时标 地址连续
typedef struct tagIDENTPT37_1
{
	int  iValue;  //整形量
	BYTE byQulity;  //品质描述
	BYTE byTime[7]; //时标
}IDENTPT37_1,*LPIDENTPT37_1;


//电量 含品质描述 地址不连续
typedef struct tagIDENTPT15_0
{
	BYTE byAddr[3]; //信息对象地址
	BYTE byVal[4];  //整形量
	BYTE byQulity;  //品质描述
}IDENTPT15_0,*LPIDENTPT15_0;

//电量 含品质描述 地址连续
typedef struct tagIDENTPT15_1
{
	BYTE byVal[4];  //整形量
	BYTE byQulity;  //品质描述
}IDENTPT15_1,*LPIDENTPT15_1;
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
};

class CMyTCPClientProtocol : public CTCPClientProtocol
{
public:
	CMyTCPClientProtocol();
	virtual ~CMyTCPClientProtocol();
	
	virtual BOOL	Connect();
protected:
	BOOL AskPower(BYTE byCmd);
	virtual DRVRET  WriteVar(LPDRREC lpval);
	virtual void	DoRun();
	virtual void	Disconnect();
	void DoRead();
	void GetAddress();
	BOOL TestLink();		//测试链路
	BOOL AffirmTestLink();  //确认链路
	BOOL StartDT();		    //启动
	BOOL AffirmStartDT();   //确认启动
	BOOL Summon();			//总召唤
	BOOL AffirmSummon();    //确认总召唤
	BOOL AffirmRecv();		//确认收到数据
	BOOL CloseSummon();     //激活关闭

	// 备用服务器IP
	BOOL m_bUseIP2;
	CString m_szIP2;
	WORD	m_wPort2;
	long	m_lDiStartAddr;
	long	m_lAiStartAddr;
	long	m_lPowerStartAddr;

	WORD m_nADPUAddr;       //ADPU地址
	BOOL m_bEnd;
	long lStartTime;
	BOOL m_bLinkOK;		    //链路已通
	BOOL m_bSendTest;       //是否发送测试确认帧
	BOOL m_bSendCall;       //是否发送总召唤
	WORD m_wSendFrameNum;   //发送帧数
	WORD m_wReceiveFrameNum;//接收帧数
	BOOL m_bCanAskPowerAgain;//可以请求电量数据
	BOOL m_bCanAskPower;    //可以请求电量数据
	BOOL m_bCanRecvPower;   //可以获得电量

	CMyFrmParse m_FrmParseStream;

	CTimeSpanDif       m_tmAutoBreak;          // 自动断开周期（用于应对复杂网络环境
private:
	
};

#endif // !defined(AFX_MYDRVOBJ_H__9D0CA9F6_F8AE_48C3_97F0_7623BFA62115__INCLUDED_)
