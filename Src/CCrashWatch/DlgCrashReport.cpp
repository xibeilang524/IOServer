// DlgCrashReport.cpp : 实现文件
//

#include "stdafx.h"
#include "CCrashWatch.h"
#include "DlgCrashReport.h"
#include "MiniVersion.h"
#include "CrashHandler.h"

// CDlgCrashReport 对话框

IMPLEMENT_DYNAMIC(CDlgCrashReport, CDialog)

CDlgCrashReport::CDlgCrashReport(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCrashReport::IDD, pParent)
	, m_bRestart(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_CRASH);
	m_pCrashHandler = NULL;
	m_nSeconds = 5;
}

CDlgCrashReport::~CDlgCrashReport()
{
}

void CDlgCrashReport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BANNER, m_ctrlBanner);
	DDX_Control(pDX, IDC_APP_ICON, m_ctrlIcon);
	DDX_Control(pDX, IDC_PLEASE_TELL_US, m_ctrlPleaseTellUs);
	DDX_Check(pDX, IDC_CHECK_RESTART, m_bRestart);
}


BEGIN_MESSAGE_MAP(CDlgCrashReport, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgCrashReport 消息处理程序

BOOL CDlgCrashReport::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	InitializeDisplay();

	m_nTimer = (UINT)SetTimer(1, 1000, NULL);
	CString szText;
	szText.Format(_T("关闭（%02d秒后关闭窗口）"), m_nSeconds);
	CButton *pButton = (CButton*)GetDlgItem(IDOK);
	pButton->SetWindowText(szText);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgCrashReport::InitializeDisplay()
{
	m_ctrlBanner.SetTextColor(RGB(0,0,0));
	m_ctrlBanner.SetBackgroundColor(RGB(255,255,255));
	m_ctrlBanner.SetBold(TRUE);

	// get version resource
	CMiniVersion ver(m_pCrashHandler->GetModuleName());
	TCHAR szBuf[1000];
	szBuf[0] = _T('\0');

	// try to get description
	ver.GetFileDescription(szBuf, _countof(szBuf)-2);

	// if that failed try to get product name
	if (szBuf[0] == _T('\0'))
		ver.GetProductName(szBuf, _countof(szBuf)-2);

	ver.Release();

	// if that failed just use exe name
	if (szBuf[0] == _T('\0'))
		_tcscpy(szBuf, m_pCrashHandler->GetAppName());

	SetWindowText(szBuf);

	CString szText;
	szText.Format(_T("退出后自动重启“%s”"), szBuf);
	CButton *pButton = (CButton*)GetDlgItem(IDC_CHECK_RESTART);
	pButton->SetWindowText(szText);
	pButton->SetCheck(m_pCrashHandler->IsAppRestart());

	szText.Format(_T("%s 遇到问题需要关闭。我们对此引起的不便表示抱歉。"), szBuf);

	m_ctrlBanner.SetWindowText(szText);
	m_ctrlBanner.SetMargins(22, 8);

	// get icon of crashed app
	HICON hIcon = ::ExtractIcon(AfxGetInstanceHandle(), m_pCrashHandler->GetModuleName(), 0);

	m_ctrlIcon.SetBackgroundColor(RGB(255,255,255), FALSE);
	if (hIcon)
		m_ctrlIcon.SetIcon(hIcon, FALSE);

	m_ctrlPleaseTellUs.SetBold(TRUE);
}

void CDlgCrashReport::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		m_nSeconds--;
		CString szText;
		szText.Format(_T("关闭（%02d秒后关闭窗口）"), m_nSeconds);
		CButton *pButton = (CButton*)GetDlgItem(IDOK);
		pButton->SetWindowText(szText);

		if (m_nSeconds == 0)
		{
			KillTimer(m_nTimer);

			OnOK();
		}
	}

	CDialog::OnTimer(nIDEvent);
}
