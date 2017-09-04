#if !defined(AFX_DLGCONFIG_H__5DD41116_39ED_4778_A475_66A069015A01__INCLUDED_)
#define AFX_DLGCONFIG_H__5DD41116_39ED_4778_A475_66A069015A01__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgConfig.h : header file
//
#include "odbcobj.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgConfig dialog

class CDlgConfig : public CDialog
{
// Construction
public:
	CDlgConfig(CWnd* pParent = NULL);   // standard constructor
	CWinTagInfo m_wintag;

	BOOL	m_bModified;
	BOOL	m_bUp;
// Dialog Data
	//{{AFX_DATA(CDlgConfig)
	enum { IDD = IDD_DIALOG_CONFIG };
	CStatic	m_stFrm;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgConfig)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL AddInPortTag(LPCTSTR szName, LPCTSTR szAliasName, LPCTSTR szDesc, LPCTSTR szUnit, LPCTSTR szDataType);
	// Generated message map functions
	//{{AFX_MSG(CDlgConfig)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnButtonDelsel();
	afx_msg void OnButtonExport();
	afx_msg void OnButtonExsamp();
	afx_msg void OnButtonInport();
	afx_msg void OnButtonUp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCONFIG_H__5DD41116_39ED_4778_A475_66A069015A01__INCLUDED_)
