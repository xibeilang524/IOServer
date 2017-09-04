#if !defined(AFX_DLGSELDRVTAG_H__5D89A754_B23B_40F8_AD7D_8F47C833223F__INCLUDED_)
#define AFX_DLGSELDRVTAG_H__5D89A754_B23B_40F8_AD7D_8F47C833223F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSelDrvTag.h : header file
//

class CDrvTagItem : public CSortItem//标签定义
{
public:
	CDrvTagItem();
	virtual ~CDrvTagItem();
public:
	virtual		int Compare(CSortItem *pOb);//排序比较
	void			Serialize(CArchive& ar);	//存盘
	void			CopyTo(CTagItem *pt);
	static  CString GetTypeStr(int ntype);
	static int		GetTypeVal(LPCTSTR lpszType);

public:
	int		m_ntype;	//数据类型
	CString m_szName;	//标签名
	CString m_szdes;	//描述
	CString m_szunit;	//单位

	BOOL	m_bSel;		//选择
	//}}
};
class CWinDrvTagList : public CWinList //标签信息窗口
{
public:
	CWinDrvTagList();
	virtual ~CWinDrvTagList();
public:
	CSortObArray	m_array;	//标签数组CDrvTagItem* 数组,用于保存查询结果

public:
	int		GetNextSelTag(int nstart);

	CString GetText(int nr,int nc);

	void	ReDraw();
	void	RedrawList();
protected:
	virtual CString GetCellText(int nrow,int ncol);//取显示字符串
	virtual CString GetHeadText(int ncol);//取显示字符串
	virtual void	GetRowColorS(int nrow,int ncol,COLORREF &clrTxt,COLORREF &clrBk);//取行颜色
	virtual void	OnClickHeadCol(int ncol);//鼠标点击表头

	virtual void	OnCurSelChange(int nrow);
	virtual void	OnSelChange(int nrowstart,int nrowend);
	virtual void	OnCtrlSelChange(int nrow);//CTRL按住时的选择


	BOOL	IsSel(int nIndex);
	int		GetItemErrStatus(int nIndex);
	BOOL	SetSelXor(int nIndex); //相反设置
	BOOL	SetSel(int nIndex,BOOL bSel);
};

/////////////////////////////////////////////////////////////////////////////
// CDlgSelDrvTag dialog

class CDlgSelDrvTag : public CDialog
{
// Construction
public:
	CDlgSelDrvTag(CWnd* pParent = NULL);   // standard constructor
	CWinDrvTagList m_wintag;
	CString m_szTagName;
	CString	m_szDrvName;
	void InitTag(CString szDrvName);
// Dialog Data
	//{{AFX_DATA(CDlgSelDrvTag)
	enum { IDD = IDD_DIALOG_SELDRVTAG };
	CStatic	m_stFrm;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSelDrvTag)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSelDrvTag)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	afx_msg LRESULT	OnDblClkWinList(WPARAM wParam, LPARAM lParam); //双击消息
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSELDRVTAG_H__5D89A754_B23B_40F8_AD7D_8F47C833223F__INCLUDED_)
