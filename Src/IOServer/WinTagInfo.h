#pragma once


class CWinTagInfo :
	public CWinList
{
public:
	CWinTagInfo(void);
	~CWinTagInfo(void);
	//CDrvItem* m_pdrv;
	CString			m_szDecName;
	//CDecValMan*		m_pvalman;

	CSortObArray	m_array;	//标签数组CTagItem* 数组,用于保存查询结果

	static CString GetQualityText(CVarVal *pval);
protected:
	CBitmap	m_bmpOK,m_bmpRDB,m_bmpDrv,m_bmpAll;
	CVarVal m_val;//值
public:
	int		GetNextSelTag(int nstart);
	int		GetNextSelected(int nstart);//返回选择
	void	ReDraw();
	void	RedrawList();
	void	QueryTag(LPCTSTR stagname,LPCTSTR sproxytagname,LPCTSTR sdes,LPCTSTR sdrvtagname,int ntype);//查询标签

	CString GetText(int nr,int nc);

	BOOL	ExportSelTags(CCSVFile* pf,int *pnExport);//导出选择标签
	BOOL	ExportRdbSelTags(CCSVFile* pf,int *pnExport,int nsel,LPCTSTR sdbname);//导出选择标签为实时库可用标签表
	int		ExportDDSTags(CCSVFile* pf,int *pnExport,int nsel);//导出DDS标签,nsel = 0表示全部

	BOOL	ExportTags(CCSVFile* pf,int *pnExport);//导出标签

	BOOL	GetTag(int nIndex,CTagItem *pi); //复制方式
	BOOL	UpdateTag(int nIndex,CTagItem *pi);

protected:
	virtual CString GetCellText(int nrow,int ncol);//取显示字符串
	virtual CString GetHeadText(int ncol);//取显示字符串
	virtual void	GetRowColorS(int nrow,int ncol,COLORREF &clrTxt,COLORREF &clrBk);//取行颜色
	virtual void	OnClickHeadCol(int ncol);//鼠标点击表头

	virtual void	OnCurSelChange(int nrow);
	virtual void	OnSelChange(int nrowstart,int nrowend);
	virtual void	OnCtrlSelChange(int nrow);//CTRL按住时的选择

	virtual CBitmap* GetBitMap(int nrow);

	BOOL	IsSel(int nIndex);
	int		GetItemErrStatus(int nIndex);
	BOOL	SetSelXor(int nIndex); //相反设置
	BOOL	SetSel(int nIndex,BOOL bSel);
};

