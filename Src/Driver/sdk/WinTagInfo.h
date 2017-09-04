// WinTagInfo.h: interface for the CWinTagInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINTAGINFO_H__50A4EBA6_5F5E_4D93_9371_A6479675C59F__INCLUDED_)
#define AFX_WINTAGINFO_H__50A4EBA6_5F5E_4D93_9371_A6479675C59F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWinTagInfo : public CWinList  
{
public:
	CWinTagInfo();
	virtual ~CWinTagInfo();
	
public:
	CTagDefTab	*m_pTagtab;	//标签表

public:
	int		GetNextSelected(int nstart);//返回选择
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
};

#endif // !defined(AFX_WINTAGINFO_H__50A4EBA6_5F5E_4D93_9371_A6479675C59F__INCLUDED_)
