#include "afxcmn.h"
#if !defined(AFX_LICENSEDLG_H__B099FD29_055E_423E_AC2E_193C85255967__INCLUDED_)
#define AFX_LICENSEDLG_H__B099FD29_055E_423E_AC2E_193C85255967__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LicenseDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLicenseDlg dialog

class CLicenseDlg : public CDialog
{
// Construction
public:
	CLicenseDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLicenseDlg)
	enum { IDD = IDD_DIALOG_LICENSE };
	//}}AFX_DATA

	CString m_szProductName;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLicenseDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	UINT m_nTimer;
	int m_nTimeCount;
	CRect m_rcCopyrightRect;
	CString m_szCopyrightText;
	CString m_szMachineNo;

	void UpdateLicInfo(LPCTSTR lpszInfo);

	// Generated message map functions
	//{{AFX_MSG(CLicenseDlg)
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonCopy();
	CListCtrl m_ctrlList;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LICENSEDLG_H__B099FD29_055E_423E_AC2E_193C85255967__INCLUDED_)
