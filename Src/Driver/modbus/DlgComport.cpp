// DlgComport.cpp : implementation file
//

#include "stdafx.h"
#include "modbus.h"
#include "DlgComport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgComport dialog


CDlgComport::CDlgComport(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgComport::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgComport)
	m_szbt = _T("");
	m_nck = -1;
	m_szdata = _T("");
	m_szport = _T("");
	m_nstop = 0;
	m_nComType = 0;
	m_nSvrPort = 502;
	m_bTcpCrc16 = FALSE;
	m_ntimeout = 200;
	m_nModType = 0;
	m_bSaveLog = FALSE;
	m_nUpdateRate = 1000;
	//}}AFX_DATA_INIT
}


void CDlgComport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgComport)
	DDX_CBString(pDX, IDC_COMBO_BT, m_szbt);
	DDX_CBIndex(pDX, IDC_COMBO_CK, m_nck);
	DDX_CBString(pDX, IDC_COMBO_DATA, m_szdata);
	DDX_CBString(pDX, IDC_COMBO_PORT, m_szport);
	DDX_CBIndex(pDX, IDC_COMBO_STOP, m_nstop);
	DDX_CBIndex(pDX, IDC_COMBO_COMTYPE, m_nComType);
	DDX_Text(pDX, IDC_EDIT_TCPPORT, m_nSvrPort);
	DDX_Check(pDX, IDC_CHECK_TCPCRC16, m_bTcpCrc16);
	DDX_Text(pDX, IDC_EDIT_TIMEOUT, m_ntimeout);
	DDV_MinMaxInt(pDX, m_ntimeout, 100, 1000);
	DDX_Radio(pDX, IDC_RADIO1, m_nModType);
	DDX_Check(pDX, IDC_CHECK_SAVELOG, m_bSaveLog);
	DDX_Text(pDX, IDC_EDIT_UPDATE_RATE, m_nUpdateRate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgComport, CDialog)
	//{{AFX_MSG_MAP(CDlgComport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgComport message handlers

BOOL CDlgComport::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetDlgItemText(IDC_IPADDRESS1,m_szip);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgComport::OnOK() 
{
	// TODO: Add extra validation here
	GetDlgItemText(IDC_IPADDRESS1,m_szip);
	CDialog::OnOK();
}
