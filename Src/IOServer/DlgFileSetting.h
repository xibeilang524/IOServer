#pragma once


// CDlgFileSetting 对话框

class CDlgFileSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgFileSetting)

public:
	CDlgFileSetting(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgFileSetting();

// 对话框数据
	enum { IDD = IDD_DLG_FILE_SETTING };

	CString m_szFilePath;
	CString m_szFileName;
	BOOL m_bReplace;
	BOOL m_bUseFileOutput;
	long m_lInterval;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg void OnBnClickedBtnFileSetting();

	DECLARE_MESSAGE_MAP()
};
