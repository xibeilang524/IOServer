#pragma once
#include "Resource.h"

// CDlgExit 对话框

class CDlgExit : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgExit)

public:
	CDlgExit(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgExit();

// 对话框数据
	enum { IDD = IDD_DIALOG_EXIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	virtual BOOL CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, LPVOID lpParam = NULL);
};
