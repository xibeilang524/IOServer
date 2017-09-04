#pragma once
#include "afxcmn.h"


// CDbpArgAttrDlg 对话框

class CDbpArgAttrDlg : public CDialog
{
	DECLARE_DYNAMIC(CDbpArgAttrDlg)

public:
	CDbpArgAttrDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDbpArgAttrDlg();

	BOOL	m_bAdd;
	CString m_szip;

// 对话框数据
	enum { IDD = IDD_DIALOG_DBPARG_ATTE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CIPAddressCtrl m_ip;
	UINT m_wport;
	CString m_szuser;
	CString m_szpass;
	int m_nNetMode;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
