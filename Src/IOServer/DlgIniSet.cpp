// DlgIniSet.cpp : implementation file
//

#include "stdafx.h"
#include "IOServer.h"
#include "DlgIniSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgIniSet dialog


CDlgIniSet::CDlgIniSet(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgIniSet::IDD, pParent)
	, m_nLogLevel(2)
	, m_szTitle(_T(""))
	, m_bSkipSameTime(FALSE)
	, m_bWaitTcpAck(TRUE)
{
	//{{AFX_DATA_INIT(CDlgIniSet)
	m_bAutoRun = FALSE;
	m_bCache = FALSE;
	m_bExitPass = FALSE;
	m_bSaveLog = FALSE;
	m_wHeartPort = 0;
	m_szHeartIp = _T("");
	m_szprj = _T("");
	m_nTimeZero = 0;
	m_nthread = 4;
	//}}AFX_DATA_INIT
}


void CDlgIniSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgIniSet)
	DDX_Check(pDX, IDC_CHECK_AUTORUN, m_bAutoRun);
	DDX_Check(pDX, IDC_CHECK_CACHE, m_bCache);
	DDX_Check(pDX, IDC_CHECK_EXITPASS, m_bExitPass);
	DDX_Check(pDX, IDC_CHECK_SAVELOG, m_bSaveLog);
	DDX_Text(pDX, IDC_EDIT_HPORT, m_wHeartPort);
	DDV_MinMaxUInt(pDX, m_wHeartPort, 1023, 32767);
	DDX_Text(pDX, IDC_EDIT_IP, m_szHeartIp);
	DDV_MaxChars(pDX, m_szHeartIp, 15);
	DDX_Text(pDX, IDC_EDIT_PRJNAME, m_szprj);
	DDX_Text(pDX, IDC_EDIT_TIMEZERO, m_nTimeZero);
	DDV_MinMaxInt(pDX, m_nTimeZero, -12, 12);
	DDX_Text(pDX, IDC_EDIT_THREAD, m_nthread);
	DDV_MinMaxInt(pDX, m_nthread, 1, 10);
	//}}AFX_DATA_MAP
	DDX_CBIndex(pDX, IDC_COMBO_LOGLEVEL, m_nLogLevel);
	DDX_Text(pDX, IDC_EDIT_TITLE, m_szTitle);
	DDX_Check(pDX, IDC_CHECK_SKIP_SAME_TIME, m_bSkipSameTime);
	DDX_Check(pDX, IDC_CHECK_WAIT_TCP_ACK, m_bWaitTcpAck);
}


BEGIN_MESSAGE_MAP(CDlgIniSet, CDialog)
	//{{AFX_MSG_MAP(CDlgIniSet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgIniSet message handlers

void CDlgIniSet::OnOK() 
{
	// TODO: Add extra validation here
	if(!UpdateData())
		return;
	CDialog::OnOK();
}
