// ItemWinList.cpp: implementation of the CLeafItemWinList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "opcdrv.h"
#include "ItemWinList.h"

#include "..\sdk\HashTableUtils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CLeafItem
//////////////////////////////////////////////////////////////////////

CLeafItem::CLeafItem()
{
	m_bSel = FALSE;
	m_szItemName = _T("");
}

CLeafItem::~CLeafItem()
{

}
	
int CLeafItem::Compare(CSortItem *pOb)//排序比较
{	
	int nret = 0;
	if(pOb == NULL)
		return nret;
	CLeafItem *pItem = (CLeafItem *)pOb;
	
	switch(m_nSortWho)
	{
	case 0:  
		nret = strcmp(m_szItemName, pItem->m_szItemName);
		break;
	}
	if(!m_bAsc)
		nret *= -1;
	return nret;	
}

//////////////////////////////////////////////////////////////////////
// CLeafItemWinList
//////////////////////////////////////////////////////////////////////

CLeafItemWinList::CLeafItemWinList()
{
	m_array.SetGrown( 1024 * 8 );
	
	m_nRowH = 18;//行高
	m_nTxtSize = 12;
	m_bGridLineH = TRUE;
	
	m_nCols = 1;
	SetColW(0, 385); 
	SetCellFmt(0, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	m_biImage.LoadBitmap(IDB_BITMAP_OPCITEM);
}

CLeafItemWinList::~CLeafItemWinList()
{
	RemoveAll();
}

CString CLeafItemWinList::GetText(int nrow, int ncol)
{
	if (ncol != 0)
		return _T("");
	else  {
		CLeafItem *pItem = (CLeafItem*)m_array.GetAt(nrow);
		if (pItem)
			return pItem->m_szItemName;
		else
			return _T("");
	}
}

BOOL	CLeafItemWinList::IsSel(int nIndex)
{
	CLeafItem *pi = (CLeafItem*)m_array.GetAt(nIndex);
	if(!pi)
		return FALSE;
	return pi->m_bSel;
}

BOOL	CLeafItemWinList::SetSel(int nIndex,BOOL bSel)
{
	CLeafItem *pi = (CLeafItem*)m_array.GetAt(nIndex);
	if(pi)
	{
		pi->m_bSel = bSel;
		return TRUE;
	}
	return FALSE;
}

BOOL	CLeafItemWinList::SetSelXor(int nIndex) //相反设置
{
	CLeafItem *pi = (CLeafItem*)m_array.GetAt(nIndex);
	if(pi)
	{
		pi->m_bSel = !pi->m_bSel;
		return TRUE;
	}
	return FALSE;
}

CLeafItem *CLeafItemWinList::GetAt(int nIndex)
{
	return (CLeafItem*)m_array.GetAt(nIndex);
}

CLeafItem *CLeafItemWinList::AddItem(LPCTSTR lpszItemName)
{
	CLeafItem *pItem = new CLeafItem();
	pItem->m_szItemName = lpszItemName;
	m_array.Add(pItem);
	return pItem;
}

void	CLeafItemWinList::RemoveAt(int nIndex)
{
	CLeafItem *pItem = (CLeafItem*)m_array.RemoveAt(nIndex);
	if (pItem)
		delete pItem;
}

void	CLeafItemWinList::RemoveAll()
{
	m_array.DeleteObjects();
}

void CLeafItemWinList::ReDraw()
{
	SetRows( m_array.GetSize() );
	CalScrolBar();
	Invalidate();
}

//取显示字符串
CString CLeafItemWinList::GetCellText(int nrow, int ncol)
{
	if (ncol != 0)
		return _T("");
	else 
		return GetText(nrow, ncol);
}

//取显示字符串
CString CLeafItemWinList::GetHeadText(int ncol)
{
	if (ncol == 0)
		return _T("Item ID");
	else
		return _T("");
}

//取行颜色
void	CLeafItemWinList::GetRowColorS(int nrow,int ncol,COLORREF &clrTxt,COLORREF &clrBk)
{
	if(IsSel(nrow))
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

//鼠标点击表头
void	CLeafItemWinList::OnClickHeadCol(int ncol)
{
}

void	CLeafItemWinList::OnCurSelChange(int nrow)
{
	int i,n = m_array.GetSize();
	for(i=0;i<n;i++)
	{
		if(i != nrow) 
			SetSel(i,FALSE);
		else
			SetSel(i,TRUE);
	}
	m_nCurSel = nrow;
	this->Invalidate();
}

void	CLeafItemWinList::OnSelChange(int nrowstart,int nrowend)
{
	int i,n = m_array.GetSize();
	for(i=0;i<n;i++)
	{
		if((i<nrowstart) || (i>nrowend))
			SetSel(i,FALSE);
		
		else
			SetSel(i,TRUE);
	}
	this->Invalidate();
}

//CTRL按住时的选择
void	CLeafItemWinList::OnCtrlSelChange(int nrow)
{
	if(SetSelXor(nrow))
		this->Invalidate();
}

CBitmap* CLeafItemWinList::GetBitMap(int nrow)
{
	return &m_biImage;
}

//////////////////////////////////////////////////////////////////////
// CTagItem
//////////////////////////////////////////////////////////////////////

CTagItem::CTagItem()
{
	m_bSel = FALSE;
	m_szItemName = _T("");
	m_szDataType = _T("");
	m_szDesc = _T("");
	m_szUnit = _T("");
	m_bZeroFilter = FALSE;
}

CTagItem::~CTagItem()
{

}

int CTagItem::Compare(CSortItem *pOb)//排序比较
{	
	int nret = 0;
	if(pOb == NULL)
		return nret;
	CTagItem *pItem = (CTagItem *)pOb;
	
	switch(m_nSortWho)
	{
	case 0:  
		nret = strcmp(m_szItemName, pItem->m_szItemName);
		break;
	case 1:  
		nret = strcmp(m_szDataType, pItem->m_szDataType);
		break;
	case 2:  
		nret = strcmp(m_szDesc, pItem->m_szDesc);
		break;
	case 3:  
		nret = strcmp(m_szUnit, pItem->m_szUnit);
		break;
	}
	if(!m_bAsc)
		nret *= -1;
	return nret;	
}

//////////////////////////////////////////////////////////////////////
// CTagItemWinList
//////////////////////////////////////////////////////////////////////

CTagItemWinList::CTagItemWinList()
{
	m_array.SetGrown( 1024 * 8 );
	
	m_nRowH = 18;//行高
	m_nTxtSize = 12;
	m_bGridLineH = TRUE;
	
	m_nCols = 5;
	SetColW(0, 170); 
	SetColW(1, 60); 
	SetColW(2, 80); 
	SetColW(3, 60); 
	SetColW(4, 60); 

	SetCellFmt(0, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	SetCellFmt(1, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	SetCellFmt(2, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	SetCellFmt(3, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	SetCellFmt(4, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	
	m_biImage.LoadBitmap(IDB_BITMAP_OPCITEM);
	
	m_mapTagItems.InitHashTable( CalcHashTableSize(100000) );
}

CTagItemWinList::~CTagItemWinList()
{
	RemoveAll();	
}

CString CTagItemWinList::GetText(int nrow, int ncol)
{
	CTagItem *pItem = (CTagItem*)m_array.GetAt(nrow);
	if (!pItem)
		return _T("");

	switch(ncol)
	{
	case 0:
		return pItem->m_szItemName;
		break;
	case 1:
		return pItem->m_szDataType;
		break;
	case 2:
		return pItem->m_szDesc;
		break;
	case 3:
		return pItem->m_szUnit;
		break;
	case 4:
		return pItem->m_bZeroFilter ? _T("YES") : _T("NO");
		break;
	default:
		return _T("");
	}
}

BOOL	CTagItemWinList::IsSel(int nIndex)
{
	CTagItem *pi = (CTagItem*)m_array.GetAt(nIndex);
	if(!pi)
		return FALSE;
	return pi->m_bSel;
}

BOOL	CTagItemWinList::SetSel(int nIndex,BOOL bSel)
{
	CTagItem *pi = (CTagItem*)m_array.GetAt(nIndex);
	if(pi)
	{
		pi->m_bSel = bSel;
		return TRUE;
	}
	return FALSE;
}

BOOL	CTagItemWinList::SetSelXor(int nIndex) //相反设置
{
	CTagItem *pi = (CTagItem*)m_array.GetAt(nIndex);
	if(pi)
	{
		pi->m_bSel = !pi->m_bSel;
		return TRUE;
	}
	return FALSE;
}

CTagItem *CTagItemWinList::GetAt(int nIndex)
{
	return (CTagItem*)m_array.GetAt(nIndex);
}

CTagItem *CTagItemWinList::AddItem(LPCTSTR lpszItemName, LPCTSTR lpszDesc, LPCTSTR lpszUnit, LPCTSTR lpszDataType,BOOL bZeroFilter)
{
	CTagItem *pItem = new CTagItem();
	pItem->m_szItemName = lpszItemName;
	pItem->m_szDataType = lpszDataType;
	pItem->m_szDesc = lpszDesc;
	pItem->m_szUnit = lpszUnit;
	pItem->m_bZeroFilter = bZeroFilter;
	m_array.Add(pItem);
	CString szName = lpszItemName;
	szName.MakeUpper();
	m_mapTagItems.SetAt(szName, pItem);

	return pItem;
}

void	CTagItemWinList::RemoveAt(int nIndex)
{
	CTagItem *pItem = (CTagItem*)m_array.RemoveAt(nIndex);
	if (pItem)
	{
		pItem->m_szItemName.MakeUpper();
		m_mapTagItems.RemoveKey(pItem->m_szItemName);
		delete pItem;
	}
}

void	CTagItemWinList::RemoveAll()
{
	m_array.DeleteObjects();
	m_mapTagItems.RemoveAll();
}

void CTagItemWinList::ReDraw()
{
	SetRows( m_array.GetSize() );
	Invalidate();
}

CTagItem *CTagItemWinList::FindItem(LPCTSTR lpszItemName)
{
	CTagItem *pItem;
	CString szName = lpszItemName;
	szName.MakeUpper();

	if (m_mapTagItems.Lookup(szName, pItem))
		return pItem;

	return NULL;
}

//取显示字符串
CString CTagItemWinList::GetCellText(int nrow, int ncol)
{
	return GetText(nrow, ncol);
}

//取显示字符串
CString CTagItemWinList::GetHeadText(int ncol)
{
	switch(ncol)
	{
	case 0:
		return _T("Item ID");
		break;
	case 1:
		return _T("类型");
		break;
	case 2:
		return _T("描述");
		break;
	case 3:
		return _T("单位");
		break;
	case 4:
		return _T("零值过滤");
		break;
	default:
		return _T("");
		break;
	}
}

//取行颜色
void	CTagItemWinList::GetRowColorS(int nrow,int ncol,COLORREF &clrTxt,COLORREF &clrBk)
{
	if(IsSel(nrow))
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

//鼠标点击表头
void	CTagItemWinList::OnClickHeadCol(int ncol)
{
}

void	CTagItemWinList::OnCurSelChange(int nrow)
{
	int i,n = m_array.GetSize();
	for(i=0;i<n;i++)
	{
		if(i != nrow) 
			SetSel(i,FALSE);
		else
			SetSel(i,TRUE);
	}
	m_nCurSel = nrow;
	this->Invalidate();
}

void	CTagItemWinList::OnSelChange(int nrowstart,int nrowend)
{
	int i,n = m_array.GetSize();
	for(i=0;i<n;i++)
	{
		if((i<nrowstart) || (i>nrowend))
			SetSel(i,FALSE);
		
		else
			SetSel(i,TRUE);
	}
	this->Invalidate();
}

//CTRL按住时的选择
void	CTagItemWinList::OnCtrlSelChange(int nrow)
{
	if(SetSelXor(nrow))
		this->Invalidate();
}

CBitmap* CTagItemWinList::GetBitMap(int nrow)
{
	return &m_biImage;
}
