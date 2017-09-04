// DlgOptMan.cpp : implementation file
//

#include "stdafx.h"
#include "Operator.h"
#include "DlgOptMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgOptMan dialog


CDlgOptMan::CDlgOptMan(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOptMan::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgOptMan)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pOperator = NULL;
	m_szOpt = "";
}


void CDlgOptMan::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOptMan)
	DDX_Control(pDX, IDC_LIST_OPT, m_ListCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgOptMan, CDialog)
	//{{AFX_MSG_MAP(CDlgOptMan)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_OPT, OnDblclkListOpt)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnButtonEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgOptMan message handlers

void CDlgOptMan::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CDlgOptMan::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

BOOL CDlgOptMan::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_ListCtrl.InsertColumn(0,"账号",LVCFMT_LEFT,80);
	m_ListCtrl.InsertColumn(1,"角色",LVCFMT_LEFT,80);
	m_ListCtrl.InsertColumn(2,"状态",LVCFMT_LEFT,80);
	
	ASSERT(m_pOperator != NULL);
	
	m_pOperator->Opt2ListCtrl(&m_ListCtrl);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

#include "DlgOptAttr.h"
void CDlgOptMan::OnDblclkListOpt(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	ASSERT(m_pOperator != NULL);

	int nIndex = m_ListCtrl.GetNextItem(-1,LVNI_SELECTED);
	if(nIndex == -1)
		return;
	OnButtonEdit();
	*pResult = 0;
}

void CDlgOptMan::OnButtonAdd() 
{
	// TODO: Add your control notification handler code here
	ASSERT(m_pOperator != NULL);

	CDlgOptAttr dlg;
	dlg.m_pOperator = m_pOperator;
	dlg.m_bAdd = TRUE;
	if(dlg.DoModal() != IDOK)
		return;
	if(m_pOperator->AddOpt(dlg.m_szName,dlg.m_szPass1,dlg.m_szGroup,dlg.m_bUse))//不能添加
	{
		AfxMessageBox("添加错误!");
	}
	int nIndex = m_ListCtrl.InsertItem(10000,dlg.m_szName);
	m_ListCtrl.SetItemText(nIndex,1,dlg.m_szGroup);
	if(dlg.m_bUse)
		m_ListCtrl.SetItemText(nIndex,2,"活动");
	else
		m_ListCtrl.SetItemText(nIndex,2,"禁用");
}

void CDlgOptMan::OnButtonDel() 
{
	// TODO: Add your control notification handler code here
	ASSERT(m_pOperator != NULL);

	int nIndex = m_ListCtrl.GetNextItem(-1,LVNI_SELECTED);
	if(nIndex == -1)
	{
		AfxMessageBox("请选择一个账号!");
		return;
	}
	CString sName = m_ListCtrl.GetItemText(nIndex,0);
	if(m_szOpt == sName)
	{
		AfxMessageBox("禁止删除当前登录账号!");
		return;
	}

	CString szMsg;
	szMsg.Format("是否要删除<%s>?",sName);
	if(IDYES != AfxMessageBox(szMsg,MB_YESNO | MB_ICONQUESTION))
		return ;
	if(m_pOperator->DelOpt(sName))
	{
		AfxMessageBox("删除错误!");
		return;
	}
	m_ListCtrl.DeleteItem(nIndex);
}

void CDlgOptMan::OnButtonEdit() 
{
	// TODO: Add your control notification handler code here
	ASSERT(m_pOperator != NULL);

	int nIndex = m_ListCtrl.GetNextItem(-1,LVNI_SELECTED);
	if(nIndex == -1)
	{
		AfxMessageBox("请选择一个账号!");
		return;
	}

	CString szopt = m_ListCtrl.GetItemText(nIndex,0);
	if(m_szOpt == szopt)
	{
		AfxMessageBox("禁止修改当前登录账号!");
		return;
	}

	CDlgOptAttr dlg;
	dlg.m_pOperator = m_pOperator;

	dlg.m_bAdd = FALSE;
	dlg.m_szName = m_ListCtrl.GetItemText(nIndex,0);
	dlg.m_szGroup = m_ListCtrl.GetItemText(nIndex,1);
	if(m_ListCtrl.GetItemText(nIndex,2) != "活动")
		dlg.m_bUse = FALSE;
	else
		dlg.m_bUse = TRUE;

	
	if(dlg.DoModal() != IDOK)
		return;
	if(m_pOperator->ModifyOpt(dlg.m_szName,dlg.m_szGroup,dlg.m_bUse))//不能添加
	{
		AfxMessageBox("修改错误错误!");
		return;
	}
	m_ListCtrl.SetItemText(nIndex,1,dlg.m_szGroup);
	if(dlg.m_bUse)
		m_ListCtrl.SetItemText(nIndex,2,"活动");
	else
		m_ListCtrl.SetItemText(nIndex,2,"禁用");
}
