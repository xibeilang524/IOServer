// OracleCfgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "odbcdrv.h"
#include "OracleCfgDlg.h"
#include "DbConnStrDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COracleCfgDlg dialog


COracleCfgDlg::COracleCfgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COracleCfgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COracleCfgDlg)
	m_szPassword = _T("");
	m_szUser = _T("");
	m_szServer = _T("");
	//}}AFX_DATA_INIT
}


void COracleCfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COracleCfgDlg)
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_szPassword);
	DDX_Text(pDX, IDC_EDIT_USER, m_szUser);
	DDX_CBString(pDX, IDC_COMBO_SERVER, m_szServer);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COracleCfgDlg, CDialog)
	//{{AFX_MSG_MAP(COracleCfgDlg)
	ON_COMMAND(IDOK, OnOK)
	ON_BN_CLICKED(IDC_BUTTON_CONNSTR, OnButtonConnstr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void COracleCfgDlg::OnOK()
{
	// 用于解决在编辑框中按回车后该窗口自动关闭的BUG
}

void COracleCfgDlg::GetConnectString(CConnStringParser &parser)
{
	UpdateData();
	
	//parser.SetDBType(DBT_MYSQL);
	parser.SetConnectString(m_szConnStr);
	if (!parser.IsSuccess())
		parser.SetDBType(DBT_ORACLE);
	
	parser.SetValue(_T("Server"), m_szServer);
	parser.SetValue(_T("UID"), m_szUser);
	parser.SetValue(_T("PWD"), m_szPassword);
	parser.GetConnectString();
}

void COracleCfgDlg::SetConnectString(CConnStringParser &parser)
{
	if (parser.GetDBType() != DBT_ORACLE)
	{
		CConnStringParser temp;
		temp.SetDBType(DBT_ORACLE);
		
		m_szServer = temp.GetValue(_T("Server"));
		m_szUser = temp.GetValue(_T("UID"));
		m_szPassword = temp.GetValue(_T("PWD"));
		
		m_szConnStr = temp.GetConnectString();
	}
	else {
		m_szConnStr = parser.GetConnectString(); // 保存原连接字符串
		
		m_szServer = parser.GetValue(_T("Server"));
		m_szUser = parser.GetValue(_T("UID"));
		m_szPassword = parser.GetValue(_T("PWD"));
	}
	
	if (GetSafeHwnd())
		UpdateData(FALSE);	
}
/////////////////////////////////////////////////////////////////////////////
// COracleCfgDlg message handlers

void COracleCfgDlg::OnButtonConnstr() 
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

BOOL COracleCfgDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
		
	CComboBox *pBox = (CComboBox*)GetDlgItem(IDC_COMBO_SERVER);
	CStringArray arrServers;
	::GetOracleServers(arrServers);
	for(int i = 0; i < arrServers.GetSize(); i++)
		pBox->AddString(arrServers[i]);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
