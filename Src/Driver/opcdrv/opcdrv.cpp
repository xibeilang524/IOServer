// opcdrv.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "opcdrv.h"
#include "myopc.h"

#include "..\sdk\DrvVersion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 同步读标签最大数量
#define READ_SYNC_TAG_MAX		1000

#define DRVCFG_VERSION		(DRVCFG_VERSION_USER + 2)	// 第一版
//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

CString g_LoadString(UINT UID)
{
	CString szt="";
	szt.LoadString(UID);
	return szt;
}

/////////////////////////////////////////////////////////////////////////////
// COpcdrvApp

BEGIN_MESSAGE_MAP(COpcdrvApp, CWinApp)
	//{{AFX_MSG_MAP(COpcdrvApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpcdrvApp construction

COpcdrvApp::COpcdrvApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only COpcdrvApp object

COpcdrvApp theApp;

BOOL COpcdrvApp::InitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class

	// Initialize COM:
	//HRESULT hr = CoInitializeEx (NULL, COINIT_MULTITHREADED);
	//m_bComInitialized = SUCCEEDED (hr);
	
	//// If we failed to initialize COM, there isn't much we can do so 
	//// return FALSE.  This will prevent application from starting.
	//if (!m_bComInitialized)
	//	return (FALSE);

	// this is for DCOM:
	// without this, callbacks from the server may get blocked, depending on
	// DCOM configuration settings

	// 如果不初始此安全令牌，在某些情况下可能会出现各种奇怪问题

	HRESULT hr =::CoInitializeSecurity(
		NULL,                    // points to security descriptor
		-1,                     // count of entries in asAuthSvc
		NULL,                    // array of names to register
		NULL,                    // reserved for future use
		RPC_C_AUTHN_LEVEL_NONE, // the default authentication level for proxies
		RPC_C_IMP_LEVEL_IMPERSONATE,// the default impersonation level for proxies
		NULL,                    // used only on Windows 2000
		EOAC_NONE,              // additional client or server-side capabilities
		NULL                     // reserved for future use
		);

	return CWinApp::InitInstance();
}

int COpcdrvApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_bComInitialized)
		CoUninitialize ();

	return CWinApp::ExitInstance();
}

//////////////////////////////////////////////////////////////////////////////

CItemDef::CItemDef()
{
	m_szName="";
	m_szDes = "";
	m_szUnit = "";
	m_nType = DRT_FLOAT32;
	m_nAccess = 0;
	m_bZeroFilter = FALSE;
	m_szres = "";
//	memset(&m_val, 0, sizeof(m_val));	// 清零
}

CItemDef::~CItemDef()
{

}

void CItemDef::Serialize(CArchive& ar, WORD dwVersion)
{
	if(ar.IsStoring())
	{
		ar << m_nType;
		ar << m_nAccess;
		ar << m_szName;
		ar << m_szDes;
		ar << m_szUnit;
		ar << m_szres;
		ar << m_bZeroFilter;
	}
	else
	{
		ar >> m_nType;
		ar >> m_nAccess;
		ar >> m_szName;
		ar >> m_szDes;
		ar >> m_szUnit;
		ar >> m_szres;

		if (dwVersion > 1)
			ar >> m_bZeroFilter;
	}
}


//IMPLEMENT_SERIAL(COpcDrv, CObject, 0)//版本0
COpcDrv::COpcDrv()
{
	m_nMultiValSize = 0; //多变量个数
	m_ItemArray.SetSize(0,2048);
	m_OpcServer.m_pdrv = this;

	m_cbOnDataOk = NULL;
	m_cbOnLogOk = NULL;
	m_cbOnSoeOk = NULL;

	m_bKillThread = FALSE;				// 删除线程标志
	m_bRun = FALSE;						// 运行标志
	m_nStatus = DRSTATUS_STOP;		// 状态
	m_handle=NULL;

	//{{ OPC Server
	m_strProgID="";						//程序ID		
	m_strRemoteMachine="";				//计算机名或IP

	//}}
	//{{ OPC Group
	m_strName="decdefgroup";// group name					组名
	m_dwUpdateRate=1000;	// update rate in milliseconds	数据更新周期 MS
	m_dwLanguageID=MAKELANGID (LANG_ENGLISH, SUBLANG_ENGLISH_US);;	// language ID,1033 英语,2052简体中文	语言
	m_bActive=TRUE;			// active state					活动状态
	m_lBias=0;				// time bias in minutes
	m_fDeadband=0;			// percent deadband				死区
	m_dwUpdateMethod=OPC_20_DATACHANGE;	// update method used by this group (see globals.h)
	m_nUpdateTimeout = 0;	// 表示不使用数据更新超时
	m_lLastUpdateTime = 0;
	//}}
	m_OpcGroup.SetServer(&m_OpcServer);
	m_OpcGroup.m_pDrv = this;//传递指针
	m_nReadMode=3;//异步2.0
	m_nOpcTime = 0; //OPC时标
	m_nTimeArea = 8;//时区
}

COpcDrv::~COpcDrv()
{
	int i,n=m_ItemArray.GetSize();
	for(i=0;i<n;i++)
		delete m_ItemArray.GetAt(i);
	m_ItemArray.RemoveAll();
	if(m_handle)
		delete m_handle;
}

void COpcDrv::AddDebug(LPCTSTR lpszLogMessage)
{
	if(!m_cbOnLogOk)
		return;

	m_cbOnLogOk(m_handle, lpszLogMessage, LOG_LEVEL_DEBUG, LOG_OPT_ALL);
}

void COpcDrv::AddDebugFmt(LPCTSTR lpszLogFormat, ...)
{
	if(!m_cbOnLogOk)
		return;

	TCHAR buf[1024];

	va_list args;
	va_start(args, lpszLogFormat);
	_vstprintf(buf, lpszLogFormat, args);
	va_end(args);

	m_cbOnLogOk(m_handle, buf, LOG_LEVEL_DEBUG, LOG_OPT_ALL);
}

void COpcDrv::AddDebug(DWORD dwLogOptType, LPCTSTR lpszLogMessage)
{
	if(!m_cbOnLogOk)
		return;

	m_cbOnLogOk(m_handle, lpszLogMessage, LOG_LEVEL_DEBUG, dwLogOptType);
}

void COpcDrv::AddDebugFmt(DWORD dwLogOptType, LPCTSTR lpszLogFormat, ...)
{
	if(!m_cbOnLogOk)
		return;

	TCHAR buf[1024];

	va_list args;
	va_start(args, lpszLogFormat);
	_vstprintf(buf, lpszLogFormat, args);
	va_end(args);

	m_cbOnLogOk(m_handle, buf, LOG_LEVEL_DEBUG, dwLogOptType);
}

void COpcDrv::AddInfo(LPCTSTR lpszLogMessage)
{
	if(!m_cbOnLogOk)
		return;

	m_cbOnLogOk(m_handle, lpszLogMessage, LOG_LEVEL_INFO, LOG_OPT_ALL);
}

void COpcDrv::AddInfoFmt(LPCTSTR lpszLogFormat, ...)
{
	if(!m_cbOnLogOk)
		return;

	TCHAR buf[1024];

	va_list args;
	va_start(args, lpszLogFormat);
	_vstprintf(buf, lpszLogFormat, args);
	va_end(args);

	m_cbOnLogOk(m_handle, buf, LOG_LEVEL_INFO, LOG_OPT_ALL);
}

void COpcDrv::AddInfo(DWORD dwLogOptType, LPCTSTR lpszLogMessage)
{
	if(!m_cbOnLogOk)
		return;

	m_cbOnLogOk(m_handle, lpszLogMessage, LOG_LEVEL_INFO, dwLogOptType);
}

void COpcDrv::AddInfoFmt(DWORD dwLogOptType, LPCTSTR lpszLogFormat, ...)
{
	if(!m_cbOnLogOk)
		return;

	TCHAR buf[1024];

	va_list args;
	va_start(args, lpszLogFormat);
	_vstprintf(buf, lpszLogFormat, args);
	va_end(args);

	m_cbOnLogOk(m_handle, buf, LOG_LEVEL_INFO, dwLogOptType);
}

void COpcDrv::AddWarning(LPCTSTR lpszLogMessage)
{
	if(!m_cbOnLogOk)
		return;

	m_cbOnLogOk(m_handle, lpszLogMessage, LOG_LEVEL_WARNING, LOG_OPT_ALL);
}

void COpcDrv::AddWarningFmt(LPCTSTR lpszLogFormat, ...)
{
	if(!m_cbOnLogOk)
		return;

	TCHAR buf[1024];

	va_list args;
	va_start(args, lpszLogFormat);
	_vstprintf(buf, lpszLogFormat, args);
	va_end(args);

	m_cbOnLogOk(m_handle, buf, LOG_LEVEL_INFO, LOG_OPT_ALL);
}

void COpcDrv::AddWarning(DWORD dwLogOptType, LPCTSTR lpszLogMessage)
{
	if(!m_cbOnLogOk)
		return;

	m_cbOnLogOk(m_handle, lpszLogMessage, LOG_LEVEL_WARNING, dwLogOptType);
}

void COpcDrv::AddWarningFmt(DWORD dwLogOptType, LPCTSTR lpszLogFormat, ...)
{
	if(!m_cbOnLogOk)
		return;

	TCHAR buf[1024];

	va_list args;
	va_start(args, lpszLogFormat);
	_vstprintf(buf, lpszLogFormat, args);
	va_end(args);

	m_cbOnLogOk(m_handle, buf, LOG_LEVEL_WARNING, dwLogOptType);
}

void COpcDrv::AddError(LPCTSTR lpszLogMessage)
{
	if(!m_cbOnLogOk)
		return;

	m_cbOnLogOk(m_handle, lpszLogMessage, LOG_LEVEL_ERROR, LOG_OPT_ALL);
}

void COpcDrv::AddErrorFmt(LPCTSTR lpszLogFormat, ...)
{
	if(!m_cbOnLogOk)
		return;

	TCHAR buf[1024];

	va_list args;
	va_start(args, lpszLogFormat);
	_vstprintf(buf, lpszLogFormat, args);
	va_end(args);

	m_cbOnLogOk(m_handle, buf, LOG_LEVEL_ERROR, LOG_OPT_ALL);
}

void COpcDrv::AddError(DWORD dwLogOptType, LPCTSTR lpszLogMessage)
{
	if(!m_cbOnLogOk)
		return;

	m_cbOnLogOk(m_handle, lpszLogMessage, LOG_LEVEL_ERROR, dwLogOptType);
}

void COpcDrv::AddErrorFmt(DWORD dwLogOptType, LPCTSTR lpszLogFormat, ...)
{
	if(!m_cbOnLogOk)
		return;

	TCHAR buf[1024];

	va_list args;
	va_start(args, lpszLogFormat);
	_vstprintf(buf, lpszLogFormat, args);
	va_end(args);

	m_cbOnLogOk(m_handle, buf, LOG_LEVEL_ERROR, dwLogOptType);
}

void	COpcDrv::Create(void *pconfigdata,int nconfiglen,DRCB_OnDataOk fundata,DRCB_OnLogOk funlog,DRCB_OnSoeOk	cbOnSoeOk)
{
	if((pconfigdata!=NULL)&&(nconfiglen > 0))
		LoadConfig(pconfigdata,nconfiglen);
	m_cbOnDataOk = fundata;
	m_cbOnLogOk = funlog;
	m_cbOnSoeOk = cbOnSoeOk;
}

DRVRET  COpcDrv::GetTagDef(int nindex,LPDRVTAG  lptag)
{
	CSafeLock lock(&m_Lock);//锁
	int n = m_ItemArray.GetSize();
	CItemDef *pi;
	if((nindex < 0)|| (nindex >= n))
		return DRERR_NOMORETAG;

	pi=m_ItemArray.GetAt(nindex);
	strncpy(lptag->sname,pi->m_szName,63);
	strncpy(lptag->sdes,pi->m_szDes,79);
	strncpy(lptag->sunit,pi->m_szUnit,15);
	lptag->waccess = (char)pi->m_nAccess;
	lptag->wtype = (char)pi->m_nType;

	lptag->cr[0]=0;
	lptag->cr[1]=0;
	lptag->cr[2]=0;
	lptag->cr[3]=0;
	return DRERR_OK;
}

/*
void COpcDrv::PraseArgs(CString &szargs)//分析参数
{
	CString szReadMode="",szOpcTime = "",szTimeArea="";
	m_strRemoteMachine = "";
	char c;
	int i,pos=0,n = szargs.GetLength();
	for(i=0;i<n;i++)
	{
		c= szargs[i];
		if(c == '\t')
		{
			pos++;
			continue;
		}
		else
		{
			if(pos == 0)
				m_strRemoteMachine += c;
			else if(pos == 1)
				szReadMode += c;
			else if(pos == 2)
				szOpcTime += c;
			else if(pos == 3)
				szTimeArea += c;
		}
	}
	if(!szReadMode.IsEmpty())
		m_nReadMode = atoi(szReadMode);
	if(!szOpcTime.IsEmpty())
		m_nOpcTime = atoi(szOpcTime);
	if(!szTimeArea.IsEmpty())
		m_nTimeArea = atoi(szTimeArea);
	
}
*/
int		COpcDrv::GetConfigSize()		//取配置数据长度
{
	CMemFile mf(4096);
	CArchive ar(&mf,CArchive::store);
	SaveConfig(ar);
	ar.Close();
	int nl = (int)mf.GetLength();
	mf.Close();
	return nl;
}
DRVRET	COpcDrv::GetConfigData(void *pbuf,int nbufsize,int *pnactsize) //取配置数据
{
	*pnactsize = 0;

	//配置写入内存文件
	CMemFile mfile;
	CArchive ar(&mfile,CArchive::store);
	SaveConfig(ar);
	ar.Close();

	//获得数据字节数
	mfile.SeekToEnd();
	int len=(int)mfile.GetLength();
	if( len > nbufsize)//空间不够
		return 0;//
		
	//从内存文件拷贝到接受块
	mfile.SeekToBegin();//移动到头部
	len = (int)mfile.Read(pbuf,(DWORD)len);
	mfile.Close();
	*pnactsize = len;
	return 0;
}

void COpcDrv::LoadConfig(const void *pblk,int nsize)
{
	ClearConfig();

	CMemFile mfile;
	mfile.Write(pblk,nsize);
	mfile.SeekToBegin();
	CArchive ar(&mfile,CArchive::load);

	// 增加版本号信息
	DWORD dwDrvFlag;
	ar >> dwDrvFlag;
	if (dwDrvFlag != DRVCFG_FLAG_OPCCLIENT) // 是否为OPC驱动配置
		return;
	WORD wVersion;
	ar >> wVersion;
	if (wVersion > DRVCFG_VERSION)
		return;

	//不能作如下调用:
	//m_ItemList.Serialize(ar);
	//CString szargs="";
	//{{ OPC Server
	ar >> m_strProgID;							//程序ID
	ar >> m_strRemoteMachine;
	ar >> m_nReadMode;
	ar >> m_nOpcTime;
	ar >> m_nTimeArea;
	
	//ar >> szargs;								//计算机名或IP
	//PraseArgs(szargs);

	//}}
	//{{ OPC Group
	ar >> m_strName;		// group name					组名
	ar >> m_dwUpdateRate;	// update rate in milliseconds	数据更新周期 MS
	ar >> m_dwLanguageID;	// language ID					语言
	ar >> m_bActive;			// active state					活动状态
	ar >> m_lBias;			// time bias in minutes
	ar >> m_fDeadband;		// percent deadband				死区
	ar >> m_dwUpdateMethod;	// update method used by this group (see globals.h)
	ar >> m_nUpdateTimeout;
	//}}

	int n=0;
	ar >> n;

	CString szt;
	CItemDef *pi;
	for(int i=0;i<n;i++)
	{
		pi=new CItemDef;
		if(pi==NULL)
			continue;
		pi->Serialize(ar, wVersion);
		m_ItemArray.Add(pi);
	}

	ar.Close();
	m_bconfigModif=FALSE;//配置修改标志
}

void COpcDrv::SaveConfig(CArchive& ar)
{
	//不能作如下调用:
	//m_ItemList.Serialize(ar);

	//必须改为如下：因为在DLL里不能自动创建对象

	/*
	CString szReadMode,sztsave= m_strRemoteMachine;	
	
	szReadMode.Format("%d\t",m_nReadMode);
	m_strRemoteMachine += '\t';
	m_strRemoteMachine += szReadMode;
	szReadMode.Format("%d\t",m_nOpcTime);//OPC时标
	m_strRemoteMachine += szReadMode;
	szReadMode.Format("%d\t",m_nTimeArea);//时区
	m_strRemoteMachine += szReadMode;
	*/
	
	// 增加版本号信息
	DWORD dwDrvFlag = DRVCFG_FLAG_OPCCLIENT;
	ar << dwDrvFlag;
	WORD wVersion = DRVCFG_VERSION;
	ar << wVersion;

	//{{ OPC Server
	ar << m_strProgID;							//程序ID
	ar << m_strRemoteMachine;
	ar << m_nReadMode;
	ar << m_nOpcTime;
	ar << m_nTimeArea;
	//ar << m_strRemoteMachine;					//计算机名或IP
	//m_strRemoteMachine = sztsave;
	//}}
	//{{ OPC Group
	ar << m_strName;		// group name					组名
	ar << m_dwUpdateRate;	// update rate in milliseconds	数据更新周期 MS
	ar << m_dwLanguageID;	// language ID					语言
	ar << m_bActive;			// active state					活动状态
	ar << m_lBias;			// time bias in minutes
	ar << m_fDeadband;		// percent deadband				死区
	ar << m_dwUpdateMethod;	// update method used by this group (see globals.h)
	ar << m_nUpdateTimeout;
	//}}

	int n=m_ItemArray.GetSize();
	ar << n;

	CItemDef *pi;
	for(int i=0;i<n;i++)
	{
		pi=m_ItemArray.GetAt(i);
		pi->Serialize(ar, wVersion);
	}
}

void COpcDrv::ClearConfig()
{
	int i,n=m_ItemArray.GetSize();
	for(i=0;i<n;i++)
		delete m_ItemArray.GetAt(i);
	m_ItemArray.RemoveAll();
}

BOOL  COpcDrv::ReadSync (bool bDeviceRead)//同步读
{
	BOOL bret= FALSE;
	CObArray cItemList;
	int nitems=m_ItemArray.GetSize();
	cItemList.SetSize(READ_SYNC_TAG_MAX);
	int i,n=0;
	CItemDef *pi;
	for(i=0;i<nitems;i++)
	{
		pi = m_ItemArray.GetAt(i);

		// 标签是否存在？？
		if (!pi->m_item.IsValid())
			continue;

		cItemList.SetAt(n % READ_SYNC_TAG_MAX,&(pi->m_item));
		n++;
		if((n % READ_SYNC_TAG_MAX) == 0)
		{
			if(m_OpcGroup.ReadSync (cItemList, READ_SYNC_TAG_MAX, bDeviceRead))
				bret = TRUE;
// 			cItemList.RemoveAll();
// 			cItemList.SetSize(READ_SYNC_TAG_MAX);
		}
	}
	if((n % READ_SYNC_TAG_MAX) != 0)
	{
		if(m_OpcGroup.ReadSync (cItemList, n % READ_SYNC_TAG_MAX, bDeviceRead))
			bret = TRUE;
	}

/*	cdwItems=m_ItemList.GetCount();
	cItemList.SetSize(108);


	BOOL bret= FALSE;
	POSITION pos;
	pos=m_ItemList.GetHeadPosition();
	int n=0;
	CItemDef *pi;
	while(pos!=NULL)
	{
		pi = m_ItemList.GetNext(pos);
		cItemList.SetAt(n%100,&(pi->m_item));
		n++;
		if((n%100) == 0)
		{
			if(m_OpcGroup.ReadSync (cItemList, 100, bDeviceRead))
				bret = TRUE;
			cItemList.RemoveAll();
			cItemList.SetSize(108);
		}
	}
	if((n%100) != 0)
	{
		if(m_OpcGroup.ReadSync (cItemList, n%100, bDeviceRead))
			bret = TRUE;
	}*/
	return bret;
}

BOOL COpcDrv::CheckLastUpdateTime()
{
	if(!m_OpcServer.IsConnected())
		return TRUE;

	// 数据更新超时检测，如果达到最大数据更新超时，则断开OPC服务器连接
	if((m_nReadMode == 2) || (m_nReadMode == 3))//异步读
	{
		if (m_nUpdateTimeout > 0)
		{
			long lNow = (long)::time(NULL);
			if (lNow - m_lLastUpdateTime > (long)m_nUpdateTimeout)
			{
				CString szInfo;				
				szInfo.Format(_T("OPC服务器<%s>数据更新超时%u秒!"), m_strProgID, m_nUpdateTimeout);
				m_cbOnLogOk(m_handle, szInfo, LOG_LEVEL_WARNING, LOG_OPT_ALL);
				Disconnect();
				return FALSE;
			}
		}
	}

	return TRUE;
}

UINT COpcDrv::RunThread(void *pParam)// 运行线程
{
	if(pParam == NULL)
		return 1;
	COpcDrv *pdrv=(COpcDrv *)pParam;
	int i;
	pdrv->m_bKillThread = FALSE;
	pdrv->m_bRun = TRUE;
	int ni=pdrv->m_dwUpdateRate/100;//更新周期
	if(ni<1)
		ni=1;
	int errorno=0;
	BOOL bflash = FALSE;
	while(!pdrv->m_bKillThread)
	{
		for(i=0;i<ni;i++)
		{
			if(pdrv->m_bKillThread)
			{
				TRACE(" opc RunThread Exit1\n");
				pdrv->m_bRun=FALSE;
				pdrv->m_bKillThread = FALSE;
				return 0;
			}
			Sleep(100);
		}

		if(!pdrv->Reconnect()) //如果没连接则连接
		{
			for(i=0;i<50;i++) //5秒后重连
			{
				if(pdrv->m_bKillThread)//要求退出
				{
					pdrv->m_bRun=FALSE;
					pdrv->m_bKillThread = FALSE;
					TRACE("opc RunThread Exit2\n");
					return 0;
				}
				Sleep(100);
			}
			continue; //连接不成功
		}

		if(pdrv->m_nReadMode == 0)//同步读catch
		{
			if(!pdrv->ReadSync(FALSE))
			{
				if(!pdrv->m_OpcServer.IsAlive())
					pdrv->Disconnect();
			}
		}
		else if(pdrv->m_nReadMode == 1)//同步读device
		{
			if(!pdrv->ReadSync(TRUE))
			{
				if(!pdrv->m_OpcServer.IsAlive())
					pdrv->Disconnect();
			}
		}
		else if(pdrv->m_nReadMode == 2)//异步读1.0
		{
			if(pdrv->m_OpcGroup.IsIAsyncIOSupported())
			{ 
				if(!pdrv->m_OpcServer.IsAlive()) //判断是否连接
				{
					pdrv->Disconnect();
					bflash = FALSE;
				}
				else
				{
					if(!bflash) //没Refresh
					{
						if(pdrv->m_OpcGroup.RefreshAsync10(FALSE)) //缓存读,1.0只需Refresh一次
							bflash = TRUE;
					}
				}
			}
		}
		else if(pdrv->m_nReadMode == 3)//异步读2.0
		{
			//pdrv->AddDebug(_T("异步读2.0检测..."));

			if(pdrv->m_OpcGroup.IsIAsyncIO2Supported())
			{
#ifndef _YUEYANG_HCPS_OPCSERVER
				if(!pdrv->m_OpcServer.IsAlive()) //判断是否连接
				{
					//pdrv->AddDebug(_T("IsAlive() = false"));
					pdrv->Disconnect();
					bflash = FALSE;
				}
				else
				{
					/* 
					 * 岳阳的上海华文HCPS OPC Server调用pdrv->m_OpcGroup.RefreshAsync20()会被堵塞，造成线程无法停止，所以不允许调用该函数
					 */
					if(!bflash) //没Refresh
					{
						if(pdrv->m_OpcGroup.RefreshAsync20(FALSE)) //缓存读,2.0只需Refresh一次
							bflash = TRUE;
					}
				}
#endif
			}
		}

		// 数据刷新超时检测
		pdrv->CheckLastUpdateTime();
	}
	TRACE("opc RunThread Exit3\n");
	pdrv->m_bRun=FALSE;
	pdrv->m_bKillThread = FALSE;
	return 0;
}

BOOL COpcDrv::Reconnect()//重新连接
{
	if(m_OpcServer.IsConnected())
		return TRUE;

	TRACE("Reconnect...\n");
	if(!m_OpcServer.IsConnected())
	{
		if(!m_OpcServer.Connect(m_strProgID,m_strRemoteMachine))
		{
			TRACE("Opc <%s> Connect Err!\n",m_OpcServer.GetProgID());
			return FALSE;
		}
	}
	if(!m_OpcGroup.IsValid())//没有加入组
	{
		CString szError="";
		if(m_nReadMode == 2)
			m_OpcGroup.SetUpdateMethod(OPC_10_DATATIMECHANGE);
		else
			m_OpcGroup.SetUpdateMethod(OPC_20_DATACHANGE);
//		m_OpcGroup.SetLanguageID(m_dwLanguageID);
		if(!m_OpcServer.AddGroup(&m_OpcGroup,szError))
		{
			CString szErr;
			szErr.Format("Err:AddGroup,%s",szError);
			TRACE(szErr);
			TRACE("\n");
			return FALSE;
		}
		AddDebugFmt(_T("OPC服务器添加Group<%s>成功"), m_OpcGroup.GetName());
	}
	//加入成员
	CObArray cItemList;
	
/*	DWORD cdwItems;
	cdwItems=this->m_ItemList.GetCount();
	cItemList.SetSize(cdwItems);
	POSITION pos;
	pos=m_ItemList.GetHeadPosition();
	int n=0;
	CItemDef *pi;
	int nEcdDataType;
	while(pos!=NULL)
	{
		pi = m_ItemList.GetNext(pos);
		pi->m_item.SetParam(pi);//保存CItemDef*
		pi->m_item.SetParam2(this);//保存COpcDrv*
		pi->m_item.SetItemID(pi->m_szName);
		pi->m_item.SetGroup(&m_OpcGroup);
		nEcdDataType = EcdApi::GetEcdDataType(pi->m_val.vt);
		pi->m_item.SetDataType(EcdApi::EcdDataType2OleDataType(nEcdDataType));
		pi->m_item.SetActive(TRUE);
		cItemList.SetAt(n,&(pi->m_item));
		n++;
	}
	*/
	int i,nitems = m_ItemArray.GetSize();
	cItemList.SetSize(nitems);
	CItemDef *pi;
	for(i=0;i<nitems;i++)
	{
		pi = m_ItemArray.GetAt(i);
		pi->m_item.SetParam(pi);//保存CItemDef*
		pi->m_item.SetParam2(this);//保存COpcDrv*
		pi->m_item.SetItemID(pi->m_szName);
		pi->m_item.SetGroup(&m_OpcGroup);

#ifdef _DABA_OUT_OF_SERVICE_FILTER
		pi->m_item.m_bOutOfServiceFilter = FALSE;
#endif
	
		//nEcdDataType = EcdApi::GetEcdDataType(pi->m_val.vt);
		//pi->m_item.SetDataType(EcdApi::EcdDataType2OleDataType(nEcdDataType));
		pi->ToOleDataType(pi->m_nType); //类型转换
		pi->m_item.SetActive(TRUE);
		cItemList.SetAt(i,&(pi->m_item));
	}

	CString szErr;
	if(!m_OpcGroup.AddItems(cItemList,nitems,szErr))
	{
		CString szError;
		szError.Format("Err:AddItems,%s",szErr);
		TRACE(szError);
		TRACE("\n");
		return FALSE;
	}
	AddDebugFmt(_T("OPCGroup<%s>添加%d个标签成功"), m_OpcGroup.GetName(), nitems);

	m_lLastUpdateTime = (long)::time(NULL);
	return TRUE;
}

void COpcDrv::StartRun()
{
	if(m_bRun)
		return;
	
	if(!m_OpcServer.IsConnected())
	{
		if(!m_OpcServer.Connect(m_strProgID,m_strRemoteMachine))
		{
			TRACE("Opc <%s> Connect Err!\n",m_OpcServer.GetProgID());
			return;
		}
	}
	if(!m_OpcGroup.IsValid())//没有加入组
	{
		// hq2011/11/11 原代码没有设置死区和刷新率
		m_OpcGroup.SetDeadband(m_fDeadband);
		m_OpcGroup.SetUpdateRate(m_dwUpdateRate);
		//m_OpcGroup.SetBias(m_lBias);

		CString szError="";
		if(m_nReadMode == 2)
			m_OpcGroup.SetUpdateMethod(OPC_10_DATATIMECHANGE);
		else
			m_OpcGroup.SetUpdateMethod(OPC_20_DATACHANGE);
		
		m_OpcGroup.SetLanguageID(m_dwLanguageID);
		if(!m_OpcServer.AddGroup(&m_OpcGroup,szError))
		{
			CString szErr;
			szErr.Format("Err:AddGroup,%s",szError);
			TRACE(szErr);
			TRACE("\n");
			return ;
		}
		AddDebugFmt(_T("OPC服务器添加Group<%s>成功"), m_OpcGroup.GetName());
	}
	//加入成员
	/*
	CObArray cItemList;
	DWORD cdwItems;
	cdwItems=this->m_ItemList.GetCount();
	cItemList.SetSize(cdwItems);
	POSITION pos;
	pos=m_ItemList.GetHeadPosition();
	int n=0;
	CItemDef *pi;
	int nEcdDataType;
	while(pos!=NULL)
	{
		pi = m_ItemList.GetNext(pos);
		pi->m_item.SetParam(pi);//保存CItemDef*
		pi->m_item.SetParam2(this);//保存COpcDrv*
		pi->m_item.SetItemID(pi->m_szName);
		pi->m_item.SetGroup(&m_OpcGroup);
		nEcdDataType = EcdApi::GetEcdDataType(pi->m_val.vt);
		pi->m_item.SetDataType(EcdApi::EcdDataType2OleDataType(nEcdDataType));
		pi->m_item.SetActive(TRUE);
		cItemList.SetAt(n,&(pi->m_item));
		n++;
	}
	CString szErr;
	if(!m_OpcGroup.AddItems(cItemList,n,szErr))
	{
		CString szError;
		szError.Format("Err:AddItems,%s",szErr);
		TRACE(szError);
		TRACE("\n");
	}*/
	CObArray cItemList;
	int i,nitems = m_ItemArray.GetSize();
	cItemList.SetSize(nitems);
	CItemDef *pi;
	for(i=0;i<nitems;i++)
	{
		pi = m_ItemArray.GetAt(i);
		pi->m_item.SetParam(pi);//保存CItemDef*
		pi->m_item.SetParam2(this);//保存COpcDrv*
		pi->m_item.SetItemID(pi->m_szName);
		pi->m_item.SetGroup(&m_OpcGroup);

#ifdef _DABA_OUT_OF_SERVICE_FILTER
		pi->m_item.m_bOutOfServiceFilter = FALSE;
#endif
		//nEcdDataType = EcdApi::GetEcdDataType(pi->m_val.vt);
		//pi->m_item.SetDataType(EcdApi::EcdDataType2OleDataType(nEcdDataType));
		pi->ToOleDataType(pi->m_nType); //类型转换
		pi->m_item.SetActive(TRUE);
		cItemList.SetAt(i,&(pi->m_item));
	}
	CString szErr;
	if(!m_OpcGroup.AddItems(cItemList,nitems,szErr))
	{
		CString szError;
		szError.Format("Err:AddItems,%s",szErr);
		TRACE(szError);
		TRACE("\n");
	}
	AddDebugFmt(_T("OPCGroup<%s>添加%d个标签成功"), m_OpcGroup.GetName(), nitems);

#ifdef _YUEYANG_HCPS_OPCSERVER
	AddInfo(_T("岳阳的上海华文HCPS OPC Server专版"));
#endif

#ifdef _DABA_OUT_OF_SERVICE_FILTER
	AddInfo(_T("大坝的OPC Server专版"));
#endif

//	if(OnLog) //使用例子
//		OnLog(m_handle,0,"Opc Strat From Drv Log!");
	cItemList.RemoveAll();// needcode 新加的可能是以前的一个小bug
	::AfxBeginThread(RunThread,this);
}
void COpcDrv::StopRun()
{
	m_bKillThread=TRUE;
	while(m_bRun)
		Sleep(100);

	if(!m_OpcServer.IsConnected())
		return;
	if(!m_OpcGroup.IsValid())
	{
		m_OpcServer.Disconnect();
		return;
	}

	// 移除items
	CObArray cItemList;
	int i,nItems;
	nItems=m_ItemArray.GetSize();
	cItemList.SetSize(nItems);
/*	POSITION pos;
	pos=m_ItemList.GetHeadPosition();
	int n=0;
	CItemDef *pi;
	while(pos!=NULL)
	{
		pi = m_ItemList.GetNext(pos);
		cItemList.SetAt(n,&(pi->m_item));
		n++;
	}*/
	CItemDef *pi;
	for(i =0 ;i < nItems;i++)
	{
		pi = m_ItemArray.GetAt(i);
		cItemList.SetAt(i,&(pi->m_item));
	}

	CString szErr="";
	if(!m_OpcGroup.RemoveItems(cItemList,nItems,szErr))
		TRACE(szErr);
	cItemList.RemoveAll();
	// 移除group
	m_OpcServer.RemoveGroup(&m_OpcGroup);
	// 断开连接
	m_OpcServer.Disconnect();
}

BOOL COpcDrv::Disconnect()
{
	// 移除items
	CObArray cItemList;
	int  nItems;
	nItems=m_ItemArray.GetSize();
	cItemList.SetSize(nItems);

	CItemDef *pi;
	for(int i=0;i<nItems;i++)
	{
		pi = m_ItemArray.GetAt(i);
		cItemList.SetAt(i,&(pi->m_item));

	}
	CString szErr="";
	if(!m_OpcGroup.RemoveItems(cItemList,nItems,szErr))
		TRACE(szErr);
	cItemList.RemoveAll();
	// 移除group
	m_OpcServer.RemoveGroup(&m_OpcGroup);
	// 断开连接
	m_OpcServer.Disconnect();
	return TRUE;
}

CItemDef *COpcDrv::FindItem(LPCTSTR lpszName)
{
	int  nItems;
	nItems=m_ItemArray.GetSize();


	CItemDef *pi;
	for(int i=0;i<nItems;i++)
	{
		pi = m_ItemArray.GetAt(i);
		if(pi->m_szName == lpszName)
			return pi;
	}
	return NULL;
}

BOOL COpcDrv::DelItem(LPCTSTR lpszName,CString &szErr)
{
	CSafeLock lock(&m_Lock);//锁

	int  nItems;
	nItems=m_ItemArray.GetSize();


	CItemDef *pi;
	for(int i=0;i<nItems;i++)
	{
		pi = m_ItemArray.GetAt(i);
		if(pi->m_szName == lpszName)
		{
			m_ItemArray.RemoveAt(i);
			delete pi;
			m_bconfigModif = TRUE;
			return TRUE;
		}
	}
	/*
	CItemDef *pi;
	POSITION pos,posp;
	pos=m_ItemList.GetHeadPosition();
	while(pos!=NULL)
	{
		posp=pos;
		pi=m_ItemList.GetNext(pos);
		if(pi->m_szName == lpszName)
		{
			m_ItemList.RemoveAt(posp);
			delete pi;
			m_bconfigModif = TRUE;
			return TRUE;
		}
	}*/
	szErr.Format("错误:没有找到<%s>",lpszName);
	return FALSE;
}

BOOL COpcDrv::ModifyItem(LPCTSTR lpszOldName,LPCTSTR lpszNewName,
						  int nType,int nAccess,CString &szErr)
{
	CSafeLock lock(&m_Lock);//锁

	CItemDef *pi,*pin=NULL;
	BOOL	bReName=FALSE;
	CString szNewName = lpszNewName,szOldName = lpszOldName;
	BOOL   bNeedFindReName=FALSE;
	if(szNewName != lpszOldName)
		bNeedFindReName=TRUE;
	
	int i,n = m_ItemArray.GetSize();
	for(i=0;i<n;i++)
	{
		pi=m_ItemArray.GetAt(i);
	
		if(pi->m_szName == szOldName)
		{
			pin=pi;
			if(!bNeedFindReName)
				break;
		}

		if(bNeedFindReName)//需要找重名
		{
			if(pi->m_szName == szNewName)
			{
				szErr.Format("<%s>重名!",lpszNewName);
				return FALSE;
			}
		}
		
	}
	if(pin != NULL)
	{
		pin->m_szName = szNewName;
		pin->m_nType = nType;
		pin->m_nAccess = nAccess;
		m_bconfigModif = TRUE;
		return TRUE;
	}
	szErr.Format("错误:没有找到<%s>",lpszOldName);
	return FALSE;
}

BOOL COpcDrv::AddItem(LPCTSTR lpszName,
					   int nType,int nAccess,CString &szErr)
{
	CSafeLock lock(&m_Lock);//锁
	int i,n=m_ItemArray.GetSize();
	CItemDef *pi;

	for(i=0;i<n;i++)
	{
		pi=m_ItemArray.GetAt(i);
		if(pi->m_szName == lpszName)
		{
			szErr.Format("错误:数据项<%s>重名!",lpszName);
			return FALSE;
		}
	}

	pi=new CItemDef;
	if(!pi)
		return FALSE;
	pi->m_szName = lpszName;
	pi->m_nType  = nType;
	pi->m_nAccess = nAccess;

	m_ItemArray.Add(pi);
	m_bconfigModif = TRUE;
	return TRUE;
}
void COpcDrv::ReMoveAll()
{
	CSafeLock lock(&m_Lock);//锁
	ClearConfig();
}
void COpcDrv::ItemsToListCtrl(CTagItemWinList *pList)
{
	CSafeLock lock(&m_Lock);//锁

	CItemDef *pi;
	int i,n = m_ItemArray.GetSize();
	int nHashSize = (int)(n * 1.2);
	if (nHashSize > 36353)
		pList->InitHashSize(nHashSize);

	for(i=0;i<n;i++)
	{
		pi = m_ItemArray.GetAt(i);
		pList->AddItem(pi->m_szName, pi->m_szDes, pi->m_szUnit, pi->GetTypeStr(pi->m_nType), pi->m_bZeroFilter);
	}
	pList->ReDraw();
}

void COpcDrv::ListCtrlToItems(CTagItemWinList *pList)
{
	CSafeLock lock(&m_Lock);//锁
	CItemDef *pi;
	CString szType;
	CTagItem *pItem;

	for(int i = 0; i < pList->GetCount(); i++)
	{
		pItem = pList->GetAt(i);
		if (!pItem)
			continue;

		pi = new CItemDef;
		if(!pi)
			continue;
		pi->m_szName = pItem->m_szItemName;
		szType = pItem->m_szDataType;
		pi->m_szDes = pItem->m_szDesc;
		pi->m_szUnit = pItem->m_szUnit;
		pi->m_bZeroFilter = pItem->m_bZeroFilter;

		pi->m_nAccess = 0;
		pi->m_nType = pi->GetTypeVal(szType);
		m_ItemArray.Add(pi);
	}
}

#include "DlgOpcSet.h"
#include "DlgOpcVar.h"

BOOL COpcDrv::Config()//配置
{
	
	m_bconfigModif = FALSE;

//	if(m_strProgID.IsEmpty())
	{
		CDlgOpcSet dlg;
		dlg.m_szMachine = m_strRemoteMachine;
		dlg.m_szProgID = m_strProgID;
		dlg.m_nTimeArea = m_nTimeArea;
		dlg.m_dwUpdateRate = m_dwUpdateRate;
		dlg.m_fPercentDead = m_fDeadband;
		dlg.m_nUpdateTimeout = m_nUpdateTimeout;
		
		if(IDOK != dlg.DoModal())
			return FALSE;
		m_strRemoteMachine = dlg.m_szMachine;
		m_strProgID = dlg.m_szProgID;
		m_dwUpdateRate=dlg.m_dwUpdateRate;
		m_fDeadband = dlg.m_fPercentDead;
		m_nTimeArea = dlg.m_nTimeArea;
		m_nUpdateTimeout = dlg.m_nUpdateTimeout;
	}
lpconnect:
	if(!m_OpcServer.IsConnected())
	{
		if(!m_OpcServer.Connect(m_strProgID,m_strRemoteMachine))
		{
			CString szErr;
			szErr.Format("连接OPC服务器:Machine=<%s>,ProgID=<%s>失败!",
				m_strRemoteMachine,m_strProgID);
			AfxMessageBox(szErr);
			CDlgOpcSet dlg;
			dlg.m_szMachine = m_strRemoteMachine;
			dlg.m_szProgID = m_strProgID;
			dlg.m_nTimeArea = m_nTimeArea;
			dlg.m_dwUpdateRate = m_dwUpdateRate;
			dlg.m_fPercentDead = m_fDeadband;
			if(IDOK != dlg.DoModal())
				return FALSE;
			m_strRemoteMachine = dlg.m_szMachine;
			m_strProgID = dlg.m_szProgID;
			m_dwUpdateRate=dlg.m_dwUpdateRate;
			m_fDeadband = dlg.m_fPercentDead;
			m_nTimeArea = dlg.m_nTimeArea;
			goto lpconnect;
		
		}
	}

	CDlgOpcVar dlgvar(m_OpcServer.GetProgID(),this->m_OpcGroup.GetIItemMgt(),m_OpcServer.GetIBrowse());
	dlgvar.m_piItemProp = m_OpcServer.GetIItemProps();//addby jy 20091221
	dlgvar.m_nReadMode = this->m_nReadMode;
	dlgvar.m_nOpcTime = m_nOpcTime;
	if(this->m_dwLanguageID == MAKELANGID (LANG_ENGLISH, SUBLANG_ENGLISH_US))
		dlgvar.m_nLangage = 0;
	else
		dlgvar.m_nLangage = 1;
	dlgvar.SetDrv(this);
//	dlgvar.m_sztitle.Format("OPC items配置<%s>",lpszDecName);
	dlgvar.m_sztitle = "OPC items配置";
	if(IDOK != dlgvar.DoModal())
		return FALSE;
	

	m_nReadMode = dlgvar.m_nReadMode;
	m_nOpcTime = dlgvar.m_nOpcTime;
	if(dlgvar.m_nLangage == 0)
		m_dwLanguageID = MAKELANGID (LANG_ENGLISH, SUBLANG_ENGLISH_US);
	else
		m_dwLanguageID = MAKELANGID (LANG_CHINESE,SUBLANG_CHINESE_SIMPLIFIED);//中文
//	return TRUE;
//	if(dlgvar.IsModify())
//		return TRUE;

	m_OpcServer.Disconnect();
	return TRUE;
}
DRVRET COpcDrv::WriteVar(LPDRREC lprec)
{
	//needcode 暂不支持写
	return 0; 
}
/*
DWORD COpcDrv::WriteVar(LPCTSTR lpszName,LPECDITEMVAL pval)
{

	CString szAddr=lpszName;

	CItemDef *pi = this->FindItem(szAddr);
	if(pi == NULL)
		return RESULT_NO_ITEM;				// 无数据项
	if((EcdApi::GetEcdDataAccess(pi->m_val.vt)) < 2)
		return RESULT_ITEM_READONLY;		// 只读
	if(!m_OpcServer.IsConnected())
		return RESULT_FAILURE;
	if(!m_OpcGroup.IsValid())
		return RESULT_FAILURE;
	int ecdtype=EcdApi::GetEcdDataType(pval->vt);
	int oletype=EcdApi::EcdDataType2OleDataType(ecdtype);
	CString szVal="";
	szVal=EcdApi::EcdItemVal2Str(pval);
	TRACE("COpcDrv::WriteVar:szVal = %s\n",szVal);
	m_OpcGroup.WriteOneSync(pi->m_item.GetServerHandle(),szVal,
		oletype);
	
	return RESULT_OK;
}*/

int COpcDrv::GetItemCount()
{
	CSafeLock lock(&m_Lock);//锁
	return m_ItemArray.GetSize();
}

BOOL COpcDrv::GetItem(LPCTSTR lpszName,int &nType,int &nAccess,CString &szErr)
{
	CSafeLock lock(&m_Lock);//锁
	int i,n = m_ItemArray.GetSize();
	CItemDef *pi;

	for(i=0;i<n;i++)
	{
		pi=m_ItemArray.GetAt(i);
		if(pi->m_szName == lpszName)
		{
			nType = pi->m_nType;
			nAccess = pi->m_nAccess;
			return TRUE;
		}
	}
	szErr.Format("%s<%s>",g_LoadString(IDS_ERRNOFIND),lpszName);//错误:没有找到
	return FALSE;
}
