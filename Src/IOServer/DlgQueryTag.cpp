// DlgQueryTag.cpp : implementation file
//

#include "stdafx.h"
#include "IOServer.h"
#include "DlgQueryTag.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgQueryTag dialog


CDlgQueryTag::CDlgQueryTag(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgQueryTag::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgQueryTag)
	m_ntype = 1;
	m_sdes = _T("");
	m_sdname = _T("");
	m_sname = _T("");
	m_spname = _T("");
	//}}AFX_DATA_INIT
}


void CDlgQueryTag::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgQueryTag)
	DDX_CBIndex(pDX, IDC_COMBO_TYPE, m_ntype);
	DDX_Text(pDX, IDC_EDIT_DES, m_sdes);
	DDV_MaxChars(pDX, m_sdes, 15);
	DDX_Text(pDX, IDC_EDIT_DNAME, m_sdname);
	DDV_MaxChars(pDX, m_sdname, 15);
	DDX_Text(pDX, IDC_EDIT_NAME, m_sname);
	DDV_MaxChars(pDX, m_sname, 15);
	DDX_Text(pDX, IDC_EDIT_PNAME, m_spname);
	DDV_MaxChars(pDX, m_spname, 15);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgQueryTag, CDialog)
	//{{AFX_MSG_MAP(CDlgQueryTag)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgQueryTag message handlers

void CDlgQueryTag::OnOK() 
{
	// TODO: Add extra validation here
	if(!UpdateData())
		return;
	CDialog::OnOK();
}
