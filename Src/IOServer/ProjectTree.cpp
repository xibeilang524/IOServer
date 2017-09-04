
#include "stdafx.h"
#include "mainfrm.h"
#include "ProjectTree.h"
#include "Resource.h"
#include "IOServer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CProjectTree


#define IMG_ROOT		0
#define IMG_USER		1
#define IMG_DRV_MGR		2
#define IMG_DRV_MODULE	3
#define IMG_DRV_RUN		5
#define IMG_DRV_STP		4

#define  TIMER_DRV_STATUS 1

const CString szProjItem[] ={_T("服务器"),_T("用户管理"),_T("设备驱动"),_T("说明")};
const int nNodeType[] ={NODE_ROOT, NODE_SAFE_USER, NODE_DRVROOT, NODE_DRVROOT};
const int nImageIndex[] = {IMG_ROOT, IMG_USER, IMG_DRV_MGR, IMG_ROOT};

CProjectTree::CProjectTree()
{
}

CProjectTree::~CProjectTree()
{
}

BEGIN_MESSAGE_MAP(CProjectTree, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PROPERTIES, OnProperties)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND(IDM_PROJ_DB_CONFIG, &CProjectTree::OnProjDbConfig)
	ON_COMMAND(IDM_PROJ_DB_CONINFO, &CProjectTree::OnProjDbConinfo)
	ON_COMMAND(IDM_PROJ_DB_START, &CProjectTree::OnProjDbStart)
	ON_COMMAND(IDM_PROJ_DB_STOP, &CProjectTree::OnProjDbStop)
	ON_COMMAND(IDM_PROJECT_DB_ADD, &CProjectTree::OnProjectDbAdd)
	ON_COMMAND(IDM_PROJECT_DB_DEL, &CProjectTree::OnProjectDbDel)
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar 消息处理程序

int CProjectTree::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建视图:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	if (!m_wndProjectTree.Create(dwViewStyle, rectDummy, this, 4))
	{
		TRACE0("未能创建文件视图\n");
		return -1;      // 未能创建
	}

	// 加载视图图像:
	m_ProjectTreeImages.Create(IDB_PROJECTTREE, 16, 0, RGB(255, 0, 255));
	m_wndProjectTree.SetImageList(&m_ProjectTreeImages, TVSIL_NORMAL);

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_EXPLORER);
	m_wndToolBar.LoadToolBar(IDR_EXPLORER, 0, 0, TRUE /* 已锁定*/);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// 所有命令将通过此控件路由，而不是通过主框架路由:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	// 填入一些静态树视图数据(此处只需填入虚拟代码，而不是复杂的数据)
	InitProject();
	AdjustLayout();

	SetTimer(TIMER_DRV_STATUS, 1000, NULL);
	return 0;
}

void CProjectTree::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CProjectTree::InitProject()
{
	HTREEITEM hRoot = m_wndProjectTree.InsertItem(_T("解决方案"), IMG_ROOT, IMG_ROOT);
	m_wndProjectTree.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);
	//NODEDATA* pNODEDATA = new NODEDATA;
	//memset(pNODEDATA, 0, sizeof(NODEDATA));
	//pNODEDATA->nType = nImageIndex[0];
	//m_wndProjectTree.SetItemData(hRoot, (DWORD)pNODEDATA);
	m_wndProjectTree.SetItemData(hRoot, nImageIndex[0]);

	int iItemCount = sizeof(nNodeType) / sizeof(int);
	for (int i=1; i<iItemCount - 1; i++)
	{
		CString szSrc = szProjItem[i];
		HTREEITEM hTreeItem = m_wndProjectTree.InsertItem(szSrc, nImageIndex[i], nImageIndex[i], hRoot);

		//NODEDATA* pNODEDATA = new NODEDATA;
		//memset(pNODEDATA, 0, sizeof(NODEDATA));
		//pNODEDATA->nType = nNodeType[i];
		//m_wndProjectTree.SetItemData(hTreeItem, (DWORD_PTR)pNODEDATA);
		m_wndProjectTree.SetItemData(hTreeItem, nNodeType[i]);
	}


	m_wndProjectTree.Expand(hRoot, TVE_EXPAND);
	//m_wndProjectTree.Expand(hSrc, TVE_EXPAND);
	//m_wndProjectTree.Expand(hInc, TVE_EXPAND);
}

void CProjectTree::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndProjectTree;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// 选择已单击的项:
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndTree->SetFocus();

			pWndTree->SelectItem(hTreeItem);
			pWndTree->GetItemData(hTreeItem);

			DWORD dwNodeType = m_wndProjectTree.GetItemData(hTreeItem);
			if(dwNodeType == NODE_RDB 
				|| dwNodeType == NODE_MODULE
				|| dwNodeType == NODE_DRVROOT)
			{
				theApp.GetContextMenuManager()->ShowPopupMenu(IDR_MENU_PROJECT, point.x, point.y, this, TRUE);
			}
		}
	}
}

void CProjectTree::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndProjectTree.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CProjectTree::OnProperties()
{
	AfxMessageBox(IDS_PROPRITY);

}

void CProjectTree::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文

	CRect rectTree;
	m_wndProjectTree.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CProjectTree::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndProjectTree.SetFocus();
}

void CProjectTree::OnChangeVisualStyle()
{
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_EXPLORER_24 : IDR_EXPLORER, 0, 0, TRUE /* 锁定*/);

	m_ProjectTreeImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_PROJECTTREE : IDB_PROJECTTREE;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("无法加载位图: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_ProjectTreeImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_ProjectTreeImages.Add(&bmp, RGB(255, 0, 255));

	m_wndProjectTree.SetImageList(&m_ProjectTreeImages, TVSIL_NORMAL);
}



int CProjectTree::AddNode(CString szModule, CString szDevice, int nNodeType)
{
	HTREEITEM hRootItem = m_wndProjectTree.GetRootItem();
	if (!hRootItem)
		return -1;
	HTREEITEM hModuleRoot = NULL;
	if(m_wndProjectTree.ItemHasChildren(hRootItem))
	{
		HTREEITEM hChild = m_wndProjectTree.GetChildItem(hRootItem);
		while(hChild)
		{
			CString szNode = m_wndProjectTree.GetItemText(hChild);
			DWORD iNode = m_wndProjectTree.GetItemData(hChild);
			//PNODEDATA pNode = (PNODEDATA)m_wndProjectTree.GetItemData(hChild);
			if(iNode == NODE_DRVROOT)
			//if(pNode->nType == NODE_DRVROOT)
			{
				hModuleRoot = hChild;
				break;
			}
			hChild = m_wndProjectTree.GetNextItem(hChild, TVGN_NEXT);
		}
	}
	BOOL bHasModule = FALSE;
	if(m_wndProjectTree.ItemHasChildren(hModuleRoot))
	{
		HTREEITEM hChild = m_wndProjectTree.GetChildItem(hModuleRoot);
		while(hChild)
		{
			CString szModuleName = m_wndProjectTree.GetItemText(hChild);
			if(szModuleName == szModule)
			{
				bHasModule = TRUE;
				hModuleRoot = hChild;
				break;
			}
			hChild = m_wndProjectTree.GetNextItem(hChild, TVGN_NEXT);
		}
	}
	HTREEITEM hModule = hModuleRoot;
	if(!bHasModule)
	{
		hModule = m_wndProjectTree.InsertItem(szModule, IMG_DRV_MODULE, IMG_DRV_MODULE,  hModuleRoot);
		m_wndProjectTree.SetItemData(hModule, NODE_WAP);
	}
	HTREEITEM hDrvInstance = m_wndProjectTree.InsertItem(szDevice, IMG_DRV_STP, IMG_DRV_STP,  hModule);
	m_wndProjectTree.SetItemData(hDrvInstance, nNodeType);

	m_wndProjectTree.Expand(hRootItem,	TVE_COLLAPSE);
	m_wndProjectTree.Expand(hModule,	TVE_EXPAND);
	m_wndProjectTree.Expand(hRootItem,	TVE_EXPAND);
	m_wndProjectTree.Expand(hModuleRoot,	TVE_COLLAPSE);
	m_wndProjectTree.Expand(hModuleRoot,	TVE_EXPAND);
	return 0;
}
CTreeCtrl& CProjectTree::GetTreeCtrl()
{
	return m_wndProjectTree;
}

HTREEITEM CProjectTree::GetDrvParentNode()//获得"设备驱动"节点的hr
{
	//TVI_ROOT 下面才是
	HTREEITEM hRootItem = m_wndProjectTree.GetRootItem();
	if (!hRootItem)
		return NULL;
	HTREEITEM hModuleRoot = NULL;
	if(m_wndProjectTree.ItemHasChildren(hRootItem))
	{
		HTREEITEM hChild = m_wndProjectTree.GetChildItem(hRootItem);
		while(hChild)
		{
			hChild = m_wndProjectTree.GetNextItem(hChild, TVGN_NEXT);
			DWORD iNode = m_wndProjectTree.GetItemData(hChild);
			PNODEDATA pNode = (PNODEDATA)m_wndProjectTree.GetItemData(hChild);
			if(iNode == NODE_DRVROOT)
			//if(pNode->nType == NODE_DRVROOT)
			{
				hModuleRoot = hChild;
				break;
			}
		}
	}
	return hModuleRoot;
}

HTREEITEM CProjectTree::FindWapInTree(LPCTSTR lpszModuleName)
{

	HTREEITEM hrp = GetDrvParentNode();
	if(hrp == NULL)
		return NULL;

	HTREEITEM hTreeItem = m_wndProjectTree.GetChildItem(hrp);
	if(hTreeItem==NULL)
		return NULL;
	
	//PNODEDATA pNode=NULL;
	CString szModuleName = lpszModuleName;
	while(hTreeItem!=NULL)
	{
		DWORD dwNodeType = m_wndProjectTree.GetItemData(hTreeItem);
		//pNode = (PNODEDATA)m_wndProjectTree.GetItemData(hTreeItem);
		//if(pNode == NULL)
		//{
		//	TRACE("GetDrvNote遍历树错误:pNode == NULL\n");
		//	return NULL;
		//}
		//if(pNode->nType != NODE_MODULE)
		if(dwNodeType != NODE_MODULE)
		{
			TRACE("GetDrvNote遍历树错误:pNode->nType != NODE_DRV \n");
			return NULL;
		}
		CString szNode = m_wndProjectTree.GetItemText(hTreeItem);
		if(szModuleName == szNode)
			return hTreeItem;
		hTreeItem = m_wndProjectTree.GetNextItem(hTreeItem,TVGN_NEXT);
	}
	return NULL;

}

HTREEITEM CProjectTree::FindDrvInTree(LPCTSTR lpszdrvname,HTREEITEM hwap)
{
	HTREEITEM hdrv = m_wndProjectTree.GetChildItem(hwap);
	if(hdrv==NULL)
		return NULL;

	//PNODEDATA pNode=NULL;
	CString szDrvName = lpszdrvname;
	while(hdrv!=NULL)
	{
		//pNode = (PNODEDATA)m_wndProjectTree.GetItemData(hdrv);
		//if(pNode == NULL)
		//{
		//	TRACE("GetDrvNote遍历树错误:pNode == NULL\n");
		//	return NULL;
		//}
		//if(pNode->nType != NODE_RDB)
		//{
		//	TRACE("GetDrvNote遍历树错误:pNode->nType != NODE_DRV \n");
		//	return NULL;
		//}
		//if(szDrvName == pNode->sName)
		//	return hdrv;
		DWORD dwNodeType = m_wndProjectTree.GetItemData(hdrv);
		if(dwNodeType != NODE_RDB)
			return NULL;
		hdrv = m_wndProjectTree.GetNextItem(hdrv,TVGN_NEXT);
	}
	return NULL;
}

void	CProjectTree::AddDrvToTree(T_DRV_DES &tDrvDes)//添加驱动到树
{
	CString szModuleName,szDrvName;
	szDrvName = tDrvDes.szName;
	szModuleName = tDrvDes.szModule;

	HTREEITEM hModuleItem = FindWapInTree(szModuleName);
	if(hModuleItem == NULL)
	{
		HTREEITEM hTreeItem = GetDrvParentNode();
		if(hTreeItem == NULL)
			return ;
		hModuleItem = m_wndProjectTree.InsertItem(szModuleName,IMG_DRV_MODULE,IMG_DRV_MODULE,hTreeItem);
		//PNODEDATA pNodeData = new NODEDATA;
		//pNodeData->nType = NODE_MODULE;
		//strcpy(pNodeData->sName,szModuleName);
		//m_wndProjectTree.SetItemData(hModuleItem,(DWORD)pNodeData);
		m_wndProjectTree.SetItemData(hModuleItem,NODE_MODULE);
	}
	HTREEITEM hDevTreeItem = FindDrvInTree(szDrvName,hModuleItem);
	if(hDevTreeItem == NULL)
	{
		hDevTreeItem = m_wndProjectTree.InsertItem(szDrvName,IMG_DRV_STP,IMG_DRV_STP,hModuleItem);
		//PNODEDATA pNodeData = new NODEDATA;
		//pNodeData->nType = NODE_RDB;
		//strcpy(pNodeData->sName,szDrvName);
		//m_wndProjectTree.SetItemData(hDevTreeItem,(DWORD)pNodeData);
		m_wndProjectTree.SetItemData(hDevTreeItem,NODE_RDB);
		m_wndProjectTree.Expand(GetDrvParentNode(),TVE_EXPAND);
		m_wndProjectTree.Expand(hModuleItem,TVE_EXPAND);
		m_wndProjectTree.Expand(hDevTreeItem,TVE_EXPAND);
	}
}


BOOL CProjectTree::DeleteItem(CString szDevName)
{
	return 0;
}

void CProjectTree::BrowseItem(HTREEITEM  hItem)
{
	if(hItem == NULL)
		return;

	GetNodeStatus(hItem);//处理节点
	
	HTREEITEM hrChild = m_wndProjectTree.GetChildItem(hItem);
	BrowseItem(hrChild);//遍历子节点

	HTREEITEM hrNext=m_wndProjectTree.GetNextItem(hItem,TVGN_NEXT);
	BrowseItem(hrNext);//删除兄弟节点

	//if(m_wndProjectTree.ItemHasChildren(hItem))
	//{
	//	HTREEITEM hCurItem = m_wndProjectTree.GetChildItem(hItem);
	//	HTREEITEM hNextItem = NULL;
	//	PNODEDATA pNode = NULL;
	//	while(hCurItem)
	//	{
	//		pNode = (PNODEDATA)m_wndProjectTree.GetItemData(hItem);
	//		if(pNode && pNode->nType != NODE_RDB)
	//		{
	//			int nStatus = theApp.GetDecStatus(pNode->sName);
	//			if(nStatus == DRSTATUS_RUN)
	//				m_wndProjectTree.SetItemImage(hItem, IMG_DRV_RUN, IMG_DRV_RUN);
	//			else
	//				m_wndProjectTree.SetItemImage(hItem, IMG_DRV_STP, IMG_DRV_STP);
	//		}

	//		GetNodeStatus(hCurItem);

	//		hNextItem = hCurItem;
	//		BrowseItem(hNextItem);
	//		hCurItem = m_wndProjectTree.GetNextSiblingItem(hCurItem);
	//	}
	//}
}


void CProjectTree::OnProjDbConfig()
{
	// TODO: 在此添加命令处理程序代码
	HTREEITEM  hTreeItem = m_wndProjectTree.GetSelectedItem();
	if(hTreeItem)
	{
		DWORD dwNodeType = m_wndProjectTree.GetItemData(hTreeItem);
		if(dwNodeType == NODE_RDB)
		{
			CString szNode = m_wndProjectTree.GetItemText(hTreeItem);
			theApp.ConfigDrv(szNode);
			theApp.Save();
		}
	}
}


void CProjectTree::OnProjDbConinfo()
{
	// TODO: 在此添加命令处理程序代码
	HTREEITEM  hTreeItem = m_wndProjectTree.GetSelectedItem();
	if(hTreeItem)
	{
		DWORD dwNodeType = m_wndProjectTree.GetItemData(hTreeItem);
		if(dwNodeType == NODE_RDB)
		{
			CString szNode = m_wndProjectTree.GetItemText(hTreeItem);
			theApp.ConfigDrv(szNode);
		}
	}
}


void CProjectTree::OnProjDbStart()
{
	// TODO: 在此添加命令处理程序代码
	HTREEITEM  hTreeItem = m_wndProjectTree.GetSelectedItem();
	if(hTreeItem)
	{
		//PNODEDATA pNodeData	= (NODEDATA *)m_wndProjectTree.GetItemData(hTreeItem);
		//if (!pNodeData)
		//	return ;
		//if(pNodeData && pNodeData->nType == NODE_RDB)
		DWORD dwNodeType = m_wndProjectTree.GetItemData(hTreeItem);
		if(dwNodeType == NODE_RDB)
		{
			CString szNode = m_wndProjectTree.GetItemText(hTreeItem);
			CString szModule("");
			HTREEITEM hParentItem = m_wndProjectTree.GetParentItem(hTreeItem);
			if(hParentItem)
			{
				DWORD dwParentNodeType = m_wndProjectTree.GetItemData(hParentItem);
				if(dwParentNodeType == NODE_MODULE)
					szModule = m_wndProjectTree.GetItemText(hParentItem);
			}
			theApp.StartDrv(szModule, szNode);
		}
	}
}


void CProjectTree::OnProjDbStop()
{
	// TODO: 在此添加命令处理程序代码
	HTREEITEM  hTreeItem = m_wndProjectTree.GetSelectedItem();
	if(hTreeItem)
	{
		//PNODEDATA pNodeData	= (NODEDATA *)m_wndProjectTree.GetItemData(hTreeItem);
		//if (!pNodeData)
		//	return ;
		//if(pNodeData && pNodeData->nType == NODE_RDB)
		DWORD dwNodeType = m_wndProjectTree.GetItemData(hTreeItem);
		if(dwNodeType == NODE_RDB)
		{
			CString szNode = m_wndProjectTree.GetItemText(hTreeItem);
			theApp.StopDrv(szNode);
		}
	}
}


void CProjectTree::OnProjectDbAdd()
{
	// TODO: 在此添加命令处理程序代码
	theApp.AddDrv();
}


void CProjectTree::OnProjectDbDel()
{
	// TODO: 在此添加命令处理程序代码
	if(AfxMessageBox("是否确定删除此对象？", MB_OKCANCEL) != IDOK)
		return ;

	HTREEITEM  hTreeItem = m_wndProjectTree.GetSelectedItem();
	if(hTreeItem)
	{
		//PNODEDATA pNodeData	= (NODEDATA *)m_wndProjectTree.GetItemData(hTreeItem);
		//if (!pNodeData)
		//	return ;
		//if(pNodeData->nType == NODE_RDB)
		DWORD dwNodeType = m_wndProjectTree.GetItemData(hTreeItem);
		if(dwNodeType == NODE_RDB)
		{
			CString szNode = m_wndProjectTree.GetItemText(hTreeItem);
			HTREEITEM hParent = m_wndProjectTree.GetParentItem(hTreeItem);
			theApp.DeleteDrv(szNode);
			m_wndProjectTree.DeleteItem(hTreeItem);

			if(!m_wndProjectTree.ItemHasChildren(hParent))
				m_wndProjectTree.DeleteItem(hParent);
		}
		//else if(pNodeData->nType == NODE_DRVROOT)
		else if(dwNodeType == NODE_DRVROOT)
		{
			if(!m_wndProjectTree.ItemHasChildren(hTreeItem))
				m_wndProjectTree.DeleteItem(hTreeItem);
		}
		//else if (pNodeData->nType == NODE_MODULE)
		else if(dwNodeType == NODE_MODULE)
		{
			if(m_wndProjectTree.ItemHasChildren(hTreeItem))
			{
				HTREEITEM hChildItem = m_wndProjectTree.GetChildItem(hTreeItem);
				while(hChildItem)
				{
					CString szNode = m_wndProjectTree.GetItemText(hChildItem);
					theApp.DeleteDrv(szNode);
					m_wndProjectTree.DeleteItem(hChildItem);
					hChildItem = m_wndProjectTree.GetNextItem(hChildItem, TVGN_NEXT);
				}
			}
			m_wndProjectTree.DeleteItem(hTreeItem);
		}
	}
	theApp.Save();
}



void CProjectTree::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == TIMER_DRV_STATUS)
	{
		HTREEITEM hTreeItem = GetDrvParentNode();
		if(hTreeItem)
		{
			BrowseItem(hTreeItem);
		}
		return;
	}
	CDockablePane::OnTimer(nIDEvent);
}


int CProjectTree::GetNodeStatus(HTREEITEM hItem)
{
	if (!hItem)
		return -1;
	DWORD dwNodeType = m_wndProjectTree.GetItemData(hItem);
	if(dwNodeType == NODE_RDB)
	{
		CString szInstance = m_wndProjectTree.GetItemText(hItem);
		int nStatus = theApp.GetDecStatus(szInstance);
		if(nStatus == DRSTATUS_RUN)
			m_wndProjectTree.SetItemImage(hItem, IMG_DRV_RUN, IMG_DRV_RUN);
		else
			m_wndProjectTree.SetItemImage(hItem, IMG_DRV_STP, IMG_DRV_STP);
	}
	return 0;
}


void CProjectTree::OnDestroy()
{
	CDockablePane::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	KillTimer(TIMER_DRV_STATUS);
}
