// DbpArgAttrDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IOServer.h"
#include "DbpArgAttrDlg.h"


// CDbpArgAttrDlg 对话框

IMPLEMENT_DYNAMIC(CDbpArgAttrDlg, CDialog)

CDbpArgAttrDlg::CDbpArgAttrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDbpArgAttrDlg::IDD, pParent)
	, m_wport(12084)
	, m_szuser(_T(""))
	, m_szpass(_T(""))
	, m_nNetMode(0)
	, m_bAdd(TRUE)
{

}

CDbpArgAttrDlg::~CDbpArgAttrDlg()
{
}

void CDbpArgAttrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS1, m_ip);
	DDX_Text(pDX, IDC_EDIT_PORT, m_wport);
	DDX_Text(pDX, IDC_EDIT_USER, m_szuser);
	DDX_Text(pDX, IDC_EDIT_PASS, m_szpass);
	DDX_CBIndex(pDX, IDC_COMBO_NETMODE, m_nNetMode);
}


BEGIN_MESSAGE_MAP(CDbpArgAttrDlg, CDialog)
END_MESSAGE_MAP()


// CDbpArgAttrDlg 消息处理程序

BOOL CDbpArgAttrDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (!m_bAdd)
		SetWindowText("修改连接参数");

	m_ip.SetWindowText(m_szip);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDbpArgAttrDlg::OnOK()
{
	UpdateData();

	GetDlgItemText(IDC_IPADDRESS1, m_szip);
	if (m_szip == "0.0.0.0")
	{
		MessageBox("请填写IP地址!","提示");
		return;
	}

	CDialog::OnOK();
}
