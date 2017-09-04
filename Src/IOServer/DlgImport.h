#if !defined(AFX_DLGINPORT_H__D78A79FE_B6E6_44BC_92FB_55A0576A6608__INCLUDED_)
#define AFX_DLGINPORT_H__D78A79FE_B6E6_44BC_92FB_55A0576A6608__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgInport.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgInport dialog

class CDlgImport : public CDialog
{
// Construction
public:
	CDlgImport(CWnd* pParent = NULL);   // standard constructor
	DWORD m_dwflag;
// Dialog Data
	//{{AFX_DATA(CDlgInport)
	enum { IDD = IDD_DIALOG_INPORT };
	BOOL	m_bcomptype;
	BOOL	m_bcompval;
	BOOL	m_bdatatype;
	BOOL	m_bdbtag;
	BOOL	m_bdes;
	BOOL	m_bdrvtag;
	BOOL	m_bexp;
	BOOL	m_bnewtag;
	BOOL	m_bunit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgInport)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgInport)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGINPORT_H__D78A79FE_B6E6_44BC_92FB_55A0576A6608__INCLUDED_)
