// DlgTagAttr.cpp : implementation file
//

#include "stdafx.h"
#include "SimdrvEx.h"
#include "DlgTagAttr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTagAttr dialog


CDlgTagAttr::CDlgTagAttr(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTagAttr::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTagAttr)
	m_nDataType = 0;
	m_nSimulateType = 0;
	m_szBaseVal = _T("100");
	m_szDesc = _T("");
	m_fPercent = 10;
	m_szTagName = _T("");
	m_szUnit = _T("");
	//}}AFX_DATA_INIT

	m_bAddTag = FALSE;
}


void CDlgTagAttr::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTagAttr)
	DDX_CBIndex(pDX, IDC_COMBO_DATATYPE, m_nDataType);
	DDX_CBIndex(pDX, IDC_COMBO_SIMULATE_TYPE, m_nSimulateType);
	DDX_Text(pDX, IDC_EDIT_BASEVAL, m_szBaseVal);
	DDX_Text(pDX, IDC_EDIT_DESC, m_szDesc);
	DDX_Text(pDX, IDC_EDIT_PERCENT, m_fPercent);
	DDX_Text(pDX, IDC_EDIT_TAGNAME, m_szTagName);
	DDX_Text(pDX, IDC_EDIT_UNIT, m_szUnit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTagAttr, CDialog)
	//{{AFX_MSG_MAP(CDlgTagAttr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTagAttr message handlers

BOOL CDlgTagAttr::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (!m_bAddTag)
		GetDlgItem(IDC_EDIT_TAGNAME)->EnableWindow(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
