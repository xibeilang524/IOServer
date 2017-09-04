#if !defined(AFX_DLGCOMPORT_H__4E67E161_1458_495C_88C5_EDCA5BCE5735__INCLUDED_)
#define AFX_DLGCOMPORT_H__4E67E161_1458_495C_88C5_EDCA5BCE5735__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgComport.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgComport dialog

class CDlgComport : public CDialog
{
// Construction
public:
	CDlgComport(CWnd* pParent = NULL);   // standard constructor
	CString m_szip;
// Dialog Data
	//{{AFX_DATA(CDlgComport)
	enum { IDD = IDD_DIALOG_COMSET };
	CString	m_szbt;
	int		m_nck;
	CString	m_szdata;
	CString	m_szport;
	int		m_nstop;
	int		m_nComType;
	int		m_nSvrPort;
	BOOL	m_bTcpCrc16;
	int		m_ntimeout;
	int		m_nModType;
	BOOL	m_bSaveLog;
	UINT	m_nUpdateRate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgComport)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgComport)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCOMPORT_H__4E67E161_1458_495C_88C5_EDCA5BCE5735__INCLUDED_)
