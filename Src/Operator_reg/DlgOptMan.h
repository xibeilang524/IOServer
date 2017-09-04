#if !defined(AFX_DLGOPTMAN_H__89853F56_A531_4A32_8D2A_B144142ADAE3__INCLUDED_)
#define AFX_DLGOPTMAN_H__89853F56_A531_4A32_8D2A_B144142ADAE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgOptMan.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgOptMan dialog

class CDlgOptMan : public CDialog
{
// Construction
public:
	CDlgOptMan(CWnd* pParent = NULL);   // standard constructor

	COperator *m_pOperator;
	CString		m_szOpt;
// Dialog Data
	//{{AFX_DATA(CDlgOptMan)
	enum { IDD = IDD_DIALOG_OPTMAN };
	CListCtrl	m_ListCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgOptMan)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgOptMan)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListOpt(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDel();
	afx_msg void OnButtonEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGOPTMAN_H__89853F56_A531_4A32_8D2A_B144142ADAE3__INCLUDED_)
