
#include "stdafx.h"
#include "ViewTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewTree

CViewTree::CViewTree()
{
}

CViewTree::~CViewTree()
{
}

BEGIN_MESSAGE_MAP(CViewTree, CTreeCtrl)
  	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, &CViewTree::OnToolTipText)
 	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, &CViewTree::OnToolTipText)
	ON_NOTIFY_REFLECT(TVN_DELETEITEM, &CViewTree::OnTvnDeleteitem)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewTree 消息处理程序

void CViewTree::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	CTreeCtrl::PreSubclassWindow();
	EnableToolTips(TRUE);
}

BOOL CViewTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != NULL);

	if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != NULL)
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}

INT_PTR CViewTree::OnToolHitTest(CPoint point, TOOLINFO * pTI) const
{
	RECT rect;

	UINT nFlags;
	HTREEITEM hitem = HitTest( point, &nFlags );
	if( nFlags & TVHT_ONITEMLABEL  )
	{
		GetItemRect( hitem, &rect, TRUE );
		pTI->hwnd = m_hWnd;
		pTI->uId = (UINT)hitem;
		pTI->lpszText = LPSTR_TEXTCALLBACK;
		pTI->rect = rect;
		return pTI->uId;
	}
	return -1;
}

BOOL CViewTree::OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	CString strTipText;
	UINT nID = pNMHDR->idFrom;

	if( nID == (UINT)m_hWnd &&
		(( pNMHDR->code == TTN_NEEDTEXTA && pTTTA->uFlags & TTF_IDISHWND ) ||
		( pNMHDR->code == TTN_NEEDTEXTW && pTTTW->uFlags & TTF_IDISHWND ) ) )
		return FALSE; //不处理内置ToolTips

	// 获取鼠标位置
	const MSG* pMessage;
	CPoint pt;
	pMessage = GetCurrentMessage();
	ASSERT ( pMessage );
	pt = pMessage->pt;
	ScreenToClient( &pt );

	UINT nFlags;
	HTREEITEM hitem = HitTest( pt, &nFlags ); //取得节点

	LPTREENODEDATA pNodeData = reinterpret_cast<LPTREENODEDATA>(GetItemData(hitem));
	if (pNodeData)
	{
		CString szText = GetItemText(hitem);
		if (pNodeData->szDescr[0] != '\0')
			strTipText.Format(_T("%s - %s"), szText, pNodeData->szDescr);
		else
			strTipText = szText;
	}
	else
		strTipText = GetItemText(hitem);

#ifndef _UNICODE //ANSI版
	if (pNMHDR->code == TTN_NEEDTEXTA)
		lstrcpyn(pTTTA->szText, strTipText, 80);
	else
		_mbstowcsz(pTTTW->szText, strTipText, 80);

#else //UNICODE版
	if (pNMHDR->code == TTN_NEEDTEXTA)
		_wcstombsz(pTTTA->szText, strTipText, 80);
	else
		lstrcpyn(pTTTW->szText, strTipText, 80);
#endif

	*pResult = 0;

	return TRUE; // 消息已处理
}

void CViewTree::OnTvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	// 删除自定数据
	LPTREENODEDATA pNodeData = reinterpret_cast<LPTREENODEDATA>(pNMTreeView->itemOld.lParam);
	if (pNodeData)
	{
		delete pNodeData;
	}

	*pResult = 0;
}
