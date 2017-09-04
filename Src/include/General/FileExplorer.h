#ifndef __FILEEXPLORER_H__
#define __FILEEXPLORER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileExplorer.h : header file
//

/*
 * 由于MS的CoInitializeEx (NULL, COINIT_MULTITHREADED)调用和CFileDialog的OFN_EXPLORER风格有冲突，
 * 会造成OPC驱动调用CoUninitialize()时被堵塞而无法终止主程序。并且文件对话框在获取Shell接口时会
 * 返回错误状态码，这样会使在文件对话框中浏览我的电脑时不能显示当前电脑的使用盘符。
 *
 * CFileExplorer类不但对最新的对话框风格进行了封装，并且通过在另一个线程中打开模态对话框的方法
 * 来避免出现以上的问题。
 *
 * CFileExplorer类的使用方法：
 *
 *		CString szPathName;
 *		CString szFileName;
 *		if (!CFileExplorer::ShowFileDialog(szFileName, szPathName, TRUE, NULL,NULL, 
 *			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
 *			_T("数据项文件(*.csv)|*.csv||"), 
 *			_T("导入标签配置文件"),
 *			this)	// 必须传递pParentWnd参数，否则无法运行于模态窗口
 *			)
 *			return; // 没有使用任何文件
 *
 *		这样szFileName返回选择的文件名（无路径），szPathName返回的是全文件名（包括全路径）。
 *
 * CFileExplorer类目录还不支持多选文件，不过通过简单的扩展就可以支持。
 */

TRUNK_NAMESPACE_BEGIN

/////////////////////////////////////////////////////////////////////////////
// CFileExplorer dialog

class _GENERAL_CLASS CFileExplorer : public CFileDialog
{
	DECLARE_DYNAMIC(CFileExplorer)

public:
	CFileExplorer(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);
	virtual ~CFileExplorer();

	static BOOL ShowFileDialog(
		CString &szFileName, CString &szPathName,
		BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		LPCTSTR lpszTitle = NULL,
		CWnd* pParentWnd = NULL);
protected:
	typedef struct tagFILEDIALOGPARAMETER
	{
		BOOL	bOpenFileDialog;
		DWORD	dwFlags;
		CWnd	*pParentWnd;
		TCHAR	szDefExt[16];
		TCHAR	szFilter[512];
		TCHAR	szFileName[512];
		TCHAR	szTitle[128];
		UINT	nReturnFlag;
		TCHAR	szReturnPathName[512];
		TCHAR	szReturnFileName[512];
	}FILEDIALOGPARAMETER, *LPFILEDIALOGPARAMETER;
	static UINT OnWorkThread(LPVOID lpParameter);

	//{{AFX_MSG(CFileExplorer)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

TRUNK_NAMESPACE_END

#endif // end of __FILEEXPLORER_H__
