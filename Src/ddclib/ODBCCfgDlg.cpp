// ODBCCfgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ddclib.h"
#include "ODBCCfgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CODBCCfgDlg dialog


CODBCCfgDlg::CODBCCfgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CODBCCfgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CODBCCfgDlg)
	m_szDataSource = _T("");
	m_szPsw = _T("");
	m_szID = _T("");
	//}}AFX_DATA_INIT
}


void CODBCCfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CODBCCfgDlg)
	DDX_Text(pDX, IDC_EDIT_DATASOURCE, m_szDataSource);
	DDX_Text(pDX, IDC_EDIT_PSW, m_szPsw);
	DDX_Text(pDX, IDC_EDIT_USERID, m_szID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CODBCCfgDlg, CDialog)
	//{{AFX_MSG_MAP(CODBCCfgDlg)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CODBCCfgDlg message handlers

BOOL CODBCCfgDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	UpdateData(TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CODBCCfgDlg::SetConfig(CConfigInfo &config)
{
	m_szDataSource = config.m_szDataSource;
	m_szID = config.m_szID;
	m_szPsw	= config.m_szPsw;
}

void CODBCCfgDlg::GetConfig(CConfigInfo &config)
{
	config.m_szDataSource = m_szDataSource;
	config.m_szID = m_szID;
	config.m_szPsw	= m_szPsw;
}

void CODBCCfgDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	UpdateData(FALSE);
	CDialog::OnClose();
}
