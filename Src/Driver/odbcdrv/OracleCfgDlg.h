#if !defined(AFX_ORACLECFGDLG_H__F459B5A3_D39E_4700_B7A3_4EC839D42847__INCLUDED_)
#define AFX_ORACLECFGDLG_H__F459B5A3_D39E_4700_B7A3_4EC839D42847__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OracleCfgDlg.h : header file
//

#include "ConnStringParser.h"
/////////////////////////////////////////////////////////////////////////////
// COracleCfgDlg dialog

class COracleCfgDlg : public CDialog
{
// Construction
public:
	COracleCfgDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COracleCfgDlg)
	enum { IDD = IDD_DIALOG_ORACLE };
	CString	m_szPassword;
	CString	m_szUser;
	CString	m_szServer;
	//}}AFX_DATA
	
	void GetConnectString(CConnStringParser &parser);
	void SetConnectString(CConnStringParser &parser);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COracleCfgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_szConnStr;

	// Generated message map functions
	//{{AFX_MSG(COracleCfgDlg)
	afx_msg void OnOK();
	afx_msg void OnButtonConnstr();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ORACLECFGDLG_H__F459B5A3_D39E_4700_B7A3_4EC839D42847__INCLUDED_)
