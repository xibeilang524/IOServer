// DlgInport.cpp : implementation file
//

#include "stdafx.h"
#include "IOServer.h"
#include "DlgImport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgImport dialog


CDlgImport::CDlgImport(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgImport::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgImport)
	m_bcomptype = TRUE;
	m_bcompval = TRUE;
	m_bdatatype = TRUE;
	m_bdbtag = TRUE;
	m_bdes = TRUE;
	m_bdrvtag = TRUE;
	m_bexp = TRUE;
	m_bnewtag = TRUE;
	m_bunit = TRUE;
	//}}AFX_DATA_INIT

	m_dwflag = 0x1FF;
}


void CDlgImport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgImport)
	DDX_Check(pDX, IDC_CHECK_COMPTYPE, m_bcomptype);
	DDX_Check(pDX, IDC_CHECK_COMPVAL, m_bcompval);
	DDX_Check(pDX, IDC_CHECK_DATATYPE, m_bdatatype);
	DDX_Check(pDX, IDC_CHECK_DBTAG, m_bdbtag);
	DDX_Check(pDX, IDC_CHECK_DES, m_bdes);
	DDX_Check(pDX, IDC_CHECK_DRVTAG, m_bdrvtag);
	DDX_Check(pDX, IDC_CHECK_EXP, m_bexp);
	DDX_Check(pDX, IDC_CHECK_NEWTAG, m_bnewtag);
	DDX_Check(pDX, IDC_CHECK_UNIT, m_bunit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgImport, CDialog)
	//{{AFX_MSG_MAP(CDlgImport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgImport message handlers

void CDlgImport::OnOK() 
{
	// TODO: Add extra validation here
	if(!UpdateData())
		return;
	BOOL b[9];
	b[0] = m_bdbtag;
	b[1] = m_bdes;
	b[2] = m_bunit;
	b[3] = m_bdatatype;
	b[4] = m_bdrvtag;
	b[5] = m_bcomptype;
	b[6] = m_bcompval;
	b[7] = m_bexp;
	b[8] = m_bnewtag;

	m_dwflag = 0;
	for(int i=0;i<9;i++)
	{
		if(b[i])
			m_dwflag |= (0x00000001 << i);
	}
	CDialog::OnOK();
}
