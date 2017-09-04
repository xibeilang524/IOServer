#if !defined(AFX_DLGLOADDRVTAG_H__40A4A3E9_074E_4552_ACCC_38290C49D0C6__INCLUDED_)
#define AFX_DLGLOADDRVTAG_H__40A4A3E9_074E_4552_ACCC_38290C49D0C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgLoadDrvTag.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgLoadDrvTag dialog

class CDlgLoadDrvTag : public CDialog
{
// Construction
public:
	CDlgLoadDrvTag(CWnd* pParent = NULL);   // standard constructor
	DWORD m_dwflag;
// Dialog Data
	//{{AFX_DATA(CDlgLoadDrvTag)
	enum { IDD = IDD_DIALOG_LOADDRVTAG };
	BOOL	m_bdes;
	BOOL	m_bnew;
	BOOL	m_btype;
	BOOL	m_bunit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgLoadDrvTag)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgLoadDrvTag)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGLOADDRVTAG_H__40A4A3E9_074E_4552_ACCC_38290C49D0C6__INCLUDED_)
