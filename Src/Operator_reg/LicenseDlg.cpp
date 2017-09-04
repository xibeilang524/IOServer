// LicenseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Operator.h"
#include "LicenseDlg.h"

#include "ComputerHardwareInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLicenseDlg dialog


CLicenseDlg::CLicenseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLicenseDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLicenseDlg)
	//}}AFX_DATA_INIT
	m_szProductName = _T("");
}


void CLicenseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLicenseDlg)
	DDX_Control(pDX, IDC_LIST_INFO, m_ctrlList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLicenseDlg, CDialog)
	//{{AFX_MSG_MAP(CLicenseDlg)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_COPY, &CLicenseDlg::OnBnClickedButtonCopy)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLicenseDlg message handlers


void CLicenseDlg::UpdateLicInfo(LPCTSTR lpszInfo)
{
	m_ctrlList.DeleteAllItems();

	CString szInfo = lpszInfo ? lpszInfo : _T("");
	if (szInfo.GetAt(szInfo.GetLength() - 1) != _T('\n'))
		szInfo.Append(_T("\n"));

	CString szLine, szt;
	int nStart = 0;
	int nPos = nPos = szInfo.Find(_T("\n"), nStart);
	while(nPos != -1)
	{
		szLine = szInfo.Mid(nStart, nPos - nStart);

		int n = szLine.Find(_T(":"));
		szt = szLine.Left(n);
		szt.Trim();
		int nItem = m_ctrlList.InsertItem(m_ctrlList.GetItemCount(), szt);

		if (nItem != -1)
		{
			szt = szLine.Right(szLine.GetLength() - n - 1);
			szt.Trim();
			m_ctrlList.SetItemText(nItem, 1, szt);
		}

		nStart = nPos + 1;
		nPos = szInfo.Find(_T("\n"), nStart);
	}
}

BOOL CLicenseDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_nTimer = SetTimer(1, 1000, NULL);
	m_nTimeCount = 10;

	
	GetDlgItem(IDC_STATIC_COPYRIGHT)->GetWindowText(m_szCopyrightText);	
	GetDlgItem(IDC_STATIC_COPYRIGHT)->GetWindowRect(m_rcCopyrightRect);	
	ScreenToClient(m_rcCopyrightRect);
	GetDlgItem(IDC_STATIC_COPYRIGHT)->SetWindowText(_T(""));
	GetDlgItem(IDC_STATIC_COPYRIGHT)->MoveWindow(0, 0, 0, 0);


	if (!m_szProductName.IsEmpty())
	{
		CString szTitle;
		szTitle.Format(_T("%s - 软件授权提示"), m_szProductName);
		SetWindowText(szTitle);
	}

	DWORD dwStyle = m_ctrlList.GetExtendedStyle();
	dwStyle |= LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;
	m_ctrlList.SetExtendedStyle(dwStyle);
	m_ctrlList.InsertColumn(0, _T("属性名"), LVCFMT_LEFT, 100);
	m_ctrlList.InsertColumn(1, _T("属性值"), LVCFMT_LEFT, 250);

	CRegLic reglic;
	UpdateLicInfo(reglic.GetLicInfo(m_szProductName));
	m_szMachineNo = reglic.m_szMachineNo;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLicenseDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == 1)
	{
		m_nTimeCount--;
		CString szTitle;
		szTitle.Format(_T("关闭（%02d秒后关闭窗口）"), m_nTimeCount);
		GetDlgItem(IDOK)->SetWindowText(szTitle);

		if(m_nTimeCount == 0) // 计数完则关闭对话框
		{
			OnOK();
		}
	}

	
	CDialog::OnTimer(nIDEvent);
}

void CLicenseDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	if (m_nTimer > 0)
		KillTimer(m_nTimer);

	CDialog::OnClose();
}

void CLicenseDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	

	LOGFONT lf;
	lf.lfHeight = 13;
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = FW_LIGHT;
	lf.lfItalic = FALSE;
	lf.lfUnderline = FALSE;
	lf.lfStrikeOut = FALSE;
	lf.lfCharSet = GB2312_CHARSET;
	strcpy(lf.lfFaceName, "宋体");
	//创建字体
	CFont font;
	font.CreateFontIndirect(&lf);
	//更改当前字体
	CFont * pOldFont = dc.SelectObject(&font);

	//绘制字体
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(255, 255, 255));
	dc.TextOut(m_rcCopyrightRect.left + 1, m_rcCopyrightRect.top + 1, m_szCopyrightText);
	dc.SetTextColor(::GetSysColor(COLOR_BTNSHADOW));
	dc.TextOut(m_rcCopyrightRect.left - 1, m_rcCopyrightRect.top - 1, m_szCopyrightText);

	//恢复设备上下文的原有设置
	dc.SelectObject(pOldFont);
	
	// Do not call CDialog::OnPaint() for painting messages
}

void CLicenseDlg::OnBnClickedButtonCopy()
{
	CString szText;
	szText.Format(_T("本机机器码：%s"), m_szMachineNo);
	HGLOBAL hGlobal = GlobalAlloc(GHND | GMEM_SHARE, (szText.GetLength() + 1) * sizeof(TCHAR));
	if (hGlobal == NULL)
		return;

	TCHAR *pGlobal = (TCHAR *)GlobalLock(hGlobal);
	_tcscpy(pGlobal, szText);

	GlobalUnlock(hGlobal);
	OpenClipboard();
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hGlobal);
	CloseClipboard();
}
