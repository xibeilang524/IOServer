#if !defined(AFX_DLGINISET_H__22F525F7_5AF0_4C63_A9D6_C817C95D8CA5__INCLUDED_)
#define AFX_DLGINISET_H__22F525F7_5AF0_4C63_A9D6_C817C95D8CA5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgIniSet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgIniSet dialog

class CDlgIniSet : public CDialog
{
// Construction
public:
	CDlgIniSet(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgIniSet)
	enum { IDD = IDD_DIALOG_INISET };
	BOOL	m_bAutoRun;
	BOOL	m_bCache;
	BOOL	m_bExitPass;
	BOOL	m_bSaveLog;
	UINT	m_wHeartPort;
	CString	m_szHeartIp;
	CString	m_szprj;
	int		m_nTimeZero;
	int		m_nthread;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgIniSet)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgIniSet)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	int m_nLogLevel;
	CString m_szTitle;
	BOOL m_bSkipSameTime;
	BOOL m_bWaitTcpAck;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGINISET_H__22F525F7_5AF0_4C63_A9D6_C817C95D8CA5__INCLUDED_)
