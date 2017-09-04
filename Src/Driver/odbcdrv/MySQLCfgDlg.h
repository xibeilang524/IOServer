#if !defined(AFX_MYSQLCFGDLG_H__0CE3941D_7279_42EC_90A4_B3D2725E4667__INCLUDED_)
#define AFX_MYSQLCFGDLG_H__0CE3941D_7279_42EC_90A4_B3D2725E4667__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MySQLCfgDlg.h : header file
//

#include "ConnStringParser.h"
/////////////////////////////////////////////////////////////////////////////
// CMySQLCfgDlg dialog

class CMySQLCfgDlg : public CDialog
{
// Construction
public:
	CMySQLCfgDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMySQLCfgDlg)
	enum { IDD = IDD_DIALOG_MYSQL };
	CString	m_szDatabase;
	CString	m_szPassword;
	UINT	m_nPort;
	CString	m_szServer;
	CString	m_szUser;
	//}}AFX_DATA
	
	void GetConnectString(CConnStringParser &parser);
	void SetConnectString(CConnStringParser &parser);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMySQLCfgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_szConnStr;

	// Generated message map functions
	//{{AFX_MSG(CMySQLCfgDlg)
	afx_msg void OnOK();
	afx_msg void OnButtonConnstr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYSQLCFGDLG_H__0CE3941D_7279_42EC_90A4_B3D2725E4667__INCLUDED_)
