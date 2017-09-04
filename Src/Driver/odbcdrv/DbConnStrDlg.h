#if !defined(AFX_DBCONNSTRDLG_H__7D2FFD68_5ED8_4D87_AEBB_76F62A104023__INCLUDED_)
#define AFX_DBCONNSTRDLG_H__7D2FFD68_5ED8_4D87_AEBB_76F62A104023__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DbConnStrDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDbConnStrDlg dialog

class CDbConnStrDlg : public CDialog
{
// Construction
public:
	CDbConnStrDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDbConnStrDlg)
	enum { IDD = IDD_DIALOG_CONNSTR };
	CString	m_szConnStr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDbConnStrDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDbConnStrDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBCONNSTRDLG_H__7D2FFD68_5ED8_4D87_AEBB_76F62A104023__INCLUDED_)
