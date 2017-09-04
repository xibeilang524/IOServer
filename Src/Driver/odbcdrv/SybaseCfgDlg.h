#if !defined(AFX_SYBASECFGDLG_H__FD115A1D_AC64_450B_BDF9_22AB92955B64__INCLUDED_)
#define AFX_SYBASECFGDLG_H__FD115A1D_AC64_450B_BDF9_22AB92955B64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SybaseCfgDlg.h : header file
//

#include "ConnStringParser.h"
/////////////////////////////////////////////////////////////////////////////
// CSybaseCfgDlg dialog

class CSybaseCfgDlg : public CDialog
{
// Construction
public:
	CSybaseCfgDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSybaseCfgDlg)
	enum { IDD = IDD_DIALOG_SYBASE };
	CString	m_szPassword;
	CString	m_szServer;
	CString	m_szUser;
	//}}AFX_DATA
	
	int m_nDbType;
	void GetConnectString(CConnStringParser &parser);
	void SetConnectString(CConnStringParser &parser);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSybaseCfgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_szConnStr;

	// Generated message map functions
	//{{AFX_MSG(CSybaseCfgDlg)
	afx_msg void OnOK();
	afx_msg void OnButtonConnstr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYBASECFGDLG_H__FD115A1D_AC64_450B_BDF9_22AB92955B64__INCLUDED_)
