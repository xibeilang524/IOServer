// UdpTcpCfgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ddclib.h"
#include "UdpTcpCfgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUdpTcpCfgDlg dialog


CUdpTcpCfgDlg::CUdpTcpCfgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUdpTcpCfgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUdpTcpCfgDlg)
	m_nPort = 0;
	m_szParam = _T("");
	//}}AFX_DATA_INIT
}


void CUdpTcpCfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUdpTcpCfgDlg)
	DDX_Control(pDX, IDC_IPADDRESS_IP, m_ctrlIPAddr);
	DDX_Text(pDX, IDC_EDIT_PORT, m_nPort);
	DDX_Text(pDX, IDC_EDIT_PARAM, m_szParam);
	//}}AFX_DATA_MAP
}

LPCTSTR CUdpTcpCfgDlg::GetIP() const
{
	return m_szIP;
}

void CUdpTcpCfgDlg::SetIP(LPCTSTR lpszIP)
{
	m_szIP = lpszIP;
}

BEGIN_MESSAGE_MAP(CUdpTcpCfgDlg, CDialog)
	//{{AFX_MSG_MAP(CUdpTcpCfgDlg)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS_IP, OnFieldchangedIpaddressIp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUdpTcpCfgDlg message handlers

BOOL CUdpTcpCfgDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();	
	
	//m_ctrlIPAddr.SetWindowText(m_szIP);	
	DWORD dwAddress= ntohl( inet_addr(m_szIP));
	m_ctrlIPAddr.SetAddress(dwAddress);
	
	GetDlgItem(IDC_STATIC_PARAM_DISPLAY_NAME)->SetWindowText(CConfigInfo::m_szParamDisplayName);
	GetDlgItem(IDC_STATIC_PARAM_HINT_TEXT)->SetWindowText(CConfigInfo::m_szParamHintText);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUdpTcpCfgDlg::OnFieldchangedIpaddressIp(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	BYTE b1, b2, b3, b4;
	m_ctrlIPAddr.GetAddress(b1,b2,b3,b4);
	m_szIP.Format(_T("%d.%d.%d.%d"), b1, b2, b3, b4);

	*pResult = 0;
}
