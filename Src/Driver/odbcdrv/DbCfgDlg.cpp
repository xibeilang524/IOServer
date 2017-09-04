// DbCfgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "odbcdrv.h"
#include "DbCfgDlg.h"
#include "DlgConfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDbCfgDlg dialog


CDbCfgDlg::CDbCfgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDbCfgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDbCfgDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pdrv = NULL;
	m_bModified = FALSE;
}


void CDbCfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDbCfgDlg)
	DDX_Control(pDX, IDC_TAB_LIST, m_ctrlTab);
	//}}AFX_DATA_MAP
}

// 显示哪个数据库的配置对话框，如果为DBT_UNKNOWN，则显示数据库选择对话框
void CDbCfgDlg::ShowDialog(int nDbType)
{
	switch(nDbType)
	{
	case DBT_UNKNOWN:
		m_ctrlTab.SetCurSel(0);
		break;
	case DBT_MSSQL:
		m_dlgSQLServerCfg.SetConnectString(m_objConnStr);
		m_ctrlTab.SetCurSel(1);
		break;
	case DBT_MYSQL:
		m_dlgMySQLCfg.SetConnectString(m_objConnStr);
		m_ctrlTab.SetCurSel(2);
		break;
	case DBT_ORACLE:		
		m_dlgOracleCfg.SetConnectString(m_objConnStr);
		m_ctrlTab.SetCurSel(3);
		break;
	case DBT_SYBASE11:
		m_dlgSybase11Cfg.SetConnectString(m_objConnStr);
		m_ctrlTab.SetCurSel(4);
		break;
	case DBT_SYBASE12:
		m_dlgSybase12Cfg.SetConnectString(m_objConnStr);
		m_ctrlTab.SetCurSel(5);
		break;
	case DBT_DB2:		
		m_dlgDB2Cfg.SetConnectString(m_objConnStr);
		m_ctrlTab.SetCurSel(6);
		break;
	case -1:
		m_ctrlTab.SetCurSel(m_ctrlTab.GetItemCount() - 1);
		break;
	default:
		m_ctrlTab.SetCurSel(0);
		break;
	}
	
	if (nDbType == DBT_UNKNOWN)
	{
		GetDlgItem(IDC_BUTTON_TEST)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_TABCFG)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_BACK)->EnableWindow(FALSE);
		
		GetDlgItem(IDC_BUTTON_NEXT)->SetWindowText(_T("下一步 >>"));
		
	}
	else if (nDbType == TAG_TABLESET) {
		GetDlgItem(IDC_BUTTON_TEST)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_TABCFG)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_BACK)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_NEXT)->SetWindowText(_T("完成"));
	}
	else {
		GetDlgItem(IDC_BUTTON_TEST)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_TABCFG)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_BACK)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_NEXT)->SetWindowText(_T("下一步 >>"));
	}
}

BOOL CDbCfgDlg::GetConnectString(CConnStringParser &parser)
{	
	LPDBTYPEDATA pData = m_dlgSelDBDrv.GetCurSel();
	if (!pData)
		return FALSE;
	
	switch(pData->nTypeID)
	{
	case DBT_MSSQL:
		m_dlgSQLServerCfg.GetConnectString(parser);
		break;
	case DBT_MYSQL:
		m_dlgMySQLCfg.GetConnectString(parser);
		break;
	case DBT_ORACLE:
		m_dlgOracleCfg.GetConnectString(parser);
		break;
	case DBT_SYBASE11:
		m_dlgSybase11Cfg.GetConnectString(parser);
		break;
	case DBT_SYBASE12:
		m_dlgSybase12Cfg.GetConnectString(parser);
		break;
	case DBT_DB2:
		m_dlgDB2Cfg.GetConnectString(parser);
		break;
	default:
		return FALSE;
		break;
	}
	
	return TRUE;
}

BOOL CDbCfgDlg::ConnectDB(CDatabase &db, LPCTSTR lpszConnStr)
{
	CString szError;
	db.SetLoginTimeout(8); //秒超时
	BOOL bret = FALSE;
	try
	{
		bret = db.OpenEx(lpszConnStr, CDatabase::noOdbcDialog | CDatabase::useCursorLib); //使用游标库
		if (db.IsOpen())
		{
			return TRUE;
		}
		else {
			szError.Format(_T("ODBC连接错误: 错误代码为%lu！"), bret);
			AfxMessageBox(szError, MB_OK | MB_ICONERROR);
			return FALSE;
		}
	}
	catch (CDBException *e)
	{
		szError.Format(_T("ODBC连接错误: %s"), e->m_strError);
		e->Delete();
		AfxMessageBox(szError, MB_OK | MB_ICONERROR);
		return FALSE;
	}
}

BEGIN_MESSAGE_MAP(CDbCfgDlg, CDialog)
	//{{AFX_MSG_MAP(CDbCfgDlg)
	ON_BN_CLICKED(IDC_BUTTON_BACK, OnButtonBack)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnButtonNext)
	ON_BN_CLICKED(IDC_BUTTON_TEST, OnButtonTest)
	ON_BN_CLICKED(IDC_BUTTON_TABCFG, OnButtonTabcfg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDbCfgDlg message handlers
BOOL CDbCfgDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	/* 设置对话框标题后CTabCtrl会由于Tab过长而显示出CUpDownCtrl控件，并且无法被隐藏
	m_ctrlTab.AddPage("选择数据库类型", &m_dlgSelDBDrv, IDD_DIALOG_SELDBDRV);
	m_ctrlTab.AddPage("SQL Server数据库配置", &m_dlgSQLServerCfg, IDD_DIALOG_MSSQL);
	m_ctrlTab.AddPage("MySQL数据库配置", &m_dlgMySQLCfg, IDD_DIALOG_MYSQL);
	m_ctrlTab.AddPage("Oracle数据库配置", &m_dlgOracleCfg, IDD_DIALOG_ORACLE);
	m_ctrlTab.AddPage("Sybase数据库配置", &m_dlgSybaseCfg, IDD_DIALOG_SYBASE);
	m_ctrlTab.AddPage("DB2数据库配置", &m_dlgDB2Cfg, IDD_DIALOG_DB2);
	*/
	
	m_ctrlTab.AddPage(_T(""), &m_dlgSelDBDrv, IDD_DIALOG_SELDBDRV);
	m_ctrlTab.AddPage(_T(""), &m_dlgSQLServerCfg, IDD_DIALOG_MSSQL);
	m_ctrlTab.AddPage(_T(""), &m_dlgMySQLCfg, IDD_DIALOG_MYSQL);
	m_ctrlTab.AddPage(_T(""), &m_dlgOracleCfg, IDD_DIALOG_ORACLE);
	m_ctrlTab.AddPage(_T(""), &m_dlgSybase11Cfg, IDD_DIALOG_SYBASE);
	m_ctrlTab.AddPage(_T(""), &m_dlgSybase12Cfg, IDD_DIALOG_SYBASE);
	m_ctrlTab.AddPage(_T(""), &m_dlgDB2Cfg, IDD_DIALOG_DB2);
	m_ctrlTab.AddPage(_T(""), &m_dlgTableCfg, IDD_DIALOG_TABLE);

	m_ctrlTab.SetHideTab(TRUE);
	m_ctrlTab.Show();

	m_dlgSybase11Cfg.m_nDbType = DBT_SYBASE11;
	m_dlgSybase12Cfg.m_nDbType = DBT_SYBASE12;

	m_dlgSelDBDrv.SetConnectString(m_objConnStr);

	if (m_objConnStr.IsSuccess())
	{
		ShowDialog(m_objConnStr.GetDBType());
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDbCfgDlg::OnOK() 
{
	GetConnectString(m_objConnStr);
	m_dlgTableCfg.UpdateData();
	
	CDialog::OnOK();
}

void CDbCfgDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CDbCfgDlg::OnButtonBack() 
{	
	int nSel = m_ctrlTab.GetCurSel();
	
	if (nSel == m_ctrlTab.GetItemCount() - 1) // 已经到了最后的配置数据表界面，可以结束了
	{		
		LPDBTYPEDATA pData = m_dlgSelDBDrv.GetCurSel();
		if (!pData)
		{
			AfxMessageBox(_T("请选择数据库类型！"), MB_OK | MB_ICONWARNING);
			return;
		}
		
		ShowDialog(pData->nTypeID);
	}
	else 
		ShowDialog(DBT_UNKNOWN);
}

void CDbCfgDlg::OnButtonNext() 
{
	int nSel = m_ctrlTab.GetCurSel();
	if (nSel == 0) // 当前显示的是数据库类型选择对话框
	{
		LPDBTYPEDATA pData = m_dlgSelDBDrv.GetCurSel();
		if (!pData)
		{
			AfxMessageBox(_T("请选择数据库类型！"), MB_OK | MB_ICONWARNING);
			return;
		}
		
		ShowDialog(pData->nTypeID);
	}
	else if (nSel == m_ctrlTab.GetItemCount() - 1) // 已经到了最后的配置数据表界面，可以结束了
	{
		m_dlgTableCfg.UpdateData();

		m_dlgTableCfg.m_szTableName.TrimLeft(); m_dlgTableCfg.m_szTableName.TrimRight();
		if (m_dlgTableCfg.m_szTableName.IsEmpty())
		{
			AfxMessageBox(_T("表名不能为空，请填写！"), MB_OK | MB_ICONERROR);
			return;
		}
		m_dlgTableCfg.m_szFieldName.TrimLeft(); m_dlgTableCfg.m_szFieldName.TrimRight();
		if (m_dlgTableCfg.m_szFieldName.IsEmpty())
		{
			AfxMessageBox(_T("标签名/ID列名不能为空，请填写！"), MB_OK | MB_ICONERROR);
			return;
		}
		m_dlgTableCfg.m_szTimeField.TrimLeft(); m_dlgTableCfg.m_szTimeField.TrimRight();
		if (m_dlgTableCfg.m_nStorageType == 1) { // 历史方式时必须填写时标字段
			if (m_dlgTableCfg.m_szTimeField.IsEmpty())
			{
				AfxMessageBox(_T("从历史表中读数据时，时标字段名不能为空，请填写！"), MB_OK | MB_ICONERROR);
				return;
			}
		}
		m_dlgTableCfg.m_szQualityField.TrimLeft(); m_dlgTableCfg.m_szQualityField.TrimRight();
		m_dlgTableCfg.m_szValField.TrimLeft(); m_dlgTableCfg.m_szValField.TrimRight();
		if (m_dlgTableCfg.m_szValField.IsEmpty())
		{
			AfxMessageBox(_T("值字段名不能为空，请填写！"), MB_OK | MB_ICONERROR);
			return;
		}

		OnOK();
	}
	else {
		ShowDialog(TAG_TABLESET); // 
	}
}

void CDbCfgDlg::OnButtonTest() 
{
	CWaitCursor cursor;
	
	CConnStringParser parser;
	GetConnectString(parser);
	
	CDatabase db;
	if (ConnectDB(db, parser.GetConnectString()))
		AfxMessageBox(_T("数据库连接成功！"), MB_OK | MB_ICONINFORMATION);
}

void CDbCfgDlg::OnButtonTabcfg() 
{
	CDlgConfig dlg; //标签配置

	dlg.m_wintag.m_pdrv = (COdbcDrvObj*)m_pdrv;
	dlg.m_bUp = FALSE;
	dlg.DoModal();
	if(dlg.m_bModified)
		m_bModified = TRUE;
}
