#pragma once


// DlgAcceCfg 对话框

class CDlgAcceCfg : public CDialog
{
	DECLARE_DYNAMIC(CDlgAcceCfg)

public:
	CDlgAcceCfg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgAcceCfg();

// 对话框数据
	enum { IDD = IDD_DIALOG_ACCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_span;
	int m_timeincr;
	COleDateTime m_starttime;
	COleDateTime m_endtime;
	BOOL m_bAllowAcce;
};
