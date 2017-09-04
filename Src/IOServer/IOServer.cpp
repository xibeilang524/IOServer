
// IOServer.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "IOServer.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "IOServerDoc.h"
#include "IOServerView.h"
#include "DbpArgConfigDlg.h"
#include "DlgAddDec.h"
#include "IniTool.h"
#include "DlgIniSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CIOServerApp
//const CString gCfgFileName = _T("system\\Server.ini");

BEGIN_MESSAGE_MAP(CIOServerApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CIOServerApp::OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// 标准打印设置命令
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
	ON_COMMAND(ID_SYS_LOGON, &CIOServerApp::OnSysLogon)
	ON_COMMAND(ID_SYS_LOGOFF, &CIOServerApp::OnSysLogoff)
	ON_COMMAND(ID_PWD_MODIFY, &CIOServerApp::OnPwdModify)
	ON_COMMAND(ID_IDMGR, &CIOServerApp::OnIdmgr)
	ON_COMMAND(ID_SERVER_PARA, &CIOServerApp::OnServerPara)
	ON_COMMAND(ID_SERVER_LOCAL, &CIOServerApp::OnServerLocal)
END_MESSAGE_MAP()


// CIOServerApp 构造

CIOServerApp::CIOServerApp()
	:m_pDriverMgr(NULL)
	,m_szAppDir("")
	,m_hMutex(NULL)
	,m_pDecVarTemplate(NULL)
	,m_pAuthorityMgr(NULL)
	,m_pLog(NULL)
	,m_bAuthority(TRUE)
{
	m_bHiColorIcons = TRUE;

	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 如果应用程序是利用公共语言运行时支持(/clr)构建的，则:
	//     1) 必须有此附加设置，“重新启动管理器”支持才能正常工作。
	//     2) 在您的项目中，您必须按照生成顺序向 System.Windows.Forms 添加引用。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("IOServer.AppID.NoVersion"));

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CIOServerApp 对象

CIOServerApp theApp;


// CIOServerApp 初始化

BOOL CIOServerApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	// 每个安装目录服务只允许一个运行实例，而不同目录的服务，可以同时运行
	CString szAppDir = Utility::GetModulePath();
	m_szAppDir = szAppDir;

	CString szAppDirMd5;
	MakeMD5Txt32(szAppDir, szAppDirMd5);
	CString szInstanceMutexName;
	szInstanceMutexName.Format(_T("_KIMI_IOSERVER_%s"), szAppDirMd5);

	//单实例 wzq 20101202
	HANDLE hMutex; 
	hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, szInstanceMutexName);
	if (hMutex)
	{
		CloseHandle(hMutex); 
		hMutex = NULL;
		return FALSE;
	}
	m_hMutex= CreateMutex(NULL, FALSE, szInstanceMutexName);

	//读入本地账号
	CString szErr = "";



	// 初始化 OLE 库
// 	if (!AfxOleInit())
// 	{
// 		AfxMessageBox(IDP_OLE_INIT_FAILED);
// 		return FALSE;
// 	}
	HRESULT hr = S_FALSE;
	//hr = CoInitializeEx (NULL, COINIT_MULTITHREADED);
	//if(!SUCCEEDED(hr))
	//	return FALSE;
	// 如果不初始此安全令牌，在某些情况下可能会出现各种奇怪问题

	/*HRESULT */hr =::CoInitializeSecurity(
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

	if (FAILED(hr))
	{
		TRACE(_T("CoInitializeSecurity error: 0x%02X"), hr);
	}

	m_pAuthorityMgr = CAuthorityMgr::GetInstance();
	if(!m_pAuthorityMgr)
		return FALSE;
	
	CString szPath = Utility::GetModulePath();

	m_pLog = CLog::GetInstance();
	if(!m_pLog)
		return FALSE;
	m_pLog->InitLogPath(szPath, "IOServer");
	
	//授权管理
	CString szLicFileName = m_szAppDir;
	szLicFileName += _T("License.lic");

	if (!m_LicenseMgr.Load(szLicFileName, "RTDBServer"))
	{
		CString szLicInfo;
		KIMILIC_ERRCODE err = m_LicenseMgr.GetLastErrorCode();

		switch(err)
		{
		case KIMI_LIC_ERR_FILE_NOT_EXIST:
			szLicInfo.LoadString(IDS_NOAUTHORITY_FILE);
			break;
		case KIMI_LIC_ERR_SOFTWARE_NOT_EXIST:
			szLicInfo.LoadString(IDS_NOAUTHORITY);
			break;
		case KIMI_LIC_ERR_NOT_AUTHORIZED:
			szLicInfo.LoadStringA(IDS_NOAUTHORITY_SOFT);
			break;
		case KIMI_LIC_ERR_MCODE_NOT_MATCH:
			szLicInfo.LoadStringA(IDS_AUTHORITY_ERR_MEC);
			break;
		case KIMI_LIC_ERR_AUTH_EXPIRED:
			szLicInfo.LoadStringA(IDS_AUTHORITY_EXPIRED);
			break;
		}

		CString szMachineCodeFile = m_szAppDir;
		szMachineCodeFile += _T("SerielCode.txt");

		try
		{
			FILE* pf = fopen(szMachineCodeFile, "wt");
			if (pf)
			{
				std::string szMachineCode = m_LicenseMgr.GetMachineCode();

				fwrite(szMachineCode.c_str(), szMachineCode.size(), 1, pf);
				fclose(pf);
			}
		}
		catch (...)
		{

		}

		AfxMessageBox(szLicInfo, MB_OK | MB_ICONERROR);
		m_bAuthority = FALSE;
	}
	
	if(!m_pAuthorityMgr->LoadDrv(szAppDir + "system\\Operator.dll",szErr)) //读入账号管理DLL文件
	{
		AfxMessageBox(szErr);
		return FALSE;
	}
	m_pAuthorityMgr->LoadOptFile(szAppDir + "system\\opt.opt"); //读入账号文件
	//系统配置
	CString szCfgFile = szAppDir + g_szLocalCfgFile;

	// 如果配置文件不存在，则创建一个默认的
	if (!::PathFileExists(szCfgFile))
	{
		CIniTool newIni(szCfgFile);
		newIni.Write();
	}

	if(!m_Config.ReadIniFile(szCfgFile)) 
	{
		AfxMessageBox(IDS_CFGERR);
		return FALSE;
	}

	m_pLog->SetAllowLogLevel(m_Config.m_nLogLevel);

	AfxEnableControlContainer();

	EnableTaskbarInteraction();

	// 使用 RichEdit 控件需要  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 注册应用程序的文档模板。文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_IOServerTYPE,
		RUNTIME_CLASS(CIOServerDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CIOServerView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	m_pDecVarTemplate = pDocTemplate;
	// 创建主 MDI 框架窗口
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	// 仅当具有后缀时才调用 DragAcceptFiles
	//  在 MDI 应用程序中，这应在设置 m_pMainWnd 之后立即发生
	// 启用拖/放
	m_pMainWnd->DragAcceptFiles();

	// 分析标准 shell 命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	//启动时无新文档
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew) 
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;

	// 启用“DDE 执行”
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);


	// 调度在命令行中指定的命令。如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// 主窗口已初始化，因此显示它并对其进行更新
	pMainFrame->ShowWindow(m_nCmdShow);

	m_pDriverMgr = CDriverMgr::GetInstance();

	InitDriveMgr();
	//读入工程
	m_pDriverMgr->OpenProject(szAppDir,m_Config.m_szProjectName);

	pMainFrame->AddProjectData();

	m_pDriverMgr->Start(m_Config.m_bAutoRun);

	pMainFrame->UpdateWindow();

	return TRUE;
}

int CIOServerApp::ExitInstance()
{
	//TODO: 处理可能已添加的附加资源
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CIOServerApp 消息处理程序


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CIOServerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CIOServerApp 自定义加载/保存方法

void CIOServerApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);

	GetContextMenuManager()->AddMenu("工程管理", IDR_MENU_PROJECT);
}

void CIOServerApp::LoadCustomState()
{
}

void CIOServerApp::SaveCustomState()
{
}

// CIOServerApp 消息处理程序



// //登录函数
void CIOServerApp::Logon(void)
{
	m_pAuthorityMgr->Login();
	if(!m_pAuthorityMgr->m_szOpt.IsEmpty())
	{
		m_pLog->AddInfoFmt("<%s>登录",theApp.m_pAuthorityMgr->m_szOpt);
	}
}


// 注销
void CIOServerApp::LogOff(void)
{
	CString szopt = theApp.m_pAuthorityMgr->m_szOpt;
	m_pAuthorityMgr->Logout();
	if(m_pAuthorityMgr->m_szOpt.IsEmpty())
	{
		m_pLog->AddInfoFmt("<%s>注销",szopt);
	}
}


void CIOServerApp::UserIDMgr(void)
{
	if(m_pAuthorityMgr->m_szOpt.IsEmpty())
	{
		AfxMessageBox("请先登录!");
		return;
	}
	if(m_pAuthorityMgr->m_dwPower != 0xFFFFFFFF)
	{
		AfxMessageBox("权限不够!");
		return;
	}
	m_pAuthorityMgr->OptMan(m_pAuthorityMgr->m_szOpt,m_pAuthorityMgr->m_szPass);
}


void CIOServerApp::UserIDModify(void)
{
	if(m_pAuthorityMgr->m_szOpt.IsEmpty())
	{
		AfxMessageBox("请先登录!");
		return;
	}

	if(!m_pAuthorityMgr->ModifyPass(m_pAuthorityMgr->m_szOpt))
	{
		m_pLog->AddInfoFmt(_T("<%s>注销"), m_pAuthorityMgr->m_szOpt);

		AfxMessageBox("修改密码成功，请重新登录!");
		m_pAuthorityMgr->Logout();
	}	
}


void CIOServerApp::SetDBConnectInfo(void)
{
	if(m_pAuthorityMgr->m_szOpt.IsEmpty())
	{
		AfxMessageBox("请先登录!");
		return;
	}
	if(m_pAuthorityMgr->m_dwPower != 0xFFFFFFFF)
	{
		AfxMessageBox("权限不够!");
		return;
	}
	CDBPArgs dbpArgs;
	CDbpArgConfigDlg dlg;
	m_pDriverMgr->GetDBPConnectArgs(dbpArgs);
	dlg.SetDbpConArgs(dbpArgs);

	if (dlg.DoModal() != IDOK)
		return;

	if (dlg.GetDbpConArgs(dbpArgs))
	{
		m_pDriverMgr->SetDBPConnectArgs(dbpArgs);//保存

		AfxMessageBox("目标服务器参数配置已改变，请重新启动本程序配置生效!");
	}

}


BOOL CIOServerApp::IsLogin(void)
{
	BOOL bRet = FALSE;
	if (m_pAuthorityMgr->m_szOpt.IsEmpty())
	{
		bRet = TRUE;
	}
	return bRet;
}


DWORD CIOServerApp::GetUserPower(void)
{
	return m_pAuthorityMgr->m_dwPower;
}


BOOL CIOServerApp::CheckPower(DWORD dwPower)
{
	if(m_pAuthorityMgr->m_szOpt.IsEmpty())
	{
		AfxMessageBox("请先登录!",MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	if((m_pAuthorityMgr->m_dwPower & dwPower) != dwPower)
	{
		AfxMessageBox("权限不够!",MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}

	return TRUE;
}


void CIOServerApp::AddDrv(void)
{
	if(!theApp.CheckPower(0XFFFFFFFF))
		return ; 
	CDlgAddDec dlg;
	dlg.m_szPath = Utility::GetModulePath() + "drivers/";
	if(dlg.DoModal() != IDOK)
		return;
	CString szDecName = dlg.m_szDecName;
	CString szDrvName = dlg.m_szDrvName;
	int nRet = m_pDriverMgr->AddDrv(dlg.m_szPath,dlg.m_szDrvName,dlg.m_szDecName);
	switch(nRet)
	{
	case  ERR_DRV_EXIST: //设备存在
		AfxMessageBox("设备名已存在!",MB_OK|MB_ICONERROR);
		break;
	case  ERR_DRV_CREATE: //设备存在
		AfxMessageBox("创建驱动实例错误!",MB_OK|MB_ICONERROR);
		break;
	case  ERR_DRV_LOAD: //设备存在
		AfxMessageBox("加载驱动错误",MB_OK|MB_ICONERROR);
		break;
	case  ERR_DRV_NO: //设备存在
		break;
	}
	GetLogger()->AddWarningFmt(LOG_ADD_DEVICE_FORMAT, LOG_LOGIN_USER, dlg.m_szDecName);
	if(nRet == ERR_DRV_OK)
	{
		//m_pDriverMgr->AddDrvToTree(pdrv,m_WorkBar.GetTree());	
		((CMainFrame *)m_pMainWnd)->AddNode2Project(szDrvName, szDecName, NODE_RDB);
		Save();//保存配置
	}
}


BOOL CIOServerApp::GetDrvInst(CString szDrvName)
{
	CDrvItem* pItem = m_pDriverMgr->GetDrvInst(szDrvName);
	if (pItem)
		return TRUE;
	return FALSE;
}


BOOL CIOServerApp::GetDrvInfo(CString szName, CString szPath, T_DRVINFO1& tDRVINFO)
{
	BOOL bRet = FALSE;
	if (szName.IsEmpty() || szPath.IsEmpty())
		return bRet;
	if(ERR_DRV_OK == m_pDriverMgr->GetDrvInfo(szName, szPath, tDRVINFO))
		bRet = TRUE;
	return bRet;
}


int CIOServerApp::QueryTag(LPCTSTR strDecName, 
	CSortObArray& SortArray,
	LPCTSTR szTagName,
	LPCTSTR szProxyTagName,
	LPCTSTR szDes,
	LPCTSTR szDrvTagName,
	int nType)
{
	return m_pDriverMgr->QueryTag(strDecName, SortArray, szTagName, szProxyTagName, szDes, szDrvTagName, nType);
}


int CIOServerApp::GetDecStatus(CString szDecName)
{
	return m_pDriverMgr->GetDecStatus(szDecName);
}

BOOL CIOServerApp::GetDrvTagDef(CString szDrvName, int iIndex, T_DRVTAG& tDRVTAG)
{
	return m_pDriverMgr->GetDrvTagDef(szDrvName,  iIndex, tDRVTAG);
}

int CIOServerApp::AddTag2Drv(CString szDecName, T_DRVTAG& tDRVTAG, DWORD dwFlag)
{
	return m_pDriverMgr->AddTag2Drv(szDecName,  tDRVTAG, dwFlag);
}
BOOL CIOServerApp::AddTag2Drv(CString szDecName, CTagItem& tTagItem, DWORD dwFlag)
{
	return m_pDriverMgr->AddTag2Drv(szDecName,  tTagItem, dwFlag);
}

int CIOServerApp::Save(void)
{
	int iRet = 0;
	iRet = m_pDriverMgr->SavePrjFile();
	return iRet;
}

int CIOServerApp::CheckExp(const CString& szExp, CString szTagName, CString& szErrMsg, int& nTagCount)
{
	return m_pDriverMgr->CheckExp(szExp, szTagName, szErrMsg, nTagCount);
}

int CIOServerApp::DelDrvTag(CString szDecName, CString szTagName)
{
	return m_pDriverMgr->DelDrvTag(szDecName, szTagName);
}


BOOL CIOServerApp::UpdateTag(CString szDecName, CTagItem& tagItem)
{
	return m_pDriverMgr->UpdateTag(szDecName, tagItem);
}


BOOL CIOServerApp::ImportTags(CString szDecName,CCSVFile& csvFile,int &nUpdate,int &nInsert,int &nAll,DWORD dwflag)
{
	return m_pDriverMgr->ImportTags(szDecName, csvFile, nUpdate, nInsert, nAll, dwflag);
}


int CIOServerApp::InitDriveMgr(void)
{
	if(!m_pDriverMgr)
		return -1;
	m_pDriverMgr->m_bAutoRun	= m_Config.m_bAutoRun;			//启动时自动运行驱动实例
	m_pDriverMgr->m_bSaveLog	= m_Config.m_bSaveLog;			//保存日志
	m_pDriverMgr->m_bExitPass	= m_Config.m_bExitPass;			//退出密码验证
	m_pDriverMgr->m_bCashe		= m_Config.m_bCashe;			//是否使用cashe
	m_pDriverMgr->m_bSkipSameTime = m_Config.m_bSkipSameTime;	//放弃相同时标快照更新
	m_pDriverMgr->m_nTimeZero	= m_Config.m_nTimeZero;			//显示时区-12,12
	m_pDriverMgr->m_wHeartPort	= m_Config.m_wheartport;		//心跳端口
	m_pDriverMgr->m_szHeartIP	= m_Config.m_szHeartIP;			//心跳IP,空表示本机是服务器
	m_pDriverMgr->m_nLogLevel	= m_Config.m_nLogLevel;			//日志级别，0-错误、1-警告、2-提示、3-调试
	//m_pDriverMgr->m_szTitle		= m_Config.m_szTitle;			//DDClient附加显示标题，以区分DDClient的功能
	m_pDriverMgr->m_bWaitTcpAck	= m_Config.m_bWaitTcpAck;		//TCP是否有响应包，部分网闸是不允许有响应包的
	m_pDriverMgr->m_nUdpSendDelay = m_Config.m_nUdpSendDelay;	//UDP发包延时
	m_pDriverMgr->m_nUpThreads	= m_Config.m_nupthreads;		//上传线程数
	//计算获得
	m_pDriverMgr->m_lAdjustSec	= m_Config.m_lAdjustSec;		//显示调整间秒,即m_nTimeZero * 3600 + TIME_ZONE_INFORMATION.Bias * 60
	//end
	return 0;
}


int CIOServerApp::GetDrvInstances(vecDrivers& vecDatas)
{
	return m_pDriverMgr->GetDrvInstances(vecDatas);
}


BOOL CIOServerApp::StartDrv(CString szModule, CString szDrvName)
{
	return m_pDriverMgr->Start(szModule, szDrvName);
}


BOOL CIOServerApp::StopDrv(CString szDrvName)
{
	return m_pDriverMgr->Stop(szDrvName);
}


BOOL CIOServerApp::ConfigDrv(CString szDrvName)
{
	return m_pDriverMgr->ConfigDrv(szDrvName);
}


BOOL CIOServerApp::DeleteDrv(CString szDevName)
{
	m_pDriverMgr->DelOneDec(szDevName);
	CloseDecVarView(szDevName);
	return 0;
}



void CIOServerApp::OnSysLogon()
{
	// TODO: 在此添加命令处理程序代码
	Logon();
}


void CIOServerApp::OnSysLogoff()
{
	// TODO: 在此添加命令处理程序代码
	LogOff();
}


void CIOServerApp::OnPwdModify()
{
	// TODO: 在此添加命令处理程序代码
	if(m_pAuthorityMgr->m_szOpt.IsEmpty())
	{
		AfxMessageBox("请先登录!");
		return;
	}

	if(!m_pAuthorityMgr->ModifyPass(m_pAuthorityMgr->m_szOpt))
	{
		m_pLog->AddInfoFmt(_T("<%s>注销"), m_pAuthorityMgr->m_szOpt);

		AfxMessageBox("修改密码成功，请重新登录!");
		m_pAuthorityMgr->Logout();
	}	
}


void CIOServerApp::OnIdmgr()
{
	// TODO: 在此添加命令处理程序代码
	if(m_pAuthorityMgr->m_szOpt.IsEmpty())
	{
		AfxMessageBox("请先登录!");
		return;
	}
	if(m_pAuthorityMgr->m_dwPower != 0xFFFFFFFF)
	{
		AfxMessageBox("权限不够!");
		return;
	}
	m_pAuthorityMgr->OptMan(m_pAuthorityMgr->m_szOpt,m_pAuthorityMgr->m_szPass);
}


void CIOServerApp::OnServerPara()
{
	// TODO: 在此添加命令处理程序代码
	if(m_pAuthorityMgr->m_szOpt.IsEmpty())
	{
		AfxMessageBox("请先登录!");
		return;
	}
	if(m_pAuthorityMgr->m_dwPower != 0xFFFFFFFF)
	{
		AfxMessageBox("权限不够!");
		return;
	}
	CDBPArgs dbpArgs;
	m_pDriverMgr->GetDBPConnectArgs(dbpArgs);
	CDbpArgConfigDlg dlg;

	dlg.SetDbpConArgs(dbpArgs);

	if (dlg.DoModal() != IDOK)
		return;

	if (dlg.GetDbpConArgs(dbpArgs))
	{
		m_pDriverMgr->SetDBPConnectArgs(dbpArgs);
		AfxMessageBox("目标服务器参数配置已改变，请重新启动本程序配置生效!");
	}
}


void CIOServerApp::OnServerLocal()
{
	// TODO: 在此添加命令处理程序代码
	if(m_pAuthorityMgr->m_szOpt.IsEmpty())
	{
		AfxMessageBox("请先登录!");
		return;
	}
	if(m_pAuthorityMgr->m_dwPower != 0xFFFFFFFF)
	{
		AfxMessageBox("权限不够!");
		return;
	}

	CDlgIniSet dlg;
	CString szFile = Utility::GetModulePath();
	szFile += g_szLocalCfgFile;

	CIniTool ini(szFile);
	ini.Read();

	dlg.m_szprj = ini.m_szProjectName;
	dlg.m_bExitPass = ini.m_bExitPass;
	dlg.m_bSaveLog = ini.m_bSaveLog;
	dlg.m_bAutoRun = ini.m_bAutoRun;
	dlg.m_bCache = ini.m_bCashe;
	dlg.m_wHeartPort = ini.m_wheartport;
	dlg.m_szHeartIp = ini.m_szHeartIP;
	dlg.m_nTimeZero = ini.m_nTimeZero;
	dlg.m_nthread = ini.m_nupthreads;
	dlg.m_nLogLevel = ini.m_nLogLevel;
	dlg.m_szTitle = ini.m_szTitle;
	dlg.m_bSkipSameTime = ini.m_bSkipSameTime;
	dlg.m_bWaitTcpAck = ini.m_bWaitTcpAck;

	if(dlg.DoModal() != IDOK)
		return;
	if(	(dlg.m_szprj != ini.m_szProjectName)||
		(dlg.m_bExitPass != ini.m_bExitPass)||
		(dlg.m_bSaveLog != ini.m_bSaveLog)||
		(dlg.m_bAutoRun != ini.m_bAutoRun)||
		(dlg.m_bCache != ini.m_bCashe)||
		(dlg.m_bSkipSameTime != ini.m_bSkipSameTime)||
		(dlg.m_wHeartPort != ini.m_wheartport)||
		(dlg.m_szHeartIp != ini.m_szHeartIP)||
		(dlg.m_nTimeZero != ini.m_nTimeZero)||
		(dlg.m_nLogLevel != ini.m_nLogLevel)||
		(dlg.m_szTitle != ini.m_szTitle)||
		(dlg.m_nthread != ini.m_nupthreads)||
		(dlg.m_bWaitTcpAck != ini.m_bWaitTcpAck))
	{
		ini.m_szProjectName = dlg.m_szprj;
		ini.m_bExitPass = dlg.m_bExitPass;
		ini.m_bSaveLog = dlg.m_bSaveLog;
		ini.m_bAutoRun = dlg.m_bAutoRun;
		ini.m_bCashe = dlg.m_bCache;
		ini.m_bSkipSameTime = dlg.m_bSkipSameTime;
		ini.m_wheartport = dlg.m_wHeartPort;
		ini.m_szHeartIP = dlg.m_szHeartIp;
		ini.m_nTimeZero = dlg.m_nTimeZero;
		ini.m_nupthreads = dlg.m_nthread;
		ini.m_nLogLevel = dlg.m_nLogLevel;
		ini.m_szTitle = dlg.m_szTitle;
		ini.m_bWaitTcpAck = dlg.m_bWaitTcpAck;

		if(ini.Write())
		{
			// 不需要重启
			m_Config.m_bExitPass = dlg.m_bExitPass;
			m_Config.m_nLogLevel = dlg.m_nLogLevel;
			m_pLog->SetAllowLogLevel((DWORD)dlg.m_nLogLevel);

			SetAdditionTitle(ini.m_szTitle);

			AfxMessageBox("静态配置文件已更改，重新启动!");
		}
	}
}

void CIOServerApp::SetAdditionTitle(LPCTSTR lpszTitle)
{
	CString szMainTitle;
	szMainTitle.LoadString(IDR_MAINFRAME);

	CString szTitle;

	// 设置标题
	if (!lpszTitle || !*lpszTitle)
	{
		szTitle = szMainTitle;
	}
	else 
	{
		szTitle.Format(_T("%s - [%s]"), szMainTitle, lpszTitle);
	}

	CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->SetTitle( szTitle );
	pMainFrame->OnUpdateFrameTitle(TRUE);
}

BOOL CIOServerApp::OpenDecVarView(CString szDecName)
{
	if(NULL == m_pDecVarTemplate)
		return FALSE;
	POSITION posDoc = m_pDecVarTemplate->GetFirstDocPosition();
	while(posDoc!=NULL)
	{
		CIOServerDoc* pDoc=(CIOServerDoc *)m_pDecVarTemplate->GetNextDoc(posDoc);
		if(pDoc->m_szDecName == szDecName)
		{
			POSITION posView=pDoc->GetFirstViewPosition();
			if(posView!=NULL)
			{
				CView* pView=pDoc->GetNextView(posView);
				((CMDIChildWnd *)(pView->GetParent()))->ActivateFrame();
				return TRUE;
			}
		}
	}
	CIOServerDoc* pDoc = (CIOServerDoc *)m_pDecVarTemplate->CreateNewDocument();
	if (pDoc == NULL)
	{
		TRACE0("CDocTemplate::CreateNewDocument returned NULL.\n");
		AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC,MB_ICONERROR);
		return FALSE;
	}

	pDoc->m_szDecName = szDecName;
	ASSERT_VALID(pDoc);

	pDoc->SetTitle(szDecName);//文档名称

	BOOL bAutoDelete = pDoc->m_bAutoDelete;
	pDoc->m_bAutoDelete = FALSE;   // don't destroy if something goes wrong

	CFrameWnd* pFrame = m_pDecVarTemplate->CreateNewFrame(pDoc, NULL);

	pDoc->m_bAutoDelete = bAutoDelete;
	if (pFrame == NULL)
	{
		TRACE0("Warning: failed to create new frame.\n");
		return FALSE;     // command failed
	}
	m_pDecVarTemplate->InitialUpdateFrame(pFrame, pDoc);
	pFrame->ShowWindow(SW_SHOWMAXIMIZED);
	return TRUE;
}


void  CIOServerApp::CloseDecVarView(CString szDecName)
{
	POSITION posDocument=m_pDecVarTemplate->GetFirstDocPosition();
	while(posDocument!=NULL)
	{
		CIOServerDoc *pDoc=(CIOServerDoc *)m_pDecVarTemplate->GetNextDoc(posDocument);
		if(pDoc->m_szDecName == szDecName)
		{
			POSITION posView = pDoc->GetFirstViewPosition();
			if(posView!=NULL)
			{
				CView* pView=pDoc->GetNextView(posView);
				((CMDIChildWnd *)(pView->GetParent()))->SendMessage(WM_CLOSE);
				return;
			}
		}
	}
}

int CIOServerApp::GetDrvTags(DWORD dwFlag, CString szDrvName, CDrvTags& DrvTags, int &nNew, int& nUpdate)
{
	if(NULL == m_pDriverMgr)
		return -1;
	int nRet = m_pDriverMgr->GetDrvTags(dwFlag, szDrvName, DrvTags, nNew, nUpdate);
	return nRet;
}


BOOL CIOServerApp::DelTag(CString szDecName, CString szTagName)
{
	if(NULL == m_pDriverMgr)
		return -1;
	int nRet = m_pDriverMgr->DelTag(szDecName, szTagName);
	return 0;
}

BOOL CIOServerApp::GetTagDef(LPCTSTR lpszDecName, int i, T_DRVTAG& tDRVTAG)
{
	if(NULL == m_pDriverMgr)
		return FALSE;
	return m_pDriverMgr->GetDrvTagDef(lpszDecName, i, tDRVTAG);
}
CLog*  CIOServerApp::GetLog(void)
{
	return m_pLog;
}

CString CIOServerApp::GetOperator()
{
	return m_pAuthorityMgr->m_szOpt;
}
CLog* GetLogger()
{
	return theApp.GetLog();
}

long g_GetTimeAdjust()
{
	return theApp.m_Config.m_lAdjustSec;
}

BOOL CIOServerApp::IsSaveLog(void)
{
	return m_Config.m_bSaveLog;
}


BOOL CIOServerApp::IsCanExit(void)
{
	if(theApp.m_Config.m_bExitPass)
	{
		if(!theApp.CheckPower(1))
			return FALSE;
	}
	return TRUE;
}


void CIOServerApp::ExitServer(void)
{
	m_pDriverMgr->ExitServer();
}


BOOL CIOServerApp::GetTagValue(LPCTSTR lpszDecName, LPCTSTR lpszTagName, CVarVal& varValue)
{
	if(m_pDriverMgr)
		return m_pDriverMgr->GetTagValue(lpszDecName, lpszTagName, varValue);
	else
		return FALSE;
}

void CIOServerApp::GetOutFileCfig(BOOL& bUseFileOutput, T_OutFileCfg& tOutFileCfg)
{
	if(m_pDriverMgr)
		m_pDriverMgr->GetOutFileCfig(bUseFileOutput, tOutFileCfg);
}

void CIOServerApp::SetOutFileCfig(BOOL bUseFileOutput, const T_OutFileCfg& tOutFileCfg)
{
	if(m_pDriverMgr)
		m_pDriverMgr->SetOutFileCfig(bUseFileOutput, tOutFileCfg);
}