#if !defined(AFX_DLGOPCSET_H__8E24ACCE_0229_4891_988D_506ED2626055__INCLUDED_)
#define AFX_DLGOPCSET_H__8E24ACCE_0229_4891_988D_506ED2626055__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgOpcSet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgOpcSet dialog

class CDlgOpcSet : public CDialog
{
// Construction
public:
	CDlgOpcSet(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgOpcSet)
	enum { IDD = IDD_DIALOG_OPCSET };
	DWORD	m_dwUpdateRate;
	float	m_fPercentDead;
	CString	m_szMachine;
	CString	m_szProgID;
	int		m_nTimeArea;
	UINT	m_nUpdateTimeout;
	//}}AFX_DATA
private:
		void DisplayComponentCatList (HTREEITEM hParent, CATID clsid);
		void DisplayGeneralOPCServers (HTREEITEM hParent);

		CTreeCtrl *m_pServerList;
		CImageList m_cImageList;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgOpcSet)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgOpcSet)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedServerlist(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg void OnButtonConnect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGOPCSET_H__8E24ACCE_0229_4891_988D_506ED2626055__INCLUDED_)
