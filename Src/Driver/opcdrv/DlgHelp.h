#if !defined(AFX_DLGHELP_H__A2059C22_87A8_40E5_B377_61CC2B87E57A__INCLUDED_)
#define AFX_DLGHELP_H__A2059C22_87A8_40E5_B377_61CC2B87E57A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgHelp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgHelp dialog

class CDlgHelp : public CDialog
{
// Construction
public:
	CDlgHelp(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgHelp)
	enum { IDD = IDD_DIALOG_HELP };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgHelp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgHelp)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGHELP_H__A2059C22_87A8_40E5_B377_61CC2B87E57A__INCLUDED_)
