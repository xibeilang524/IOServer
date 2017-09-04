#if !defined(AFX_UDPTCPCFGDLG_H__BCCA8250_EFED_427E_A81C_A6F824411CF0__INCLUDED_)
#define AFX_UDPTCPCFGDLG_H__BCCA8250_EFED_427E_A81C_A6F824411CF0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UdpTcpCfgDlg.h : header file
//

#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CUdpTcpCfgDlg dialog

class CUdpTcpCfgDlg : public CDialog
{
// Construction
public:
	CUdpTcpCfgDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUdpTcpCfgDlg)
	enum { IDD = IDD_DIALOG_UDPTCP };
	CIPAddressCtrl	m_ctrlIPAddr;
	UINT	m_nPort;
	CString	m_szParam;
	//}}AFX_DATA

	LPCTSTR GetIP() const;
	void SetIP(LPCTSTR lpszIP);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUdpTcpCfgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
	protected:
		CString m_szIP;

	// Generated message map functions
	//{{AFX_MSG(CUdpTcpCfgDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnFieldchangedIpaddressIp(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UDPTCPCFGDLG_H__BCCA8250_EFED_427E_A81C_A6F824411CF0__INCLUDED_)
