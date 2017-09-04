#if !defined(AFX_DLGITEMATTR_H__B580A509_DEE0_4C92_8097_75F5D86F2544__INCLUDED_)
#define AFX_DLGITEMATTR_H__B580A509_DEE0_4C92_8097_75F5D86F2544__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgItemAttr.h : header file
//

#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgItemAttr dialog

class CDlgItemAttr : public CDialog
{
// Construction
public:
	CDlgItemAttr(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgItemAttr)
	enum { IDD = IDD_DIALOG_ITEMATTR };
	CComboBox	m_ctrlDataType;
	int		m_nDataType;
	CString	m_szDesc;
	CString	m_szNote;
	CString	m_szTagname;
	CString	m_szUnit;
	CString	m_szID;
	//}}AFX_DATA

	
	BOOL m_bAdd;
	CString  m_szOldName;
	WORD GetDataType();
	void SetDataType(WORD wtype);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgItemAttr)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgItemAttr)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGITEMATTR_H__B580A509_DEE0_4C92_8097_75F5D86F2544__INCLUDED_)
