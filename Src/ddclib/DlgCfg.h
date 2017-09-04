#if !defined(AFX_DLGCFG_H__4AE5296B_B94C_4AA4_813F_C7CED7FB3854__INCLUDED_)
#define AFX_DLGCFG_H__4AE5296B_B94C_4AA4_813F_C7CED7FB3854__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgCfg.h : header file
//

#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgCfg dialog

class CDlgCfg : public CDialog
{
// Construction
public:
	CDlgCfg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgCfg)
	enum { IDD = IDD_DIALOG_CFG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCfg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgCfg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCFG_H__4AE5296B_B94C_4AA4_813F_C7CED7FB3854__INCLUDED_)
