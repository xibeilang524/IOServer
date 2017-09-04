#if !defined(AFX_DLGEXPORT_H__DBBBD8F7_91F8_49A8_9076_9227351DC406__INCLUDED_)
#define AFX_DLGEXPORT_H__DBBBD8F7_91F8_49A8_9076_9227351DC406__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgExport.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgExport dialog

class CDlgExport : public CDialog
{
// Construction
public:
	CDlgExport(CWnd* pParent = NULL);   // standard constructor

	CString m_sztitle;
// Dialog Data
	//{{AFX_DATA(CDlgExport)
	enum { IDD = IDD_DIALOG_EXPORT };
	int		m_nsel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExport)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgExport)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEXPORT_H__DBBBD8F7_91F8_49A8_9076_9227351DC406__INCLUDED_)
