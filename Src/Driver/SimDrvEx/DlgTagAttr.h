#if !defined(AFX_DLGTAGATTR_H__D3AAAD5B_DC0E_4043_90BB_FF60804537FC__INCLUDED_)
#define AFX_DLGTAGATTR_H__D3AAAD5B_DC0E_4043_90BB_FF60804537FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTagAttr.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgTagAttr dialog

class CDlgTagAttr : public CDialog
{
// Construction
public:
	CDlgTagAttr(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgTagAttr)
	enum { IDD = IDD_DIALOG_TAGATTR };
	int		m_nDataType;
	int		m_nSimulateType;
	CString	m_szBaseVal;
	CString	m_szDesc;
	float	m_fPercent;
	CString	m_szTagName;
	CString	m_szUnit;
	//}}AFX_DATA

	BOOL m_bAddTag;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTagAttr)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTagAttr)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTAGATTR_H__D3AAAD5B_DC0E_4043_90BB_FF60804537FC__INCLUDED_)
