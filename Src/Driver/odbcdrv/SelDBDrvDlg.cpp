// SelDBDrvDlg.cpp : implementation file
//

#include "stdafx.h"
#include "odbcdrv.h"
#include "SelDBDrvDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelDBDrvDlg dialog


CSelDBDrvDlg::CSelDBDrvDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelDBDrvDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelDBDrvDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSelDBDrvDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelDBDrvDlg)
	DDX_Control(pDX, IDC_LIST_DBTYPE, m_ctrlList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelDBDrvDlg, CDialog)
	//{{AFX_MSG_MAP(CSelDBDrvDlg)
	ON_COMMAND(IDOK, OnOK)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSelDBDrvDlg::OnOK()
{
	// 用于解决在编辑框中按回车后该窗口自动关闭的BUG
}

LPDBTYPEDATA CSelDBDrvDlg::GetCurSel()
{
	CListBox *pListBox = (CListBox *)GetDlgItem(IDC_LIST_DBTYPE);
	int nIndex = pListBox->GetCurSel();
	if (nIndex == -1)
		return NULL;
	else
		return (LPDBTYPEDATA)pListBox->GetItemData(nIndex);
}

void CSelDBDrvDlg::SetConnectString(CConnStringParser &parser)
{
	for (int i = 0; i < m_ctrlList.GetCount(); i++)
	{
		LPDBTYPEDATA pData = (LPDBTYPEDATA)m_ctrlList.GetItemData(i);
		if (pData->nTypeID == parser.GetDBType())
		{
			m_ctrlList.SetCurSel(i);
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSelDBDrvDlg message handlers

BOOL CSelDBDrvDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ctrlList.ResetContent();
	
	LPDBTYPEDATA pDbTypeDatas = CConnStringParser::GetDBTypeData();
	for(int i = 0; i < CConnStringParser::GetDBTypeCount(); i++)
	{
		LPDBTYPEDATA pData = &pDbTypeDatas[i];
		
		if (pData->nTypeID != DBT_UNKNOWN)
		{			
			int nIndex = m_ctrlList.AddString(pData->szName);
			m_ctrlList.SetItemDataPtr(nIndex, pData);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
