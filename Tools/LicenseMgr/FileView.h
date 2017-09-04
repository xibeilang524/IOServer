
#pragma once

#include "ViewTree.h"

#define ID_VIEW_TREE	1000

class CFileView : public CDockablePane
{
// 构造
public:
	CFileView();

	void AdjustLayout();
	void OnChangeVisualStyle();

	CViewTree* GetTreeCtrl() {
		return &m_wndFileView;
	}

// 属性
protected:

	CViewTree m_wndFileView;
	CImageList m_FileViewImages;

protected:

// 实现
public:
	virtual ~CFileView();

	virtual BOOL CanBeClosed() const { 
		//return m_dwControlBarStyle & AFX_CBRS_CLOSE; 
		return FALSE;
	}

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
 	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
// 	afx_msg void OnProperties();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg BOOL OnViewTreeSelchanging(UINT nID, NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnViewTreeSelchanged(UINT nID, NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
};

