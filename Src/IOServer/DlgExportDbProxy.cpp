// DlgExportDbProxy.cpp : implementation file
//

#include "stdafx.h"
#include "IOServer.h"
#include "DlgExportDbProxy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgExportDbProxy dialog


CDlgExportDbProxy::CDlgExportDbProxy(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgExportDbProxy::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgExportDbProxy)
	m_szdbname = _T("db");
	m_nsel = 0;
	//}}AFX_DATA_INIT
}


void CDlgExportDbProxy::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExportDbProxy)
	DDX_Text(pDX, IDC_EDIT_DBNAME, m_szdbname);
	DDV_MaxChars(pDX, m_szdbname, 15);
	DDX_Radio(pDX, IDC_RADIO1, m_nsel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgExportDbProxy, CDialog)
	//{{AFX_MSG_MAP(CDlgExportDbProxy)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgExportDbProxy message handlers
