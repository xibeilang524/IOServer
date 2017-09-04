// DlgLoadDrvTag.cpp : implementation file
//

#include "stdafx.h"
#include "IOServer.h"
#include "DlgLoadDrvTag.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgLoadDrvTag dialog


CDlgLoadDrvTag::CDlgLoadDrvTag(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLoadDrvTag::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgLoadDrvTag)
	m_bdes = FALSE;
	m_bnew = TRUE;
	m_btype = FALSE;
	m_bunit = FALSE;
	//}}AFX_DATA_INIT
	m_dwflag = 0;
}


void CDlgLoadDrvTag::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgLoadDrvTag)
	DDX_Check(pDX, IDC_CHECK_DES, m_bdes);
	DDX_Check(pDX, IDC_CHECK_NEWTAG, m_bnew);
	DDX_Check(pDX, IDC_CHECK_TYPE, m_btype);
	DDX_Check(pDX, IDC_CHECK_UNIT, m_bunit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgLoadDrvTag, CDialog)
	//{{AFX_MSG_MAP(CDlgLoadDrvTag)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgLoadDrvTag message handlers

void CDlgLoadDrvTag::OnOK() 
{
	// TODO: Add extra validation here
	if(!UpdateData())
		return;
	m_dwflag = 0;
	if(m_bdes)
		m_dwflag += 1;
	if(m_bunit)
		m_dwflag += 2;
	if(m_btype)
		m_dwflag += 4;
	if(m_bnew)
		m_dwflag += 8;
	CDialog::OnOK();
}
