#pragma once


// CDlgPIServerItem 对话框

class CDlgPIServerItem : public CDialog
{
	DECLARE_DYNAMIC(CDlgPIServerItem)

public:
	CDlgPIServerItem(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgPIServerItem();

	int		m_nConnTimeout;
	int		m_nDATimeout;
	CString	m_szIP;
	CString	m_szPort;
	CString	m_szPassword;
	CString	m_szUserName;
// 对话框数据
	enum { IDD = IDD_DIALOG_PISERVER_ITEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonTest();
protected:
	virtual void OnOK();
};
