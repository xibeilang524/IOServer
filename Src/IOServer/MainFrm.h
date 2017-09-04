
// MainFrm.h : CMainFrame 类的接口
//

#pragma once
#include "ProjectTree.h"
#include "OutputWnd.h"
#include "DlgExit.h"

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();
	int			AddNode2Project(CString szModule, CString szName, int nNodeType);
	CTreeCtrl&	GetProjectTree(void);
	void		AddProjectData(void);
// 特性
public:

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CMFCMenuBar			m_wndMenuBar;
	CMFCToolBar			m_wndToolBar;
	CMFCToolBar			m_wndToolBar32;
	//CTrueColorToolBar    m_wndMyToolBar;
	CMFCStatusBar		m_wndStatusBar;
	CMFCToolBarImages	m_UserImages;
	CProjectTree		m_wndProjectTree;
	COutputWnd			m_wndOutput;
	long				m_lLogTimer;
	long				m_lAuthorCounter;
	long				m_nRunLogCounter;
	CString				m_szLogContent;
	CDlgExit			m_dlgExit;
// 生成的消息映射函数
protected:
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnWindowManager();
	afx_msg void	OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void	OnApplicationLook(UINT id);
	afx_msg void	OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void	OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void	OnTimer(UINT_PTR nIDEvent);
	afx_msg void	OnDestroy();
	afx_msg void	OnClose();
	afx_msg void	OnDevadd();
	afx_msg void	OnOutFileSetting();
	afx_msg void	OnOutFileSetting2();
	DECLARE_MESSAGE_MAP()

	BOOL		CreateDockingWindows();
	void		SetDockingWindowIcons(BOOL bHiColorIcons);
	BOOL		WriteLog2Disk(CString &szLog);
	void		DoRunLog();
	void		ExitDoLog();
public:
};


