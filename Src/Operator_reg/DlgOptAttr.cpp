// DlgOptAttr.cpp : implementation file
//

#include "stdafx.h"
#include "Operator.h"
#include "DlgOptAttr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgOptAttr dialog


CDlgOptAttr::CDlgOptAttr(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOptAttr::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgOptAttr)
	m_bUse = TRUE;
	m_szName = _T("");
	m_szPass1 = _T("");
	m_szPass2 = _T("");
	//}}AFX_DATA_INIT

	m_szGroup = ""; //组
	m_bAdd = TRUE;
	m_pOperator = NULL;
}


void CDlgOptAttr::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOptAttr)
	DDX_Control(pDX, IDC_COMBO_GROUP, m_cbGroup);
	DDX_Check(pDX, IDC_CHECK_USE, m_bUse);
	DDX_Text(pDX, IDC_EDIT_NAME, m_szName);
	DDV_MaxChars(pDX, m_szName, 16);
	DDX_Text(pDX, IDC_EDIT_PASS1, m_szPass1);
	DDV_MaxChars(pDX, m_szPass1, 16);
	DDX_Text(pDX, IDC_EDIT_PASS2, m_szPass2);
	DDV_MaxChars(pDX, m_szPass2, 16);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgOptAttr, CDialog)
	//{{AFX_MSG_MAP(CDlgOptAttr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgOptAttr message handlers

BOOL CDlgOptAttr::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	ASSERT(m_pOperator != NULL);

	m_pOperator->Group2ComboBox(&m_cbGroup);
	
	m_cbGroup.SelectString(-1,m_szGroup);


	if(!m_bAdd)
	{
		GetDlgItem(IDC_EDIT_NAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_PASS1)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_PASS2)->EnableWindow(FALSE);

		SetWindowText("修改账号");
	}
	else
		SetWindowText("添加账号");
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgOptAttr::OnOK() 
{
	// TODO: Add extra validation here
	if(!UpdateData(TRUE))
		return;
	m_szName.TrimLeft();
	m_szName.TrimRight();
	if(m_szName.IsEmpty())
	{
		AfxMessageBox("请填写账号!");
		return;
	}
	int nIndex = m_cbGroup.GetCurSel();
	if(nIndex == -1)
	{
		AfxMessageBox("请指定角色!");
		return;
	}
	m_cbGroup.GetLBText(nIndex,m_szGroup);
	if(m_bAdd)
	{
		if(m_szPass1.IsEmpty() || m_szPass2.IsEmpty())
		{
			AfxMessageBox("请填写初始密码!");
			return;
		}
		if(m_szPass1 != m_szPass2) 
		{
			AfxMessageBox("初始密码验证错误!");
			return;
		}
		if(m_pOperator->FindOpt(m_szName))
		{
			AfxMessageBox("账号已存在!");
			return;
		}
	}

	CDialog::OnOK();
}
