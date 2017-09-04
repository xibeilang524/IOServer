#if !defined(AFX_DLGTAGATTR_H__1FBA3906_FF54_487B_9464_BD484D878447__INCLUDED_)
#define AFX_DLGTAGATTR_H__1FBA3906_FF54_487B_9464_BD484D878447__INCLUDED_

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
	BOOL m_bAdd;
	//CDrvItem* m_pdrv;
	CString		m_szDrvName;
// Dialog Data
	//{{AFX_DATA(CDlgTagAttr)
	enum { IDD = IDD_DIALOG_TAGATTR };
	CComboBox	m_cbptype;
	int		m_ncomptype;
	int		m_ndatatype;
	CString	m_szcompval;
	CString	m_szdes;
	CString	m_sztagdbname;
	CString	m_sztagdrvname;
	CString	m_sztagname;
	CString	m_szunit;
	CString	m_szexp;
	//}}AFX_DATA


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
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnButtonSeltag();
	afx_msg void OnSelchangeComboComptype();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonHelp();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTAGATTR_H__1FBA3906_FF54_487B_9464_BD484D878447__INCLUDED_)
