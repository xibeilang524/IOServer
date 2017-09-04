// DlgAddDec.cpp : implementation file
//

#include "stdafx.h"
#include "IOServer.h"
#include "DlgAddDec.h"
#include "Define.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAddDec dialog


CDlgAddDec::CDlgAddDec(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAddDec::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAddDec)
	m_szDecName = _T("");
	//}}AFX_DATA_INIT
	m_szPath = "";
	m_szDrvName = "";
}


void CDlgAddDec::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAddDec)
	DDX_Control(pDX, IDC_EDIT_DRVINFO, m_editinfo);
	DDX_Control(pDX, IDC_LIST_DRV,	m_ListBox);
	DDX_Text(pDX, IDC_EDIT_DECNAME, m_szDecName);
	DDV_MaxChars(pDX, m_szDecName, 16);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAddDec, CDialog)
	//{{AFX_MSG_MAP(CDlgAddDec)
	ON_LBN_SELCHANGE(IDC_LIST_DRV, OnSelchangeListDrv)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAddDec message handlers

BOOL CDlgAddDec::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	LoadDrivers();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAddDec::OnOK() 
{
	// TODO: Add extra validation here
	if(!UpdateData(TRUE))
		return;
	m_szDecName.TrimLeft();
	m_szDecName.TrimRight();
	if(m_szDecName.IsEmpty())
	{
		MessageBox("请填写一个唯一设备名称!","提示!",MB_OK|MB_ICONINFORMATION);
		return;
	}
	int nIndex = m_ListBox.GetCurSel();
	if(nIndex == -1)
	{
		MessageBox("请选择一个驱动或按放弃按钮!","提示!",MB_OK|MB_ICONINFORMATION);
		return;
	}
	m_ListBox.GetText(nIndex,m_szDrvName); //取驱动程序文件名
	if(theApp.GetDrvInst(m_szDrvName))
	{
		MessageBox("设备名称重复，请重新填写!","提示!",MB_OK|MB_ICONINFORMATION);
		return;
	}

	CDialog::OnOK();
}

void CDlgAddDec::LoadDrivers()
{
	m_ListBox.ResetContent();
	if(m_szPath.IsEmpty())
		return;
	int nIndex;
	CFileFind fl;
	CString szFilter = m_szPath + "*.dll";
	BOOL bFind = fl.FindFile(szFilter,0);
	while(bFind)
	{
		bFind = fl.FindNextFile();
		if( (!fl.IsDirectory()) && (!fl.IsDots()) )
		{
			nIndex = m_ListBox.AddString(fl.GetFileName());
			m_ListBox.SetItemHeight(nIndex,18);
		}
	}
}

void CDlgAddDec::OnSelchangeListDrv() 
{
	// TODO: Add your control notification handler code here
	CString szName = "";
	int nIndex = m_ListBox.GetCurSel();
	if(nIndex == -1)
		return ;
	m_ListBox.GetText(nIndex,szName);

	CString szErr="";
	CString szDrv = m_szPath + szName;

	T_DRVINFO1 tDRVINFO;
	if(!theApp.GetDrvInfo(szName, m_szPath, tDRVINFO))
	{
		AfxMessageBox(IDS_GET_INFO_FAILD);
		return ;
	}
	CString szInfo;
	szInfo.Format("名    称:%s\n\r\n\r\n\r",tDRVINFO.szName);

	szErr.Format("说    明:%s\n\r\n\r\n\r",tDRVINFO.szDes);
	szInfo += szErr;

	szErr.Format("驱动版本:%s\n\r\n\r\n\r",tDRVINFO.szVer);
	szInfo += szErr;

	szErr.Format("规范版本:%02X.%02X",(tDRVINFO.dwVer & 0xFF00)>>8,(tDRVINFO.dwVer & 0xFF));
	szInfo += szErr;

	m_editinfo.SetWindowText(szInfo);

}
