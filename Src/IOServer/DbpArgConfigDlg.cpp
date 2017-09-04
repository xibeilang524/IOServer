// DbpArgConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IOServer.h"
#include "DbpArgConfigDlg.h"
#include "DbpArgAttrDlg.h"

// CDbpArgConfigDlg 对话框

IMPLEMENT_DYNAMIC(CDbpArgConfigDlg, CDialog)

CDbpArgConfigDlg::CDbpArgConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDbpArgConfigDlg::IDD, pParent)
	, m_bModify(FALSE)
	, m_nSize(0)
{
	memset(m_DbpArgs, 0, sizeof(TDBPARG) * MAX_DBP_CONARGS);
}

CDbpArgConfigDlg::~CDbpArgConfigDlg()
{
}

void CDbpArgConfigDlg::SetDbpConArgs(CDBPArgs& dbpArgs)
{
	m_nSize = dbpArgs.size();
	for (int i=0; i<dbpArgs.size(); i++)
	{
		TDBPARG tDBPArg = dbpArgs.at(i);
		m_DbpArgs[i].shNetMode = tDBPArg.shNetMode;
		m_DbpArgs[i].wport = tDBPArg.wport;
		memcpy_s(m_DbpArgs[i].sip, sizeof(m_DbpArgs[i].sip), tDBPArg.sip, sizeof(tDBPArg.sip));
		memcpy_s(m_DbpArgs[i].suser, sizeof(m_DbpArgs[i].suser), tDBPArg.suser, sizeof(tDBPArg.suser));
		memcpy_s(m_DbpArgs[i].spass, sizeof(m_DbpArgs[i].spass), tDBPArg.spass, sizeof(tDBPArg.spass));
		memcpy_s(m_DbpArgs[i].sres, sizeof(m_DbpArgs[i].sres), tDBPArg.sres, sizeof(tDBPArg.sres));
	}

}
BOOL CDbpArgConfigDlg::GetDbpConArgs(CDBPArgs& dbpArgs)
{
	dbpArgs.clear();
	for (int i=0; i<m_nSize; i++)
	{
		TDBPARG tDBPArg;
		tDBPArg.shNetMode = m_DbpArgs[i].shNetMode;
		tDBPArg.wport = m_DbpArgs[i].wport;
		memcpy_s(tDBPArg.sip, sizeof(tDBPArg.sip), m_DbpArgs[i].sip, sizeof(m_DbpArgs[i].sip));
		memcpy_s(tDBPArg.suser, sizeof(tDBPArg.suser), m_DbpArgs[i].suser, sizeof(m_DbpArgs[i].suser));
		memcpy_s(tDBPArg.spass, sizeof(tDBPArg.spass), m_DbpArgs[i].spass, sizeof(m_DbpArgs[i].spass));
		memcpy_s(tDBPArg.sres, sizeof(tDBPArg.sres), m_DbpArgs[i].sres, sizeof(m_DbpArgs[i].sres));
		dbpArgs.push_back(tDBPArg);
	}
	return TRUE;
}
//
//void CDbpArgConfigDlg::SetDbpConArgs(CDbpConArgs *pDbpArgs)
//{
//	memcpy(m_DbpArgs, pDbpArgs->m_args, sizeof(TDBPARG) * MAX_DBP_CONARGS);
//	m_nSize = pDbpArgs->m_nsize;
//}
//
//BOOL CDbpArgConfigDlg::GetDbpConArgs(CDbpConArgs *pDbpArgs)
//{
//	if ((m_nSize != pDbpArgs->m_nsize) || (memcmp(pDbpArgs->m_args, m_DbpArgs, sizeof(TDBPARG) * MAX_DBP_CONARGS)))
//	{
//		memcpy(pDbpArgs->m_args, m_DbpArgs, sizeof(TDBPARG) * MAX_DBP_CONARGS);
//		pDbpArgs->m_nsize = m_nSize;
//
//		return TRUE;
//	}
//	else
//		return FALSE;
//}

void CDbpArgConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ARGS, m_ctrlDbpArgs);
}


BEGIN_MESSAGE_MAP(CDbpArgConfigDlg, CDialog)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ARGS, &CDbpArgConfigDlg::OnNMDblclkListArgs)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CDbpArgConfigDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, &CDbpArgConfigDlg::OnBnClickedButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &CDbpArgConfigDlg::OnBnClickedButtonDel)
END_MESSAGE_MAP()


// CDbpArgConfigDlg 消息处理程序

BOOL CDbpArgConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bModify = FALSE;
	m_ImageList.Create(IDB_BITMAP_DBPARG, 16, 1, RGB(255,255,255)); 

	m_ctrlDbpArgs.SetImageList(&m_ImageList,LVSIL_SMALL);
	m_ctrlDbpArgs.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

	m_ctrlDbpArgs.InsertColumn(0,"IP地址",LVCFMT_LEFT,120);
	m_ctrlDbpArgs.InsertColumn(1,"端口",LVCFMT_LEFT,60);
	m_ctrlDbpArgs.InsertColumn(2,"账号",LVCFMT_LEFT,80);
	m_ctrlDbpArgs.InsertColumn(3,"密码",LVCFMT_LEFT,80);
	m_ctrlDbpArgs.InsertColumn(4,"网络类型",LVCFMT_CENTER,80);

	int i,nIndex = 0;
	CString szt;
	for(i = 0; i < m_nSize; i++)
	{
		nIndex = m_ctrlDbpArgs.InsertItem(nIndex + 1, m_DbpArgs[i].sip, 0);
		szt.Format("%d",m_DbpArgs[i].wport);
		m_ctrlDbpArgs.SetItemText(nIndex, 1, szt);
		m_ctrlDbpArgs.SetItemText(nIndex, 2, m_DbpArgs[i].suser);
		m_ctrlDbpArgs.SetItemText(nIndex, 3, m_DbpArgs[i].spass);

		if (m_DbpArgs[i].shNetMode == NET_MODE_UDP)
			szt = _T("UDP");
		else
			szt = _T("TCP");
		m_ctrlDbpArgs.SetItemText(nIndex, 4, szt);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDbpArgConfigDlg::OnNMDblclkListArgs(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnBnClickedButtonEdit();
	*pResult = 0;
}

void CDbpArgConfigDlg::OnBnClickedButtonAdd()
{
	// TODO: Add your control notification handler code here
	if (m_ctrlDbpArgs.GetItemCount() >= MAX_DBP_CONARGS)
	{
		MessageBox("已经达到最大连接数!","提示");
		return;
	}

	CDbpArgAttrDlg dlg;

	if(dlg.DoModal() != IDOK)
		return;

	int nIndex = m_ctrlDbpArgs.InsertItem(m_ctrlDbpArgs.GetItemCount(),dlg.m_szip,0);
	CString szt;
	szt.Format("%d",dlg.m_wport);
	m_ctrlDbpArgs.SetItemText(nIndex, 1, szt);
	m_ctrlDbpArgs.SetItemText(nIndex, 2, dlg.m_szuser);
	m_ctrlDbpArgs.SetItemText(nIndex, 3, dlg.m_szpass);

	if (dlg.m_nNetMode == NET_MODE_UDP)
		szt = _T("UDP");
	else
		szt = _T("TCP");
	m_ctrlDbpArgs.SetItemText(nIndex, 4, szt);

	m_bModify = TRUE;
}

void CDbpArgConfigDlg::OnBnClickedButtonEdit()
{
	int nIndex = -1;
	nIndex = m_ctrlDbpArgs.GetNextItem(-1,LVNI_SELECTED);
	if(-1 == nIndex)
	{
		MessageBox("请选择一个连接参数!","提示");
		return;
	}
	CString szip,szuser,szpass, sznet;
	WORD wport;
	int nNetMode;

	CDbpArgAttrDlg dlg;

	dlg.m_bAdd = FALSE;
	dlg.m_szip = m_ctrlDbpArgs.GetItemText(nIndex,0);
	dlg.m_wport = (WORD)atoi(m_ctrlDbpArgs.GetItemText(nIndex,1));
	dlg.m_szuser = m_ctrlDbpArgs.GetItemText(nIndex,2);
	dlg.m_szpass = m_ctrlDbpArgs.GetItemText(nIndex,3);

	sznet = m_ctrlDbpArgs.GetItemText(nIndex, 4);
	if (sznet.CompareNoCase(_T("UDP")) == 0)
		dlg.m_nNetMode = NET_MODE_UDP;
	else
		dlg.m_nNetMode = NET_MODE_TCP;

	szip = dlg.m_szip;
	szuser = dlg.m_szuser;
	szpass = dlg.m_szpass;
	wport = dlg.m_wport;
	nNetMode = dlg.m_nNetMode;

	if(dlg.DoModal()!= IDOK)
		return;
	if((szip != dlg.m_szip)||
		(szuser != dlg.m_szuser)||
		(szpass != dlg.m_szpass)||
		(wport != (WORD)dlg.m_wport)||
		(nNetMode != dlg.m_nNetMode)
		)
	{
		CString szt;
		szt.Format("%d",dlg.m_wport);
		m_ctrlDbpArgs.SetItemText(nIndex,0,dlg.m_szip);
		m_ctrlDbpArgs.SetItemText(nIndex,1,szt);
		m_ctrlDbpArgs.SetItemText(nIndex,2,dlg.m_szuser);
		m_ctrlDbpArgs.SetItemText(nIndex,3,dlg.m_szpass);

		if (dlg.m_nNetMode == NET_MODE_UDP)
			szt = _T("UDP");
		else
			szt = _T("TCP");
		m_ctrlDbpArgs.SetItemText(nIndex, 4, szt);

		m_bModify = TRUE;
	}
}

void CDbpArgConfigDlg::OnBnClickedButtonDel()
{
	int nIndex = -1;
	nIndex = m_ctrlDbpArgs.GetNextItem(-1, LVNI_SELECTED);
	if(nIndex == -1)
	{
		MessageBox("请选择一个连接参数!","提示");
		return;
	}

	if(MessageBox("是否要删除?", "询问", MB_YESNOCANCEL | MB_ICONQUESTION) != IDYES)
		return;
	m_ctrlDbpArgs.DeleteItem(nIndex);

	m_bModify = TRUE;
}

void CDbpArgConfigDlg::OnOK()
{
	CString szt;

	memset(m_DbpArgs, 0, sizeof(TDBPARG) * MAX_DBP_CONARGS);

	m_nSize = m_ctrlDbpArgs.GetItemCount();
	for (int i = 0; i < m_nSize; i++)
	{
		szt = m_ctrlDbpArgs.GetItemText(i, 0);
		strcpy(m_DbpArgs[i].sip, szt);

		m_DbpArgs[i].wport = (WORD)atoi(m_ctrlDbpArgs.GetItemText(i, 1));

		szt = m_ctrlDbpArgs.GetItemText(i, 2);
		strcpy(m_DbpArgs[i].suser, szt);

		szt = m_ctrlDbpArgs.GetItemText(i, 3);
		strcpy(m_DbpArgs[i].spass, szt);

		szt = m_ctrlDbpArgs.GetItemText(i, 4);
		if (szt.CompareNoCase(_T("UDP")) == 0)
			m_DbpArgs[i].shNetMode = NET_MODE_UDP;
		else
			m_DbpArgs[i].shNetMode = NET_MODE_TCP;
	}

	CDialog::OnOK();
}
