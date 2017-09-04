// opcdrv.h : main header file for the OPCDRV DLL
//

#if !defined(AFX_OPCDRV_H__E64FC63B_CEEF_494D_BD17_40BAF9D1BBB3__INCLUDED_)
#define AFX_OPCDRV_H__E64FC63B_CEEF_494D_BD17_40BAF9D1BBB3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "ItemWinList.h"

/////////////////////////////////////////////////////////////////////////////
// COpcdrvApp
// See opcdrv.cpp for the implementation of this class
//

class COpcdrvApp : public CWinApp
{
public:
	COpcdrvApp();
protected:
		bool m_bComInitialized;
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpcdrvApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(COpcdrvApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#include <afxtempl.h>
#include <afxmt.h>

#include "../sdk/drvsdk.h"
#include "../sdk/sdktool.h"
#include "myopc.h"

class CItemDef 
{
public:
	CItemDef();
	~CItemDef();
	static int ToOleDataType(int ndrvtype)
	{
		int nolet=VT_I4;
		switch(ndrvtype)
		{
		case  DRT_DIGITAL:
		case  DRT_INT32:
			nolet=VT_I4;
			break;
		
		case  DRT_FLOAT32:
			nolet=VT_R4;
			break;
		case  DRT_INT64:
			nolet=VT_I8;
			break;
		case  DRT_FLOAT64:
			nolet=VT_R8;
			break;
		default:
			break;
		}
		return nolet;
	};
	static CString GetTypeStr(int ntype)
	{
		if(ntype == DRT_DIGITAL)
			return CString("digital");
		else if(ntype == DRT_INT32)
			return CString("int32");
		else if(ntype == DRT_FLOAT32)
			return CString("float32");
		else if(ntype == DRT_INT64)
			return CString("int64");
		else if(ntype == DRT_FLOAT64)
			return CString("float64");
		else if(ntype == DRT_STRING)
			return CString("string");
		else if(ntype == DRT_BLOB)
			return CString("blob");
		return CString("");
	};
	static int	GetTypeVal(LPCTSTR lpszType)
	{
		if(!_stricmp("digital",lpszType))
			return DRT_DIGITAL;
		else if(!_stricmp("int32",lpszType))
			return DRT_INT32;
		else if(!_stricmp("int64",lpszType))
			return DRT_INT64;
		else if(!_stricmp("float32",lpszType))
			return DRT_FLOAT32;
		else if(!_stricmp("float64",lpszType))
			return DRT_FLOAT64;
		else if(!_stricmp("string",lpszType))
			return DRT_STRING;
		else if(!_stricmp("blob",lpszType))
			return DRT_BLOB;
		return DRT_DIGITAL;
	};
	virtual void	Serialize(CArchive& ar, WORD dwVersion);

	CString		m_szName;	//数据项名称 OPC fully qualified item ID
	CString		m_szDes;	//描述
	CString		m_szUnit;
	int			m_nType;	//数据类型
	BOOL		m_bZeroFilter;
	int			m_nAccess;	//访问模式:0只读
	CString     m_szres;	//保留
	CKItem		m_item;

};


#define MULTIMAXTAGS 2000
class COpcDrv// : public CObject //具体的驱动实例对象
{

public:
	DRHANDLE		m_handle;
	DRCB_OnDataOk	m_cbOnDataOk;	//数据回调函数
	DRCB_OnLogOk	m_cbOnLogOk;		//日志回调函数
	DRCB_OnSoeOk	m_cbOnSoeOk;	//事件回调函数


	BOOL		m_bconfigModif;//配置修改
	BOOL		m_bKillThread;				// 删除线程标志
	BOOL		m_bRun;						// 运行标志
	int			m_nStatus;// 状态

	CKServer	m_OpcServer;	//	OPC Server
	CKGroup		m_OpcGroup;		//	OPC 组,在m_OpcServer连接的情况下添加组到m_OpcServer
	
	//{{ OPC Group
	CString		m_strName;		// group name					组名
	DWORD		m_dwUpdateRate;	// update rate in milliseconds	数据更新周期 MS
	LCID		m_dwLanguageID;	// language ID					语言
	BOOL		m_bActive;			// active state					活动状态
	long		m_lBias;			// time bias in minutes
	float		m_fDeadband;		// percent deadband				死区
	DWORD		m_dwUpdateMethod;	// update method used by this group (see globals.h)
	UINT		m_nUpdateTimeout;	// 数据更新超时
	long		m_lLastUpdateTime;
	//}}
	
	//{{ OPC Server
	CString		m_strProgID;							//程序ID		
	CString		m_strRemoteMachine;						//计算机名或IP
	//}}

	//{{附加的参数，保存时跟在m_strRemoteMachine后面用\t分开
	int			m_nReadMode;//读取方式
	//0 :		同步CATCH
	//1:		同步DEVICE
	//2:		异步1.0
	//3:		异步2.0
	//}}
	int			m_nOpcTime; //OPC时标,0:opc 1:本地
	int			m_nTimeArea;	//时区 -12,12
	//void	PraseArgs(CString &szargs);//分析参数

	void RefreshLastUpdateTime() { m_lLastUpdateTime = (long)::time(NULL); }
	BOOL CheckLastUpdateTime();
public:	
	COpcDrv();
	~COpcDrv();

	void AddDebug(LPCTSTR lpszLogMessage);
	void AddDebug(DWORD dwLogOptType, LPCTSTR lpszLogMessage);
	void AddDebugFmt(LPCTSTR lpszLogFormat, ...);
	void AddDebugFmt(DWORD dwLogOptType, LPCTSTR lpszLogFormat, ...);

	void AddInfo(LPCTSTR lpszLogMessage);
	void AddInfo(DWORD dwLogOptType, LPCTSTR lpszLogMessage);
	void AddInfoFmt(LPCTSTR lpszLogFormat, ...);
	void AddInfoFmt(DWORD dwLogOptType, LPCTSTR lpszLogFormat, ...);

	void AddWarning(LPCTSTR lpszLogMessage);
	void AddWarning(DWORD dwLogOptType, LPCTSTR lpszLogMessage);
	void AddWarningFmt(LPCTSTR lpszLogFormat, ...);
	void AddWarningFmt(DWORD dwLogOptType, LPCTSTR lpszLogFormat, ...);

	void AddError(LPCTSTR lpszLogMessage);
	void AddError(DWORD dwLogOptType, LPCTSTR lpszLogMessage);
	void AddErrorFmt(LPCTSTR lpszLogFormat, ...);
	void AddErrorFmt(DWORD dwLogOptType, LPCTSTR lpszLogFormat, ...);

	void	Create(void *pconfigdata,int nconfiglen,DRCB_OnDataOk fundata,DRCB_OnLogOk funlog,DRCB_OnSoeOk	cbOnSoeOk);
	void	ClearConfig();					// 清除数据项(配置信息)

	static	UINT RunThread(void *pParam);// 运行线程

	void		StartRun();
	void		StopRun();

	void		SaveConfig(CArchive& ar);
	void		LoadConfig(const void *pblk,int nsize);

	BOOL		Config();//配置
	CItemDef*	FindItem(LPCTSTR lpszAddr);// 查找数据项
	
	BOOL	DelItem(LPCTSTR lpszName,CString &szErr);
	BOOL	ModifyItem(LPCTSTR lpszOldName,LPCTSTR lpszNewName,int nType,int nAccess,CString &szErr);
	BOOL	AddItem(LPCTSTR lpszName,int nType,int nAccess,CString &szErr);
	void	ReMoveAll();
	void	ItemsToListCtrl(CTagItemWinList *pList);
	void	ListCtrlToItems(CTagItemWinList *pList);
	int		GetItemCount();
	BOOL	GetItem(LPCTSTR lpszName,int &nType,int &nAccess,CString &szErr);
	
	DRVRET  GetTagDef(int nindex,LPDRVTAG  lptag);
	int		GetConfigSize();		//取配置数据长度
	DRVRET	GetConfigData(void *pbuf,int nbufsize,int *pnactsize); //取配置数据


	DRVRET WriteVar(LPDRREC lprec);//写变量
	
	BOOL  ReadSync (bool bDeviceRead);//同步读

	BOOL	Disconnect();

	BOOL	Reconnect();//重新连接
	
	void	AddDrvRec(CRdRec* prec); //添加待回调的数据记录,复制方式
	void	DoMultiVal();//不到4000的要发送
protected:
	
private:
	CCriticalSection m_Lock;

	CArray <CItemDef*,CItemDef*> m_ItemArray;

	CRdRec	m_recs[MULTIMAXTAGS];
	int		m_nMultiValSize; //多变量个数
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPCDRV_H__E64FC63B_CEEF_494D_BD17_40BAF9D1BBB3__INCLUDED_)
