#if !defined(AFX_DLGEXPORTDBPROXY_H__CCF4283F_62FE_4EAB_A83A_98248B6FD019__INCLUDED_)
#define AFX_DLGEXPORTDBPROXY_H__CCF4283F_62FE_4EAB_A83A_98248B6FD019__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgExportDbProxy.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgExportDbProxy dialog

class CDlgExportDbProxy : public CDialog
{
// Construction
public:
	CDlgExportDbProxy(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgExportDbProxy)
	enum { IDD = IDD_DIALOG_EXPORTPROXY };
	CString	m_szdbname;
	int		m_nsel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExportDbProxy)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgExportDbProxy)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEXPORTDBPROXY_H__CCF4283F_62FE_4EAB_A83A_98248B6FD019__INCLUDED_)
