// SybaseCfgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "odbcdrv.h"
#include "SybaseCfgDlg.h"
#include "DbConnStrDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSybaseCfgDlg dialog


CSybaseCfgDlg::CSybaseCfgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSybaseCfgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSybaseCfgDlg)
	m_szPassword = _T("");
	m_szServer = _T("");
	m_szUser = _T("");
	//}}AFX_DATA_INIT
}


void CSybaseCfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSybaseCfgDlg)
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_szPassword);
	DDX_Text(pDX, IDC_EDIT_SERVER, m_szServer);
	DDX_Text(pDX, IDC_EDIT_USER, m_szUser);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSybaseCfgDlg, CDialog)
	//{{AFX_MSG_MAP(CSybaseCfgDlg)
	ON_COMMAND(IDOK, OnOK)
	ON_BN_CLICKED(IDC_BUTTON_CONNSTR, OnButtonConnstr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSybaseCfgDlg::OnOK()
{
	// 用于解决在编辑框中按回车后该窗口自动关闭的BUG
}

void CSybaseCfgDlg::GetConnectString(CConnStringParser &parser)
{
	UpdateData();
	
	//parser.SetDBType(DBT_MYSQL);
	parser.SetConnectString(m_szConnStr);
	if (!parser.IsSuccess())
		parser.SetDBType(m_nDbType);
	
	parser.SetValue(_T("Srvr"), m_szServer);
	parser.SetValue(_T("UID"), m_szUser);
	parser.SetValue(_T("PWD"), m_szPassword);
	parser.GetConnectString();
}

void CSybaseCfgDlg::SetConnectString(CConnStringParser &parser)
{
	if (parser.GetDBType() != m_nDbType)
	{
		CConnStringParser temp;
		temp.SetDBType(m_nDbType);
		
		m_szServer = temp.GetValue(_T("Srvr"));
		m_szUser = temp.GetValue(_T("UID"));
		m_szPassword = temp.GetValue(_T("PWD"));
		
		m_szConnStr = temp.GetConnectString();
	}
	else {
		m_szConnStr = parser.GetConnectString(); // 保存原连接字符串
		
		m_szServer = parser.GetValue(_T("Srvr"));
		m_szUser = parser.GetValue(_T("UID"));
		m_szPassword = parser.GetValue(_T("PWD"));
	}
	
	if (GetSafeHwnd())
		UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
// CSybaseCfgDlg message handlers

void CSybaseCfgDlg::OnButtonConnstr() 
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
