#if !defined(AFX_CFGDLG_H__5FE08E14_F7C9_414D_8F1D_824A2E7070AC__INCLUDED_)
#define AFX_CFGDLG_H__5FE08E14_F7C9_414D_8F1D_824A2E7070AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CfgDlg.h : header file
//

#include "resource.h"
#include "WinTagInfo.h"

/////////////////////////////////////////////////////////////////////////////
// CCfgDlg dialog

class CCfgDlg : public CDialog
{
// Construction
public:
	CCfgDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCfgDlg)
	enum { IDD = IDD_DIALOG_CFG };
	CStatic	m_ctrlFrame;
	CString	m_szConnectString;
	//}}AFX_DATA
	
	CWinTagInfo m_winTagInfo;
	CDDCDrvObj *m_pDrvObj;
	CTagDefTab	m_tagtab;
	CConfigInfo m_config;
	WORD m_wComTypes;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCfgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCfgDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnSysCommand(UINT nID, LONG lParam);
	afx_msg void OnButtonCom();
	afx_msg void OnButtonComset();
	afx_msg void OnButtonExample();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonExport();
	afx_msg void OnButtonDel();
	afx_msg void OnButtonAddone();
	afx_msg void OnButtonEdittag();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CFGDLG_H__5FE08E14_F7C9_414D_8F1D_824A2E7070AC__INCLUDED_)
