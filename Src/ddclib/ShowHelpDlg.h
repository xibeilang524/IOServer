#pragma once

#include "resource.h"

// CShowHelpDlg 对话框

class CShowHelpDlg : public CDialog
{
	DECLARE_DYNAMIC(CShowHelpDlg)

public:
	CShowHelpDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CShowHelpDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_HELP };

protected:
	void LoadHelpInfo(CString &szHelpInfo);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
