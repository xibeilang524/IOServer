#ifndef __RESIZESHEET_H__
#define __RESIZESHEET_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ResizeSheet.h : header file
//

TRUNK_NAMESPACE_BEGIN

/////////////////////////////////////////////////////////////////////////////
// CResizeSheet

class _GENERAL_CLASS CResizeSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CResizeSheet)

// Construction
public:
	CResizeSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CResizeSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResizeSheet)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CResizeSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CResizeSheet)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

TRUNK_NAMESPACE_END

#endif // __RESIZESHEET_H__
