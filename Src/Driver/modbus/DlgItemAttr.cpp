// DlgItemAttr.cpp : implementation file
//

#include "stdafx.h"
#include "modbus.h"
#include "DlgItemAttr.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgItemAttr dialog

/*
CDlgItemAttr::CDlgItemAttr(CWnd* pParent )
	: CDialog(CDlgItemAttr::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgItemAttr)
	m_nrtype = 2;
	m_ucDecAddr = 0;
	m_sName = _T("");
	m_sNote = _T("");
	m_ucOrder = 3;
	m_sRegAddr = _T("");
	m_nb4pos = FALSE;
	//}}AFX_DATA_INIT

	m_bAdd = TRUE;//添加还是删除
	m_szID = "" ;//保存旧的ID,用于修改
	m_wRegAddr = 0;//寄存器地址
	m_pconfig = NULL;//配置表
}


void CDlgItemAttr::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgItemAttr)
	DDX_CBIndex(pDX, IDC_COMBO_RTYPE, m_nrtype);
	DDX_Text(pDX, IDC_EDIT_DECADDR, m_ucDecAddr);
	DDV_MinMaxByte(pDX, m_ucDecAddr, 1, 255);
	DDX_Text(pDX, IDC_EDIT_NAME, m_sName);
	DDX_Text(pDX, IDC_EDIT_NOTE, m_sNote);
	DDX_Text(pDX, IDC_EDIT_ORDER, m_ucOrder);
	DDV_MinMaxByte(pDX, m_ucOrder, 1, 255);
	DDX_Text(pDX, IDC_EDIT_REGADDR, m_sRegAddr);
	DDV_MaxChars(pDX, m_sRegAddr, 6);
	DDX_Check(pDX, IDC_CHECK_4, m_nb4pos);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgItemAttr, CDialog)
	//{{AFX_MSG_MAP(CDlgItemAttr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgItemAttr message handlers

BOOL CDlgItemAttr::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if(m_bAdd)
		SetWindowText(g_LoadString(IDS_ADDITEM));//"添加数据项"
	else
		SetWindowText(g_LoadString(IDS_ADDITEM));//"修改数据项"

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
*/
/*
void CDlgItemAttr::OnOK() 
{
	// TODO: Add extra validation here
	if(!UpdateData(TRUE))
		return;
	m_sName.TrimLeft();
	m_sName.TrimRight();
	if(m_sName.IsEmpty())
	{
		AfxMessageBox(IDS_NEEDNAME);//"请填写名称");
		return;
	}
	*/
//	if(-1 != m_sName.FindOneOf("+-*/()<>%^"))
//	{
//		AfxMessageBox(IDS_ITEMNAMEERR);//"变量名中不能含有 + - * / ( ) < > % ^ 字符!");
//		return;
//	}
/*	m_wRegAddr = (WORD)EcdApi::HexStr2DWORD(		// 16进制字符串转换为DWORD,支持0x打头和H结尾
		m_sRegAddr				// [in]
		);

	if((m_ucOrder != 0x03)&&(m_ucOrder != 0x06)&&(m_ucOrder != 0x10))
	{
		AfxMessageBox(IDS_NOTSUPORTOD);//"目前支持03H,06H,10H命令！");
		return;
	}
	if(this->m_bAdd)
	{
		CItemDef *pi = this->m_pconfig->Find(m_sName);
		if(pi!=NULL)
		{
			CString szMsg;
			szMsg.Format("[%s]%s",m_sName,g_LoadString(IDS_RENAME));//名称作为ID重复，请重新命名!
			MessageBox(szMsg);
			return;
		}
		pi = new CItemDef;
		pi->sname = this->m_sName;
		pi->snote = this->m_sNote;
		pi->ucdecaddr = this->m_ucDecAddr;
		pi->ucorder = this->m_ucOrder;
		pi->ucrtype = (BYTE)this->m_nrtype;
		pi->waddr = this->m_wRegAddr;
		pi->Setflagbit(0,this->m_nb4pos);
		if(m_pconfig->Add(pi))
		{
			m_pconfig->sort();
			m_pconfig->m_bModify = TRUE;
		}
	}
	else //修改
	{
		CItemDef *pi;
		if(m_szID != m_sName)
		{
			 pi = m_pconfig->Find(m_sName);
			 if(pi!=NULL)
			 {
				CString szMsg;
				szMsg.Format("[%s]%s",m_sName,g_LoadString(IDS_RENAME));//名称作为ID重复，请重新命名!
				MessageBox(szMsg);
				return;
			 }
		}
		pi = m_pconfig->Find(m_szID);
		if( (pi->sname != m_sName)||
			(pi->snote != m_sNote)||
			(pi->ucdecaddr != m_ucDecAddr)||
			(pi->ucorder != m_ucOrder)||
			(pi->ucrtype != (BYTE)m_nrtype)||
			(pi->Getflagbit(0) != this->m_nb4pos)||				
			(pi->waddr != m_wRegAddr)
		 )
		{
			pi->sname = m_sName;
			pi->snote = m_sNote;
			pi->ucdecaddr = m_ucDecAddr;
			pi->ucorder = m_ucOrder;
			pi->ucrtype = (BYTE)m_nrtype;
			pi->waddr = m_wRegAddr;
			pi->Setflagbit(0,this->m_nb4pos);
			m_pconfig->sort();
			m_pconfig->m_bModify = TRUE;
		}
	}

	CDialog::OnOK();
}

void CDlgItemAttr::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}
*/