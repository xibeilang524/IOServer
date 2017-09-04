// FileExplorer.cpp : implementation file
//

#include "stdafx.h"
#include "FileExplorer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileExplorer

IMPLEMENT_DYNAMIC(CFileExplorer, CFileDialog)

CFileExplorer::CFileExplorer(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd)
	: CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
	// 以下代码允许使用新风格的对话框
	OSVERSIONINFO vi;
	ZeroMemory(&vi, sizeof(OSVERSIONINFO));
	vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	::GetVersionEx(&vi);
	
	if (vi.dwPlatformId == VER_PLATFORM_WIN32_NT && vi.dwMajorVersion >= 5 &&
		sizeof(OPENFILENAME) < 88 && m_ofn.lStructSize < 88)
	{
		struct OPENFILENAMEEX
		{
			void* pvReserved; // 4 bytes
			DWORD dwReserved; // 4 bytes
			DWORD FlagsEx; // 4 bytes
		};
		
		m_ofn.lStructSize += sizeof(OPENFILENAMEEX);
	}
}

CFileExplorer::~CFileExplorer()
{
}

BOOL CFileExplorer::ShowFileDialog(
	CString &szFileName, CString &szPathName,
	BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
	LPCTSTR lpszDefExt,
	LPCTSTR lpszFileName,
	DWORD dwFlags,
	LPCTSTR lpszFilter,
	LPCTSTR lpszTitle,
	CWnd* pParentWnd)
{
	FILEDIALOGPARAMETER param;
	memset(&param, 0, sizeof(FILEDIALOGPARAMETER));

	param.bOpenFileDialog = bOpenFileDialog;
	if (NULL != lpszDefExt)
		_tcsncpy(param.szDefExt, lpszDefExt, 15);
	
	if (NULL != lpszFileName)
		_tcsncpy(param.szFileName, lpszFileName, 511);

	param.dwFlags = dwFlags;
	
	if (NULL != lpszFilter)
		_tcsncpy(param.szFilter, lpszFilter, 511);
	
	if (NULL != lpszTitle)
		_tcsncpy(param.szTitle, lpszTitle, 127);

	param.pParentWnd = pParentWnd;
	CWinThread *pThread = AfxBeginThread((AFX_THREADPROC)OnWorkThread, &param);
	pThread->m_bAutoDelete = FALSE;

	// 等待线程结束
	while(TRUE) {
		MSG msg;	
		if (MsgWaitForMultipleObjects(1, &(pThread->m_hThread), FALSE, INFINITE, QS_ALLINPUT) == WAIT_OBJECT_0)
		{
			break;
		}
		else
		{
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	delete pThread;
	
	if (param.nReturnFlag == IDOK)
	{
		szFileName = param.szReturnFileName;
		szPathName = param.szReturnPathName;
		return TRUE;
	}
	else {
		return FALSE;
	}
}

BEGIN_MESSAGE_MAP(CFileExplorer, CFileDialog)
	//{{AFX_MSG_MAP(CFileExplorer)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

UINT CFileExplorer::OnWorkThread(LPVOID lpParameter)
{
	LPFILEDIALOGPARAMETER pParam = (LPFILEDIALOGPARAMETER)lpParameter;

	CFileExplorer dlg(pParam->bOpenFileDialog, pParam->szDefExt, pParam->szFileName,
		pParam->dwFlags, pParam->szFilter, pParam->pParentWnd);
	
	if (*(pParam->szTitle) != _T('\0'))
		dlg.m_ofn.lpstrTitle = pParam->szTitle;

	pParam->nReturnFlag = dlg.DoModal();
	if (pParam->nReturnFlag == IDOK)
	{
		_tcsncpy(pParam->szReturnFileName, dlg.GetFileName(), 511);
		_tcsncpy(pParam->szReturnPathName, dlg.GetPathName(), 511);
	}
	return 0;
}