#if !defined(AFX_SQLSERVERCFGDLG_H__CD840D77_3FCF_4183_A867_244F3638C3E6__INCLUDED_)
#define AFX_SQLSERVERCFGDLG_H__CD840D77_3FCF_4183_A867_244F3638C3E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SQLServerCfgDlg.h : header file
//

#include "ConnStringParser.h"
/////////////////////////////////////////////////////////////////////////////
// CSQLServerCfgDlg dialog

class CSQLServerCfgDlg : public CDialog
{
// Construction
public:
	CSQLServerCfgDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSQLServerCfgDlg)
	enum { IDD = IDD_DIALOG_MSSQL };
	CString	m_szDatabase;
	CString	m_szInstance;
	CString	m_szPassword;
	UINT	m_nPort;
	CString	m_szServer;
	CString	m_szUser;
	//}}AFX_DATA
	
	void GetConnectString(CConnStringParser &parser);
	void SetConnectString(CConnStringParser &parser);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSQLServerCfgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_szConnStr;

	// Generated message map functions
	//{{AFX_MSG(CSQLServerCfgDlg)
	afx_msg void OnOK();
	afx_msg void OnButtonConnstr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SQLSERVERCFGDLG_H__CD840D77_3FCF_4183_A867_244F3638C3E6__INCLUDED_)
