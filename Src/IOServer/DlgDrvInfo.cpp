// DlgDrvInfo.cpp : implementation file
//

#include "stdafx.h"
#include "IOServer.h"
#include "DlgDrvInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgDrvInfo dialog


CDlgDrvInfo::CDlgDrvInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDrvInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgDrvInfo)
	m_szDec = _T("");
	m_szInfo = _T("");
	m_szWap = _T("");
	//}}AFX_DATA_INIT
}


void CDlgDrvInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDrvInfo)
	DDX_Text(pDX, IDC_EDIT_DEC, m_szDec);
	DDX_Text(pDX, IDC_EDIT_INFO, m_szInfo);
	DDX_Text(pDX, IDC_EDIT_WAP, m_szWap);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgDrvInfo, CDialog)
	//{{AFX_MSG_MAP(CDlgDrvInfo)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDrvInfo message handlers
