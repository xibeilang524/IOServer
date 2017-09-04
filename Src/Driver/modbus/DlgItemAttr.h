#if !defined(AFX_DLGITEMATTR_H__A63D6B8D_2A36_4159_9063_0B5035B11D95__INCLUDED_)
#define AFX_DLGITEMATTR_H__A63D6B8D_2A36_4159_9063_0B5035B11D95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgItemAttr.h : header file
//

#include "modbusdrv.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgItemAttr dialog
/*
class CDlgItemAttr : public CDialog
{
// Construction
public:
	CDlgItemAttr(CWnd* pParent = NULL);   // standard constructor
	BOOL m_bAdd;//添加还是删除
	CString m_szID;//保存旧的ID,用于修改
	WORD	m_wRegAddr;//寄存器地址

	CModConfig *m_pconfig;//配置表
	
// Dialog Data
	//{{AFX_DATA(CDlgItemAttr)
	enum { IDD = IDD_DIALOG_ITEMATTR };
	int		m_nrtype;
	BYTE	m_ucDecAddr;
	CString	m_sName;
	CString	m_sNote;
	BYTE	m_ucOrder;
	CString	m_sRegAddr;
	BOOL	m_nb4pos;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgItemAttr)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgItemAttr)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
*/
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGITEMATTR_H__A63D6B8D_2A36_4159_9063_0B5035B11D95__INCLUDED_)
