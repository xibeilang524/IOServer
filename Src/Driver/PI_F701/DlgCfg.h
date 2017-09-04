#include "afxcmn.h"
#if !defined(AFX_DLGCFG_H__DCE5D45F_AC1D_4FF8_B15A_1607D870F444__INCLUDED_)
#define AFX_DLGCFG_H__DCE5D45F_AC1D_4FF8_B15A_1607D870F444__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgCfg.h : header file
//
#include "PiDBDrv.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgCfg dialog

class CDlgCfg : public CDialog
{
// Construction
public:
	CDlgCfg(CWnd* pParent = NULL);   // standard constructor
	
// Dialog Data
	//{{AFX_DATA(CDlgCfg)
	enum { IDD = IDD_DIALOG_CFG };
	//}}AFX_DATA
	
	CPIServerList m_arrPIServerList;
	T_ServerInfo m_tServerInfo;

	CString m_szSqlServer;
	DWORD m_dwPort;
	CString m_szDBName;
	CString m_szUser;
	CString m_szPsw;
	BOOL m_bPIServer;
	BOOL m_bSqlServer;
	CString m_szTable;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCfg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void SetPIServerList();
	void GetPIServerList();
	void UpdateButtonState();

	// Generated message map functions
	//{{AFX_MSG(CDlgCfg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ctrlPIServers;
protected:
	virtual void OnOK();
public:
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonEdit();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();
	afx_msg void OnNMClickListPiserver(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListPiserver(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnSqlTest();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCFG_H__DCE5D45F_AC1D_4FF8_B15A_1607D870F444__INCLUDED_)
