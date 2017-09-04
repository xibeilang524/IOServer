
// LicenseMgrView.h : CLicenseMgrView 类的接口
//


#pragma once


class CPropertiesToolBar : public CMFCToolBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CLicenseMgrView : public CView
{
protected: // 仅从序列化创建
	CLicenseMgrView();
	DECLARE_DYNCREATE(CLicenseMgrView)

// 属性
public:
	CLicenseMgrDoc* GetDocument() const;

	void SetVSDotNetLook(BOOL bSet)
	{
		m_wndPropList.SetVSDotNetLook(bSet);
		m_wndPropList.SetGroupNameFullWidth(bSet);
	}

// 操作
public:
	void AdjustLayout();
	void RemoveAllLicensePropList();
	void UpdateCurSelLicensePropList();
	BOOL UpdateLicensePropList(LPCTSTR lpszSoftwareName = NULL);

// 重写
public:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 实现
public:
	virtual ~CLicenseMgrView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CFont m_fntPropList;
	CPropertiesToolBar m_wndToolBar;
	CMFCPropertyGridCtrl m_wndPropList;

	CString m_szSoftwareName;
#ifdef _LICENSE_VIEW_APP
	BOOL	m_bFirstOpenView;
#endif

	void SetPropListFont();

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnExpandAllProperties();
	afx_msg void OnUpdateExpandAllProperties(CCmdUI* pCmdUI);
	afx_msg void OnSortProperties();
	afx_msg void OnUpdateSortProperties(CCmdUI* pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg LRESULT OnPropertyChanged(WPARAM,LPARAM);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // LicenseMgrView.cpp 中的调试版本
inline CLicenseMgrDoc* CLicenseMgrView::GetDocument() const
   { return reinterpret_cast<CLicenseMgrDoc*>(m_pDocument); }
#endif

