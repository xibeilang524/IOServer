// TableCfgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "odbcdrv.h"
#include "TableCfgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTableCfgDlg dialog


CTableCfgDlg::CTableCfgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTableCfgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTableCfgDlg)
	m_szFieldName = _T("");
	m_szTimeField = _T("");
	m_szValField = _T("");
	m_szTableName = _T("");
	m_nStorageType = 0;
	m_szQualityField = _T("");
	//}}AFX_DATA_INIT
}


void CTableCfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTableCfgDlg)
	DDX_Text(pDX, IDC_EDIT_FNAME, m_szFieldName);
	DDX_Text(pDX, IDC_EDIT_FTIME, m_szTimeField);
	DDX_Text(pDX, IDC_EDIT_FVAL, m_szValField);
	DDX_Text(pDX, IDC_EDIT_TABNAME, m_szTableName);
	DDX_Radio(pDX, IDC_RADIO_SL, m_nStorageType);
	DDX_Text(pDX, IDC_EDIT_FQA, m_szQualityField);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTableCfgDlg, CDialog)
	//{{AFX_MSG_MAP(CTableCfgDlg)
	ON_COMMAND(IDOK, OnOK)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTableCfgDlg message handlers

void CTableCfgDlg::OnOK()
{
	// 用于解决在编辑框中按回车后该窗口自动关闭的BUG
}