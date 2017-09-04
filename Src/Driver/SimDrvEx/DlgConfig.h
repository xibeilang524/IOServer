#if !defined(AFX_DLGCONFIG_H__679F4AAD_3E88_4DDB_B2DF_57A30394F718__INCLUDED_)
#define AFX_DLGCONFIG_H__679F4AAD_3E88_4DDB_B2DF_57A30394F718__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgConfig.h : header file
//
#include "SimdrvExobj.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgConfig dialog

class CDlgConfig : public CDialog
{
// Construction
public:
	CDlgConfig(CWnd* pParent = NULL);   // standard constructor
	CWinTagInfo m_wintag;
	CSimCfg* m_pcfg;//配置数据
	BOOL m_bModified;
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
	BOOL AddInPortTag(LPCTSTR sz0,LPCTSTR sz1,LPCTSTR sz2,LPCTSTR sz3,LPCTSTR sz4,LPCTSTR sz5,LPCTSTR sz6);
	// Generated message map functions
	//{{AFX_MSG(CDlgConfig)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnButtonDelsel();
	afx_msg void OnButtonExport();
	afx_msg void OnButtonInport();
	afx_msg void OnButtonExsamp();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonEdit();
	//}}AFX_MSG
	afx_msg LRESULT	OnDblClkWinList(WPARAM wParam, LPARAM lParam); //双击消息
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonAcce();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCONFIG_H__679F4AAD_3E88_4DDB_B2DF_57A30394F718__INCLUDED_)
