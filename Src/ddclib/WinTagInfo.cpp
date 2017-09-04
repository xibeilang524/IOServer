// WinTagInfo.cpp: implementation of the CWinTagInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WinList.h"
#include "ddclib.h"
#include "WinTagInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWinTagInfo::CWinTagInfo()
{
	m_nRowH = 18;//行高
	m_nTxtSize = 12;
	m_bGridLineH = TRUE;

	m_nCols = FIELD_LAST + CTagItem::GetCustomFieldCount() + 1;
	SetColW(0, 80); //FIELD_ID
	SetColW(1, 160); //FIELD_NAME
	SetColW(2, 200); //FIELD_DES
	SetColW(3, 80); //FIELD_UNIT
	SetColW(4, 80);//FIELD_TYPE
	for(int i = 0; i < CTagItem::GetCustomFieldCount(); i++)
	{
		CCustomField &field = CTagItem::GetCustomField(i);
		SetColW(FIELD_LAST + i, field.m_nColWidth);		
		SetCellFmt(FIELD_LAST + i, field.m_dwAlignFormat);
	}
	SetColW(FIELD_LAST + CTagItem::GetCustomFieldCount(), 150); //FIELD_NOTE

	
	SetCellFmt(0, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	SetCellFmt(1, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	SetCellFmt(2, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	SetCellFmt(3, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	SetCellFmt(4, DT_LEFT|DT_VCENTER|DT_SINGLELINE);

	SetCellFmt(FIELD_LAST + CTagItem::GetCustomFieldCount(),DT_LEFT|DT_VCENTER|DT_SINGLELINE);

	m_pTagtab = NULL;
}

CWinTagInfo::~CWinTagInfo()
{

}

CString CWinTagInfo::GetHeadText(int ncol)//取显示字符串
{
	CString szt= "";
	switch(ncol)
	{
	case 0://FIELD_ID
		szt = "ID";
		break;
	case 1://FIELD_NAME
		szt = "标签名";
		break;
	case 2://FIELD_DES
		szt = "描述";
		break;
	case 3://FIELD_UNIT
		szt = "单位";
		break;
	case 4://FIELD_TYPE
		szt = "类型";
		break;
	default:
		if (ncol == FIELD_LAST + CTagItem::GetCustomFieldCount())
			szt = "备注";
		else if (ncol < FIELD_LAST + CTagItem::GetCustomFieldCount() && ncol > 4)
		{
			int nIndex = ncol - FIELD_LAST;
			CCustomField &field = CTagItem::GetCustomField(nIndex);
			szt = field.m_szDisplayName;
		}
		break;
	}
	return szt;
}

void CWinTagInfo::ReDraw()
{
	SetRows(m_pTagtab->GetSize());
	this->Invalidate();
}

void	CWinTagInfo::GetRowColorS(int nrow,int ncol,COLORREF &clrTxt,COLORREF &clrBk)//取行颜色
{
	if(m_pTagtab->IsSelected(nrow))
	{
		clrTxt = RGB(255,255,255);
		clrBk = RGB(0,0,128);
		if(this != GetFocus())
		{
			clrBk = ::GetSysColor(COLOR_BTNFACE);
			clrTxt = RGB(0,0,0);
		}
		return;
	}
	else
	{
		clrTxt = RGB(0,0,0);
		clrBk =  RGB(255,255,255);
	}
}

CString CWinTagInfo::GetCellText(int nrow,int ncol)//取显示字符串
{
	return m_pTagtab->GetText(nrow,ncol);
}

void	CWinTagInfo::OnClickHeadCol(int ncol)//鼠标点击表头
{
	if(ncol > FIELD_LAST + CTagItem::GetCustomFieldCount())
		return;
	
	if(ncol == m_nSortCol)
		m_bSortAsc = !m_bSortAsc;
	m_nSortCol = ncol;
	m_pTagtab->m_oaTag.SetSortType(m_nSortCol,m_bSortAsc);//设置排序
	m_pTagtab->m_oaTag.Qsort();

	SetSortCol(m_nSortCol,m_bSortAsc);//设置表头显示
	Invalidate();
}

int	 CWinTagInfo::GetNextSelected(int nstart)//返回选择
{
	CTagItem *pi;
	int i,ns = nstart,n = m_pTagtab->m_oaTag.GetSize();
	if(ns < 0) ns = 0;
	for(i=ns;i<n;i++)
	{
		pi = (CTagItem	*)m_pTagtab->m_oaTag.GetAt(i);
		if(pi->m_bSel)
			return i;
	}
	return -1;
}

void	CWinTagInfo::OnCurSelChange(int nrow)
{
	CTagItem *pi;
	int i,n = m_pTagtab->m_oaTag.GetSize();
	for(i=0;i<n;i++)
	{
		pi = (CTagItem	*)m_pTagtab->m_oaTag.GetAt(i);
		if(i != nrow) 
			m_pTagtab->SetSel(i,FALSE);
		else
			m_pTagtab->SetSel(i,TRUE);
	}
	m_nCurSel = nrow;
	this->Invalidate();
}
void	CWinTagInfo::OnCtrlSelChange(int nrow)//CTRL按住时的选择
{
	CTagItem *pi;
	pi = (CTagItem *)m_pTagtab->m_oaTag.GetAt(nrow);
	if(!pi)
		return;
	pi->m_bSel = !(pi->m_bSel);
	this->Invalidate();
}

void	CWinTagInfo::OnSelChange(int nrowstart,int nrowend)
{
	CTagItem *pi;
	int i,n = m_pTagtab->m_oaTag.GetSize();
	for(i=0;i<n;i++)
	{
		pi = (CTagItem	*)m_pTagtab->m_oaTag.GetAt(i);
		if((i<nrowstart) || (i>nrowend))
			pi->m_bSel = FALSE;
		else
			pi->m_bSel = TRUE;
	}
	this->Invalidate();
}

void	CWinTagInfo::RedrawList()
{
	m_pTagtab->m_oaTag.Qsort();
	SetRowScrollTop(m_pTagtab->m_oaTag.GetSize());
}
