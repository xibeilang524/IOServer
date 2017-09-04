//LogWinList.cpp

#include "stdafx.h"
#include "Resource.h"
#include "GeneralAPI.h"
#include "General\LogWinList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

TRUNK_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////
// CLogItemView
//////////////////////////////////////////////////////////////////////////

CLogItemView::CLogItemView()
:	m_bSel(FALSE),
	m_nLogLevel(0)
{
}

CLogItemView::~CLogItemView()
{

}

void CLogItemView::SetText(LPCTSTR lpszText)
{
	m_szText = lpszText;

	// [2011-03-30 14:39:11.421] [调试] jib(3) time = 0.087
	if (m_szText.GetLength() < 32)
		m_nLogLevel = -1;
	else {
		CString szt = m_szText.Mid(27, 4);
		if (szt == _T("提示"))
			m_nLogLevel = 2;
		else if (szt == _T("错误"))
			m_nLogLevel = 0;
		else if (szt == _T("警告"))
			m_nLogLevel = 1;
		else if (szt == _T("调试"))
			m_nLogLevel = 3;
		else
			m_nLogLevel = -1;
	}
}

//////////////////////////////////////////////////////////////////////////
// CLogWinList
//////////////////////////////////////////////////////////////////////////

CLogWinList::CLogWinList(LPCTSTR lpszTitle/* = NULL*/)
:	m_szTitle(lpszTitle ? lpszTitle : _T(""))
{
	//m_array.SetGrown(1024);

	memset(m_array, 0, sizeof(CLogItemView*) * LOG_LIST_MAX);
	m_nStart = 0;
	m_nCount = 0;

	m_nRowH = 18;//行高
	m_nTxtSize = 12;
	m_bGridLineH = TRUE;

	m_nCols = 1;
	SetColW(0, 1000);

	SetCellFmt(0,DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	m_bmpError.LoadBitmap(IDB_BITMAP_LOG_ERROR);
	m_bmpWarning.LoadBitmap(IDB_BITMAP_LOG_WARNING);
	m_bmpInfo.LoadBitmap(IDB_BITMAP_LOG_INFO);
	m_bmpDebug.LoadBitmap(IDB_BITMAP_LOG_DEBUG);

	// 不显示Grid线
	m_pengrid.DeleteObject();
	m_pengrid.CreatePen(PS_NULL, 0, m_bkclr);
}

CLogWinList::~CLogWinList()
{
	for(int i = 0; i < LOG_LIST_MAX; i++)
	{
		if (m_array[i])
			delete m_array[i];
	}
	memset(m_array, 0, sizeof(CLogItemView*));
}

void CLogWinList::AddLog(LPCTSTR lpszLogText)
{
	// 如果列表已经满了，则删除最先的一个
	if (m_nCount + 1 == LOG_LIST_MAX)
	{
		m_nStart++;
		m_nCount--;
	}

	int nPos = m_nStart + m_nCount;
	nPos = nPos % LOG_LIST_MAX;

	CLogItemView *pi = m_array[nPos];
	if (pi == NULL)
	{
		pi = new CLogItemView();
		m_array[nPos] = pi;
	}

	// 内存分配失败
	if (pi == NULL)
		return;
	
	pi->SetText(lpszLogText);
	m_nCount++;
}

CLogItemView *CLogWinList::GetLogItem(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_nCount)
		return NULL;

	int nPos = m_nStart + nIndex;
	nPos = nPos % LOG_LIST_MAX;

	return m_array[nPos];
}

void CLogWinList::Resize(CRect &rc)
{
	if(GetSafeHwnd() != NULL)
	{
		MoveWindow(rc);

		CSize sizesb,sizeClient;
		GetTrueClientSize(sizeClient, sizesb);
		SetColW(0, max(sizeClient.cx - 20, 100));
	}
}

void CLogWinList::SetSel(int nIndex)
{
	CLogItemView *pi = NULL;
	for (int i = 0; i < m_nCount; i++)
	{
		pi = GetLogItem(i);
		if (pi)
			pi->SetSelected( i == nIndex);
	}
}

CBitmap* CLogWinList::GetBitMap(int nrow)
{
	CLogItemView* pi = GetLogItem(nrow);
	if(pi == NULL)
		return NULL;

	switch(pi->GetLogLevel())
	{
	case 0:
		return &m_bmpError;
	case 1:
		return &m_bmpWarning;
	case 2:
		return &m_bmpInfo;
	case 3:
		return &m_bmpDebug;
	default:
		return NULL;
	}
}

CString CLogWinList::GetHeadText(int ncol)//取显示字符串
{
	return m_szTitle;
}

void CLogWinList::ReDraw(BOOL bSelLast/* = FALSE*/)
{
	CSize sizesb,sizeClient;
	GetTrueClientSize(sizeClient, sizesb);
	int pagerow = sizeClient.cy / m_nRowH; // 每页显示几行？

	if (pagerow >= m_nCount)
	{
		SetRowScrollTop(m_nCount);
		if (bSelLast)
			SetSel(m_nCount - 1);
		Invalidate();
	}
	else {
		SetRows(m_nCount);
		if (bSelLast)
			SetSel(m_nCount - 1);
		CalScrolBar();

		SCROLLINFO scinfo;
		memset(&scinfo, 0, sizeof(SCROLLINFO));
		scinfo.cbSize = sizeof(SCROLLINFO);
		scinfo.fMask = SIF_ALL;

		if(!GetScrollInfo(SB_VERT, &scinfo))
		{
			Invalidate();
			return;
		}

		int npage = (int)scinfo.nPage;

		m_scrollPos.y = m_nCount;
		if( m_scrollPos.y > scinfo.nMax -  npage)
			m_scrollPos.y = scinfo.nMax -  npage;
		SetScrollPos32(SB_VERT, m_scrollPos.y);
		Invalidate();
	}
}

void CLogWinList::GetRowColorS(int nrow,int ncol,COLORREF &clrTxt,COLORREF &clrBk)//取行颜色
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

CString CLogWinList::GetCellText(int nrow,int ncol)//取显示字符串
{
	CLogItemView* pi = GetLogItem(nrow);
	if(pi == NULL)
		return _T("");
	
	return pi->GetText();
}

void CLogWinList::OnClickHeadCol(int ncol)//鼠标点击表头
{
	return;
}

void CLogWinList::SortCol(int ncol,BOOL bAsc)
{
	return;
}

int	 CLogWinList::GetNextSelected(int nstart)//返回选择
{
	int i,ns = nstart;
	if (ns < 0) 
		ns = 0;

	CLogItemView *pi;
	for (i = ns; i < m_nCount; i++)
	{
		pi = GetLogItem(i);
		if (pi && pi->IsSelected())
			return i;
	}
	return -1;
}

BOOL CLogWinList::IsSel(int row)
{
	CLogItemView* pi = GetLogItem(row);
	if(pi == NULL)
		return FALSE;
	return pi->IsSelected();
}

void CLogWinList::SetSel(int row,BOOL bsel)
{
	CLogItemView* pi = GetLogItem(row);
	if(pi )
		pi->SetSelected(bsel);
}

BOOL CLogWinList::SetSelXor(int row)
{
	CLogItemView* pi = GetLogItem(row);
	if(pi )
	{
		pi->SetSelected( !pi->IsSelected() );
		return TRUE;
	}
	return FALSE;
}

void CLogWinList::OnCurSelChange(int nrow)
{
	int i;
	for(i = 0; i < m_nCount; i++)
	{
		if(i != nrow) 
			SetSel(i,FALSE);
		else
			SetSel(i,TRUE);
	}
	m_nCurSel = nrow;
	Invalidate();
}

void CLogWinList::OnCtrlSelChange(int nrow)//CTRL按住时的选择
{
	if(SetSelXor(nrow))
		Invalidate();
}

void CLogWinList::OnSelChange(int nrowstart,int nrowend)
{
	int i;

	for(i=0;i<m_nCount;i++)
	{
		if((i<nrowstart) || (i>nrowend))
			SetSel(i,FALSE);
	
		else
			SetSel(i,TRUE);
	}
	Invalidate();
}

CString	CLogWinList::GetItemText(int nrow,int ncol)
{
	CLogItemView* pi = GetLogItem(nrow);
	if(pi == NULL)
		return _T("");
	
	return pi->GetText();
}

TRUNK_NAMESPACE_END