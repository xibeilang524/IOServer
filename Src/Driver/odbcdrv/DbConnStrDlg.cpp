// DbConnStrDlg.cpp : implementation file
//

#include "stdafx.h"
#include "odbcdrv.h"
#include "DbConnStrDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDbConnStrDlg dialog


CDbConnStrDlg::CDbConnStrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDbConnStrDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDbConnStrDlg)
	m_szConnStr = _T("");
	//}}AFX_DATA_INIT
}


void CDbConnStrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDbConnStrDlg)
	DDX_Text(pDX, IDC_EDIT_CONNSTR, m_szConnStr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDbConnStrDlg, CDialog)
	//{{AFX_MSG_MAP(CDbConnStrDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDbConnStrDlg message handlers
