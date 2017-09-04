#ifndef __EDITNUMBER_H__
#define __EDITNUMBER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditNumber.h : header file
//

TRUNK_NAMESPACE_BEGIN

/////////////////////////////////////////////////////////////////////////////
// CNumberBox window

class _GENERAL_CLASS CNumberBox : public CComboBox
{
// Construction
public:
	CNumberBox();
	CWnd *m_pWin;
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNumberBox)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL IsNumber(TCHAR cCode);
	virtual ~CNumberBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CNumberBox)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

TRUNK_NAMESPACE_END

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // end of __EDITNUMBER_H__
