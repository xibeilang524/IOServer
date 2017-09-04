// DlgFileSetting.cpp : 实现文件
//

#include "stdafx.h"
#include "IOServer.h"
#include "DlgFileSetting.h"
#include "afxdialogex.h"


// CDlgFileSetting 对话框

IMPLEMENT_DYNAMIC(CDlgFileSetting, CDialogEx)

CDlgFileSetting::CDlgFileSetting(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgFileSetting::IDD, pParent)
	, m_szFilePath(_T(""))
	, m_szFileName(_T(""))
	, m_bReplace(FALSE)
	, m_bUseFileOutput(FALSE)
	, m_lInterval(0)
{

}

CDlgFileSetting::~CDlgFileSetting()
{
}

void CDlgFileSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FILE_PATH, m_szFilePath);
	DDX_Text(pDX, IDC_FILE_NAME, m_szFileName);
	DDX_Check(pDX, IDC_CHK_FILE_REPLACE, m_bReplace);
	DDX_Check(pDX, IDC_CHK_USE_FILEOUTPUT, m_bUseFileOutput);
	DDX_Text(pDX, IDC_SAVE_INTERVAL, m_lInterval);
}


BEGIN_MESSAGE_MAP(CDlgFileSetting, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_FILE_SETTING, &CDlgFileSetting::OnBnClickedBtnFileSetting)
END_MESSAGE_MAP()


// CDlgFileSetting 消息处理程序


void CDlgFileSetting::OnBnClickedBtnFileSetting()
{
	// TODO: 在此添加控件通知处理程序代码
	TCHAR szFolderPath[MAX_PATH] = {0}; 
	CString strFolderPath = ""; 
	BROWSEINFO sInfo; 
	ZeroMemory(&sInfo, sizeof(BROWSEINFO));

	sInfo.pidlRoot = 0; 
	sInfo.lpszTitle = "选择存储路径:"; 
	sInfo.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_EDITBOX;
	sInfo.lpfn = NULL; 

	// 显示文件夹选择对话框 
	LPITEMIDLIST lpidlBrowse = SHBrowseForFolder(&sInfo);
	if (lpidlBrowse != NULL) 
	{ 
		// 获取文件夹名 
		if (SHGetPathFromIDList(lpidlBrowse,szFolderPath)) 
		{ 
			m_szFilePath = szFolderPath; 
		} 
	} 
	if(lpidlBrowse != NULL) 
	{ 
		CoTaskMemFree(lpidlBrowse); 
	}
}
