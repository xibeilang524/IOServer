#pragma once
#include "afxcmn.h"

class CDbpConArgs;

// CDbpArgConfigDlg 对话框

class CDbpArgConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(CDbpArgConfigDlg)

public:
	CDbpArgConfigDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDbpArgConfigDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_DBPARG_CONFIG };

	BOOL m_bModify;
	void SetDbpConArgs(CDBPArgs& dbpArgs);
	BOOL GetDbpConArgs(CDBPArgs& dbpArgs);
	//void SetDbpConArgs(CDbpConArgs *pDbpArgs);
	//BOOL GetDbpConArgs(CDbpConArgs *pDbpArgs);

protected:
	CImageList  m_ImageList;
	TDBPARG		m_DbpArgs[MAX_DBP_CONARGS]; //空间
	int			m_nSize; //参数个数


	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	CListCtrl m_ctrlDbpArgs;
	afx_msg void OnNMDblclkListArgs(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonEdit();
	afx_msg void OnBnClickedButtonDel();
protected:
	virtual void OnOK();
};
