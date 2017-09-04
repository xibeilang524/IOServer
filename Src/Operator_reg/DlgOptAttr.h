#if !defined(AFX_DLGOPTATTR_H__67E622CA_37C5_4DFD_A4AF_5DD596854D9E__INCLUDED_)
#define AFX_DLGOPTATTR_H__67E622CA_37C5_4DFD_A4AF_5DD596854D9E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgOptAttr.h : header file
//
#include "Operator.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgOptAttr dialog

class CDlgOptAttr : public CDialog
{
// Construction
public:
	CDlgOptAttr(CWnd* pParent = NULL);   // standard constructor
	CString m_szGroup; //ื้
	BOOL	m_bAdd;

	COperator *m_pOperator;
// Dialog Data
	//{{AFX_DATA(CDlgOptAttr)
	enum { IDD = IDD_DIALOG_OPT };
	CComboBox	m_cbGroup;
	BOOL	m_bUse;
	CString	m_szName;
	CString	m_szPass1;
	CString	m_szPass2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgOptAttr)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgOptAttr)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGOPTATTR_H__67E622CA_37C5_4DFD_A4AF_5DD596854D9E__INCLUDED_)
