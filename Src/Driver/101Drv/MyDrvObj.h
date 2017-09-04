/*****************************************************************
* CopyRight By 西安热工研究院有限公司
* All rights reserved.
* 文件名称: CDrvFactory.h 101规约接口驱动实现文件
* 摘    要：按需求实现101规约。
* 当前版本：1.0
* 作    者：ZCQ
* 创建日期：2013.06.11
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

typedef enum emPROTOCOL  //SQ值
{
	PROTOCOL_TCP,
	PROTOCOL_COM     
}PROTOCOL;

#define TCP_BUFFER_SIZE		    (1024 * 2) // 2K

#define  RTU_REFRESH_CIRCLE     60     //多久召唤一次数据（秒）
#define  ANALOG_START_INDEX     1793   //遥测点开始索引
#define  DIGIT_START_INDEX      1793   //遥信点开始索引
#define  TAG_NAME_FORMAT        "TESTPOINT"   //遥测点开始索引

#define  FRM_LENGTH_MAX   255
#define  FRM_LENGTH_VAR   15
#define  FRM_LENGTH_TIME  21
#define  FRM_LENGTH_FIX   5
#define  FRM_LENGTH_E5    1

// 程序执行状态
enum STATE_OPER{
	S_INIT,			              // 初始化状态
	S_LINK_REQ,                   // 请求远方链路{10 49 01 4A 16 }
	S_LINK_OK,		              // 确认远方链路{10 0B 01 0C 16 }
	S_RESET_REQ,                  // 复位远方链路{10 40 01 41 16 }
	S_RESET_OK,	                  // 确认远方链路{10 00 01 01 16 }
	S_CALL_SYS_TIME,		      // 获取系统时间 {0x68,0x0F,0x0F,0x68,0x53,nDomain,0x67,0x01,0x06,0x01,00,00,00,00,00,00,0x24,0x0B,0x05,nCS,0x16};
	S_SYS_TIME_OK,                // 获取系统时间
	S_CALL_DATA_FIRST,		      // 召唤一类数据 {10 5A 01 5B 16}
	S_CALL_DATA_SECOND,		      // 召唤二类数据 {10 5B 01 5C 16}
	S_CALL_DATA_WHOLE,		      // 总召  {68 09 09 68 88 01 64 01 0a 01 00 00 14 0d 16}
	S_RECIEVE_DATA 	              // 数据接收
};

typedef enum emFrmType  //帧类型
{
	FRM_FIX,
	FRM_TIME,
	FRM_CONFIRM,
	FRM_VARI
}FrmType; 

typedef enum emSQ  //SQ值
{
	SQ_FALSE,
	SQ_TRUE     
}SQ;

#pragma pack(1)

typedef struct tagFixedFrm//固定帧长结构
{
	BYTE byLaunch;      //启动字符（10H)
	BYTE byControl;     //控制域（C)
	BYTE byLA;          //链路地址域（A）
	BYTE byCheckSum;    //帧校验和（CS）-- 求和取模256
	BYTE btProtect;     //结束字符（16H）
}FixedFrm, *LPFixedFrm;   // 五个字节


typedef struct tagDATAHEADER //报头结构
{
	BYTE btFlag;        //报文标识
	BYTE btFrameLen;    //帧长
	BYTE btFrameLen2;   //确认帧长
	BYTE btFlag2;       //确认报文标识
	BYTE btCtrl;        //控制域C
	BYTE btAddr;        //地址域
	BYTE btType;        //类型标识TI
	BYTE btVSQ;         //可变结构限定词
	BYTE btCot;         //传送原因
	BYTE btComAddr;       //公共地址
}DATAHEADER, *LPDATAHEADER;

typedef struct tagADDR   //信息体地址
{
	short shAddr;      //信息体低位地址     
}ADDR, *LPADDR;

typedef struct tagDIGIT_ADDR_AVALUE //开关量 地址不连续
{
	BYTE byID;     
	BYTE bySIQ;      
}DIGIT_ADDR_AVALUE, *LPDIGIT_ADDR_AVALUE;

typedef struct tagDIGIT_AVALUE //开关量 地址连续
{   
	BYTE bySIQ;      
}DIGIT_AVALUE, *LPDIGIT_AVALUE;

typedef struct tagANALOG_ADDR_AVALUE//测量值-短浮点数 地址不连续
{
	short shID; 
	BYTE btLowAddr;      //模拟值低位地址
	BYTE btHighAddr;     //模拟值高位地址
	BYTE btQDS;			 //品质描述词
}ANALOG_ADDR_AVALUE, *LPANALOG_ADDR_AVALUE;

typedef struct tagANALOG_AVALUE//测量值-短浮点数 地址连续
{
	BYTE btLowAddr;      //模拟值低位地址
	BYTE btHighAddr;     //模拟值高位地址
	BYTE btQDS;			 //品质描述词
}ANALOG_AVALUE, *LPANALOG_AVALUE;


typedef struct tagCONFIRMFRAME//确认帧结构
{
	BYTE btFlags[FIXEDPACKETLENGTH];
}CONFIRMFRAME,*LPCONFIRMFRAME;

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

	virtual DRVRET  WriteVar(LPDRREC lpval);
	virtual void	DoRun();
	BOOL DoRead(int iTypeCode);
	BOOL DoSend(int iTypeCode);
	void GetAddress();
	int  TestMsg(BYTE *byDataType); // 测试报文 从现场获取
	bool FrmBaseCheck(BYTE *pBuf, int iLen,char cCheckType);
	int  CheckSum(BYTE *pData,int iLen,int iLeft,int iRight);
	int  ParseByte(BYTE byData,BYTE byPos);
	bool HandleAnologPoint(BYTE *bufRev,int nSize,long lTime);
	bool HandleDigitPoint(BYTE *bufRev,int nSize,long lTime);

	void SetLinkStateMachine(int nState) 
	{
		m_nLinkStateMachine = nState;
		m_pDrvObj->AddInfoFmt(_T("状态机：%d"),nState);
	}

	long    m_lStartTime;//开始时间
	int     m_nAddr;     //起始地址
	int m_nLinkStateMachine; // 链路状态机

	BYTE		*m_pBuffer;
	CMyFrmParse  m_objBuffer;
private:

};


#define DEBUG_2
class CMyTCPClientProtocol : public CTCPClientProtocol
{
public:
	CMyTCPClientProtocol();
	virtual ~CMyTCPClientProtocol();

protected:

	virtual DRVRET  WriteVar(LPDRREC lpval);
	virtual void	DoRun();
	BOOL DoRead(int iTypeCode);
	BOOL DoSend(int iTypeCode);
	void GetAddress();
	int  TestMsg(BYTE *byDataType); // 测试报文 从现场获取
	bool FrmBaseCheck(BYTE *pBuf, int iLen,char cCheckType);
	int  CheckSum(BYTE *pData,int iLen,int iLeft,int iRight);
	int  ParseByte(BYTE byData,BYTE byPos);
	bool HandleAnologPoint(BYTE *bufRev,int nSize,long lTime);
	bool HandleDigitPoint(BYTE *bufRev,int nSize,long lTime);

	void SetLinkStateMachine(int nState) 
	{
		m_nLinkStateMachine = nState;
		m_pDrvObj->AddInfoFmt(_T("状态机：%d"),nState);
	}

	long    m_lStartTime;//开始时间
	int     m_nAddr;     //起始地址   

	int m_nLinkStateMachine; // 链路状态机
	BYTE		*m_pBuffer;
	CMyFrmParse  m_objBuffer;
private:

};

#endif // !defined(AFX_MYDRVOBJ_H__9D0CA9F6_F8AE_48C3_97F0_7623BFA62115__INCLUDED_)
