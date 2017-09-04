// SerialCfgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ddclib.h"
#include "SerialCfgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSerialCfgDlg dialog


CSerialCfgDlg::CSerialCfgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSerialCfgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSerialCfgDlg)
	m_nBaudRate = -1;
	m_nByteSize = -1;
	m_nComPort = -1;
	m_nParity = -1;
	m_nStopBits = -1;
	m_szParam = _T("");
	//}}AFX_DATA_INIT
}


void CSerialCfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSerialCfgDlg)
	DDX_Control(pDX, IDC_COMBO_STOPBITS, m_ctrlStopBits);
	DDX_Control(pDX, IDC_COMBO_PARITY, m_ctrlParity);
	DDX_Control(pDX, IDC_COMBO_COMPORT, m_ctrlComPort);
	DDX_Control(pDX, IDC_COMBO_BYTESIZE, m_ctrlByteSize);
	DDX_Control(pDX, IDC_COMBO_BAUDRATE, m_ctrlBaudRate);
	DDX_CBIndex(pDX, IDC_COMBO_BAUDRATE, m_nBaudRate);
	DDX_CBIndex(pDX, IDC_COMBO_BYTESIZE, m_nByteSize);
	DDX_CBIndex(pDX, IDC_COMBO_COMPORT, m_nComPort);
	DDX_CBIndex(pDX, IDC_COMBO_PARITY, m_nParity);
	DDX_CBIndex(pDX, IDC_COMBO_STOPBITS, m_nStopBits);
	DDX_Text(pDX, IDC_EDIT_PARAM, m_szParam);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSerialCfgDlg, CDialog)
	//{{AFX_MSG_MAP(CSerialCfgDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_COMPORT, OnSelchangeComboComport)
	ON_CBN_SELCHANGE(IDC_COMBO_BAUDRATE, OnSelchangeComboBaudrate)
	ON_CBN_SELCHANGE(IDC_COMBO_BYTESIZE, OnSelchangeComboBytesize)
	ON_CBN_SELCHANGE(IDC_COMBO_PARITY, OnSelchangeComboParity)
	ON_CBN_SELCHANGE(IDC_COMBO_STOPBITS, OnSelchangeComboStopbits)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSerialCfgDlg::SetConfig(CConfigInfo &config)
{
	m_szComPort		= config.m_szComPort;
	m_dwBaudRate	= config.m_dwBaudRate;
	m_cByteSize		= config.m_cByteSize;
	m_nParity		= config.m_cParity;
	m_nStopBits		= config.m_cStopBits;
	m_szParam		= config.m_szComParam;
}

void CSerialCfgDlg::GetConfig(CConfigInfo &config)
{
	config.m_szComPort	= m_szComPort;
	config.m_dwBaudRate	= m_dwBaudRate;
	config.m_cByteSize	= m_cByteSize;
	config.m_cParity	= (BYTE)m_nParity;
	config.m_cStopBits	= (BYTE)m_nStopBits;
	config.m_szComParam	= m_szParam;
}

/////////////////////////////////////////////////////////////////////////////
// CSerialCfgDlg message handlers

BOOL CSerialCfgDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	int nIndex = m_ctrlComPort.FindStringExact(0, m_szComPort);
	if (nIndex == -1) nIndex = 0;
	m_ctrlComPort.SetCurSel(nIndex);

	CString szt;
	szt.Format(_T("%ld"), m_dwBaudRate);
	//m_ctrlBaudRate.SetWindowText(szt);
	nIndex = m_ctrlBaudRate.FindStringExact(0, szt);
	if (nIndex  == -1) nIndex = 3;
	m_ctrlBaudRate.SetCurSel(nIndex);

	if (m_cByteSize == 7)
		m_ctrlByteSize.SetCurSel(0);
	else
		m_ctrlByteSize.SetCurSel(1);

	m_ctrlParity.SetCurSel(m_nParity);
	m_ctrlStopBits.SetCurSel(m_nStopBits);
	
	GetDlgItem(IDC_STATIC_PARAM_DISPLAY_NAME)->SetWindowText(CConfigInfo::m_szParamDisplayName);
	GetDlgItem(IDC_STATIC_PARAM_HINT_TEXT)->SetWindowText(CConfigInfo::m_szParamHintText);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSerialCfgDlg::OnSelchangeComboComport() 
{
	m_ctrlComPort.GetWindowText(m_szComPort);
}

void CSerialCfgDlg::OnSelchangeComboBaudrate() 
{
	CString szt;
	m_ctrlBaudRate.GetWindowText(szt);
	m_dwBaudRate = (DWORD)_ttol(szt);
}

void CSerialCfgDlg::OnSelchangeComboBytesize() 
{
	if (m_ctrlByteSize.GetCurSel() == 0)
		m_cByteSize = 7;
	else
		m_cByteSize = 8;
}


void CSerialCfgDlg::OnSelchangeComboParity() 
{
	m_nParity = m_ctrlParity.GetCurSel();
}

void CSerialCfgDlg::OnSelchangeComboStopbits() 
{
	m_nStopBits = m_ctrlStopBits.GetCurSel();
}
