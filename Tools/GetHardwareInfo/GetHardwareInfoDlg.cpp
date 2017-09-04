
// GetHardwareInfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GetHardwareInfo.h"
#include "GetHardwareInfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CGetHardwareInfoDlg 对话框




CGetHardwareInfoDlg::CGetHardwareInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGetHardwareInfoDlg::IDD, pParent)
	, m_szMachineCode(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGetHardwareInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CODE, m_szMachineCode);
}

BEGIN_MESSAGE_MAP(CGetHardwareInfoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_GET, &CGetHardwareInfoDlg::OnBnClickedButtonGet)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CGetHardwareInfoDlg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_COPY, &CGetHardwareInfoDlg::OnBnClickedButtonCopy)
	ON_BN_CLICKED(IDOK, &CGetHardwareInfoDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CGetHardwareInfoDlg 消息处理程序

BOOL CGetHardwareInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_fntText.CreateFont(16, 0, 0, 0, 400, 0, 0, 0, GB2312_CHARSET, 0, 0, PROOF_QUALITY, VARIABLE_PITCH | FF_ROMAN, "宋体");

	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT_CODE);
	pEdit->SetFont(&m_fntText);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CGetHardwareInfoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CGetHardwareInfoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CGetHardwareInfoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CGetHardwareInfoDlg::OnBnClickedButtonGet()
{
	CMachineCodeHelper helper(TRUE);
	std::string strMachineCode = helper.GetMachineBase64Encode();

	m_szMachineCode = strMachineCode.c_str();

	UpdateData(FALSE);
}

void CGetHardwareInfoDlg::OnBnClickedButtonSave()
{
	CFileDialog dlg(FALSE, _T(".txt"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("文本格式(*.txt)|*.txt|"));

	if (dlg.DoModal() != IDOK)
		return;

	CFile file;

	try
	{
		file.Open(dlg.GetPathName(), CFile::modeWrite | CFile::modeCreate);
		file.Write((void *)(LPCTSTR)m_szMachineCode, m_szMachineCode.GetLength());
		file.Close();
	}
	catch (CFileException* e)
	{
		delete e;
		AfxMessageBox(_T("保存机器码时发生写文件失败！"));
	}
}

void CGetHardwareInfoDlg::OnBnClickedButtonCopy()
{
	UpdateData();

	DWORD dwsize = (m_szMachineCode.GetLength() + 1) * sizeof(TCHAR);
	HGLOBAL hGlobal = GlobalAlloc(GHND | GMEM_SHARE, dwsize);
	if (hGlobal == NULL)
		return;

	TCHAR *pGlobal = (TCHAR *)GlobalLock(hGlobal);
	_tcscpy_s(pGlobal, (dwsize / sizeof(TCHAR)), m_szMachineCode);

	GlobalUnlock(hGlobal);
	OpenClipboard();
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hGlobal);
	CloseClipboard();
}

void CGetHardwareInfoDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}
