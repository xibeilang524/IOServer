
#pragma once
/////////////////////////////////////////////////////////////////////////////
// COutputList 窗口
using namespace KIMI;

class COutputList : public CListBox
{
// 构造
public:
	COutputList();

// 实现
public:
	virtual ~COutputList();

protected:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnViewOutput();

	DECLARE_MESSAGE_MAP()
};

class COutputWnd : public CDockablePane
{
// 构造
public:
	COutputWnd();

	void UpdateFonts();
	void AddLog(const LPCTSTR lpszLog);
	void UpdateLogList();

// 特性
protected:
	CMFCTabCtrl	m_wndTabs;

	//COutputList m_wndOutputBuild;
	CLogWinList m_wndLogList;

protected:
	void FillBuildWindow();
	void AdjustHorzScroll(CListBox& wndListBox);

// 实现
public:
	virtual ~COutputWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};

