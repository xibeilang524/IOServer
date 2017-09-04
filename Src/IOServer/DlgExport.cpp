// DlgExport.cpp : implementation file
//

#include "stdafx.h"
#include "IOServer.h"
#include "DlgExport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgExport dialog


CDlgExport::CDlgExport(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgExport::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgExport)
	m_nsel = 0;
	//}}AFX_DATA_INIT
	m_sztitle = "导出DDS2标签表";
}


void CDlgExport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExport)
	DDX_Radio(pDX, IDC_RADIO1, m_nsel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgExport, CDialog)
	//{{AFX_MSG_MAP(CDlgExport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgExport message handlers

BOOL CDlgExport::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetWindowText(m_sztitle);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
