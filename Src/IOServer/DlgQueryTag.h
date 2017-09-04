#if !defined(AFX_DLGQUERYTAG_H__7EA824BA_4247_4218_8153_ABC101D33238__INCLUDED_)
#define AFX_DLGQUERYTAG_H__7EA824BA_4247_4218_8153_ABC101D33238__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgQueryTag.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgQueryTag dialog

class CDlgQueryTag : public CDialog
{
// Construction
public:
	CDlgQueryTag(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgQueryTag)
	enum { IDD = IDD_DIALOG_QUERYTAG };
	int		m_ntype;
	CString	m_sdes;
	CString	m_sdname;
	CString	m_sname;
	CString	m_spname;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgQueryTag)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgQueryTag)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGQUERYTAG_H__7EA824BA_4247_4218_8153_ABC101D33238__INCLUDED_)
