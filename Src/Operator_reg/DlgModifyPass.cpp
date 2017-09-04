// DlgModifyPass.cpp : implementation file
//

#include "stdafx.h"
#include "Operator.h"
#include "DlgModifyPass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgModifyPass dialog


CDlgModifyPass::CDlgModifyPass(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgModifyPass::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgModifyPass)
	m_szName = _T("");
	m_szOldPass = _T("");
	m_szNewPass1 = _T("");
	m_szNewPass2 = _T("");
	//}}AFX_DATA_INIT
}


void CDlgModifyPass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgModifyPass)
	DDX_Text(pDX, IDC_EDIT_NAME, m_szName);
	DDX_Text(pDX, IDC_EDIT_PASS, m_szOldPass);
	DDX_Text(pDX, IDC_EDIT_NEWPASS1, m_szNewPass1);
	DDX_Text(pDX, IDC_EDIT_NEWPASS2, m_szNewPass2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgModifyPass, CDialog)
	//{{AFX_MSG_MAP(CDlgModifyPass)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgModifyPass message handlers

void CDlgModifyPass::OnOK() 
{
	// TODO: Add extra validation here
	if(!UpdateData(TRUE))
		return;
	if(m_szOldPass.IsEmpty())
	{
		AfxMessageBox("«ÎÃÓ–¥æ…√‹¬Î!");
		return;
	}
	if(m_szNewPass1.IsEmpty() || m_szNewPass2.IsEmpty())
	{
		AfxMessageBox("«ÎÃÓ–¥–¬√‹¬Î!");
		return;
	}
	if(m_szNewPass1 != m_szNewPass2)
	{
		AfxMessageBox("–¬√‹¬Î—È÷§¥ÌŒÛ!");
		return;
	}
	CDialog::OnOK();
}
