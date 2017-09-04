#ifndef __LISTBOXEX_H__
#define __LISTBOXEX_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListBoxEx.h : header file
//

TRUNK_NAMESPACE_BEGIN

/////////////////////////////////////////////////////////////////////////////
// CListBoxEx － 实现水平滚动条智能显示

class _GENERAL_CLASS CListBoxEx : public CListBox
{
// Construction
public:
	CListBoxEx();

// Attributes
public:

// Operations
public:
	int AddString( LPCTSTR lpszItem );
	int InsertString(int nIndex, LPCTSTR lpszItem);
	int DeleteString(UINT nIndex);
	void ResetContent();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListBoxEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListBoxEx();

	// Generated message map functions
protected:
	int m_nMaxWidth;
	void UpdateScrollBar(LPCTSTR lpszItem);

	//{{AFX_MSG(CListBoxEx)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

TRUNK_NAMESPACE_END

#endif // __LISTBOXEX_H__
