#if !defined(AFX_SELDBDRVDLG_H__71AD0047_62FE_4FB9_904A_1C4868E1BA15__INCLUDED_)
#define AFX_SELDBDRVDLG_H__71AD0047_62FE_4FB9_904A_1C4868E1BA15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelDBDrvDlg.h : header file
//

#include "ConnStringParser.h"
/////////////////////////////////////////////////////////////////////////////
// CSelDBDrvDlg dialog

class CSelDBDrvDlg : public CDialog
{
// Construction
public:
	CSelDBDrvDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelDBDrvDlg)
	enum { IDD = IDD_DIALOG_SELDBDRV };
	CListBox	m_ctrlList;
	//}}AFX_DATA
	
	LPDBTYPEDATA GetCurSel();
	void SetConnectString(CConnStringParser &parser);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelDBDrvDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelDBDrvDlg)
	afx_msg void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELDBDRVDLG_H__71AD0047_62FE_4FB9_904A_1C4868E1BA15__INCLUDED_)
