#if !defined(AFX_DLGCOMSET_H__AE622367_C6B5_4CB5_9E44_160E3E5B103E__INCLUDED_)
#define AFX_DLGCOMSET_H__AE622367_C6B5_4CB5_9E44_160E3E5B103E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgComset.h : header file
//

#include "resource.h"
#include "TabSheet.h"
#include "SerialCfgDlg.h"
#include "UdpTcpCfgDlg.h"
#include "ODBCCfgDlg.h"
#include "FileCfgDlg.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgComset dialog

class CDlgComset : public CDialog
{
// Construction
public:
	CDlgComset(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgComset)
	enum { IDD = IDD_DIALOG_COMSET };
	CTabSheet	m_ctrlTab;
	CComboBox	m_ctrlComTypes;
	UINT	m_nReadTimeout;
	UINT	m_nUpdateSec;
	UINT	m_nWriteTimeout;
	BOOL	m_bDebug;
	BOOL	m_bLocalTime;
	int		m_nTimeBias;
	//}}AFX_DATA

	WORD m_wComTypes;
	WORD m_wComType;
	CSerialCfgDlg	m_dlgSerialCfg;
	CUdpTcpCfgDlg	m_dlgUdpTcpCfg;
	CODBCCfgDlg     m_dlgODBCCfg;
	CFileCfgDlg		m_dlgFileCfg;

	CDDCDrvObj *m_pDrvObj;

	void SetConfig(CConfigInfo &config);
	void GetConfig(CConfigInfo &config);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgComset)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgComset)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboComtype();
	virtual void OnOK();
	afx_msg void OnButtonHelp();
	afx_msg void OnButtonTest();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCOMSET_H__AE622367_C6B5_4CB5_9E44_160E3E5B103E__INCLUDED_)
