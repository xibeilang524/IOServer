#if !defined(AFX_DLGCONFIG_H__F98175DA_7159_4673_BCC0_C5F22B163103__INCLUDED_)
#define AFX_DLGCONFIG_H__F98175DA_7159_4673_BCC0_C5F22B163103__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgConfig.h : header file
//
#include "modbus.h"
#include "modbusdrv.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgConfig dialog

class CDlgConfig : public CDialog
{
// Construction
public:
	CDlgConfig(CWnd* pParent = NULL);   // standard constructor
	CModBusCfg *m_pcfg;//≈‰÷√ ˝æ›
	CString m_szTitle;
//	void InitConfigData(CModConfig *ps);

	CImageList m_imagelist,m_imagelist2;
// Dialog Data
	//{{AFX_DATA(CDlgConfig)
	enum { IDD = IDD_DIALOG_CFG };
	CListCtrl	m_ListItem;
	CListCtrl	m_ListDec;
	CEdit	m_EditCom;
	CString	m_szComSet;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgConfig)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void LoadDec();
	void LoadItem(CModDec *pdec);
	// Generated message map functions
	//{{AFX_MSG(CDlgConfig)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonCom();
	afx_msg void OnButtonDel();
	afx_msg void OnButtonHelp();
	afx_msg void OnButtonExample();
	afx_msg void OnDblclkListDec(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonExport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCONFIG_H__F98175DA_7159_4673_BCC0_C5F22B163103__INCLUDED_)
