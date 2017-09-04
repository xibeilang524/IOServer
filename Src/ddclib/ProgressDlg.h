#if !defined(AFX_PROGRESSDLG_H__4A85BB8F_E6AD_4D15_9E09_0A7C31F1210A__INCLUDED_)
#define AFX_PROGRESSDLG_H__4A85BB8F_E6AD_4D15_9E09_0A7C31F1210A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProgressDlg.h : header file
//

#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog

class CProgressDlg : public CDialog
{
// Construction
public:
	CProgressDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProgressDlg)
	enum { IDD = IDD_DIALOG_PROGRESS };
	CProgressCtrl	m_ctrlProgress;
	CString	m_szInfo;
	//}}AFX_DATA
	
	CProgressCtrl *GetProgressCtrl() {
		return (CProgressCtrl*)GetDlgItem(IDC_PROGRESS1);
	}
	void SetInfo(LPCTSTR lpszInfo) {
		SetDlgItemText(IDC_STATIC_INFO, lpszInfo);
		RedrawWindow();
	}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProgressDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGRESSDLG_H__4A85BB8F_E6AD_4D15_9E09_0A7C31F1210A__INCLUDED_)
