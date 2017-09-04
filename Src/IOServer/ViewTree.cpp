
#include "stdafx.h"
#include "ViewTree.h"
#include "IOServer.h"

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
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CViewTree::OnNMDblclk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewTree 消息处理程序

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


void CViewTree::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	HTREEITEM hSelectItem = GetSelectedItem();
	if(hSelectItem == NULL)
	{
		*pResult = 0;
		return;
	}
	//CString szStation;
	//DWORD_PTR pdwData = GetItemData(hSelectItem);
	//if(pdwData == 0)
	//{
	//	*pResult = 0;
	//	return;
	//}
	//PNODEDATA pNodeData;
	//CString szDecName = "";
	//pNodeData = (PNODEDATA)(pdwData);
	//if(pNodeData->nType == NODE_DEC)
	DWORD dwNodeType = GetItemData(hSelectItem);
	if(dwNodeType == NODE_RDB)
	{
		//szDecName = pNodeData->sName;
		//if(szDecName.IsEmpty())
		//	return;
		CString szDecName = GetItemText(hSelectItem);
		theApp.OpenDecVarView(szDecName); 
	}

	*pResult = 0;
}
