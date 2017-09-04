#if !defined(AFX_SERIALCFGDLG_H__E2489332_2B61_49E7_A488_C92CFD490561__INCLUDED_)
#define AFX_SERIALCFGDLG_H__E2489332_2B61_49E7_A488_C92CFD490561__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SerialCfgDlg.h : header file
//

#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CSerialCfgDlg dialog

class CSerialCfgDlg : public CDialog
{
// Construction
public:
	CSerialCfgDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSerialCfgDlg)
	enum { IDD = IDD_DIALOG_SERIAL };
	CComboBox	m_ctrlStopBits;
	CComboBox	m_ctrlParity;
	CComboBox	m_ctrlComPort;
	CComboBox	m_ctrlByteSize;
	CComboBox	m_ctrlBaudRate;
	int		m_nBaudRate;
	int		m_nByteSize;
	int		m_nComPort;
	int		m_nParity;
	int		m_nStopBits;
	CString	m_szParam;
	//}}AFX_DATA
	
	void SetConfig(CConfigInfo &config);
	void GetConfig(CConfigInfo &config);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSerialCfgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_szComPort;
	DWORD	m_dwBaudRate;
	BYTE	m_cByteSize;		//Êý¾ÝÎ» number of bits/byte, 4-8

	// Generated message map functions
	//{{AFX_MSG(CSerialCfgDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboComport();
	afx_msg void OnSelchangeComboBaudrate();
	afx_msg void OnSelchangeComboBytesize();
	afx_msg void OnSelchangeComboParity();
	afx_msg void OnSelchangeComboStopbits();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERIALCFGDLG_H__E2489332_2B61_49E7_A488_C92CFD490561__INCLUDED_)
