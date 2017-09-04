// DB2CfgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "odbcdrv.h"
#include "DB2CfgDlg.h"
#include "DbConnStrDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDB2CfgDlg dialog


CDB2CfgDlg::CDB2CfgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDB2CfgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDB2CfgDlg)
	m_szDatabase = _T("");
	m_szPassword = _T("");
	m_nPort = 0;
	m_szServer = _T("");
	m_szUser = _T("");
	//}}AFX_DATA_INIT
}


void CDB2CfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDB2CfgDlg)
	DDX_Text(pDX, IDC_EDIT_DATABASE, m_szDatabase);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_szPassword);
	DDX_Text(pDX, IDC_EDIT_PORT, m_nPort);
	DDX_Text(pDX, IDC_EDIT_SERVER, m_szServer);
	DDX_Text(pDX, IDC_EDIT_USER, m_szUser);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDB2CfgDlg, CDialog)
	//{{AFX_MSG_MAP(CDB2CfgDlg)
	ON_COMMAND(IDOK, OnOK)
	ON_BN_CLICKED(IDC_BUTTON_CONNSTR, OnButtonConnstr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDB2CfgDlg::OnOK()
{
	// 用于解决在编辑框中按回车后该窗口自动关闭的BUG
}

void CDB2CfgDlg::GetConnectString(CConnStringParser &parser)
{
	UpdateData();
	
	parser.SetConnectString(m_szConnStr);
	if (!parser.IsSuccess())
		parser.SetDBType(DBT_DB2);
	
	parser.SetValue(_T("HostName"), m_szServer);
	parser.SetValue(_T("Port"), m_nPort);
	parser.SetValue(_T("Database"), m_szDatabase);
	parser.SetValue(_T("UID"), m_szUser);
	parser.SetValue(_T("PWD"), m_szPassword);
	parser.GetConnectString();	
}

void CDB2CfgDlg::SetConnectString(CConnStringParser &parser)
{
	if (parser.GetDBType() != DBT_DB2)
	{
		CConnStringParser temp;
		temp.SetDBType(DBT_DB2);
		
		m_szServer = temp.GetValue(_T("HostName"));
		m_nPort = _ttoi(temp.GetValue(_T("Port")));
		m_szDatabase = temp.GetValue(_T("Database"));
		m_szUser = temp.GetValue(_T("UID"));
		m_szPassword = temp.GetValue(_T("PWD"));
		
		m_szConnStr = temp.GetConnectString();
	}
	else {
		m_szConnStr = parser.GetConnectString(); // 保存原连接字符串
		
		m_szServer = parser.GetValue(_T("HostName"));
		m_nPort = _ttoi(parser.GetValue(_T("Port")));
		m_szDatabase = parser.GetValue(_T("Database"));
		m_szUser = parser.GetValue(_T("UID"));
		m_szPassword = parser.GetValue(_T("PWD"));
	}
	
	if (GetSafeHwnd())
		UpdateData(FALSE);	
}

/////////////////////////////////////////////////////////////////////////////
// CDB2CfgDlg message handlers

void CDB2CfgDlg::OnButtonConnstr() 
{
	CDbConnStrDlg dlg;
	CConnStringParser parser;
	GetConnectString(parser);
	
	dlg.m_szConnStr = parser.GetConnectString();
	
	if (dlg.DoModal() == IDOK)
	{
		parser.SetConnectString(dlg.m_szConnStr);
		SetConnectString(parser);
	}
}
