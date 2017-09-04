// DlgTagAttr.cpp : implementation file
//

#include "stdafx.h"
#include "IOServer.h"
#include "DlgTagAttr.h"

#include <htmlhelp.h>
#pragma comment( lib, "htmlhelp.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTagAttr dialog


CDlgTagAttr::CDlgTagAttr(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTagAttr::IDD, pParent)
	, m_szDrvName("")
{
	//{{AFX_DATA_INIT(CDlgTagAttr)
	m_ncomptype = 1;
	m_ndatatype = 2;
	m_szcompval = _T("0.1");
	m_szdes = _T("");
	m_sztagdbname = _T("");
	m_sztagdrvname = _T("");
	m_sztagname = _T("");
	m_szunit = _T("");
	m_szexp = _T("");
	//}}AFX_DATA_INIT

	m_bAdd  = TRUE;
	//m_pdrv = NULL;
}


void CDlgTagAttr::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTagAttr)
	DDX_Control(pDX, IDC_COMBO_COMPTYPE, m_cbptype);
	DDX_CBIndex(pDX, IDC_COMBO_COMPTYPE, m_ncomptype);
	DDX_CBIndex(pDX, IDC_COMBO_TYPE, m_ndatatype);
	DDX_Text(pDX, IDC_EDIT_COMPVAL, m_szcompval);
	DDX_Text(pDX, IDC_EDIT_DES, m_szdes);
	DDV_MaxChars(pDX, m_szdes, 79);
	DDX_Text(pDX, IDC_EDIT_TAGDBNAME, m_sztagdbname);
	DDV_MaxChars(pDX, m_sztagdbname, 79);
	DDX_Text(pDX, IDC_EDIT_TAGDRVNAME, m_sztagdrvname);
	DDV_MaxChars(pDX, m_sztagdrvname, 79);
	DDX_Text(pDX, IDC_EDIT_TAGNAME, m_sztagname);
	DDV_MaxChars(pDX, m_sztagname, 79);
	DDX_Text(pDX, IDC_EDIT_UNIT, m_szunit);
	DDV_MaxChars(pDX, m_szunit, 15);
	DDX_Text(pDX, IDC_EDIT_EXP, m_szexp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTagAttr, CDialog)
	//{{AFX_MSG_MAP(CDlgTagAttr)
	ON_BN_CLICKED(IDC_BUTTON_SELTAG, OnButtonSeltag)
	ON_CBN_SELCHANGE(IDC_COMBO_COMPTYPE, OnSelchangeComboComptype)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_HELP, &CDlgTagAttr::OnBnClickedButtonHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTagAttr message handlers

BOOL CDlgTagAttr::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if(m_bAdd)
		SetWindowText("添加标签");
	else
	{
		SetWindowText("修改标签");
		GetDlgItem(IDC_EDIT_TAGNAME)->EnableWindow(FALSE);
	}
	if(m_ncomptype == 0)
		GetDlgItem(IDC_EDIT_COMPVAL)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_EDIT_COMPVAL)->EnableWindow(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTagAttr::OnOK() 
{
	// TODO: Add extra validation here
	if(!UpdateData())
		return;


	if(m_sztagname.IsEmpty() || m_sztagdrvname.IsEmpty() || m_sztagdbname.IsEmpty())
	{
		AfxMessageBox(IDS_TAG_MUST_RIGHT);
		return;
	}
	if(!m_szcompval.IsEmpty())
	{
		if(!IsDigtal(m_szcompval))
		{
			AfxMessageBox(IDS_COMPRESS_NUMBER);
			return;
		}
	}

	m_szexp.Trim();
	UpdateData(FALSE);

	CDialog::OnOK();
}

void CDlgTagAttr::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}
#include "DlgSelDrvTag.h"
void CDlgTagAttr::OnButtonSeltag() 
{
	// TODO: Add your control notification handler code here
	CDlgSelDrvTag dlg;
	dlg.InitTag(m_szDrvName);
	if(dlg.DoModal() != IDOK)
		return;
	SetDlgItemText(IDC_EDIT_TAGDRVNAME,dlg.m_szTagName);
}

void CDlgTagAttr::OnSelchangeComboComptype() 
{
	// TODO: Add your control notification handler code here
	int n = m_cbptype.GetCurSel();
	if(n == 0)
		GetDlgItem(IDC_EDIT_COMPVAL)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_EDIT_COMPVAL)->EnableWindow(TRUE);
}

void CDlgTagAttr::OnBnClickedButtonHelp()
{
	CString szChmFile = theApp.m_szAppDir + _T("script.chm");
	::HtmlHelp(m_hWnd, szChmFile, HH_DISPLAY_TOPIC, 0);
}
