#if !defined(AFX_DLGOPCVAR_H__B6CB41D6_6E09_451B_BE9F_DC492FD326E3__INCLUDED_)
#define AFX_DLGOPCVAR_H__B6CB41D6_6E09_451B_BE9F_DC492FD326E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgOpcVar.h : header file
//

#include "ItemWinList.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgOpcVar dialog

class CDlgOpcVar : public CDialog
{
// Construction
public:
	CDlgOpcVar(LPCTSTR lpszProgID,IOPCItemMgt *m_pIItemMgt,IOPCBrowseServerAddressSpace *pIBrowse, CWnd *pParent = NULL);   // standard constructor
	CString m_sztitle;
	IOPCItemProperties *m_piItemProp;	//数据项的属性查询接口 addby jy 20091221
// Dialog Data
	//{{AFX_DATA(CDlgOpcVar)
	enum { IDD = IDD_DIALOG_VAR };
	CString	m_strItemID;
	int		m_nDataType;
	int		m_nListDataType;
	int		m_nReadMode;
	int		m_nLangage;
	int		m_nOpcTime;
	//}}AFX_DATA
protected:
		BOOL m_bModified;
		BOOL m_bBrowseFlat;

		CString m_szProgID;	// ProgID
		IOPCBrowseServerAddressSpace *m_pIBrowse; // 可选的，用于浏览服务器中有用组员的数据 在CKServer
		
		IOPCItemMgt *m_pIItemMgt;	// item管理接口,在CKGroup中

		CString	m_strFilterBranch;
		CString	m_strFilterLeaf;
	
		VARTYPE m_vtFilterType;
		DWORD m_dwFilterAccessRights;
		OPCNAMESPACETYPE m_cOpcNameSpace;

		CTreeCtrl *m_pBranchList;
		CImageList m_cBranchImageList;
		CImageList m_cLeafImageList;

		CLeafItemWinList	m_ctrlLeafList;
		CTagItemWinList		m_ctrlItemList;

		void InitializeBrowser ();
		void BrowseRootLevel ();
		void BrowseToRoot ();

		void AddBranches (LPENUMSTRING pIEnumString, HTREEITEM hParent, DWORD dwData);
		void AddLeaves (LPENUMSTRING pIEnumString);
		void AddDummyBranch (HTREEITEM hParent);
		void SelectBranch (HTREEITEM hItem);
		void ExpandBranch (HTREEITEM hItem);
		void DeleteChildBranches (HTREEITEM hParent);
		void RemoveDummyBranch (HTREEITEM hParent);

		BOOL AddItem(LPCTSTR lpszItem,int nDataType, LPCTSTR lpszDesc, LPCTSTR lpszUnit, BOOL bZeroFilter,BOOL bModifyType = FALSE);// 添加到List
		BOOL AddItemInPort(LPCTSTR lpszItem,LPCTSTR lpszDes,LPCTSTR lpszUnit,int nDataType,BOOL bZeroFilter,BOOL bModifyType = FALSE);// 添加到List
		
public:
	void SetDrv(COpcDrv* pDrv){m_pDrv=pDrv;};
	BOOL IsModify(){return m_bModified;};
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgOpcVar)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	COpcDrv* m_pDrv;
	void DoDblclkLeaflist(int nRow);
	void DoDblclkListVar(int nRow);

	void OnDblclkLeaflist(NMHDR* pNMHDR, LRESULT* pResult);
	void OnDblclkListVar(NMHDR* pNMHDR, LRESULT* pResult);
	// Generated message map functions
	//{{AFX_MSG(CDlgOpcVar)
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnSelchangedBranchlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemexpandingBranchlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	afx_msg void OnDel();
	afx_msg void OnAddsel();
	afx_msg void OnAddone();
	afx_msg void OnButtonTypelist();
	afx_msg void OnButtonLoadtxt();
	afx_msg void OnButtonExporttxt();
	afx_msg void OnButtonExportdemo();
	afx_msg void OnButtonHelp();
	//}}AFX_MSG
	afx_msg LRESULT OnDblclkWinList(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGOPCVAR_H__B6CB41D6_6E09_451B_BE9F_DC492FD326E3__INCLUDED_)
