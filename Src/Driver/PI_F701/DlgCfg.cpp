// DlgCfg.cpp : implementation file
//

#include "stdafx.h"
#include "PI_F701Drv.h"
#include "DlgCfg.h"
#include "DlgPIServerItem.h"
#include "SqlServerConnect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgCfg dialog


CDlgCfg::CDlgCfg(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCfg::IDD, pParent)
	, m_szSqlServer(_T("172.0.0.1"))
	, m_dwPort(1433)
	, m_szDBName(_T("F701"))
	, m_szUser(_T("sa"))
	, m_szPsw(_T("sa123456"))
	, m_bPIServer(FALSE)
	, m_bSqlServer(FALSE)
	, m_szTable(_T(""))
{
	//{{AFX_DATA_INIT(CDlgCfg)
	//}}AFX_DATA_INIT
}

void CDlgCfg::SetPIServerList()
{
	for (int i = 0; i < m_arrPIServerList.GetCount(); i++)
	{
		CPIServerItem &item = m_arrPIServerList[i];

		CString szt;
		szt.Format(_T("%d"), i + 1);
		int nIndex = m_ctrlPIServers.InsertItem(m_ctrlPIServers.GetItemCount(), szt);
		if (nIndex != -1)
		{
			m_ctrlPIServers.SetItemText(nIndex, 1, item.m_szIP);
			m_ctrlPIServers.SetItemText(nIndex, 2, item.m_szPort);
			m_ctrlPIServers.SetItemText(nIndex, 3, item.m_szUserName);
			m_ctrlPIServers.SetItemText(nIndex, 4, item.m_szPassword);
			szt.Format(_T("%d"), item.m_nConnTimeout);
			m_ctrlPIServers.SetItemText(nIndex, 5, szt);
			szt.Format(_T("%d"), item.m_nDATimeout);
			m_ctrlPIServers.SetItemText(nIndex, 6, szt);
		}
	}
}

void CDlgCfg::GetPIServerList()
{
	m_arrPIServerList.RemoveAll();
	for (int i = 0; i < m_ctrlPIServers.GetItemCount(); i++)
	{
		CPIServerItem item;

		item.m_szIP = m_ctrlPIServers.GetItemText(i, 1);
		item.m_szPort = m_ctrlPIServers.GetItemText(i, 2);
		item.m_szUserName = m_ctrlPIServers.GetItemText(i, 3);
		item.m_szPassword = m_ctrlPIServers.GetItemText(i, 4);
		
		CString szt = m_ctrlPIServers.GetItemText(i, 5);
		item.m_nConnTimeout = atoi(szt);

		szt = m_ctrlPIServers.GetItemText(i, 6);
		item.m_nDATimeout = atoi(szt);

		m_arrPIServerList.Add(item);
	}
}

void CDlgCfg::UpdateButtonState()
{
	POSITION pos = m_ctrlPIServers.GetFirstSelectedItemPosition();
	int nIndex = m_ctrlPIServers.GetNextSelectedItem(pos);

	if (nIndex == -1)
	{
		GetDlgItem(IDC_BUTTON_EDIT)->EnableWindow(FALSE);	
		GetDlgItem(IDC_BUTTON_DELETE)->EnableWindow(FALSE);	
		GetDlgItem(IDC_BUTTON_UP)->EnableWindow(FALSE);	
		GetDlgItem(IDC_BUTTON_DOWN)->EnableWindow(FALSE);
	}
	else {
		GetDlgItem(IDC_BUTTON_EDIT)->EnableWindow(TRUE);	
		GetDlgItem(IDC_BUTTON_DELETE)->EnableWindow(TRUE);	
		GetDlgItem(IDC_BUTTON_UP)->EnableWindow(nIndex != 0);	
		GetDlgItem(IDC_BUTTON_DOWN)->EnableWindow(nIndex != m_ctrlPIServers.GetItemCount() - 1);
	}
}

void CDlgCfg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCfg)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_LIST_PISERVER, m_ctrlPIServers);
	DDX_Text(pDX, IDC_EDIT_SERVER, m_szSqlServer);
	DDX_Text(pDX, IDC_EDIT_PORT, m_dwPort);
	DDX_Text(pDX, IDC_EDIT_DATABASE, m_szDBName);
	DDX_Text(pDX, IDC_EDIT_USER, m_szUser);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_szPsw);
	DDX_Check(pDX, IDC_CHK_PI_SERVER, m_bPIServer);
	DDX_Check(pDX, IDC_CHK_SQL_SERVER, m_bSqlServer);
	DDX_Text(pDX, IDC_EDIT_TABLE, m_szTable);
}


BEGIN_MESSAGE_MAP(CDlgCfg, CDialog)
	//{{AFX_MSG_MAP(CDlgCfg)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CDlgCfg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, &CDlgCfg::OnBnClickedButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CDlgCfg::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_UP, &CDlgCfg::OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, &CDlgCfg::OnBnClickedButtonDown)
	ON_NOTIFY(NM_CLICK, IDC_LIST_PISERVER, &CDlgCfg::OnNMClickListPiserver)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_PISERVER, &CDlgCfg::OnNMDblclkListPiserver)
	ON_BN_CLICKED(IDC_BTN_SQL_TEST, &CDlgCfg::OnBnClickedBtnSqlTest)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgCfg message handlers

BOOL CDlgCfg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	DWORD dwStyle = m_ctrlPIServers.GetExtendedStyle();
	dwStyle |= LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP;
	m_ctrlPIServers.SetExtendedStyle(dwStyle);

	m_ctrlPIServers.InsertColumn(0, _T("序号"), LVCFMT_LEFT, 40);
	m_ctrlPIServers.InsertColumn(1, _T("IP地址"), LVCFMT_LEFT, 120);
	m_ctrlPIServers.InsertColumn(2, _T("端口号"), LVCFMT_LEFT, 60);
	m_ctrlPIServers.InsertColumn(3, _T("用户名"), LVCFMT_LEFT, 80);
	m_ctrlPIServers.InsertColumn(4, _T("密码"), LVCFMT_LEFT, 80);
	m_ctrlPIServers.InsertColumn(5, _T("连接超时"), LVCFMT_LEFT, 70);
	m_ctrlPIServers.InsertColumn(6, _T("访问超时"), LVCFMT_LEFT, 70);

	SetPIServerList();

	m_szSqlServer = m_tServerInfo.szServer;
	m_szDBName = m_tServerInfo.szDBName;
	m_szUser = m_tServerInfo.szUser;
	m_szPsw = m_tServerInfo.szPsw;
	m_dwPort = m_tServerInfo.uiPort;
	m_szTable = m_tServerInfo.szTable;

	UpdateButtonState();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgCfg::OnOK()
{
	UpdateData();

	GetPIServerList();
	m_tServerInfo.szServer = m_szSqlServer;
	m_tServerInfo.uiPort = (UINT)m_dwPort;
	m_tServerInfo.szDBName = m_szDBName;
	m_tServerInfo.szUser = m_szUser;
	m_tServerInfo.szPsw = m_szPsw;
	m_tServerInfo.szTable = m_szTable;

	CDialog::OnOK();
}

void CDlgCfg::OnBnClickedButtonAdd()
{
	CDlgPIServerItem dlg;

	if (dlg.DoModal() != IDOK)
		return;

	CString szt;
	szt.Format(_T("%d"), m_ctrlPIServers.GetItemCount() + 1);
	int nIndex = m_ctrlPIServers.InsertItem(m_ctrlPIServers.GetItemCount(), szt);
	if (nIndex != -1)
	{
		m_ctrlPIServers.SetItemText(nIndex, 1, dlg.m_szIP);
		m_ctrlPIServers.SetItemText(nIndex, 2, dlg.m_szPort);
		m_ctrlPIServers.SetItemText(nIndex, 3, dlg.m_szUserName);
		m_ctrlPIServers.SetItemText(nIndex, 4, dlg.m_szPassword);
		szt.Format(_T("%d"), dlg.m_nConnTimeout);
		m_ctrlPIServers.SetItemText(nIndex, 5, szt);
		szt.Format(_T("%d"), dlg.m_nDATimeout);
		m_ctrlPIServers.SetItemText(nIndex, 6, szt);
	}

	UpdateButtonState();
}

void CDlgCfg::OnBnClickedButtonEdit()
{
	if (m_ctrlPIServers.GetSelectedCount() == 0)
	{
		AfxMessageBox(_T("请选择需要修改的配置项！"), MB_OK | MB_ICONERROR);
		return;
	}

	POSITION pos = m_ctrlPIServers.GetFirstSelectedItemPosition();
	int nIndex = m_ctrlPIServers.GetNextSelectedItem(pos);

	if (nIndex == -1)
	{
		return;
	}

	CDlgPIServerItem dlg;

	CString szt;
	dlg.m_szIP = m_ctrlPIServers.GetItemText(nIndex, 1);
	dlg.m_szPort = m_ctrlPIServers.GetItemText(nIndex, 2);
	dlg.m_szUserName = m_ctrlPIServers.GetItemText(nIndex, 3);
	dlg.m_szPassword = m_ctrlPIServers.GetItemText(nIndex, 4);

	szt = m_ctrlPIServers.GetItemText(nIndex, 5);
	dlg.m_nConnTimeout = atoi(szt);

	szt = m_ctrlPIServers.GetItemText(nIndex, 6);
	dlg.m_nDATimeout = atoi(szt);

	if (dlg.DoModal() != IDOK)
		return;

	m_ctrlPIServers.SetItemText(nIndex, 1, dlg.m_szIP);
	m_ctrlPIServers.SetItemText(nIndex, 2, dlg.m_szPort);
	m_ctrlPIServers.SetItemText(nIndex, 3, dlg.m_szUserName);
	m_ctrlPIServers.SetItemText(nIndex, 4, dlg.m_szPassword);
	szt.Format(_T("%d"), dlg.m_nConnTimeout);
	m_ctrlPIServers.SetItemText(nIndex, 5, szt);
	szt.Format(_T("%d"), dlg.m_nDATimeout);
	m_ctrlPIServers.SetItemText(nIndex, 6, szt);
}

void CDlgCfg::OnBnClickedButtonDelete()
{
	if (m_ctrlPIServers.GetSelectedCount() == 0)
	{
		AfxMessageBox(_T("请选择需要删除的配置项！"), MB_OK | MB_ICONERROR);
		return;
	}

	POSITION pos = m_ctrlPIServers.GetFirstSelectedItemPosition();
	int nIndex = m_ctrlPIServers.GetNextSelectedItem(pos);

	if (nIndex == -1)
	{
		return;
	}

	if (AfxMessageBox(_T("是否真的要删除当前选择的配置项？"), MB_YESNO | MB_ICONQUESTION) != IDYES)
		return;

	m_ctrlPIServers.DeleteItem(nIndex);
	
	UpdateButtonState();
}

void CDlgCfg::OnBnClickedButtonUp()
{
	POSITION pos = m_ctrlPIServers.GetFirstSelectedItemPosition();
	int nIndex = m_ctrlPIServers.GetNextSelectedItem(pos);

	if (nIndex == 0)
		return;

	CString szt1, szt2, szt3, szt4, szt5, szt6;

	szt1 = m_ctrlPIServers.GetItemText(nIndex - 1, 1);
	szt2 = m_ctrlPIServers.GetItemText(nIndex - 1, 2);
	szt3 = m_ctrlPIServers.GetItemText(nIndex - 1, 3);
	szt4 = m_ctrlPIServers.GetItemText(nIndex - 1, 4);
	szt5 = m_ctrlPIServers.GetItemText(nIndex - 1, 5);
	szt6 = m_ctrlPIServers.GetItemText(nIndex - 1, 6);

	m_ctrlPIServers.SetItemText(nIndex - 1, 1, m_ctrlPIServers.GetItemText(nIndex, 1));
	m_ctrlPIServers.SetItemText(nIndex - 1, 2, m_ctrlPIServers.GetItemText(nIndex, 2));
	m_ctrlPIServers.SetItemText(nIndex - 1, 3, m_ctrlPIServers.GetItemText(nIndex, 3));
	m_ctrlPIServers.SetItemText(nIndex - 1, 4, m_ctrlPIServers.GetItemText(nIndex, 4));
	m_ctrlPIServers.SetItemText(nIndex - 1, 5, m_ctrlPIServers.GetItemText(nIndex, 5));
	m_ctrlPIServers.SetItemText(nIndex - 1, 6, m_ctrlPIServers.GetItemText(nIndex, 6));

	m_ctrlPIServers.SetItemText(nIndex, 1, szt1);
	m_ctrlPIServers.SetItemText(nIndex, 2, szt2);
	m_ctrlPIServers.SetItemText(nIndex, 3, szt3);
	m_ctrlPIServers.SetItemText(nIndex, 4, szt4);
	m_ctrlPIServers.SetItemText(nIndex, 5, szt5);
	m_ctrlPIServers.SetItemText(nIndex, 6, szt6);

	UINT flag = LVIS_SELECTED|LVIS_FOCUSED;
	m_ctrlPIServers.SetItemState(nIndex - 1, flag, flag);

	UpdateButtonState();
}

void CDlgCfg::OnBnClickedButtonDown()
{
	POSITION pos = m_ctrlPIServers.GetFirstSelectedItemPosition();
	int nIndex = m_ctrlPIServers.GetNextSelectedItem(pos);

	if (nIndex == m_ctrlPIServers.GetItemCount() - 1)
		return;

	CString szt1, szt2, szt3, szt4, szt5, szt6;

	szt1 = m_ctrlPIServers.GetItemText(nIndex + 1, 1);
	szt2 = m_ctrlPIServers.GetItemText(nIndex + 1, 2);
	szt3 = m_ctrlPIServers.GetItemText(nIndex + 1, 3);
	szt4 = m_ctrlPIServers.GetItemText(nIndex + 1, 4);
	szt5 = m_ctrlPIServers.GetItemText(nIndex + 1, 5);
	szt6 = m_ctrlPIServers.GetItemText(nIndex + 1, 6);

	m_ctrlPIServers.SetItemText(nIndex + 1, 1, m_ctrlPIServers.GetItemText(nIndex, 1));
	m_ctrlPIServers.SetItemText(nIndex + 1, 2, m_ctrlPIServers.GetItemText(nIndex, 2));
	m_ctrlPIServers.SetItemText(nIndex + 1, 3, m_ctrlPIServers.GetItemText(nIndex, 3));
	m_ctrlPIServers.SetItemText(nIndex + 1, 4, m_ctrlPIServers.GetItemText(nIndex, 4));
	m_ctrlPIServers.SetItemText(nIndex + 1, 5, m_ctrlPIServers.GetItemText(nIndex, 5));
	m_ctrlPIServers.SetItemText(nIndex + 1, 6, m_ctrlPIServers.GetItemText(nIndex, 6));

	m_ctrlPIServers.SetItemText(nIndex, 1, szt1);
	m_ctrlPIServers.SetItemText(nIndex, 2, szt2);
	m_ctrlPIServers.SetItemText(nIndex, 3, szt3);
	m_ctrlPIServers.SetItemText(nIndex, 4, szt4);
	m_ctrlPIServers.SetItemText(nIndex, 5, szt5);
	m_ctrlPIServers.SetItemText(nIndex, 6, szt6);

	UINT flag = LVIS_SELECTED|LVIS_FOCUSED;
	m_ctrlPIServers.SetItemState(nIndex + 1, flag, flag);

	UpdateButtonState();
}


void CDlgCfg::OnNMClickListPiserver(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW *pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;

	UpdateButtonState();
}

void CDlgCfg::OnNMDblclkListPiserver(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	
	OnBnClickedButtonEdit();
}

void CDlgCfg::OnBnClickedBtnSqlTest()
{
	// TODO: 在此添加控件通知处理程序代码

	UpdateData();

	CSqlServerConnect dbConnet;

	dbConnet.Disconnect();

	BOOL bConnet = dbConnet.Connect(_T(""), 
		m_szSqlServer, 
		m_dwPort, 
		m_szDBName, 
		m_szUser, 
		m_szPsw);

	if(bConnet)
		AfxMessageBox(_T("链接成功"));
	else
		AfxMessageBox(_T("链接失败"));

	dbConnet.Disconnect();
}
