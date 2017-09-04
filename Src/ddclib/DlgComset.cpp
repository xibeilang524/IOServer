// DlgComset.cpp : implementation file
//

#include "stdafx.h"
#include "ddclib.h"
#include "DlgComset.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef struct tagComTypeItem {
	WORD wType;
	TCHAR szTypeName[16];
}COMTYPEITEM, *LPCOMTYPEITEM;

static COMTYPEITEM ComTypeList[] = {
	{COM_TYPE_COM,		_T("COM")},
	{COM_TYPE_UDP,		_T("UDP Client")},
	{COM_TYPE_UDPSERV,	_T("UDP Server")},
	{COM_TYPE_TCP,		_T("TCP Client")},
	{COM_TYPE_TCPSERV,	_T("TCP Server")},
	{COM_TYPE_DB,       _T("ODBC数据源")},
	{COM_TYPE_FILE,     _T("文件格式")}
};

void OpenHelpFile();

/////////////////////////////////////////////////////////////////////////////
// CDlgComset dialog


CDlgComset::CDlgComset(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgComset::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgComset)
	m_nReadTimeout = 0;
	m_nUpdateSec = 0;
	m_nWriteTimeout = 0;
	m_bDebug = FALSE;
	m_bLocalTime = FALSE;
	m_nTimeBias = 0;
	//}}AFX_DATA_INIT

	m_wComTypes = COM_TYPE_COM | COM_TYPE_UDP | COM_TYPE_UDPSERV | COM_TYPE_TCP |
		COM_TYPE_TCPSERV | COM_TYPE_DB | COM_TYPE_FILE;
}


void CDlgComset::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgComset)
	DDX_Control(pDX, IDC_TAB_COMCONFIG, m_ctrlTab);
	DDX_Control(pDX, IDC_COMBO_COMTYPE, m_ctrlComTypes);
	DDX_Text(pDX, IDC_EDIT_READTIMEOUT, m_nReadTimeout);
	DDX_Text(pDX, IDC_EDIT_UPDATESEC, m_nUpdateSec);
	DDX_Text(pDX, IDC_EDIT_WRITETIMEOUT, m_nWriteTimeout);
	DDX_Check(pDX, IDC_CHECK_DEBUG, m_bDebug);
	DDX_Check(pDX, IDC_CHECK_LOCALTIME, m_bLocalTime);
	DDX_Text(pDX, IDC_EDIT_TIMEBIAS, m_nTimeBias);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgComset, CDialog)
	//{{AFX_MSG_MAP(CDlgComset)
	ON_CBN_SELCHANGE(IDC_COMBO_COMTYPE, OnSelchangeComboComtype)
	ON_BN_CLICKED(IDC_BUTTON_HELP, OnButtonHelp)
	ON_BN_CLICKED(IDC_BUTTON_TEST, OnButtonTest)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDlgComset::SetConfig(CConfigInfo &config)
{
	m_nReadTimeout = config.m_nReadTimeout;
	m_nWriteTimeout = config.m_nWriteTimeout;
	m_nUpdateSec = config.m_nUpdateSec;
	m_wComType = config.m_wComType;
	m_bDebug = config.m_bDebug;
	m_bLocalTime = config.m_bLocalTime;
	m_nTimeBias	= config.m_nTimeBias;

	m_dlgUdpTcpCfg.m_nPort = config.m_wPort;
	m_dlgUdpTcpCfg.SetIP(config.m_szIP);
	m_dlgUdpTcpCfg.m_szParam = config.m_szTcpUdpParam;

	m_dlgSerialCfg.SetConfig(config);

	m_dlgODBCCfg.m_szDataSource = config.m_szDataSource;
	m_dlgODBCCfg.m_szPsw = config.m_szPsw;
	m_dlgODBCCfg.m_szID = config.m_szID;

	m_dlgFileCfg.m_szFileName = config.m_szFileName;
	m_dlgFileCfg.m_bAutoDeleteFile = config.m_bAutoDeleteFile;
	m_dlgFileCfg.m_szFileParam = config.m_szFileParam;
}

void CDlgComset::GetConfig(CConfigInfo &config)
{
	config.m_nReadTimeout = m_nReadTimeout;
	config.m_nWriteTimeout = m_nWriteTimeout;
	config.m_nUpdateSec = m_nUpdateSec;
	config.m_wComType = m_wComType;
	config.m_bDebug = m_bDebug;
	config.m_bLocalTime = m_bLocalTime;
	config.m_nTimeBias	= m_nTimeBias;

	config.m_wPort = (WORD)m_dlgUdpTcpCfg.m_nPort;
	config.m_szIP = m_dlgUdpTcpCfg.GetIP();
	config.m_szTcpUdpParam = m_dlgUdpTcpCfg.m_szParam;

	m_dlgSerialCfg.GetConfig(config);

	config.m_szDataSource = m_dlgODBCCfg.m_szDataSource;
	config.m_szPsw = m_dlgODBCCfg.m_szPsw;
	config.m_szID = m_dlgODBCCfg.m_szID;

	config.m_szFileName = m_dlgFileCfg.m_szFileName;
	config.m_bAutoDeleteFile = m_dlgFileCfg.m_bAutoDeleteFile;
	config.m_szFileParam = m_dlgFileCfg.m_szFileParam;
}
/////////////////////////////////////////////////////////////////////////////
// CDlgComset message handlers

BOOL CDlgComset::OnInitDialog() 
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_BUTTON_TEST)->EnableWindow(m_pDrvObj->CanTestConnect());
	
	m_ctrlComTypes.ResetContent();
	TRACE("CDlgComset OnInitDialog %d\n", m_wComTypes);
	for(int i = 0; i < sizeof(ComTypeList) / sizeof(COMTYPEITEM); i++)
	{
		if ((m_wComTypes & ComTypeList[i].wType) == ComTypeList[i].wType)
			m_ctrlComTypes.AddString(ComTypeList[i].szTypeName);
//		TRACE("m_wComTypes=%d %s\n",m_wComTypes,ComTypeList[i].szTypeName);

		if (m_wComType == ComTypeList[i].wType)
			m_ctrlComTypes.SetCurSel(m_ctrlComTypes.GetCount() - 1);
	}
	// 如果无默认选择协议，则选择协议列表中的第一种协议作为默认协议
	if (m_ctrlComTypes.GetCurSel() == -1)
	{
		m_ctrlComTypes.SetCurSel(0);
	}

	m_ctrlTab.AddPage("串行口配置", &m_dlgSerialCfg, IDD_DIALOG_SERIAL);
	m_ctrlTab.AddPage("UDP/TCP配置", &m_dlgUdpTcpCfg, IDD_DIALOG_UDPTCP);
	m_ctrlTab.AddPage("ODBC配置", &m_dlgODBCCfg, IDD_DIALOG_ODBC);
	m_ctrlTab.AddPage("文件配置", &m_dlgFileCfg, IDD_DIALOG_FILE);

	m_ctrlTab.SetHideTab(TRUE);
	m_ctrlTab.Show();

	OnSelchangeComboComtype();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgComset::OnSelchangeComboComtype() 
{
	CString szComTypeName;
	m_ctrlComTypes.GetWindowText(szComTypeName);
	if (szComTypeName == ComTypeList[0].szTypeName) // 选择了串行口通讯方式
	{
		m_ctrlTab.SetCurSel(0);
	}
	else if (szComTypeName == ComTypeList[1].szTypeName ||
			 szComTypeName == ComTypeList[2].szTypeName ||
			 szComTypeName == ComTypeList[3].szTypeName ||
			 szComTypeName == ComTypeList[4].szTypeName)
	{
		m_ctrlTab.SetCurSel(1);
	}
	else if (szComTypeName == ComTypeList[5].szTypeName)
		m_ctrlTab.SetCurSel(2);
	else
		m_ctrlTab.SetCurSel(3);
	
	for(int i = 0; i < sizeof(ComTypeList) / sizeof(COMTYPEITEM); i++)
	{
		if (szComTypeName == ComTypeList[i].szTypeName)		
			m_wComType = ComTypeList[i].wType;
	}

	switch(m_wComType)
	{
	case COM_TYPE_UDP:
	case COM_TYPE_TCP:
		m_dlgUdpTcpCfg.m_ctrlIPAddr.EnableWindow(TRUE);
		break;
	case COM_TYPE_UDPSERV:
	case COM_TYPE_TCPSERV:
		m_dlgUdpTcpCfg.m_ctrlIPAddr.EnableWindow(FALSE);
		break;
	}
}

void CDlgComset::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();

	if (COM_TYPE_COM == m_wComType)
		m_dlgSerialCfg.UpdateData();
	else if (COM_TYPE_TCP == m_wComType ||
			 COM_TYPE_TCPSERV == m_wComType ||
			 COM_TYPE_UDP == m_wComType ||
			 COM_TYPE_UDPSERV == m_wComType)
		m_dlgUdpTcpCfg.UpdateData();
	else if (COM_TYPE_DB == m_wComType)
		m_dlgODBCCfg.UpdateData();
	else
		m_dlgFileCfg.UpdateData();

	CDialog::OnOK();
}

void CDlgComset::OnButtonHelp() 
{
	OpenHelpFile();
}

void CDlgComset::OnButtonTest() 
{
	CConfigInfo config;
	GetConfig(config);
	m_pDrvObj->TestConnect(config);
}
