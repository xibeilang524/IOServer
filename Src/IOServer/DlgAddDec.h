#if !defined(AFX_DLGADDDEC_H__457D24BE_4AA2_49E8_B97C_28CCF3A301BD__INCLUDED_)
#define AFX_DLGADDDEC_H__457D24BE_4AA2_49E8_B97C_28CCF3A301BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAddDec.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgAddDec dialog

class CDlgAddDec : public CDialog
{
// Construction
public:
	CDlgAddDec(CWnd* pParent = NULL);   // standard constructor
	CString m_szPath;
	CString m_szDrvName;//驱动程序文件名
// Dialog Data
	//{{AFX_DATA(CDlgAddDec)
	enum { IDD = IDD_DIALOG_ADDDEC };
	CEdit		m_editinfo;
	CListBox	m_ListBox;
	CString		m_szDecName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAddDec)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void LoadDrivers();
	// Generated message map functions
	//{{AFX_MSG(CDlgAddDec)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeListDrv();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGADDDEC_H__457D24BE_4AA2_49E8_B97C_28CCF3A301BD__INCLUDED_)
