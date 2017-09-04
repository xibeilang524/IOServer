#if !defined(AFX_DB2CFGDLG_H__F935C94C_CEFC_4EF7_B74A_9B03886A5930__INCLUDED_)
#define AFX_DB2CFGDLG_H__F935C94C_CEFC_4EF7_B74A_9B03886A5930__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DB2CfgDlg.h : header file
//

#include "ConnStringParser.h"

/////////////////////////////////////////////////////////////////////////////
// CDB2CfgDlg dialog

class CDB2CfgDlg : public CDialog
{
// Construction
public:
	CDB2CfgDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDB2CfgDlg)
	enum { IDD = IDD_DIALOG_DB2 };
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
	//{{AFX_VIRTUAL(CDB2CfgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_szConnStr;

	// Generated message map functions
	//{{AFX_MSG(CDB2CfgDlg)
	afx_msg void OnOK();
	afx_msg void OnButtonConnstr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DB2CFGDLG_H__F935C94C_CEFC_4EF7_B74A_9B03886A5930__INCLUDED_)
