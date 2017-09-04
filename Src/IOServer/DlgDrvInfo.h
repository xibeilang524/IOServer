#if !defined(AFX_DLGDRVINFO_H__1E69D77D_4818_4FA3_96EC_34C33FC7CFF8__INCLUDED_)
#define AFX_DLGDRVINFO_H__1E69D77D_4818_4FA3_96EC_34C33FC7CFF8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDrvInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgDrvInfo dialog

class CDlgDrvInfo : public CDialog
{
// Construction
public:
	CDlgDrvInfo(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgDrvInfo)
	enum { IDD = IDD_DIALOG_DRVINFO };
	CString	m_szDec;
	CString	m_szInfo;
	CString	m_szWap;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDrvInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgDrvInfo)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDRVINFO_H__1E69D77D_4818_4FA3_96EC_34C33FC7CFF8__INCLUDED_)
