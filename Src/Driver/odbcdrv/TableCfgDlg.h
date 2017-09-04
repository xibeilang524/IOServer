#if !defined(AFX_TABLECFGDLG_H__3E8AC36A_3B1E_4762_A62B_E435B7618C14__INCLUDED_)
#define AFX_TABLECFGDLG_H__3E8AC36A_3B1E_4762_A62B_E435B7618C14__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TableCfgDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTableCfgDlg dialog

class CTableCfgDlg : public CDialog
{
// Construction
public:
	CTableCfgDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTableCfgDlg)
	enum { IDD = IDD_DIALOG_TABLE };
	CString	m_szFieldName;
	CString	m_szTimeField;
	CString	m_szValField;
	CString	m_szTableName;
	int		m_nStorageType;
	CString	m_szQualityField;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTableCfgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTableCfgDlg)
	afx_msg void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABLECFGDLG_H__3E8AC36A_3B1E_4762_A62B_E435B7618C14__INCLUDED_)
