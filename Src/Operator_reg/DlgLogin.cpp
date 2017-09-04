// DlgLogin.cpp : implementation file
//

#include "stdafx.h"
#include "Operator.h"
#include "DlgLogin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgLogin dialog


CDlgLogin::CDlgLogin(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLogin::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgLogin)
	m_szName = _T("");
	m_szPass = _T("");
	//}}AFX_DATA_INIT
}


void CDlgLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgLogin)
	DDX_Text(pDX, IDC_EDIT_NAME, m_szName);
	DDX_Text(pDX, IDC_EDIT_PASS, m_szPass);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgLogin, CDialog)
	//{{AFX_MSG_MAP(CDlgLogin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgLogin message handlers

void CDlgLogin::OnOK() 
{
	// TODO: Add extra validation here
	if(!UpdateData(TRUE))
		return;
	if(m_szName.IsEmpty())
	{
		AfxMessageBox("«ÎÃÓ–¥’À∫≈!");
		return;
	}
	if(m_szPass.IsEmpty())
	{
		AfxMessageBox("«ÎÃÓ–¥√‹¬Î!");
		return;
	}
	CDialog::OnOK();
}
