// MySQLCfgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "odbcdrv.h"
#include "MySQLCfgDlg.h"
#include "DbConnStrDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMySQLCfgDlg dialog


CMySQLCfgDlg::CMySQLCfgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMySQLCfgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMySQLCfgDlg)
	m_szDatabase = _T("");
	m_szPassword = _T("");
	m_nPort = 0;
	m_szServer = _T("");
	m_szUser = _T("");
	//}}AFX_DATA_INIT
}


void CMySQLCfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMySQLCfgDlg)
	DDX_Text(pDX, IDC_EDIT_DATABASE, m_szDatabase);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_szPassword);
	DDX_Text(pDX, IDC_EDIT_PORT, m_nPort);
	DDX_Text(pDX, IDC_EDIT_SERVER, m_szServer);
	DDX_Text(pDX, IDC_EDIT_USER, m_szUser);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMySQLCfgDlg, CDialog)
	//{{AFX_MSG_MAP(CMySQLCfgDlg)
	ON_COMMAND(IDOK, OnOK)
	ON_BN_CLICKED(IDC_BUTTON_CONNSTR, OnButtonConnstr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CMySQLCfgDlg::OnOK()
{
	// 用于解决在编辑框中按回车后该窗口自动关闭的BUG
}

void CMySQLCfgDlg::GetConnectString(CConnStringParser &parser)
{
	UpdateData();
	
	//parser.SetDBType(DBT_MYSQL);
	parser.SetConnectString(m_szConnStr);
	if (!parser.IsSuccess())
		parser.SetDBType(DBT_MYSQL);
	
	parser.SetValue(_T("Server"), m_szServer);
	parser.SetValue(_T("Port"), m_nPort);
	parser.SetValue(_T("Database"), m_szDatabase);
	parser.SetValue(_T("UID"), m_szUser);
	parser.SetValue(_T("PWD"), m_szPassword);
	parser.GetConnectString();
}

void CMySQLCfgDlg::SetConnectString(CConnStringParser &parser)
{
	if (parser.GetDBType() != DBT_MYSQL)
	{
		CConnStringParser temp;
		temp.SetDBType(DBT_MYSQL);
		
		m_szServer = temp.GetValue(_T("Server"));
		m_nPort = _ttoi(temp.GetValue(_T("Port")));
		m_szDatabase = temp.GetValue(_T("Database"));
		m_szUser = temp.GetValue(_T("UID"));
		m_szPassword = temp.GetValue(_T("PWD"));
		
		m_szConnStr = temp.GetConnectString();
	}
	else {
		m_szConnStr = parser.GetConnectString(); // 保存原连接字符串
		
		m_szServer = parser.GetValue(_T("Server"));
		m_nPort = _ttoi(parser.GetValue(_T("Port")));
		m_szDatabase = parser.GetValue(_T("Database"));
		m_szUser = parser.GetValue(_T("UID"));
		m_szPassword = parser.GetValue(_T("PWD"));
	}
	
	if (GetSafeHwnd())
		UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
// CMySQLCfgDlg message handlers

void CMySQLCfgDlg::OnButtonConnstr() 
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
