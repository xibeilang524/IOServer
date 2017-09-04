#ifndef __TABSHEET_H__
#define __TABSHEET_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabSheet.h : header file
//

TRUNK_NAMESPACE_BEGIN
/////////////////////////////////////////////////////////////////////////////
// CTabSheet window
#define MAXPAGE 16

class _GENERAL_CLASS CTabSheet : public CTabCtrl
{
// Construction
public:
	CTabSheet();

// Attributes
public:
	BOOL GetHideTab() const { return m_bHideTab; }
	void SetHideTab(BOOL bHide = FALSE) { m_bHideTab = bHide; }

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabSheet)
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	int GetCurSel();
	int SetCurSel(int nItem);
	void Show();
	void SetRect();
	BOOL AddPage(LPCTSTR title, CDialog *pDialog, UINT ID);
	virtual ~CTabSheet();

	// Generated message map functions
protected:
	BOOL	m_bHideTab;
	CString m_Title[MAXPAGE];
	UINT	m_IDD[MAXPAGE];
	CDialog	*m_pPages[MAXPAGE];
	int		m_nNumOfPages;
	int		m_nCurrentPage;
	//{{AFX_MSG(CTabSheet)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

TRUNK_NAMESPACE_END

#endif // __TABSHEET_H__
