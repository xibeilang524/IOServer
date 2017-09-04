// WinList.cpp : implementation file
//
/*
  编码：蒋勇
  日期：2009.4.5

  */

#include "stdafx.h"
#include "GeneralAPI.h"
#include "General\WinList.h"
#include "General\MemDC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning(disable : 4311) // 类型转换警告

TRUNK_NAMESPACE_BEGIN

/////////////////////////////////////////////////////////////////////////////
// CWinList

CWinList::CWinList()
{
	m_nRows=0;//行数
	m_nPreRows = 0;
	m_nCols=0;//列数
	
	m_nRowH = 20;//行高
	for(int i=0;i<MAXWINLISTCOLS;i++)
	{
		m_nColW[i] = 80;
		m_nColFmt[i] = DT_CENTER|DT_VCENTER|DT_SINGLELINE;//列显示格式
	}

	m_logpen.lopnColor = ::GetSysColor(COLOR_3DSHADOW);
	m_logpen.lopnStyle = PS_SOLID;
	m_logpen.lopnWidth.x = 0;
	m_logpen.lopnWidth.y = 0;

	m_pengrid.CreatePenIndirect(&m_logpen);

	m_logpen.lopnColor = RGB(0,0,0);
	m_pendrog.CreatePenIndirect(&m_logpen);

	m_LastSelType = AT_NONE;//上次选择类型
	m_nMinColW = 16;//最小行列尺寸

	m_scrollPos   = 0;
	m_nCurSel = -1;

	m_bkclr = RGB(255,255,255);
	
	m_bGridLineH = FALSE;

	m_nTxtSize = 14;

	m_nSortCol = -1;//排序列,-1表示无
	m_bSortAsc = TRUE;
}

CWinList::~CWinList()
{
}

CString CWinList::GetCellText(int nrow,int ncol)//取显示字符串
{
	CString szt;
	szt.Format(_T("%d,%d"),nrow,ncol);
	return szt;
}
CString CWinList::GetHeadText(int ncol)//取显示字符串
{
	CString szt;
	szt.Format(_T("head%d"),ncol);
	return szt;
}

void	CWinList::GetRowColorS(int nrow,int ncol,COLORREF &clrTxt,COLORREF &clrBk)//取行颜色
{
	if(nrow == m_nCurSel)
	{
		clrTxt = RGB(255,255,255);
		clrBk = RGB(0,0,255);
		return;
	}
	if(!(nrow % 2))
	{
		clrTxt = RGB(0,0,0);
		clrBk = RGB(255,255,255);
	}
	else
	{
		clrTxt = RGB(0,0,0);
		clrBk = RGB(255,255,255);
	}
}
void	CWinList::OnCurSelChange(int nrow)
{
	if(m_nCurSel != nrow)
	{
		m_nCurSel = nrow;
		this->Invalidate();
	}
}

void	CWinList::DrawSortFlag(CDC *pDC,CRect &rtpos)//绘制排序标志
{
	COLORREF oldclr = pDC->SetTextColor(RGB(255,255,255));
	
	CString szflag = "▲";
	if(!m_bSortAsc)
		szflag=  "▼";
	pDC->DrawText(szflag,&rtpos,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	pDC->SetTextColor(RGB(0,0,0));
	rtpos.left+=1;
	rtpos.top +=2;
	pDC->DrawText(szflag,&rtpos,DT_LEFT|DT_VCENTER|DT_SINGLELINE);

	
	pDC->SetTextColor(oldclr);
	
}

void	CWinList::DrawHead(CDC *pDC,CRect &rtdraw)
{
	if(m_nCols == 0)
		return;

	int i,n=0;
	CRect rt,rtt;
	COLORREF bkclr = ::GetSysColor(COLOR_3DFACE);//RGB(220,220,220);
	CPen *oldpen=pDC->SelectObject(&m_pengrid);
	COLORREF clrbk=pDC->SetTextColor(RGB(0,0,0));
	CFont *oldfnt = pDC->SelectObject(&m_fntText);
	int olbbkmode = pDC->SetBkMode(TRANSPARENT);

	for(i=0;i<m_nCols;i++)
	{
		rt.left = rtdraw.left - m_scrollPos.x + n;
		if(rt.left > rtdraw.right)
			break;
		rt.right = rt.left + m_nColW[i];
		if(rt.right < rtdraw.left)
		{
			n+=m_nColW[i];
			continue;
		}
		rt.top = rtdraw.top;
		rt.bottom = rt.top + m_nRowH;
		pDC->FillSolidRect(&rt,bkclr);
		pDC->DrawEdge(&rt,EDGE_RAISED,BF_RECT);
		
		rtt=rt;
		if(this->m_nSortCol == i)
		{
			rtt.DeflateRect(16,2,2,2);
			pDC->DrawText(GetHeadText(i),&rtt,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			rtt.left -= 14;
			DrawSortFlag(pDC,rtt);
		}
		else
		{
			rtt.DeflateRect(2,2,2,2);
			pDC->DrawText(GetHeadText(i),&rtt,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		}

		n+=m_nColW[i];
	}
	if(rt.right < rtdraw.right)
	{
		rt.left = rtdraw.left + n;
		rt.right = rtdraw.right;

		pDC->FillSolidRect(&rt,bkclr);
		pDC->DrawEdge(&rt,EDGE_RAISED,BF_RECT);
	}
	pDC->SelectObject(oldpen);
	pDC->SetTextColor(clrbk);
	pDC->SelectObject(oldfnt);
	pDC->SetBkMode(olbbkmode);
}

void	CWinList::GetViewRows(int &nstart,int &nend)//取可见区域起止行
{
	nstart = -1;
	nend = -1;
	if(m_nRows == 0)
		return;

	CRect rtdraw;
	GetClientRect(&rtdraw);

	nstart = m_scrollPos.y;
	CRect rt;
	for(int j=m_scrollPos.y;j<m_nRows;j++)
	{
		rt.top = rtdraw.top + (j+1 - m_scrollPos.y) * m_nRowH;
		if(rt.top > rtdraw.bottom)
		{
			nend = j;
			return;
		}
	}
	nend = m_nRows-1;
}

void	CWinList::DrawCell(CDC *pDC,CRect &rtdraw)
{
	if(m_nRows == 0)
		return;
	CBitmap* pbitmap=NULL;
	int i, j,n=0;
	CRect rt,rtt;
	COLORREF bkclr = RGB(255,255,255);
	CPen *oldpen=pDC->SelectObject(&m_pengrid);
	COLORREF oldclrbk=pDC->SetTextColor(RGB(0,0,0));
	CFont *oldfnt = pDC->SelectObject(&m_fntText);
	int olbbkmode = pDC->SetBkMode(TRANSPARENT);
	COLORREF clrTxt,clrBk;

	for(j=m_scrollPos.y;j<m_nRows;j++)
	{
		n=0;
		rt.top = rtdraw.top + (j+1 - m_scrollPos.y) * m_nRowH;
		if(rt.top > rtdraw.bottom)
			break;
		

		rt.bottom = rt.top + m_nRowH;
		for(i=0;i<m_nCols;i++)
		{
			GetRowColorS(j,i,clrTxt,clrBk);
			rt.left = rtdraw.left  - m_scrollPos.x + n;
			if(rt.left > rtdraw.right)
				break;
			rt.right = rt.left + m_nColW[i];
			if(rt.right < rtdraw.left)
			{
				n+=m_nColW[i];
				continue;
			}
			rtt=rt;
			rtt.DeflateRect(1,1,0,0);
			
			pDC->SetTextColor(clrTxt);
			pDC->FillSolidRect(&rtt,clrBk);
			pbitmap = GetBitMap(j);
			rtt.DeflateRect(1,1,2,2);
			if((pbitmap == NULL)||(i>0))
			{
				//pDC->DrawText(GetCellText(j,i),&rtt,GetCellFmt(i));

				CString szdraw = GetCellText(j,i);
				szdraw.Replace("&","&&");
				pDC->DrawText(szdraw,&rtt,GetCellFmt(i));
			}
			else
			{
				CDC dcMemory;
				dcMemory.CreateCompatibleDC(pDC);
				CBitmap* pOldBitmap = dcMemory.SelectObject(pbitmap);
				BITMAP bmpInfo;
				pbitmap->GetBitmap(&bmpInfo);
				pDC->BitBlt(rtt.left,rtt.top, bmpInfo.bmWidth, bmpInfo.bmHeight, &dcMemory,0, 0, SRCCOPY);
				dcMemory.SelectObject(pOldBitmap);
				rtt.left += bmpInfo.bmWidth + 2;
				//pDC->DrawText(GetCellText(j,i),&rtt,GetCellFmt(i));
				CString szdraw = GetCellText(j,i);
				szdraw.Replace("&","&&");
				pDC->DrawText(szdraw,&rtt,GetCellFmt(i));
			}

			pDC->MoveTo(rt.left,rt.top);
			pDC->LineTo(rt.left,rt.bottom);
			if(m_bGridLineH)
			{
				pDC->LineTo(rt.right,rt.bottom);
				if(i == m_nCols-1)
					pDC->LineTo(rt.right,rt.top);
			}
			n+=m_nColW[i];
			
		}

		if(rt.right < rtdraw.right)
		{
			rt.left = rtdraw.left + n;
			rt.right = rtdraw.right;
	
			pDC->MoveTo(rt.left,rt.top);
			pDC->LineTo(rt.left,rt.bottom);
			if(m_bGridLineH)
				pDC->LineTo(rt.right,rt.bottom);

		}
	}
	if(j==m_nRows)
	{
		pDC->MoveTo(rt.left,rt.top);
		pDC->LineTo(rt.left,rt.bottom);
		if(m_bGridLineH)
			pDC->LineTo(rt.right,rt.bottom);
	}
	pDC->SelectObject(oldpen);
	pDC->SetTextColor(oldclrbk);
	pDC->SelectObject(oldfnt);
	pDC->SetBkMode(olbbkmode);
}

void CWinList::DrawAdJust(CDC *pDC,CRect &rtdraw)//绘制调整
{
	if(m_LastSelType != AT_ADJUSTW) 
		return;

	if(m_LastSelType == AT_ADJUSTW){

		CPoint pt1 = m_ptAdjust;
		CPoint pt2 = m_ptAdjust;
		pt1.x -= m_scrollPos.x;
		pt2.x -= m_scrollPos.x;
		pt1.y = rtdraw.top;
		pt2.y = rtdraw.bottom;
		CPen *oldpen = pDC->SelectObject(&m_pendrog);
		pDC->MoveTo(pt1);
		pDC->LineTo(pt2);

		pDC->SelectObject(oldpen);
	}

}

BEGIN_MESSAGE_MAP(CWinList, CWnd)
	//{{AFX_MSG_MAP(CWinList)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_MOUSELEAVE,   OnMouseLeave)  
	ON_MESSAGE(WM_MOUSEHOVER,   OnMouseHover) 
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWinList message handlers

void CWinList::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CWnd::OnPaint() for painting messages
	if(m_nRows != m_nPreRows) //行数改变
	{
		CalScrolBar();
		m_nPreRows = m_nRows;
	}
	COLORREF oldclr = dc.SetBkColor(m_bkclr);
	CMemDC mDC(&dc);

	CRect rectc;
	GetClientRect(&rectc);

	DrawHead(&mDC,rectc);
	DrawCell(&mDC,rectc);
	DrawAdJust(&mDC,rectc);
	
	dc.SetBkColor(oldclr);
}

BOOL CWinList::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
//	return CWnd::OnEraseBkgnd(pDC);
}

BOOL CWinList::ClearSelected(BOOL bReset)//清除选择，选择复位m_rtSel,m_LastSelType返回是否需要刷新界面
{
	if(bReset){
		m_LastSelType = AT_NONE;//上次选择类型
	}
	this->m_nCurSel = -1;
	return TRUE;
}

void CWinList::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	int row = -1,col = -1;
	int	nret = HitTest(point,row,col);

	if(nFlags & MK_LBUTTON){//鼠标按下

		if(m_LastSelType == AT_ADJUSTW){
			CPoint pt = point;
			int n=0;
			for(int i=0;i<m_ptFirstCell.x;i++)
				n+=m_nColW[i];

			if( pt.x + m_scrollPos.x < m_nMinColW + n ) 
				return;
			m_ptAdjust.x = pt.x  + m_scrollPos.x;
			m_ptAdjust.y = 0;

			Invalidate(FALSE);
			return;
		}
		return;
	}

	::SetClassLong(GetSafeHwnd(),GCL_HCURSOR,NULL);
	if(nret == AT_ADJUSTW){
		::SetClassLong(GetSafeHwnd(),GCL_HCURSOR,
				(LONG)(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE)));
		return;
	}

	::SetClassLong(GetSafeHwnd(),GCL_HCURSOR,
				(LONG)(AfxGetApp()->LoadStandardCursor(IDC_ARROW)));

	TRACKMOUSEEVENT  tme;  
 	tme.cbSize  =  sizeof(TRACKMOUSEEVENT);  
	tme.dwFlags  =  TME_LEAVE|TME_HOVER;
	tme.hwndTrack  =  GetSafeHwnd();  //  目标窗口句柄  
	tme.dwHoverTime  =  1000;      //  时间
	BOOL bret = _TrackMouseEvent(&tme);  

	OnMouseAt(point.x,point.y,row,col);
	//CWnd::OnMouseMove(nFlags, point);
}

long   CWinList::OnMouseLeave(WPARAM   wParam,   LPARAM   lParam)   
{   
	CPoint pt;
	pt.x   =   LOWORD(lParam);   
	pt.y   =   HIWORD(lParam);  
	OnViewToolBar(pt.x,pt.y,-1,-1,FALSE);

	::SetClassLong(GetSafeHwnd(),GCL_HCURSOR,NULL);
	::SetClassLong(GetSafeHwnd(),GCL_HCURSOR,
				(LONG)(AfxGetApp()->LoadStandardCursor(IDC_ARROW)));

	return   0;   
}   

long   CWinList::OnMouseHover(WPARAM   wParam,   LPARAM   lParam)   
{   
	CPoint pt;
	pt.x   =   LOWORD(lParam);   
	pt.y   =   HIWORD(lParam);  
	
	int row = -1,col = -1;
	int	nret = HitTest(pt,row,col);
	if((row != -1)&&(col!= -1))
		OnViewToolBar(pt.x,pt.y,row,col,TRUE);
	return  0;   
}   
void CWinList::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (GetFocus() != this) 
		SetFocus();

	int row = -1,col = -1;
	
	int nret = HitTest(point,row,col);

	if(nret == AT_NONE){ //没有选择
		if(ClearSelected(TRUE))
			Invalidate(FALSE);
		return;
	}
	else if(nret == AT_HEAD)
	{
		OnClickHeadCol(col);
		return;
	}
	else if(nret == AT_ADJUSTW){//调整列宽
		CRect rt;
		
		m_LastSelType = AT_ADJUSTW;
		m_ptAdjustOrg.x=0 ;//+ m_scrollPos.x;
		for(int i=0;i<=col;i++)
			m_ptAdjustOrg.x+=m_nColW[i];
		
		m_ptAdjustOrg.y = 0;

		m_ptAdjust = m_ptAdjustOrg;

		m_ptFirstCell.x = col;//被调整的列
		m_ptFirstCell.y = 0;
		
		Invalidate(FALSE);//
		return;
	}
	else if(nret == AT_SELROW)
	{
		if((nFlags & MK_SHIFT) != 0)//多选
		{
			if(m_nCurSel == -1)
				OnCurSelChange(row);
			else
			{
				if(m_nCurSel >= row)
				{
					OnSelChange(row,m_nCurSel);
				//	m_nCurSel = row;
				}
				else
					OnSelChange(m_nCurSel,row);
			}
		}
		else if((nFlags & MK_CONTROL) != 0)
		{
			OnCtrlSelChange(row);
		}
		else
		{
			if(col == 0)
				OnClickRowCheck(row);
			OnCurSelChange(row);
		}
	}
	CWnd::OnLButtonDown(nFlags, point);
}

void CWinList::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	if(m_LastSelType == AT_ADJUSTW){
		int nNewW = m_ptAdjust.x - m_ptAdjustOrg.x;
		m_nColW[m_ptFirstCell.x] += nNewW;
	
		m_LastSelType = AT_NONE;
		CalScrolBar();
		return;
	}

	m_LastSelType = AT_NONE;
	CWnd::OnLButtonUp(nFlags, point);
}

void CWinList::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (GetFocus() != this) 
		SetFocus();

	int row = -1,col = -1;
	
	int nret = HitTest(point,row,col);

	if(nret == AT_SELROW)
	{
		OnCurSelChange(row);
		if(row != -1)
		{
			GetParent()->PostMessage(WINLIST_LBDBLCLK,GetDlgCtrlID(),row);
		}
	}

	CWnd::OnLButtonDblClk(nFlags, point);
}

//测试鼠标位置,point设备坐标
// 返回值 AT_XXXX
// row和col用于接收行列
int	CWinList::HitTest(CPoint point,int &row,int &col)
{
	int n=0;
	int dx=4;
	int i;
	for(i=0;i<m_nCols;i++)
	{
		n+=m_nColW[i];
		if((abs(point.x - n + m_scrollPos.x) < dx ) && (point.y < m_nRowH) )
		{
			col = i;
			return AT_ADJUSTW;
		}
	}
	if(point.y < m_nRowH)
	{
		n=0;
		for(i=0;i<m_nCols;i++)
		{
			if( ((point.x + m_scrollPos.x) > n) && ((point.x + m_scrollPos.x) < (n + m_nColW[i])) )
			{
				col = i;
				row = -1;
				return AT_HEAD;
			}
			n+=m_nColW[i];
		}
		row = -1;
	}
	else
	{
		n=0;
		for(int i=0;i<m_nCols;i++)
		{
			if(( point.x > n + m_scrollPos.x )&&(point.x < n + m_scrollPos.x + m_nColW[i] ))
			{
				col = i;
				break;
			}
			n+=m_nColW[i];
		}

		int nr = point.y/m_nRowH - 1;
		if( (nr + m_scrollPos.y)   <  m_nRows)
			row = nr + m_scrollPos.y;
		else 
			row = -1;
	}
	if(point.x + m_scrollPos.x > GetAllColsW())
		row = -1;
	if(row != -1)
		return AT_SELROW;
	return AT_NONE;
}

BOOL CWinList::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style |= WS_VSCROLL;
	cs.style |= WS_HSCROLL;
	return CWnd::PreCreateWindow(cs);
}

void CWinList::GetScrollBarSizes(CSize& sizeSb)
{
	sizeSb.cx = sizeSb.cy = 0;
	DWORD dwStyle = GetStyle();

	if (GetScrollBarCtrl(SB_VERT) == NULL)
		sizeSb.cx = GetSystemMetrics(SM_CXVSCROLL)+GetSystemMetrics(SM_CXBORDER);

	if (GetScrollBarCtrl(SB_HORZ) == NULL)
		sizeSb.cy = GetSystemMetrics(SM_CYHSCROLL)+GetSystemMetrics(SM_CYBORDER);
}

BOOL CWinList::GetTrueClientSize(CSize& size, CSize& sizeSb)
{
	CRect rect;
	GetClientRect(&rect);
	ASSERT(rect.top == 0 && rect.left == 0);
	size.cx = rect.right;
	size.cy = rect.bottom;
	DWORD dwStyle = GetStyle();
	GetScrollBarSizes(sizeSb);

	return (size.cx > sizeSb.cx && size.cy > sizeSb.cy);
}

int		CWinList::GetAllColsW()//取所有列宽
{
	int i,n=0;
	for(i=0;i<m_nCols;i++)
		n+=m_nColW[i];
	return n;
}

void CWinList::CalScrolBar(void)
{

	CSize sizesb,sizeClient;

	GetTrueClientSize(sizeClient, sizesb);

	CSize sizeDoc;
	sizeDoc.cx = GetAllColsW();
	sizeDoc.cy = (m_nRows + 1);

	CSize psView;//视图尺寸
	psView.cx = sizeClient.cx;
	//psView.cy = 1+(sizeClient.cy) / m_nRowH;
	psView.cy = 0+(sizeClient.cy) / m_nRowH; // hq20100622，修正有水平滚动条是最后一行不能完全显示的BUG

	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE | SIF_RANGE;
	si.nPage = psView.cx;
	si.nMin = 0;
	si.nMax = sizeDoc.cx;
	SetScrollInfo(SB_HORZ, &si, TRUE);

	//  si.fMask |= SIF_DISABLENOSCROLL;
	si.nPage = psView.cy ;
	si.nMin = 0;
	si.nMax = sizeDoc.cy;
	SetScrollInfo(SB_VERT, &si, TRUE);

	if(m_nRows <= psView.cy)
	{
		m_scrollPos.y = 0;
		SetScrollPos32(SB_VERT,m_scrollPos.y);
	}
	//{{修正原来的因滚动条产生的花屏 jy20100618
	SCROLLINFO scinfo;
	memset(&scinfo, 0, sizeof(SCROLLINFO));
	scinfo.cbSize = sizeof(SCROLLINFO);
	scinfo.fMask=SIF_ALL;

	if(GetScrollInfo(SB_HORZ, &scinfo))
	{
		if(scinfo.nMax <= psView.cx)
			m_scrollPos.x = 0;
		else
		{
			if(m_scrollPos.x + (int)scinfo.nPage >= scinfo.nMax)
				m_scrollPos.x = scinfo.nMax - scinfo.nPage;

			// 避免在一些特殊情况下出现m_scrollPos.x < 0
			if (m_scrollPos.x < 0)
				m_scrollPos.x = 0;
		}
		SetScrollPos32(SB_HORZ,m_scrollPos.x);
	}
	//}}

	//{{重新计算垂直滚动条的滚动位置 hq20100622
	memset(&scinfo, 0, sizeof(SCROLLINFO));
	scinfo.cbSize = sizeof(SCROLLINFO);
	scinfo.fMask=SIF_ALL;

	if(GetScrollInfo(SB_VERT, &scinfo))
	{
		if(scinfo.nMax <= psView.cy)
			m_scrollPos.y = 0;
		else
		{
			if(m_scrollPos.y + (int)scinfo.nPage >= scinfo.nMax)
				m_scrollPos.y = scinfo.nMax - scinfo.nPage;

			// 避免在一些特殊情况下出现m_scrollPos.y < 0
			if (m_scrollPos.y < 0)
				m_scrollPos.y = 0;
		}
		SetScrollPos32(SB_VERT,m_scrollPos.y);
	}
	//}}
	Invalidate(FALSE);
}

int CWinList::GetScrollPos32(int nBar, BOOL bGetTrackPos /* = FALSE */)
{
    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);

    if (bGetTrackPos)
    {
        if (GetScrollInfo(nBar, &si, SIF_TRACKPOS))
            return si.nTrackPos;
    }
    else
    {
        if (GetScrollInfo(nBar, &si, SIF_POS))
            return si.nPos;
    }

    return 0;
}

BOOL CWinList::SetScrollPos32(int nBar, int nPos, BOOL bRedraw /* = TRUE */)
{
    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask  = SIF_POS;
    si.nPos   = nPos;
    return SetScrollInfo(nBar, &si, bRedraw);
}

void CWinList::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	
	if (GetFocus() != this) 
		SetFocus();
	SCROLLINFO scinfo;
	memset(&scinfo, 0, sizeof(SCROLLINFO));
    scinfo.cbSize = sizeof(SCROLLINFO);
	scinfo.fMask=SIF_ALL;

	if(!GetScrollInfo(SB_HORZ, &scinfo))
		return;


	int nline = 48;
	int npage = scinfo.nPage;
	
	if(nSBCode==SB_LINERIGHT)//
	{
		m_scrollPos.x += nline;
		if( m_scrollPos.x > scinfo.nMax - npage)
			m_scrollPos.x = scinfo.nMax - npage;
		SetScrollPos(SB_HORZ,m_scrollPos.x);
	}
	else if(nSBCode==SB_LINELEFT)
	{
		m_scrollPos.x -= nline;
		if(m_scrollPos.x < 0)
			m_scrollPos.x = 0;
		SetScrollPos(SB_HORZ,m_scrollPos.x);	
	}
	else if(nSBCode==SB_PAGELEFT)
	{
		m_scrollPos.x -= npage;
		if(m_scrollPos.x <=0)
			m_scrollPos.x=0;
		SetScrollPos(SB_HORZ,m_scrollPos.x);	
	}

	else if(nSBCode==SB_PAGERIGHT)
	{
		m_scrollPos.x += npage;
		if(m_scrollPos.x >= scinfo.nMax - npage)
			m_scrollPos.x = scinfo.nMax - npage;
		SetScrollPos(SB_HORZ,m_scrollPos.x);	
	}
	else if(nSBCode==SB_THUMBTRACK)
	{
		m_scrollPos.x=(INT)nPos;
		SetScrollPos(SB_HORZ,m_scrollPos.x);
	}
	else if(nSBCode==SB_RIGHT)
	{
		if(m_scrollPos.x != scinfo.nMax - npage)
		{
			m_scrollPos.x = scinfo.nMax - npage;
			SetScrollPos(SB_HORZ,m_scrollPos.x);
		}
	}
	else if(nSBCode==SB_LEFT)
	{
		if(m_scrollPos.x != 0)
		{
			m_scrollPos.x = 0;
			SetScrollPos(SB_HORZ,m_scrollPos.x);
		}
	}
	else
		return;
	Invalidate(FALSE);
}

void CWinList::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	
	if (GetFocus() != this) 
		SetFocus();

	SCROLLINFO scinfo;
	memset(&scinfo, 0, sizeof(SCROLLINFO));
    scinfo.cbSize = sizeof(SCROLLINFO);
	scinfo.fMask=SIF_ALL;

	if(!GetScrollInfo(SB_VERT, &scinfo))
		return;

	int nline = 1;
	int npage = (int)scinfo.nPage;

	if(nSBCode==SB_LINEDOWN)
	{
		m_scrollPos.y += nline;
		if( m_scrollPos.y > scinfo.nMax -  npage)
			m_scrollPos.y = scinfo.nMax -  npage +1;
	//	if( m_scrollPos.y > m_nRows -  npage)
	//		m_scrollPos.y = m_nRows -  npage;
		SetScrollPos32(SB_VERT,m_scrollPos.y);
	}
	else if(nSBCode==SB_LINEUP)
	{
		m_scrollPos.y -= nline;
		if(m_scrollPos.y < 0)
			m_scrollPos.y = 0;
		SetScrollPos32(SB_VERT,m_scrollPos.y);	
	}
	else if(nSBCode==SB_PAGEUP)
	{
		m_scrollPos.y -= npage;
		if(m_scrollPos.y < 0)
			m_scrollPos.y = 0;
		SetScrollPos32(SB_VERT,m_scrollPos.y);	
	}
	else if(nSBCode==SB_PAGEDOWN)
	{
		m_scrollPos.y += npage;
		if(m_scrollPos.y >= scinfo.nMax - npage)
			m_scrollPos.y = scinfo.nMax - npage;
		SetScrollPos32(SB_VERT,m_scrollPos.y);
	}
	else if((nSBCode==SB_THUMBTRACK)||( nSBCode == SB_THUMBPOSITION))
	{
		m_scrollPos.y = GetScrollPos32(SB_VERT,TRUE);
		SetScrollPos32(SB_VERT,m_scrollPos.y);
	}
	else if(nSBCode==SB_BOTTOM)
	{
		m_scrollPos.y = scinfo.nMax - npage;
		SetScrollPos32(SB_VERT,m_scrollPos.y);
	}

	else if(nSBCode==SB_TOP)
	{
		m_scrollPos.y = 0;
		SetScrollPos32(SB_VERT,m_scrollPos.y);
	}
	Invalidate(FALSE);
	
//	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CWinList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CSize sizesb,sizeClient;
	
	GetTrueClientSize(sizeClient, sizesb);

	int pagerow = sizeClient.cy / m_nRowH;
	if((m_nRows + 1) < pagerow) //无滚动条
	{
		CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
		return;
	}

	SCROLLINFO scinfo;
	memset(&scinfo, 0, sizeof(SCROLLINFO));
    scinfo.cbSize = sizeof(SCROLLINFO);
	scinfo.fMask=SIF_ALL;

	if(!GetScrollInfo(SB_VERT, &scinfo))
	{
		CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
		return;
	}

	int nline = 1;
	int npage = (int)scinfo.nPage;

	if(nChar == VK_PRIOR)
	{
		m_scrollPos.y -= npage;
		if(m_scrollPos.y < 0)
			m_scrollPos.y = 0;
		SetScrollPos32(SB_VERT,m_scrollPos.y);	
		Invalidate(FALSE);
	}
	else if(nChar == VK_NEXT) 
	{
		m_scrollPos.y += npage;
		if(m_scrollPos.y >= scinfo.nMax - npage)
			m_scrollPos.y = scinfo.nMax - npage;
		SetScrollPos32(SB_VERT,m_scrollPos.y);
		Invalidate(FALSE);
	}
	else if(nChar == VK_UP)
	{
		m_scrollPos.y -= nline;
		if(m_scrollPos.y < 0)
			m_scrollPos.y = 0;
		SetScrollPos32(SB_VERT,m_scrollPos.y);	
		Invalidate(FALSE);
	}
	else if(nChar == VK_DOWN)
	{
		m_scrollPos.y += nline;
		if( m_scrollPos.y > scinfo.nMax -  npage)
			m_scrollPos.y = scinfo.nMax -  npage;
		SetScrollPos32(SB_VERT,m_scrollPos.y);
		Invalidate(FALSE);
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void	CWinList::SetRowScrollTop(int nrow)//滚动到顶部
{
	m_nRows = nrow;
	m_scrollPos.y = 0;
	CalScrolBar();
	SetScrollPos32(SB_VERT,m_scrollPos.y);
}

BOOL CWinList::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	
	CSize sizesb,sizeClient;
	
	GetTrueClientSize(sizeClient, sizesb);

	int pagerow = sizeClient.cy / m_nRowH;
	if(m_nRows  < pagerow) //无滚动条
	{
		return TRUE;
	}

	int nPageRows = sizeClient.cy / m_nRowH;
	int nline = 1;
	if(zDelta > 0)
		nline = -nline; 

	if(nline > 0)
	{
		//m_scrollPos = GetScrollPos32(SB_VERT);
		if((GetScrollPos32(SB_VERT) + nPageRows) > m_nRows)
			return TRUE;
		m_scrollPos.y += nline;
		SetScrollPos32(SB_VERT,m_scrollPos.y);	
	}
	else
	{
		if(m_scrollPos.y == 0)
			return TRUE;
		m_scrollPos.y += nline;
		if(m_scrollPos.y < 0)
			m_scrollPos.y = 0;
		SetScrollPos32(SB_VERT,m_scrollPos.y);	
	}

	Invalidate(FALSE);

	return TRUE;
}

void CWinList::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	//重新计算滚动条
	if((nType==SIZE_MAXIMIZED)
		||(nType==SIZE_RESTORED)
		||(nType==SIZE_MAXSHOW))
	{
		CalScrolBar();
	}	
}

int CWinList::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	m_fntText.CreateFont(m_nTxtSize, 0, 0, 0, 400, 0, 0, 0, GB2312_CHARSET, 0, 0, PROOF_QUALITY, VARIABLE_PITCH | FF_ROMAN, _T("宋体"));

	return 0;
}

void CWinList::OnDestroy() 
{
	CWnd::OnDestroy();
	
	// TODO: Add your message handler code here

}

void CWinList::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd ::OnSetFocus(pOldWnd);
	
	// TODO: Add your message handler code here
	this->Invalidate();
	TRACE("OnSetFocus\n");
}

void CWinList::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd ::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
	this->Invalidate();	
	TRACE("OnKillFocus\n");
}

TRUNK_NAMESPACE_END