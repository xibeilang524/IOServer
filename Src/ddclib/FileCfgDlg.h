#pragma once

#include "resource.h"
// CFileCfgDlg 对话框

#include <ShlObj.h>

class CFileCfgDlg : public CDialog
{
	DECLARE_DYNAMIC(CFileCfgDlg)

public:
	CFileCfgDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFileCfgDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_FILE };

protected:
	LPITEMIDLIST ParsePidlFromPath(LPCSTR lpszPath);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_szFileName;
	CString m_szFileParam;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonFile();
	BOOL m_bAutoDeleteFile;
};
