#pragma once
#include "SimdrvExobj.h"

// CConfigPage 对话框

class CConfigPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CConfigPage)

public:
	CConfigPage();
	virtual ~CConfigPage();
	CWinTagInfo m_wintag;
	CSimCfg* m_pcfg;//配置数据
	BOOL m_bModified;
	CStatic	m_stFrm;
	// Dialog Data
	//{{AFX_DATA(CDlgConfig)
	enum { IDD = IDD_DIALOG_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	BOOL AddInPortTag(LPCTSTR sz0,LPCTSTR sz1,LPCTSTR sz2,LPCTSTR sz3,LPCTSTR sz4,LPCTSTR sz5,LPCTSTR sz6);
	// Generated message map functions
	//{{AFX_MSG(CDlgConfig)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonDelsel();
	afx_msg void OnButtonExport();
	afx_msg void OnButtonInport();
	afx_msg void OnButtonExsamp();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonEdit();
	//}}AFX_MSG
	afx_msg LRESULT	OnDblClkWinList(WPARAM wParam, LPARAM lParam); //双击消息
	DECLARE_MESSAGE_MAP()
public:;
	  afx_msg void OnBnClickedButtonAcce();
};
