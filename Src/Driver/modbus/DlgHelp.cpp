// DlgHelp.cpp : implementation file
//

#include "stdafx.h"
#include "modbus.h"
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
	//}}AFX_DATA_INIT
}


void CDlgHelp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgHelp)
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
	HRSRC hRsrc = FindResource(hi, MAKEINTRESOURCE(IDR_HELP), TEXT("csv"));
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

/*
BOOL UseCustomResource()
{
	//定位我们的自定义资源，这里因为我们是从本模块定位资源，所以将句柄简单地置为NULL即可
	HRSRC hRsrc = FindResource(NULL, MAKEINTRESOURCE(ITEMID), TEXT("MyType"));
	if (NULL == hRsrc)
		return FALSE;
	//获取资源的大小
	DWORD dwSize = SizeofResource(NULL, hRsrc); 
	if (0 == dwSize)
		return FALSE;
	//加载资源
	HGLOBAL hGlobal = LoadResource(NULL, hRsrc); 
	if (NULL == hGlobal)
		return FALSE;
	//锁定资源
	LPVOID pBuffer = LockResource(hGlobal); 
	if (NULL == pBuffer)
		return FALSE;
	
	//我们用刚才得到的pBuffer和dwSize来做一些需要的事情。可以直接在内存中使
	//用，也可以写入到硬盘文件。这里我们简单的写入到硬盘文件，如果我们的自定
	//义资源是作为嵌入DLL来应用，情况可能要复杂一些。
	BOOL bRt = FALSE;
	FILE* fp = _tfopen(_T("demo.exe"), _T("wb"));
	if (fp != NULL)
	{
		if (dwSize == fwrite(pBuffer, sizeof(char), dwSize, fp))
			bRt = TRUE;
		fclose(fp);
	}
	
	//FreeResource(hGlobal);
	return bRt;
}      
  */
