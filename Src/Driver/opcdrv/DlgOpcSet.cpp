// DlgOpcSet.cpp : implementation file
//

#include "stdafx.h"
#include "opcdrv.h"
#include "myopc.h"
#include "DlgOpcSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// image list indices
#define ILI_CATAGORY	0
#define ILI_COMPONENT	1
#define ILI_INTERFACE	2
/////////////////////////////////////////////////////////////////////////////
// CDlgOpcSet dialog


CDlgOpcSet::CDlgOpcSet(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOpcSet::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgOpcSet)
	m_dwUpdateRate = 1000;
	m_fPercentDead = 0.0f;
	m_szMachine = _T("");
	m_szProgID = _T("");
	m_nTimeArea = 0;
	m_nUpdateTimeout = 0;
	//}}AFX_DATA_INIT
	m_pServerList=NULL;
}


void CDlgOpcSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOpcSet)
	DDX_Text(pDX, IDC_UPDATERATE, m_dwUpdateRate);
	DDV_MinMaxDWord(pDX, m_dwUpdateRate, 0, 4294967295);
	DDX_Text(pDX, IDC_PERCENTDEADBAND, m_fPercentDead);
	DDV_MinMaxFloat(pDX, m_fPercentDead, 0.f, 100.f);
	DDX_Text(pDX, IDC_MACHINENAME, m_szMachine);
	DDV_MaxChars(pDX, m_szMachine, 256);
	DDX_Text(pDX, IDC_PROGID, m_szProgID);
	DDV_MaxChars(pDX, m_szProgID, 256);
	DDX_Text(pDX, IDC_EDIT_TIMEAREA, m_nTimeArea);
	DDV_MinMaxInt(pDX, m_nTimeArea, -12, 12);
	DDX_Text(pDX, IDC_EDIT_UPDATE_TIMEOUT, m_nUpdateTimeout);
	DDV_MinMaxUInt(pDX, m_nUpdateTimeout, 0, 86400);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgOpcSet, CDialog)
	//{{AFX_MSG_MAP(CDlgOpcSet)
	ON_NOTIFY(TVN_SELCHANGED, IDC_SERVERLIST, OnSelchangedServerlist)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, OnButtonConnect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgOpcSet message handlers

BOOL CDlgOpcSet::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CWaitCursor wc;
	CDialog::OnInitDialog ();

	// Initialize the tree control:
	m_pServerList = (CTreeCtrl *) GetDlgItem (IDC_SERVERLIST);
	ASSERT (m_pServerList != NULL);


	m_cImageList.Create (IDB_COMPONENTS, 16, 4, RGB (255, 0, 255));
	m_cImageList.SetBkColor (CLR_NONE);
	m_pServerList->SetImageList (&m_cImageList, TVSIL_NORMAL);

	HTREEITEM hParent;

	// Insert general OPC registered servers:
	hParent = m_pServerList->InsertItem (_T("OPC服务"), ILI_CATAGORY, ILI_CATAGORY);
	DisplayGeneralOPCServers (hParent);
	m_pServerList->Expand(hParent,TVE_EXPAND);

	// return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgOpcSet::OnSelchangedServerlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here

	// see if it was a server selection (i.e, has a parent item)

	// Update page is server was selected (non-NULL parent item):
	if (m_pServerList->GetParentItem (pNMTreeView->itemNew.hItem) != NULL)
		{
		// Transfer selection to ProgID edit control:
		m_szProgID = m_pServerList->GetItemText (pNMTreeView->itemNew.hItem);

		// Update controls:
		this->SetDlgItemText(IDC_PROGID,m_szProgID);
	
		// Make sure selected server is visible:
		m_pServerList->EnsureVisible (pNMTreeView->itemNew.hItem); 
		}

	*pResult = 0;
}

// **************************************************************************
// DisplayGeneralOPCServers ()
//
// Description:
//	Construct a list of installed general OPC servers and insert into tree
//	control.
//
// Parameters:
//  HTREEITEM	hParent		Handle of parent tree control item.
//
// Returns:
//  void
// **************************************************************************
void CDlgOpcSet::DisplayGeneralOPCServers (HTREEITEM hParent)
	{
	HKEY hKey = HKEY_CLASSES_ROOT;		// search under this key
	TCHAR szKey [DEFBUFFSIZE];			// allocate key buffer
	DWORD dwLength = DEFBUFFSIZE;

	// Search the registry for installed OPC Servers:
	for (DWORD dwIndex = 0; 
		 RegEnumKey (hKey, dwIndex, szKey, dwLength) == ERROR_SUCCESS; 
		 ++dwIndex)
		{
		HKEY hSubKey;

		// Open the registry key:
		if (RegOpenKey (hKey, szKey, &hSubKey) == ERROR_SUCCESS)
			{
			// Search for OPC subkey:
			if (RegQueryValue (hSubKey, _T("OPC"), NULL, NULL) == ERROR_SUCCESS)
				{
				// Display the prog ID for this server:
				m_pServerList->InsertItem (szKey, ILI_COMPONENT, ILI_COMPONENT, hParent);
				}

			// Close the registry key:
			RegCloseKey (hSubKey);
			}

		// Re-initialize length:
		dwLength = DEFBUFFSIZE;
		}
	}

void CDlgOpcSet::OnOK() 
{
	// TODO: Add extra validation here
	if(!UpdateData(TRUE))
		return;

	if(m_szProgID.IsEmpty())
	{
		AfxMessageBox("请输入OPC服务接口的ProgID！");
		this->GetDlgItem(IDC_PROGID)->SetFocus();
		return;
	}
	CDialog::OnOK();
}

BOOL OPCServerList(LPCTSTR lpszIP,CString &szProgID);

void CDlgOpcSet::OnButtonConnect() 
{
	// TODO: Add your control notification handler code here
	CString szProgID = "";
	CString szIP = "";
	this->GetDlgItemText(IDC_MACHINENAME,szIP);
	szIP.TrimLeft();
	szIP.TrimRight();
	if(szIP.IsEmpty())
	{
		m_pServerList->DeleteAllItems();
		HTREEITEM hParent;
		hParent = m_pServerList->InsertItem (_T("OPC服务"), ILI_CATAGORY, ILI_CATAGORY);
		DisplayGeneralOPCServers (hParent);
		m_pServerList->Expand(hParent,TVE_EXPAND);
		return;
	}

	if(OPCServerList(szIP,szProgID))
	{
		m_pServerList->DeleteAllItems();
	
		HTREEITEM hParent;
		hParent = m_pServerList->InsertItem (_T("OPC服务"), ILI_CATAGORY, ILI_CATAGORY);
		LPCTSTR pc = szProgID;
		
		CString szt = "";
		while(*pc != 0)
		{
			if(*pc == '\n')
			{
				m_pServerList->InsertItem (szt, ILI_COMPONENT, ILI_COMPONENT, hParent);
				szt = "";
			}
			else 
				szt += *pc;
			pc++;
		}
		m_pServerList->Expand(hParent,TVE_EXPAND);
	}
	else
	{
		AfxMessageBox("连接失败!");
	}
	
}
