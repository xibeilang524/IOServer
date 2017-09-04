#if !defined(AFX_DLGMODIFYPASS_H__812DA5FE_F902_4874_ADA7_933D8C3DE47E__INCLUDED_)
#define AFX_DLGMODIFYPASS_H__812DA5FE_F902_4874_ADA7_933D8C3DE47E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgModifyPass.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgModifyPass dialog

class CDlgModifyPass : public CDialog
{
// Construction
public:
	CDlgModifyPass(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgModifyPass)
	enum { IDD = IDD_DIALOG_EDITPASS };
	CString	m_szName;
	CString	m_szOldPass;
	CString	m_szNewPass1;
	CString	m_szNewPass2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgModifyPass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgModifyPass)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMODIFYPASS_H__812DA5FE_F902_4874_ADA7_933D8C3DE47E__INCLUDED_)
