// DlgOpcVar.cpp : implementation file
//

#include "stdafx.h"
#include "opcdrv.h"
#include "myopc.h"
#include "DlgOpcVar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Default add item list size:
#define DEF_ITEM_LIST_SIZE		16

// Branch dummy item data:
#define NULL_ITEM_NAME			_T("_QCNULL_DUMMY")
#define NULL_ITEM_DATA			0xFFFFFFFF
// Image list indices:
#define ILI_BRANCH				8
#define ILI_SELBRANCH			9
#define ILI_LEAF				3


// Access rights filter combo box indices:
#define CB_ACCESS_ANY			0
#define CB_ACCESS_READONLY		1
#define CB_ACCESS_WRITEONLY		2
#define CB_ACCESS_READWRITE		4

/////////////////////////////////////////////////////////////////////////////
// CDlgOpcVar dialog


CDlgOpcVar::CDlgOpcVar(LPCTSTR lpszProgID, IOPCItemMgt *pIItemMgt,IOPCBrowseServerAddressSpace *pIBrowse, CWnd *pParent /*=NULL*/)
	: CDialog(CDlgOpcVar::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgOpcVar)
	m_strItemID = _T("");
	m_nDataType = DRT_FLOAT32;
	m_nListDataType = 6;
	m_nReadMode = 3;
	m_nLangage = 0;
	m_nOpcTime = 0;
	//}}AFX_DATA_INIT
	m_pDrv=NULL;
	m_szProgID = lpszProgID;

	m_pIItemMgt = pIItemMgt;
	m_pIBrowse = pIBrowse;
	
	m_bBrowseFlat=FALSE;

	m_bModified = false;
	m_strFilterLeaf = _T("");
	m_strFilterBranch = _T("");

	m_piItemProp = NULL;
}


void CDlgOpcVar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOpcVar)
	DDX_Text(pDX, IDC_ITEMID, m_strItemID);
	DDX_CBIndex(pDX, IDC_DATATYPE, m_nDataType);
	DDX_CBIndex(pDX, IDC_DATATYPE_LIST, m_nListDataType);
	DDX_CBIndex(pDX, IDC_COMBO_READMODE, m_nReadMode);
	DDX_CBIndex(pDX, IDC_COMBOLANGUAGE, m_nLangage);
	DDX_CBIndex(pDX, IDC_COMBO_TIME, m_nOpcTime);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgOpcVar, CDialog)
	//{{AFX_MSG_MAP(CDlgOpcVar)
	ON_NOTIFY(TVN_SELCHANGED, IDC_BRANCHLIST, OnSelchangedBranchlist)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_BRANCHLIST, OnItemexpandingBranchlist)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(NM_DBLCLK, IDC_LEAFLIST, OnDblclkLeaflist)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	ON_BN_CLICKED(IDC_ADDSEL, OnAddsel)
	ON_BN_CLICKED(IDC_ADDONE, OnAddone)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_VAR, OnDblclkListVar)
	ON_BN_CLICKED(IDC_BUTTON_TYPELIST, OnButtonTypelist)
	ON_BN_CLICKED(IDC_BUTTON_LOADTXT, OnButtonLoadtxt)
	ON_BN_CLICKED(IDC_BUTTON_EXPORTTXT, OnButtonExporttxt)
	ON_BN_CLICKED(IDC_BUTTON_EXPORTDEMO, OnButtonExportdemo)
	ON_BN_CLICKED(IDC_BUTTON_HELP, OnButtonHelp)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WINLIST_LBDBLCLK, OnDblclkWinList)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgOpcVar message handlers

void CDlgOpcVar::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CDlgOpcVar::OnOK() 
{
	// TODO: Add extra validation here
	if(m_bModified)
	{
		m_pDrv->ReMoveAll();
		m_pDrv->ListCtrlToItems(&m_ctrlItemList);
	}
	CDialog::OnOK();
}

BOOL CDlgOpcVar::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_RETURN)
		{
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgOpcVar::OnSelchangedBranchlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	
	NM_TREEVIEW* pnmtv = (NM_TREEVIEW*)pNMHDR;

	// Select the branch:
	SelectBranch (pnmtv->itemNew.hItem);
	*pResult = 0;
}

void CDlgOpcVar::OnItemexpandingBranchlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Cast notification header to tree view notification header:
	NM_TREEVIEW* pnmtv = (NM_TREEVIEW*) pNMHDR;

	// Expand branch:
	if (pnmtv->action & TVE_EXPAND)
		{
		ExpandBranch (pnmtv->itemNew.hItem);
		}

	// else delete child branches on collapse:
	else if (pnmtv->action & TVE_COLLAPSE)
		{
		DeleteChildBranches (pnmtv->itemNew.hItem);
		}

	*pResult = 0;
}

void CDlgOpcVar::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	
	// If showing dialog, set focus to item ID edit box:
	if (bShow)
		GetDlgItem (IDC_ITEMID)->SetFocus ();
}

BOOL CDlgOpcVar::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	// Get our group's item management interface pointer for the group:
	SetWindowText(m_sztitle);
	InitializeBrowser ();
	
	m_pDrv->ItemsToListCtrl(&m_ctrlItemList);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// **************************************************************************
// InitializeBrowser ()
//
// Description:
//	Initialize things related to the browser.
//
// Parameters:
//  none
//
// Returns:
//  void
// **************************************************************************
void CDlgOpcVar::InitializeBrowser ()
	{
	// Get pointers to our browse controls for easy access:
	m_pBranchList = (CTreeCtrl *) GetDlgItem (IDC_BRANCHLIST);
	ASSERT (m_pBranchList != NULL);

	// Initialize the image list for the branch control.  These are the 
	// images that show up to the left of each item in list.  The bitmap
	// must use a purple background color, RGB (255, 0, 255), so that the
	// CImageList object can construct a mask.  The images are 16x16 pixels.
	// Set the image list background color to CLR_NONE so masked pixels will
	// be transparent. 
	//
	// Image number		Use
	//	0-7				not used
	//	8				Branch
	//	9				Self branch
	m_cBranchImageList.Create (IDB_GROUPIMAGES, 16, 3, RGB (255, 0, 255));
	m_cBranchImageList.SetBkColor (CLR_NONE);
	m_pBranchList->SetImageList (&m_cBranchImageList, TVSIL_NORMAL);

	// Initialize the image list for the leaf control.  These are the images
	// that show up to the left of each item in list.  The bitmap must use a 
	// purple background color, RGB (255, 0, 255), so that the CImageList
	// object can construct a mask.  The images are 14x14 pixels.  Set the
	// image list background color to CLR_NONE so masked pixels will be
	// transparent. 
	//
	// Image number		Use
	//	0-2				not used
	//	3				Leaf
	m_cLeafImageList.Create (IDB_ITEMIMAGES, 14, 2, RGB (255, 0, 255));
	m_cLeafImageList.SetBkColor (CLR_NONE);
	//m_pLeafList->SetImageList (&m_cLeafImageList, LVSIL_SMALL);

	// Insert a column for our leaf item list control:
	CRect rc;
	//m_pLeafList->GetClientRect (&rc);
	//m_pLeafList->InsertColumn (0, _T(""), LVCFMT_LEFT, rc.Width ()-25);
	

	GetDlgItem(IDC_STATIC_LEAFLIST)->GetWindowRect(&rc);
	ScreenToClient(&rc) ;  
	rc.DeflateRect(1, 1, 1, 1);	
	m_ctrlLeafList.Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rc, this, 100001);//标签表
	m_ctrlLeafList.ReDraw();
	
	GetDlgItem(IDC_STATIC_ITEMLIST)->GetWindowRect(&rc);
	ScreenToClient(&rc) ;  
	rc.DeflateRect(1, 1, 1, 1);	
	m_ctrlItemList.Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rc, this, 100002);//标签表
	m_ctrlItemList.ReDraw();	

	// Initialize filters:
	m_strFilterBranch = "*";

	m_strFilterLeaf = "*";

	m_vtFilterType = VT_EMPTY;

	m_dwFilterAccessRights = CB_ACCESS_ANY;

	// Intialize browse controls if browsing is supported, otherwise
	// just disable the controls:
	if (m_pIBrowse != NULL)
		{
		// Create a wait cursor object.  This will cause the wait cursor, 
		// usually an hourglass, to be displayed.  When this object goes
		// out of scope, its destructor will restore the previous cursor
		// type.
		CWaitCursor wc;
		HRESULT hr;

		// Query the server name space:
		hr = m_pIBrowse->QueryOrganization (&m_cOpcNameSpace);

		if (SUCCEEDED (hr))
			{
			// Browse root level:
			switch (m_cOpcNameSpace)
				{
				case OPC_NS_HIERARCHIAL:
				case OPC_NS_FLAT:
					BrowseRootLevel ();
					break;

				// Did the foundation add a new type?
				default:
					ASSERT (FALSE);
					break;
				}
			}

		// We will assume a hierarchial search if the server fails this request:
		// (This allows us to work with any server that uses the ICONICS toolkit.)
		else
			{
			TRACE (_T("OTC: Attempting agressive browsing since the server failed on QueryOrganization ()\r\n"));

			m_cOpcNameSpace = OPC_NS_HIERARCHIAL;
			BrowseRootLevel ();
			}
		}
	}

// **************************************************************************
// BrowseRootLevel ()
//
// Description:
//	Reset the browser to the root object.
//
// Parameters:
//  none
//
// Returns:
//  void
// **************************************************************************
void CDlgOpcVar::BrowseRootLevel ()
	{
	HRESULT hr = E_FAIL;
	HTREEITEM hItem = NULL;

	try
		{
		// If hierarchial namespace:
		if (m_cOpcNameSpace == OPC_NS_HIERARCHIAL)
			{
			// Re-initialize server's browse position to the root:
			do
				{
				// Use the OPC_BROWSE_UP rather than the OPC_BROWSE_TO which
				// is only supported in OOPC version 2.0.  We will have to 
				// browse up to root one level at a time.  Function will
				// fail when we are at root.
				hr = m_pIBrowse->ChangeBrowsePosition (OPC_BROWSE_UP, L"\0");
				} while (SUCCEEDED (hr));

			// Insert our root level item:
			hItem = m_pBranchList->InsertItem (m_szProgID, 
				ILI_BRANCH, ILI_SELBRANCH, TVI_ROOT);

			// Set the item data and add a dummy child branch:
			m_pBranchList->SetItemData (hItem, DWORD (-1));
			AddDummyBranch (hItem);
			}

		// Else flat namespace:
		else
			{
			ASSERT (m_cOpcNameSpace == OPC_NS_FLAT);

			// Insert our root level item (there is no need to add any dummy
			// branches since a flat space will only have one level of leaves)
			hItem = m_pBranchList->InsertItem (m_szProgID, 
				ILI_BRANCH, ILI_SELBRANCH, TVI_ROOT);
			}

		// Select root item:
		if (hItem)
			m_pBranchList->SelectItem (hItem);
		}
	
	catch (...)
		{
		m_pIBrowse = NULL;
//		UpdateStatus ();//jydel
		}
	}

// **************************************************************************
// AddDummyBranch ()
//
// Description:
//	Add a dummy branch to tree control.
//
// Parameters:
//  HTREEITEM	hParent		Handle to parent item.
//
// Returns:
//  void
// **************************************************************************
void CDlgOpcVar::AddDummyBranch (HTREEITEM hParent)
	{
	ASSERT (hParent != NULL);
	
	HTREEITEM hDummyItem;

	// Insert a dummy item:
	hDummyItem = m_pBranchList->InsertItem (NULL_ITEM_NAME, hParent);
	ASSERT (hDummyItem != NULL);
			
	m_pBranchList->SetItemData (hDummyItem, NULL_ITEM_DATA);
	}


// **************************************************************************
// ExpandBranch ()
//
// Description:
//	Called to expand branch.
//
// Parameters:
//  HTREEITEM	hItem		Handle of tree control branch to expand.
//
// Returns:
//  void
// **************************************************************************
void CDlgOpcVar::ExpandBranch (HTREEITEM hItem)
	{
	ASSERT (hItem != NULL);

	int nPos;
	HRESULT hr;
	LPENUMSTRING pIEnumString;

	WCHAR szFilter [DEFBUFFSIZE];

	// Get the new browse position from the item that was previously
	// selected:
	nPos = (int) m_pBranchList->GetItemData (hItem);
	nPos++;
	ASSERT (nPos >= 0);

	try
		{
		// Re-intialize the server's position to the root level. 
		do
			{
			// Use the OPC_BROWSE_UP rather than the OPC_BROWSE_TO which
			// is only supported in OOPC version 2.0.  We will have to 
			// browse up to root one level at a time.  Function will
			// fail when we are at root.
			hr = m_pIBrowse->ChangeBrowsePosition (OPC_BROWSE_UP, L"\0");
			} while (SUCCEEDED (hr));

		// Now browse down to the new position:
		CStringArray strBranches;
		HTREEITEM hParentItem;

		strBranches.SetSize (nPos + 1);
		hParentItem = hItem;

		for (int i = 0; i <= nPos; i++)
			{
			ASSERT (hItem);
			strBranches [i] = m_pBranchList->GetItemText (hParentItem);
			hParentItem = m_pBranchList->GetParentItem (hParentItem);
			}

		hr = S_OK;

		// > 0 we do not want to include the "Root" item since the
		// client only uses this branch:
		while (SUCCEEDED (hr) && nPos-- > 0)	
		{
			WCHAR szBranch [DEFBUFFSIZE];
			MytoWchar(szBranch,strBranches [nPos]);//jy200807w
			hr = m_pIBrowse->ChangeBrowsePosition (OPC_BROWSE_DOWN, szBranch);
		}

		// Browse for root level:

		MytoWchar(szFilter,m_strFilterBranch);//jy200807w
		hr = m_pIBrowse->BrowseOPCItemIDs (OPC_BRANCH,				// provide items with children
										  szFilter,					// id filtering
										  VT_EMPTY,					// no datatype filtering on a branch
										  0,						// no access filtering on a branch
										  &pIEnumString);			// store the interface pointer here

		// On success add the branches to the root:
		if (SUCCEEDED (hr) && pIEnumString)
			{
			AddBranches (pIEnumString, hItem, m_pBranchList->GetItemData (hItem) + 1);
			pIEnumString->Release ();
			}
		else
			{
			RemoveDummyBranch (hItem);
			throw (-1);
			}
		}
	
	catch (...)
		{
		m_pIBrowse = NULL;
		}
	}

// **************************************************************************
// SelectBranch ()
//	
// Description:
//	Select a branch in tree control.
//
// Parameters:
//  HTREEITEM	hItem		Handle of tree control's item to select
//
// Returns:
//  void
// **************************************************************************
void CDlgOpcVar::SelectBranch (HTREEITEM hItem)
{
	ASSERT (hItem != NULL);

	int nPos;
	HRESULT hr;
	LPENUMSTRING pIEnumString;
	CStringArray strBranches;//字符串数组
	HTREEITEM hParentItem;

	WCHAR szFilter [DEFBUFFSIZE];

	// Get the new browse position from the item that was previously
	// selected:
	nPos = (int) m_pBranchList->GetItemData (hItem);
	nPos++;
	ASSERT (nPos >= 0);

	try
	{
		// Re-intialize the server's position to the root level:
		do
			{
			// Use the OPC_BROWSE_UP rather than the OPC_BROWSE_TO which
			// is only supported in OOPC version 2.0.  We will have to 
			// browse up to root one level at a time.  Function will
			// fail when we are at root.
			hr = m_pIBrowse->ChangeBrowsePosition (OPC_BROWSE_UP, L"\0");
			} while (SUCCEEDED (hr));

		// Now browse down to the new position:
		strBranches.SetSize (nPos + 1);
		hParentItem = hItem;

		for (int i = 0; i <= nPos; i++)
		{
			ASSERT (hItem);
			strBranches [i] = m_pBranchList->GetItemText (hParentItem);
			hParentItem = m_pBranchList->GetParentItem (hParentItem);
		}

		hr = S_OK;

		// > 0 we do not want to include the "Root" item since the client
		// only uses this branch:
		while (SUCCEEDED (hr) && nPos-- > 0)	
		{
			WCHAR szBranch [DEFBUFFSIZE];
			MytoWchar(szBranch,strBranches [nPos]);//jy200807w
			hr = m_pIBrowse->ChangeBrowsePosition (OPC_BROWSE_DOWN, szBranch);
		}

		// Browse for root level:
		MytoWchar(szFilter,m_strFilterLeaf);//jy200807w
		hr = m_pIBrowse->BrowseOPCItemIDs (
			m_bBrowseFlat ? OPC_FLAT : OPC_LEAF,	// provide items with children
			szFilter,								// item id filtering
			m_vtFilterType,							// datatype filter
			m_dwFilterAccessRights,					// access rights filtering
			&pIEnumString);							// store the interface pointer here

		// On success add the branches to the root:
		if (SUCCEEDED (hr) && pIEnumString)
		{
			AddLeaves (pIEnumString);
			pIEnumString->Release ();
		}
		else
			throw (-1);
	}
	
	catch (...)
	{
		m_pIBrowse = NULL;

	}
}

// **************************************************************************
// BrowseToRoot ()
//
// Description:
//	Reset brows position to root object.
//
// Parameters:
//  none
//
// Returns:
//  void
// **************************************************************************
void CDlgOpcVar::BrowseToRoot ()
{
	try
	{
		// Try to browse to root in one step using "OPC_BROWSE_TO" (this
		// is not supported and will fail for OPC 1.0 servers):
		HRESULT hr = m_pIBrowse->ChangeBrowsePosition (OPC_BROWSE_TO, L"");
		
		// If that fails, browse to root one level at a time using "OPC_BROWSE_UP".
		// (Browse up will fail when we are at root.)
		if (FAILED (hr))
		{
			do
			{
				hr = m_pIBrowse->ChangeBrowsePosition (OPC_BROWSE_UP, L"\0");
			} while (SUCCEEDED (hr));
		}
	}
	
	catch (...)
	{
	}
}


// **************************************************************************
// AddBranches ()
//
// Description:
//	Add branch to tree control.
//
// Parameters:
//  LPENUMSTRING	pIEnumString	Stores the interface pointer
//	HTREEITEM		hParent			Handle of parent tree item
//	DWORD			dwData			Item data value to give new branches
//
// Returns:
//  void
// **************************************************************************
void CDlgOpcVar::AddBranches (LPENUMSTRING pIEnumString, HTREEITEM hParent, DWORD dwData)
	{
	ASSERT (hParent != NULL);

	ULONG celt = 1;			
	LPOLESTR rgelt;			
	ULONG celtFetched = 0;	

	// Remove the dummy branch if one exists:
	RemoveDummyBranch (hParent);

	// Start at the beginning of the list:
	pIEnumString->Reset ();	
	pIEnumString->Next (celt, &rgelt, &celtFetched);

	CString szItem;////jy200807w
	// Add each branch to the browse control:
	while (celtFetched > 0) 
	{
		HTREEITEM hNewItem = NULL;

		// COM requis that all strings be sent in UNICODE format.
		// Convert if necessary and copy to szBuffer:
		szItem = rgelt;////jy200807w

		// Insert the branch:
		hNewItem = m_pBranchList->InsertItem (szItem, ILI_BRANCH, ILI_SELBRANCH, hParent);//jy200807w
		
		m_pBranchList->SetItemData (hNewItem, dwData);	

		// Always fake each branch into having a sub item in the tree:
		AddDummyBranch (hNewItem);

		// Free the branch name:
		CoTaskMemFree (rgelt);	

		// Re-initialize and get the next item:
		celt = 1;
		celtFetched = 0;
		pIEnumString->Next (celt, &rgelt, &celtFetched);
	}
}


// **************************************************************************
// AddLeaves ()
//
// Description:
//	Add leaves to tree control.
//
// Parameters:
//  LPENUMSTRING	pIEnumString	Stores the interface pointer
//
// Returns:
//  void
// **************************************************************************
void CDlgOpcVar::AddLeaves (LPENUMSTRING pIEnumString)
	{
	ULONG celt = 1;			
	LPOLESTR rgelt;			
	ULONG celtFetched = 0;	
	int nIndex = 0;

	CWaitCursor wc;

	// Delete any leaves that are presently being displayed:
	m_ctrlLeafList.RemoveAll();

	// Start at the beginning of the list:
	pIEnumString->Reset ();	
	pIEnumString->Next (celt, &rgelt, &celtFetched);

	// Add each leaf to the leaf control:
	while (celtFetched > 0) 
	{
		// Insert the leaf:
		CString szitem = rgelt;//COleVariant 

		m_ctrlLeafList.AddItem(szitem);

		// Free the branch name:
		CoTaskMemFree (rgelt);	

		// Re-initialize and get the next item:
		celt = 1;
		celtFetched = 0;
		pIEnumString->Next (celt, &rgelt, &celtFetched);
	}
	m_ctrlLeafList.ReDraw();

	// Select first leaf by default:
	//if (m_pLeafList->GetItemCount ())
	//	m_pLeafList->SetItemState (0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
}

// **************************************************************************
// RemoveDummyBranch ()
//
// Description:
//	Remove a dummy branch from tree control.
//
// Parameters:
//  HTREEITEM	hParent		Handle of parent item.
//
// Returns:
//  void
// **************************************************************************
void CDlgOpcVar::RemoveDummyBranch (HTREEITEM hParent)
	{
	ASSERT (hParent != NULL);

	HTREEITEM hDummyItem;

	// Get child item:
	hDummyItem = m_pBranchList->GetChildItem (hParent);
	while (hDummyItem)
		{
		CString strItem = m_pBranchList->GetItemText (hDummyItem);
		
		if (strItem.CompareNoCase (NULL_ITEM_NAME) == 0)
			{
			if (m_pBranchList->GetItemData (hDummyItem) == NULL_ITEM_DATA)
				{
				m_pBranchList->DeleteItem (hDummyItem);
				break;
				}
			}

		hDummyItem = m_pBranchList->GetNextSiblingItem (hDummyItem);
		}
	}

// **************************************************************************
// DeleteChildBranches ()
//
// Description:
//	Deletes all child branches of specified item in tree control
//
// Parameters:
//  HTREEITEM	hParent		Handle to parent item.
//
// Returns:
//  void
// **************************************************************************
void CDlgOpcVar::DeleteChildBranches (HTREEITEM hParent)
{
	ASSERT (hParent != NULL);

	HTREEITEM hItem;
	hItem = m_pBranchList->GetChildItem (hParent);

	while (hItem)
		{
		m_pBranchList->DeleteItem (hItem);
		hItem = m_pBranchList->GetChildItem (hParent);
		}
	
	AddDummyBranch (hParent);
}

void CDlgOpcVar::DoDblclkLeaflist(int nRow) 
{
	CLeafItem *pItem = m_ctrlLeafList.GetAt(nRow);
	if (pItem)
	{		
		HRESULT hr;
		WCHAR szItemID [DEFBUFFSIZE];
		LPWSTR lpszQualifiedID;

		MytoWchar(szItemID, pItem->m_szItemName);//jy200807w

		try
		{
			// If we are using a flat browse space, then we need to browse to
			// root first: 
			if (m_bBrowseFlat)
				BrowseToRoot ();

			// User browser to get item's fully qualified ID:
			hr = m_pIBrowse->GetItemID (szItemID, &lpszQualifiedID);

			// If we succeeded, update controls:
			if (SUCCEEDED (hr) && lpszQualifiedID)
			{
				// Re-initialize for new item:
				m_strItemID = lpszQualifiedID;
				this->SetDlgItemText(IDC_ITEMID,m_strItemID);

				// Free server allocation for qualified item id:
				CoTaskMemFree (lpszQualifiedID);
			}

			// If we didn't get qualified ID, issue a trace statement for debugging:
			else
			{
				TRACE (_T("OTC: Unable to get the qualified item id for %s\r\n"), 
					pItem->m_szItemName);
			}
		}

		// Catch exceptions:
		catch (...)
		{
			m_pIBrowse = NULL;
		}
	}
}

void CDlgOpcVar::OnDblclkLeaflist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	DoDblclkLeaflist(pNMItemActivate->iItem);
}

void CDlgOpcVar::OnDel() 
{
	// TODO: Add your control notification handler code here
	if (AfxMessageBox(_T("是否真的要删除当前选择的标签项？"), MB_YESNO | MB_ICONQUESTION) != IDYES)
		return;

	for (int i = m_ctrlItemList.GetCount() - 1; i >= 0; i--)
	{
		if (m_ctrlItemList.IsSel(i))
		{
			m_ctrlItemList.RemoveAt(i);			
			m_bModified = TRUE;
		}
	}
	m_ctrlItemList.ReDraw();
}
// OnAddsel ()
//
// Description:
//	Add Leaves button event handler.  Add leaves to list.
//
// Parameters:
//  none
//
// Returns:
//  void
// **************************************************************************
int OleType2DrvType(int noletype)
{
	int ntype = -1;
	switch (noletype)
	{
		case VT_BOOL:
			ntype = DRT_DIGITAL;
			break;
		case VT_UI1:
		case VT_I1:
		case VT_UI2:
		case VT_I2:
		case VT_I4:
		case VT_UI4:
			ntype = DRT_INT32;
			break;
		case VT_R4:
			ntype = DRT_FLOAT32;
			break;
		case VT_I8:
		case VT_UI8:
			ntype = DRT_INT64;
			break;
		case VT_R8:
			ntype = DRT_FLOAT64;
			break;
		
		case VT_BSTR:
			ntype = DRT_STRING;
			break;
		default:
			return -1;
	}
	return ntype;
};
void CDlgOpcVar::OnAddsel() 
{
	// TODO: Add your control notification handler code here
	// **************************************************************************
	UpdateData(TRUE);
	if(this->m_nDataType == -1)
	{
		AfxMessageBox("请指定默认转换类型!");
		return;
	}
	CWaitCursor wc;
	CLeafItem *pItem;
	HRESULT hr;
	WCHAR szItemID [DEFBUFFSIZE];
	LPWSTR lpszQualifiedID;
	for(int i = 0; i < m_ctrlLeafList.GetCount(); i++)
	{
		pItem = m_ctrlLeafList.GetAt(i);
		if (!pItem->m_bSel)
			continue;

		// COM requires all strings to be in UNICODE format.  Convert item ID
		// if needed and copy to allocated memory:
		MytoWchar(szItemID, pItem->m_szItemName);//jy200807w

		// Use browser to get item's fully qualified ID:
		try
		{
			// If we are using a flag browse space, need to browse to 
			// root first:
			if (m_bBrowseFlat)
				BrowseToRoot ();

			// Get item's fully qualified ID:
			hr = m_pIBrowse->GetItemID (szItemID, &lpszQualifiedID);

			// If we succeeded, then update controls accordingly:
			if (SUCCEEDED (hr) && lpszQualifiedID)
			{
				// Update selector to end of the list:
				// Re-initialize for new item:
				m_strItemID = lpszQualifiedID;
				//{{取数据类型
				int ntp = -1;
				CString szDesc = _T("");
				CString szUnit = _T("");
				if(this->m_piItemProp)
				{
					/* 枚举可用属性ID
					HRESULT QueryAvailableProperties(
						[in] LPWSTR szItemID,
						[out] DWORD * pdwCount,
						[out, size_is(,*pdwCount)] DWORD **ppPropertyIDs,
						[out, size_is(,*pdwCount)] LPWSTR *ppDescriptions,
						[out, size_is(,*pdwCount)] VARTYPE **ppvtDataTypes
						);		*/
			/*		DWORD dwCount=0;
					DWORD *pdwids=NULL;
					LPWSTR *pdes = NULL;
					VARTYPE *psntype = NULL;
					HRESULT hrp = m_piItemProp->QueryAvailableProperties(
						lpszQualifiedID,
						&dwCount,
						&pdwids,
						&pdes,
						&psntype
						);
					if(SUCCEEDED (hrp) && dwCount>0)
					{
						TRACE("\n%s:type = ",m_strItemID);
						for(DWORD ui=0;ui<dwCount;ui++)
						{
							if(pdes[ui])
							{
								CString szt = pdes[ui];
								TRACE(" des = %s",szt);
								CoTaskMemFree (pdes[ui]);
								
								
							}
							if(psntype)
							{
								ntp = OleType2DrvType(psntype[ui]);
								TRACE(", id=%d TYPE=%d(%d)\n",pdwids[ui],psntype[ui],ntp);
							}
						}
						
					}*/
					// ID=1表示数据类型，101表示描述
					/* 枚举属性值
					HRESULT GetItemProperties(
						[in] LPWSTR szItemID,
						[in] DWORD dwCount,
						[in, size_is(dwCount)] DWORD * pdwPropertyIDs,
						[out, size_is(,dwCount)] VARIANT ** ppvData,
						[out, size_is(,dwCount)] HRESULT **ppErrors
						); 
					*/
					DWORD dwids = 1; //ID=1表示取数据类型
					VARIANT *pv = NULL;
					HRESULT *phr = NULL;
					HRESULT hrd = m_piItemProp->GetItemProperties(
							lpszQualifiedID,
							1,
							&dwids,
							&pv,
							&phr);
					if(SUCCEEDED (hrd) && SUCCEEDED ((phr[0]))  )
					{
						
						ntp = OleType2DrvType(pv[0].iVal); 
						TRACE("\n %s: oletype = %d ,TYPE=%d",m_strItemID,pv[0].iVal,ntp);
						
					}
					if (pv) CoTaskMemFree(pv);
					if (phr) CoTaskMemFree(phr);
					
					// 增加工程单位和标签描述的信息提取

					dwids = 100; // 工程单位
					pv = NULL;
					phr = NULL;
					hrd = m_piItemProp->GetItemProperties(
						lpszQualifiedID,
						1,
						&dwids,
						&pv,
						&phr);
					if(SUCCEEDED (hrd) && SUCCEEDED ((phr[0])) && pv[0].bstrVal )
					{							
						szUnit = pv[0].bstrVal;						
					}
					if (pv) CoTaskMemFree(pv);
					if (phr) CoTaskMemFree(phr);
					
					dwids = 101; // 标签描述
					pv = NULL;
					phr = NULL;
					hrd = m_piItemProp->GetItemProperties(
						lpszQualifiedID,
						1,
						&dwids,
						&pv,
						&phr);
					if(SUCCEEDED (hrd) && SUCCEEDED ((phr[0])) && pv[0].bstrVal )
					{							
						szDesc = pv[0].bstrVal;						
					}
					if (pv) CoTaskMemFree(pv);
					if (phr) CoTaskMemFree(phr);
				}
				//}}
				
				
				// 在此添加一个数据项
				/*
				if (AddItem(m_strItemID, (ntp >= 0)?ntp:m_nDataType, szDesc, szUnit, FALSE))
					m_bModified = TRUE;
				else {
					CString szInfo;
					szInfo.Format(_T("标签“%s”已经被添加！"), m_strItemID);
					AfxMessageBox(szInfo, MB_OK | MB_ICONERROR);
				}
				*/
				if (AddItemInPort(m_strItemID, szDesc, szUnit, (ntp >= 0)?ntp:m_nDataType, TRUE))
					m_bModified = TRUE;
	
				// Free server allocation for qualified item ID:
				CoTaskMemFree (lpszQualifiedID);
			}

			// If we failed to get fully qualified ID, issue a trace
			// statement for debugging:
			else {
				TRACE (_T("OTC: Unable to get the qualified item id for %s\r\n"), 
					pItem->m_szItemName);
			}
		}
		
		// Catch exceptions:
		catch (...)
		{
			m_pIBrowse = NULL;

			break;
		}
	}
	m_ctrlItemList.ReDraw();
}

void CDlgOpcVar::OnAddone() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_strItemID.IsEmpty())
		return;
	if(this->m_nDataType == -1)
		return;

	if (AddItem(m_strItemID, m_nDataType, _T(""), _T(""), FALSE))
	{
		m_bModified = TRUE;
		m_ctrlItemList.ReDraw();
	}
}

BOOL CDlgOpcVar::AddItem(LPCTSTR lpszItem,int nDataType,LPCTSTR lpszDesc, LPCTSTR lpszUnit, BOOL bZeroFilter,BOOL bModifyType)// 添加到List
{
	ASSERT((nDataType >=0)&&(nDataType < 7));

	if (m_ctrlItemList.FindItem(lpszItem))
		return FALSE;
	m_ctrlItemList.AddItem(lpszItem, lpszDesc, lpszUnit, CItemDef::GetTypeStr(nDataType), bZeroFilter);
	//m_ctrlItemList.ReDraw();

	return TRUE;
}

BOOL CDlgOpcVar::AddItemInPort(LPCTSTR lpszItem,LPCTSTR lpszDes,LPCTSTR lpszUnit,int nDataType,BOOL bZeroFilter,BOOL bModifyType)// 添加到List
{
	ASSERT((nDataType >=0)&&(nDataType < 7));

	CTagItem *pItem = m_ctrlItemList.FindItem(lpszItem);
	if (pItem)
	{
		if(bModifyType)
		{
			pItem->m_szItemName = lpszItem;
			pItem->m_szDesc = lpszDes;
			pItem->m_szUnit = lpszUnit;
			pItem->m_szDataType = CItemDef::GetTypeStr(nDataType);
			pItem->m_bZeroFilter = bZeroFilter;
		}
		return TRUE;
	}	
	m_ctrlItemList.AddItem(lpszItem, lpszDes, lpszUnit, CItemDef::GetTypeStr(nDataType), bZeroFilter);
	//m_ctrlItemList.ReDraw();

	return TRUE;
}

void CDlgOpcVar::DoDblclkListVar(int nRow) 
{	
	CTagItem *pItem = m_ctrlItemList.GetAt(nRow);
	if (pItem)
	{		
		this->SetDlgItemText(IDC_ITEMID, pItem->m_szItemName);
		
		CComboBox *pCom=(CComboBox *)GetDlgItem(IDC_DATATYPE);
		pCom->SetCurSel(pCom->SelectString(-1, pItem->m_szDataType));
	}
}

void CDlgOpcVar::OnDblclkListVar(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	DoDblclkListVar(pNMItemActivate->iItem);
}


void CDlgOpcVar::OnButtonTypelist() 
{
	// TODO: Add your control notification handler code here
	CTagItem *pItem;
	CString szType,szType2;
	GetDlgItemText(IDC_DATATYPE_LIST,szType);
	
	for(int i = 0; i < m_ctrlItemList.GetCount(); i++)
	{
		pItem = m_ctrlItemList.GetAt(i);
		if (!pItem || !pItem->m_bSel)
			continue;

		if(pItem->m_szDataType.CompareNoCase(szType) != 0)
		{
			pItem->m_szDataType = szType;
			m_bModified = TRUE;
		}
	}
	m_ctrlItemList.ReDraw();
}

void CDlgOpcVar::OnButtonLoadtxt() //导入
{
	// TODO: Add your control notification handler code here

	/*
	CFileDialog dlg(TRUE, NULL,NULL,OFN_HIDEREADONLY | 	OFN_OVERWRITEPROMPT,"Ecc OPC数据项文件(*.csv)|*.csv||",NULL);

	dlg.m_ofn.Flags &= ~OFN_EXPLORER; //必须加上这一句，否则不能释放退出
	dlg.m_ofn.lpstrTitle = "导入Ecc OPC数据项文件";
	if(IDOK != dlg.DoModal())
		return ;

	CString szPathName = dlg.GetPathName();
	CString szFile = dlg.GetFileName();
	*/
	
	CString szPathName;
	CString szFileName;
	if (!CFileExplorer::ShowFileDialog(szFileName, szPathName, TRUE, _T(".csv"),
		NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("DDC的OPC标签表文件(*.csv)|*.csv||"), 
		_T("导入DDC的OPC标签表文件"), this))
		return;

	FILE* pf=NULL;
	pf = fopen(szPathName,"rt");//文本模式打开
	if(pf == NULL)
	{
		AfxMessageBox("不能打开文件!");
		return ;
	}
	CWaitCursor wc;

	//第一行,第一格
	int nrow=0;
	int ncol = 0;
	TCHAR c;
	CString sz0="";
	while((c = fgetc( pf)) != EOF)
	{
		if(c == ',')
			break;
		else if(c == '\n')
		{
			nrow++;//1
			break;
		}
		else
			sz0 += c;
	}
	if(_stricmp(sz0,"opcitems") != 0)		
	{	
		AfxMessageBox("不是DDC的OPC标签表文件");
		fclose(pf);
		return;
	}
	//移动到第3行
	while(nrow < 2)
	{
		c = fgetc( pf);
		if(c == EOF)
			break;
		if(c == '\n')
			nrow++;//2
	}
	//读第3行参数
	if(nrow != 2)
	{
		AfxMessageBox("文件无参数！");
		fclose(pf);
		return;
	}
	//读数据想
	ncol = 0;
	sz0 = "";
	CString sz1="",sz2 = "",sz3="",sz4="";
	int nDataType = DRT_FLOAT32;//float
	while((c = fgetc( pf)) != EOF)
	{
		if(c == ',')
		{
			ncol++;
			continue;
		}
		else if(c=='\n')
		{
			ncol = 0;
			nrow++;
			if(sz0.IsEmpty())
			{
				sz0 = "";
				sz1 = "";
				sz2 = "";
				sz3 = "";
				sz4 = "";
				continue;
			}
			nDataType = CItemDef::GetTypeVal(sz1);
			sz4.Trim();
			BOOL bZeroFilter = (sz4.CompareNoCase(_T("YES")) == 0);

			m_bModified=AddItemInPort(sz0,sz2,sz3,nDataType,bZeroFilter,TRUE);

			sz0 = "";
			sz1 = "";
			sz2 = "";
			sz3 = "";
			sz4 = "";
			continue;
		}
		if(ncol == 0)
			sz0 += c;
		else if(ncol == 1)
			sz1 += c;
		else if(ncol == 2)
			sz2 += c;
		else if(ncol == 3)
			sz3 += c;
		else if (ncol == 4)
			sz4 += c;
	}
	fclose(pf);

	m_ctrlItemList.ReDraw();

	AfxMessageBox(_T("标签导入成功！"), MB_OK | MB_ICONINFORMATION);
}

void CDlgOpcVar::OnButtonExporttxt() 
{
	// TODO: Add your control notification handler code here
	int n = m_ctrlItemList.GetCount();
	if(n == 0)
	{
		AfxMessageBox("没有数据项!");
		return;
	}

	/*
	CFileDialog	dlg(FALSE,_T(".csv"),NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("CSV(逗号分隔)文件(*.csv)|*.csv||"),
		NULL);
	dlg.m_ofn.Flags &= ~OFN_EXPLORER; //必须加上这一句，否则不能释放退出
	dlg.m_ofn.lpstrTitle=_T("导出OPC数据项配置文件"); 
	if(IDOK!=dlg.DoModal())
	return;
	CString szPathFile=dlg.GetPathName();
	*/
	
	CString szPathName;
	CString szFileName;
	if (!CFileExplorer::ShowFileDialog(szFileName, szPathName, FALSE, _T(".csv"),
		NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("DDC的OPC标签表文件(*.csv)|*.csv||"), 
		_T("导出DDC的OPC标签表文件"), this))
		return;


	FILE *f = fopen (szPathName, "wt");
	if(f==NULL)
	{
		MessageBox(_T("文件不能写！"),_T("DDC提示"),MB_OK|MB_ICONWARNING);
		return;
	}
	CWaitCursor wc;
	CString szline = "opcitems,,,,\n";
	fwrite(szline,szline.GetLength(),1,f);

	szline = "点名,数据类型,描述,单位,零值过滤\n";
	fwrite(szline,szline.GetLength(),1,f);

	
	//开始写数据项
	int i;
	for(i=0;i<n;i++)
	{
		CTagItem *pItem = m_ctrlItemList.GetAt(i);
		if (pItem)
		{
			szline.Format("%s,%s,%s,%s,%s\n", pItem->m_szItemName, pItem->m_szDataType, pItem->m_szDesc, pItem->m_szUnit,
				pItem->m_bZeroFilter?_T("YES"):_T("NO"));
			fwrite(szline,szline.GetLength(),1,f);
		}
	}
	fclose(f);
	AfxMessageBox(_T("标签导出成功！"), MB_OK | MB_ICONINFORMATION);
}

void CDlgOpcVar::OnButtonExportdemo() 
{
	// TODO: Add your control notification handler code here
	/*
	CFileDialog	dlg(FALSE,_T(".csv"),NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("CSV(逗号分隔)文件(*.csv)|*.csv||"),
		NULL);
	dlg.m_ofn.Flags &= ~OFN_EXPLORER; //必须加上这一句，否则不能释放退出
	dlg.m_ofn.lpstrTitle=_T("保存例子OPC设备配置文件"); 
	if(IDOK!=dlg.DoModal())
		return;

	CString szPathFile=dlg.GetPathName();
	*/
	CString szPathName;
	CString szFileName;
	if (!CFileExplorer::ShowFileDialog(szFileName, szPathName, FALSE, _T(".csv"),
		NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("DDC的OPC标签表文件(*.csv)|*.csv||"), 
		_T("导出DDC的OPC标签表文件例子"), this))
		return;

	FILE *f = fopen (szPathName, "wt");
	if(f==NULL)
	{
		MessageBox(_T("文件不能写！"),_T("提示"),MB_OK|MB_ICONWARNING);
		return;
	}
	CString szline = "opcitems,,,,\n";
	fwrite(szline,szline.GetLength(),1,f);

	szline = "点名,数据类型,描述,单位,零值过滤\n";
	fwrite(szline,szline.GetLength(),1,f);

	szline = "numeric.sin.float,float32,模拟正旋曲线,弧度,no\n";
	fwrite(szline,szline.GetLength(),1,f);

	fclose(f);
	AfxMessageBox("创建成功，您可用Excel打开并编辑这个配置文件!",MB_OK|MB_ICONINFORMATION);
}
#include "DlgHelp.h"
void CDlgOpcVar::OnButtonHelp() 
{
	// TODO: Add your control notification handler code here
	CDlgHelp  dlg;
	dlg.DoModal();
}

LRESULT CDlgOpcVar::OnDblclkWinList(WPARAM wParam, LPARAM lParam)
{
	int nRow = (int)lParam;

	if (wParam == 100001)
	{
		DoDblclkLeaflist(nRow);
	}
	else if (wParam == 100002) 
	{
		DoDblclkListVar(nRow);
	}

	return 0;
}
