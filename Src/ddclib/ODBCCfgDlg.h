#if !defined(AFX_ODBCCFGDLG_H__064FA500_9D94_4522_BB2B_CA9C734C6FEB__INCLUDED_)
#define AFX_ODBCCFGDLG_H__064FA500_9D94_4522_BB2B_CA9C734C6FEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ODBCCfgDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CODBCCfgDlg dialog
#include "resource.h"
class CODBCCfgDlg : public CDialog
{
// Construction
public:
	CODBCCfgDlg(CWnd* pParent = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CODBCCfgDlg)
	enum { IDD = IDD_DIALOG_ODBC };
	CString	m_szDataSource;
	CString	m_szPsw;
	CString	m_szID;
	//}}AFX_DATA
	void SetConfig(CConfigInfo &config);
	void GetConfig(CConfigInfo &config);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CODBCCfgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CODBCCfgDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ODBCCFGDLG_H__064FA500_9D94_4522_BB2B_CA9C734C6FEB__INCLUDED_)
