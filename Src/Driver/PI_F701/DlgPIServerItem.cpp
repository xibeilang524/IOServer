// DlgPIServerItem.cpp : 实现文件
//

#include "stdafx.h"
#include "PI_F701Drv.h"
#include "DlgPIServerItem.h"
#include <piapix.h>

// CDlgPIServerItem 对话框

IMPLEMENT_DYNAMIC(CDlgPIServerItem, CDialog)

CDlgPIServerItem::CDlgPIServerItem(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPIServerItem::IDD, pParent)
	, m_nConnTimeout(10)
	, m_nDATimeout(60)
	, m_szIP(_T("127.0.0.1"))
	, m_szPort(_T("5450"))
	, m_szPassword(_T(""))
	, m_szUserName(_T("piadmin"))
{

}

CDlgPIServerItem::~CDlgPIServerItem()
{
}

void CDlgPIServerItem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CONNTIMEOUT, m_nConnTimeout);
	DDV_MinMaxInt(pDX, m_nConnTimeout, 0, 6000);
	DDX_Text(pDX, IDC_EDIT_DATIMEOUT, m_nDATimeout);
	DDV_MinMaxInt(pDX, m_nDATimeout, 0, 6000);
	DDX_Text(pDX, IDC_EDIT_SERVERNAME, m_szIP);
	DDX_Text(pDX, IDC_EDIT_PORTNUM, m_szPort);
	DDX_Text(pDX, IDC_EDIT_PASSWARD, m_szPassword);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_szUserName);
}


BEGIN_MESSAGE_MAP(CDlgPIServerItem, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_TEST, &CDlgPIServerItem::OnBnClickedButtonTest)
END_MESSAGE_MAP()


// CDlgPIServerItem 消息处理程序

void CDlgPIServerItem::OnBnClickedButtonTest()
{
	UpdateData();

#ifdef USE_STATIC_SAFELOCK
	CPiDBDrv::m_csLock.Enter();
#endif

	CWaitCursor wc;
	BOOL bConnected = FALSE;
	PIINT32	nret = ::piut_setservernode(m_szIP);
	//if(0 == nret)
	{
		LONG valid;
		nret = ::piut_login( m_szUserName, m_szPassword, &valid );	
		if(0 == nret)
			bConnected = TRUE;
	}
	wc.Restore();
#ifdef USE_STATIC_SAFELOCK
	CPiDBDrv::m_csLock.Leave();
#endif

	// 不释放连接，否则可能会造成当前运行的驱动连接中断

	if (bConnected)
	{
		AfxMessageBox(_T("PI数据库连接成功！"), MB_OK | MB_ICONINFORMATION);
	}
	else {
		AfxMessageBox(_T("PI数据库连接失败！"), MB_OK | MB_ICONERROR);
	}
}

void CDlgPIServerItem::OnOK()
{
	UpdateData();

	m_szIP.Trim();
	if (m_szIP.IsEmpty())
	{
		GetDlgItem(IDC_EDIT_SERVERNAME)->SetFocus();
		AfxMessageBox(_T("请填写PI服务器的IP地址！"), MB_OK | MB_ICONERROR);
		return;
	}

	m_szPort.Trim();
	if (m_szPort.IsEmpty())
	{
		GetDlgItem(IDC_EDIT_PORTNUM)->SetFocus();
		AfxMessageBox(_T("请填写PI服务器的端口号！"), MB_OK | MB_ICONERROR);
		return;
	}

	if (atol(m_szPort) <= 0)
	{
		GetDlgItem(IDC_EDIT_PORTNUM)->SetFocus();
		AfxMessageBox(_T("请填写有效的PI服务器端口号，默认为5450！"), MB_OK | MB_ICONERROR);
		return;
	}

	CDialog::OnOK();
}
