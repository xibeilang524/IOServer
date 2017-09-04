// DlgHelp.cpp : implementation file
//

#include "stdafx.h"
#include "opcdrv.h"
#include "DlgHelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgHelp dialog


CDlgHelp::CDlgHelp(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgHelp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgHelp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgHelp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgHelp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgHelp, CDialog)
	//{{AFX_MSG_MAP(CDlgHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgHelp message handlers

BOOL CDlgHelp::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	HINSTANCE hi = AfxGetResourceHandle( );
	HRSRC hRsrc = FindResource(hi, MAKEINTRESOURCE(IDR_CSV1), TEXT("csv"));
	if (NULL == hRsrc)
		return FALSE;
	//获取资源的大小
	int nsize = (int)SizeofResource(hi, hRsrc); 
	if (0 == nsize)
		return TRUE;
	//加载资源
	HGLOBAL hGlobal = LoadResource(hi, hRsrc); 
	if (NULL == hGlobal)
		return TRUE;
	//锁定资源
	LPVOID pBuffer = LockResource(hGlobal); 
	if (NULL == pBuffer)
		return TRUE;

	CString szt = "";
	char *pc= (char*)pBuffer;
	for(int i=0;i<nsize;i++)
	{
		szt+=pc[i];
	}
	SetDlgItemText(IDC_EDIT1,szt);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
