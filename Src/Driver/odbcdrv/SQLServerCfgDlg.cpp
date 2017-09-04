// SQLServerCfgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "odbcdrv.h"
#include "SQLServerCfgDlg.h"
#include "DbConnStrDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSQLServerCfgDlg dialog


CSQLServerCfgDlg::CSQLServerCfgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSQLServerCfgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSQLServerCfgDlg)
	m_szDatabase = _T("");
	m_szInstance = _T("");
	m_szPassword = _T("");
	m_nPort = 0;
	m_szServer = _T("");
	m_szUser = _T("");
	//}}AFX_DATA_INIT
}


void CSQLServerCfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSQLServerCfgDlg)
	DDX_Text(pDX, IDC_EDIT_DATABASE, m_szDatabase);
	DDX_Text(pDX, IDC_EDIT_INST, m_szInstance);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_szPassword);
	DDX_Text(pDX, IDC_EDIT_PORT, m_nPort);
	DDX_Text(pDX, IDC_EDIT_SERVER, m_szServer);
	DDX_Text(pDX, IDC_EDIT_USER, m_szUser);
	//}}AFX_DATA_MAP
}

void CSQLServerCfgDlg::GetConnectString(CConnStringParser &parser)
{
	UpdateData();
	
	//parser.SetDBType(DBT_MSSQL);
	parser.SetConnectString(m_szConnStr); // 为了支持用户手动添加连接字符串参数
	if (!parser.IsSuccess())
		parser.SetDBType(DBT_MYSQL);
	
	parser.SetValue(_T("Server"), m_szServer);
	parser.SetValue(_T("Port"), m_nPort);
	parser.SetValue(_T("INSTANCE"), m_szInstance);
	parser.SetValue(_T("Database"), m_szDatabase);
	parser.SetValue(_T("UID"), m_szUser);
	parser.SetValue(_T("PWD"), m_szPassword);
	parser.GetConnectString();
}

void CSQLServerCfgDlg::SetConnectString(CConnStringParser &parser)
{
	if (parser.GetDBType() != DBT_MSSQL)
	{
		CConnStringParser temp;
		temp.SetDBType(DBT_MSSQL);
		
		m_szServer = temp.GetValue(_T("Server"));
		m_nPort = _ttoi(temp.GetValue(_T("Port")));
		m_szInstance = temp.GetValue(_T("INSTANCE"));
		m_szDatabase = temp.GetValue(_T("Database"));
		m_szUser = temp.GetValue(_T("UID"));
		m_szPassword = temp.GetValue(_T("PWD"));
		
		m_szConnStr = temp.GetConnectString();
	}
	else {
		m_szConnStr = parser.GetConnectString();
		
		m_szServer = parser.GetValue(_T("Server"));
		m_nPort = _ttoi(parser.GetValue(_T("Port")));
		m_szInstance = parser.GetValue(_T("INSTANCE"));
		m_szDatabase = parser.GetValue(_T("Database"));
		m_szUser = parser.GetValue(_T("UID"));
		m_szPassword = parser.GetValue(_T("PWD"));
	}
	
	if (GetSafeHwnd())
		UpdateData(FALSE);
}

BEGIN_MESSAGE_MAP(CSQLServerCfgDlg, CDialog)
	//{{AFX_MSG_MAP(CSQLServerCfgDlg)
	ON_COMMAND(IDOK, OnOK)
	ON_BN_CLICKED(IDC_BUTTON_CONNSTR, OnButtonConnstr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSQLServerCfgDlg::OnOK()
{
	// 用于解决在编辑框中按回车后该窗口自动关闭的BUG
}

/////////////////////////////////////////////////////////////////////////////
// CSQLServerCfgDlg message handlers

void CSQLServerCfgDlg::OnButtonConnstr() 
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
