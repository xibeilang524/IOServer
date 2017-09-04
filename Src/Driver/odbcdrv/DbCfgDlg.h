#if !defined(AFX_DBCFGDLG_H__58CB17CF_6513_4050_A48E_0C67CEE8DF68__INCLUDED_)
#define AFX_DBCFGDLG_H__58CB17CF_6513_4050_A48E_0C67CEE8DF68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DbCfgDlg.h : header file
//

#include "ConnStringParser.h"
#include "SelDBDrvDlg.h"
#include "SQLServerCfgDlg.h"
#include "MySQLCfgDlg.h"
#include "OracleCfgDlg.h"
#include "SybaseCfgDlg.h"
#include "DB2CfgDlg.h"
#include "TableCfgDlg.h"

#include "../sdk/drvsdk.h"

// 数据表的配置页
#define TAG_TABLESET -1

/////////////////////////////////////////////////////////////////////////////
// CDbCfgDlg dialog

class CDbCfgDlg : public CDialog
{
// Construction
public:
	CDbCfgDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDbCfgDlg)
	enum { IDD = IDD_DIALOG_DBCONFIG };
	CTabSheet	m_ctrlTab;
	//}}AFX_DATA
	
	BOOL				m_bModified;
	CDrvObj				*m_pdrv;
	CConnStringParser	m_objConnStr;
	CSelDBDrvDlg		m_dlgSelDBDrv;
	CSQLServerCfgDlg	m_dlgSQLServerCfg;
	CMySQLCfgDlg		m_dlgMySQLCfg;
	COracleCfgDlg		m_dlgOracleCfg;
	CSybaseCfgDlg		m_dlgSybase11Cfg;
	CSybaseCfgDlg		m_dlgSybase12Cfg;
	CDB2CfgDlg			m_dlgDB2Cfg;
	CTableCfgDlg		m_dlgTableCfg;
	
	BOOL GetConnectString(CConnStringParser &parser);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDbCfgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ShowDialog(int nDbType);	// 显示哪个数据库的配置对话框，如果为DBT_UNKNOWN，则显示数据库选择对话框
	BOOL ConnectDB(CDatabase &db, LPCTSTR lpszConnStr);
	BOOL ExeSQL(CDatabase &db, LPCTSTR sSQL);//执行SQL语句，返回 DBERR_OK,DBERR_FAIL,DBERR_CONNECT之一
	int CheckTables(CStringArray &tables);// 检查指定数据库中的表名是否有效？
	CString LoadSQLScript(DWORD dwResID);
	
	void MSSQLCreateDB(CConnStringParser &parser);
	void MySQLCreateDB(CConnStringParser &parser);
	void OracleCreateDB(CConnStringParser &parser);
	void Sybase11CreateDB(CConnStringParser &parser);
	void Sybase12CreateDB(CConnStringParser &parser);
	void DB2CreateDB(CConnStringParser &parser);

	// Generated message map functions
	//{{AFX_MSG(CDbCfgDlg)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnButtonBack();
	afx_msg void OnButtonNext();
	afx_msg void OnButtonTest();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonTabcfg();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBCFGDLG_H__58CB17CF_6513_4050_A48E_0C67CEE8DF68__INCLUDED_)
