// DlgConfig.cpp : implementation file
//

#include "stdafx.h"
#include "modbus.h"
#include "DlgConfig.h"
#include "DlgComport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgConfig dialog


CDlgConfig::CDlgConfig(CWnd* pParent )
	: CDialog(CDlgConfig::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgConfig)
	m_szComSet = _T("");
	//}}AFX_DATA_INIT
	m_szTitle.LoadString(IDS_CONFIG);// = "配置";

	m_pcfg = NULL;
}


void CDlgConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConfig)
	DDX_Control(pDX, IDC_LIST_ITEM, m_ListItem);
	DDX_Control(pDX, IDC_LIST_DEC, m_ListDec);
	DDX_Control(pDX, IDC_EDIT_COM, m_EditCom);
	DDX_Text(pDX, IDC_EDIT_COM, m_szComSet);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgConfig, CDialog)
	//{{AFX_MSG_MAP(CDlgConfig)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_COM, OnButtonCom)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_HELP, OnButtonHelp)
	ON_BN_CLICKED(IDC_BUTTON_EXAMPLE, OnButtonExample)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_DEC, OnDblclkListDec)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, OnButtonExport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgConfig message handlers


BOOL CDlgConfig::OnInitDialog() 
{
	CDialog::OnInitDialog();

	_ASSERT(m_pcfg!=NULL);

	m_imagelist.Create(IDB_BITMAP_REC, 4, 1, RGB(255,255,255));
	m_imagelist2.Create(IDB_BITMAP_REC, 4, 1, RGB(255,255,255));
	m_ListDec.SetImageList(&m_imagelist, LVSIL_SMALL);
	m_ListItem.SetImageList(&m_imagelist2, LVSIL_SMALL);
	m_ListDec.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP);
	m_ListItem.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP);
	

	m_ListDec.InsertColumn(0,"设备名",LVCFMT_LEFT,60);
	m_ListDec.InsertColumn(1,"地址",LVCFMT_LEFT,40);
	m_ListDec.InsertColumn(2,"SEQCRC",LVCFMT_LEFT,50);
	m_ListDec.InsertColumn(3,"SEQ2",LVCFMT_LEFT,50);
	m_ListDec.InsertColumn(4,"SEQ4",LVCFMT_LEFT,50);
	m_ListDec.InsertColumn(5,"SEQF",LVCFMT_LEFT,50);


	m_ListItem.InsertColumn(0,"点名",LVCFMT_LEFT,80);
	m_ListItem.InsertColumn(1,"功能码",LVCFMT_LEFT,50);
	m_ListItem.InsertColumn(2,"地址",LVCFMT_LEFT,50);
	m_ListItem.InsertColumn(3,"值类型",LVCFMT_LEFT,50);
	m_ListItem.InsertColumn(4,"描述",LVCFMT_LEFT,80);
	m_ListItem.InsertColumn(5,"单位",LVCFMT_LEFT,80);
	

	SetWindowText(m_szTitle);

	//m_EditCom.SetWindowText(m_config.GetComSetString());//设置通信参数显示项
	m_EditCom.SetWindowText(m_pcfg->GetComMsg());
	LoadDec();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CDlgConfig::LoadDec()
{
	m_ListDec.SetRedraw(FALSE);
	m_ListDec.DeleteAllItems();
	CModDec *pdec;
	int i,nIndex=0,n = m_pcfg->GetSize();
	CString szt;
	for(i=0;i<n;i++)
	{
		pdec = m_pcfg->GetAt(i);
		if(pdec == NULL)
			continue;
		nIndex = m_ListDec.InsertItem(nIndex + 1,pdec->m_szname);
		szt.Format("%d",pdec->m_ucaddr);
		m_ListDec.SetItemText(nIndex,1,szt);

		szt.Format("%d",pdec->m_ncrc16seq);
		m_ListDec.SetItemText(nIndex,2,szt);

		szt.Format("%d",pdec->m_nwordseq);
		m_ListDec.SetItemText(nIndex,3,szt);

		szt.Format("%d",pdec->m_ndwordseq);
		m_ListDec.SetItemText(nIndex,4,szt);

		szt.Format("%d",pdec->m_nfloatseq);
		m_ListDec.SetItemText(nIndex,5,szt);
	}
	m_ListDec.SetRedraw(TRUE);
}

void CDlgConfig::OnOK() //保存退出
{
	// TODO: Add extra validation here
	CDialog::OnOK();
}

void CDlgConfig::OnCancel() //不保存退出
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}
#include "DlgItemAttr.h"
void CDlgConfig::OnButtonAdd() //导入设备
{
	// TODO: Add your control notification handler code here
	
	/*
	CFileDialog dlg(TRUE, NULL,NULL,OFN_HIDEREADONLY | 	OFN_OVERWRITEPROMPT,"Ecc Modbus配置文件(*.csv)|*.csv||",NULL);

	dlg.m_ofn.Flags &= ~OFN_EXPLORER; //必须加上这一句，否则不能释放退出
	dlg.m_ofn.lpstrTitle = "打开Ecc Modbus配置文件";
	if(IDOK != dlg.DoModal())
		return ;

	CString szPathFile = dlg.GetPathName();
	CString szFile = dlg.GetFileName();
	*/
	
	CString szPathName;
	CString szFileName;
	if (!CFileExplorer::ShowFileDialog(szFileName, szPathName, TRUE, _T(".csv"),
		NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("DDC Modbus配置文件(*.csv)|*.csv||"), 
		_T("打开DDC Modbus配置文件"), this))
		return;

	FILE* pf=NULL;
	pf = fopen(szPathName,"rt");//文本模式打开
	if(pf == NULL)
	{
		AfxMessageBox("不能打开文件!");
		return ;
	}
	//第一行,第一格
	int nrow=0;
	int i,n,ncol = 0;
	TCHAR c;
	CString sz0="";
	while((c = fgetc( pf)) != EOF)
	{
		if(c == ',')
			break;
		else if(c == '\n')
		{
			nrow++;//1
			break;
		}
		else
			sz0 += c;
	}
	if(_stricmp(sz0,"modbus") != 0)		
	{	
		AfxMessageBox("不是DDCC modbus格式文件");
		fclose(pf);
		return;
	}
	//移动到第3行
	while(nrow < 2)
	{
		c = fgetc( pf);
		if(c == EOF)
			break;
		if(c == '\n')
			nrow++;//2
	}
	//读第3行参数
	if(nrow != 2)
	{
		AfxMessageBox("文件无参数！");
		fclose(pf);
		return;
	}
	CString sz1="",sz2 = "",sz3="",sz4="",sz5="";
	ncol = 0;
	sz0 = "";
	while((c = fgetc( pf)) != EOF)
	{
		if(c == ',')
		{
			ncol++;
			continue;
		}
		else if(c=='\n')
		{
			nrow++;//3
			break;
		}
		if(ncol == 0)
			sz0 += c;
		else if(ncol == 1)
			sz1 += c;
		else if(ncol == 2)
			sz2 += c;
		else if(ncol == 3)
			sz3 += c;
		else if(ncol == 4)
			sz4 += c;
		else if(ncol == 5)
			sz5 += c;
	}
	if(sz0.IsEmpty())
	{
		AfxMessageBox("无设备名！");
		fclose(pf);
		return;
	}
	BYTE ucaddr  = (BYTE)atoi(sz1);
	if(ucaddr == 0)
	{
		AfxMessageBox("设备地址错误！");
		fclose(pf);
		return;
	}
	BOOL brepalce = FALSE;
	CModDec *pdec = NULL;
	pdec = m_pcfg->GetDecByAddr(ucaddr);
	if(pdec != NULL)
	{
		CString szMsg;
		szMsg.Format("已存在一个设备地址为 %d 的设备!是否覆盖?",ucaddr);
		if(IDYES != AfxMessageBox(szMsg,MB_YESNOCANCEL | MB_ICONQUESTION))
		{
			fclose(pf);
			return;
		}
		CString sname = pdec->m_szname;
		n=m_ListDec.GetItemCount();
		for(i=0;i<n;i++)
		{
			if(sz1 == m_ListDec.GetItemText(i,1))
			{
				m_ListDec.DeleteItem(i);
				break;
			}
		}
		m_pcfg->Delete(ucaddr);//删除原来设备
		pdec = NULL;
	}
	
	n=m_ListDec.GetItemCount();
	for(i=0;i<n;i++)
	{
		if(sz0 == m_ListDec.GetItemText(i,0))
		{
			AfxMessageBox("设备名冲突!");
			fclose(pf);
			return ;
		}
	}

	pdec = new CModDec; //新建设备配置
	pdec->m_ucaddr = ucaddr;
	pdec->m_szname = sz0;
	
	int nv;
	nv =  atoi(sz2);
	if((nv == 12)||(nv ==21))
		pdec->m_ncrc16seq = nv;

	nv =  atoi(sz3);
	if((nv == 12)||(nv ==21))
		pdec->m_nwordseq = nv;

	nv =  atoi(sz4);
	if((nv == 1234)||(nv ==2143)||(nv ==4321)||(nv == 3412))
		pdec->m_ndwordseq = nv;

	nv =  atoi(sz5);
	if((nv == 1234)||(nv ==2143)||(nv ==4321)||(nv == 3412))
		pdec->m_nfloatseq = nv;

	//移动到第5行
	while(nrow < 4)
	{
		c = fgetc( pf);
		if(c == EOF)
			break;
		if(c == '\n')
			nrow++;//4
	}

	int nc,naddr;
	//读数据想
	ncol = 0;
	sz0 = "";sz1="";sz2 = "";sz3="";sz4="";sz5="";
	CModItem item;
	while((c = fgetc( pf)) != EOF)
	{
		if(c == ',')
		{
			ncol++;
			continue;
		}
		else if(c=='\n')
		{
			nrow++;
			ncol = 0;
			//新建数据项
			//判断有效性
			nc = atoi(sz1);//功能码
			naddr = atoi(sz2);//地址
			if((sz0.IsEmpty()) || 
				( (nc != 1)&&(nc != 2)&&(nc != 3)&&(nc != 4)&&(nc != 5)&&(nc != 6)&&(nc != 16)) ||
				(naddr < 0))

			{
				sz0 = "";
				sz1 = "";
				sz2 = "";
				sz3 = "";
				sz4 = "";
				sz5 = "";
				continue;
			}
			item.m_sname = sz0;
			item.m_waddr = WORD(naddr);
			item.m_szdes = sz4;
			item.m_szunit = sz5;
			item.m_ucorder = nc;

			if(0 == _stricmp(sz3,"SHORT"))
				item.m_uctype = CModItem::SHORTV;

			else if(0 == _stricmp(sz3,"WORD"))
				item.m_uctype = CModItem::WORDV;

			else if(0 == _stricmp(sz3,"LONG"))
				item.m_uctype = CModItem::LONGV;

			else if(0 == _stricmp(sz3,"DWORD"))
				item.m_uctype = CModItem::DWORDV;
			
			else if(0 == _stricmp(sz3,"FLOAT"))
				item.m_uctype = CModItem::FLOATV;
			else
				item.m_uctype = CModItem::WORDV;
			
			pdec->Add(&item);

			sz0 = "";
			sz1 = "";
			sz2 = "";
			sz3 = "";
			sz4 = "";
			sz5 = "";
			continue;
		}
		if(ncol == 0)
			sz0 += c;
		else if(ncol == 1)
			sz1 += c;
		else if(ncol == 2)
			sz2 += c;
		else if(ncol == 3)
			sz3 += c;
		else if(ncol == 4)
			sz4 += c;
		else if(ncol == 5)
			sz5 += c;
	}
	fclose(pf);
		
	m_pcfg->Add(pdec);
	
	int nIndex = m_ListDec.InsertItem(m_ListDec.GetItemCount(),pdec->m_szname);
	CString szt;
	szt.Format("%d",pdec->m_ucaddr);
	m_ListDec.SetItemText(nIndex,1,szt);

	szt.Format("%d",pdec->m_ncrc16seq);
	m_ListDec.SetItemText(nIndex,2,szt);

	szt.Format("%d",pdec->m_nwordseq);
	m_ListDec.SetItemText(nIndex,3,szt);

	szt.Format("%d",pdec->m_ndwordseq);
	m_ListDec.SetItemText(nIndex,4,szt);

	szt.Format("%d",pdec->m_nfloatseq);
	m_ListDec.SetItemText(nIndex,5,szt);

	m_pcfg->SetModify(TRUE);
	LoadItem(pdec);
}
#include "DlgComport.h"
void CDlgConfig::OnButtonCom() //通信参数
{
	// TODO: Add your control notification handler code here
	CDlgComport dlg;
	dlg.m_nComType = m_pcfg->m_nComType;
	dlg.m_nstop = m_pcfg->m_cStopBits;
	dlg.m_nSvrPort = m_pcfg->m_wtcpport;
	dlg.m_szip = m_pcfg->m_szIP;
	dlg.m_szport = m_pcfg->m_szPort;
	dlg.m_nck = m_pcfg->m_cParity;
	dlg.m_szbt.Format("%d",m_pcfg->m_dwBaudRate);
	dlg.m_szdata.Format("%d",m_pcfg->m_cByteSize);
	dlg.m_bTcpCrc16 = m_pcfg->m_bTcpCrc16;
	dlg.m_ntimeout = m_pcfg->m_nTimeOut;
	dlg.m_bSaveLog = m_pcfg->m_bSaveLog;
	dlg.m_nUpdateRate = m_pcfg->m_nUpdateRate;

	if(dlg.DoModal() !=IDOK)
		return;

	m_pcfg->m_nComType = dlg.m_nComType;
	m_pcfg->m_cStopBits = dlg.m_nstop;
	m_pcfg->m_wtcpport = dlg.m_nSvrPort;
	m_pcfg->m_szIP = dlg.m_szip;
	m_pcfg->m_szPort = dlg.m_szport;
	m_pcfg->m_cParity = dlg.m_nck;

	m_pcfg->m_cByteSize = atoi(dlg.m_szdata);
	m_pcfg->m_dwBaudRate = atol(dlg.m_szbt);
	m_pcfg->m_bTcpCrc16 = dlg.m_bTcpCrc16;
	m_pcfg->m_nTimeOut = dlg.m_ntimeout;
	m_pcfg->m_bSaveLog = dlg.m_bSaveLog;
	m_pcfg->m_nUpdateRate = dlg.m_nUpdateRate;

	m_pcfg->SetModify(TRUE);//设置配置更改

	m_EditCom.SetWindowText(m_pcfg->GetComMsg());
}

void CDlgConfig::OnButtonDel() //删除数据项
{
	// TODO: Add your control notification handler code here
	int  nIndex =-1;
	nIndex = m_ListDec.GetNextItem(-1,LVNI_SELECTED);
	if(nIndex == -1)
	{
		AfxMessageBox(IDS_PLSSELMODDEC);//"请选择一个modbus设备!");
		return;
	}
	CString szid="";
	szid = m_ListDec.GetItemText(nIndex,0);

	CModDec *pdec=NULL;
	if(m_pcfg->Delete(szid))//删除对象
	{
		m_ListDec.DeleteItem(nIndex);
	}
	CString szcurid;
	GetDlgItemText(IDC_STATIC_DEC,szcurid);
	if(szcurid == szid)
	{
		m_ListItem.DeleteAllItems();
		SetDlgItemText(IDC_STATIC_DEC,"");
	}
	m_pcfg->SetModify(TRUE);
}


#include "DlgHelp.h"
void CDlgConfig::OnButtonHelp() 
{
	// TODO: Add your control notification handler code here
	CDlgHelp dlg;
	dlg.DoModal();
}

void CDlgConfig::OnButtonExample() 
{
	// TODO: Add your control notification handler code here
	/*
	CFileDialog	dlg(FALSE,_T(".csv"),NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("CSV(逗号分隔)文件(*.csv)|*.csv||"),
		NULL);
	dlg.m_ofn.Flags &= ~OFN_EXPLORER; //必须加上这一句，否则不能释放退出
	dlg.m_ofn.lpstrTitle=_T("保存例子modbus设备配置文件"); 
	if(IDOK!=dlg.DoModal())
		return;

	CString szPath=dlg.GetPathName();
	*/
	
	CString szPathName;
	CString szFileName;
	if (!CFileExplorer::ShowFileDialog(szFileName, szPathName, FALSE, _T(".csv"),
		NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("DDC Modbus配置文件(*.csv)|*.csv||"), 
		_T("保存例子modbus设备配置文件"), this))
		return;

	FILE *f = fopen (szPathName, "wb");
	if(f==NULL)
	{
		MessageBox(_T("文件不能写！"),_T("提示"),MB_OK|MB_ICONWARNING);
		return;
	}

	HINSTANCE hi = AfxGetResourceHandle( );
	HRSRC hRsrc = FindResource(hi, MAKEINTRESOURCE(IDR_CSV), TEXT("csv"));
	if (NULL == hRsrc)
	{
		fclose(f);
		return ;
	}
	//获取资源的大小
	int nsize = (int)SizeofResource(hi, hRsrc); 
	if (0 == nsize)
	{
		fclose(f);
		return ;
	}
	//加载资源
	HGLOBAL hGlobal = LoadResource(hi, hRsrc); 
	if (NULL == hGlobal)
	{
		fclose(f);
		return ;
	}
	//锁定资源
	LPVOID pBuffer = LockResource(hGlobal); 
	if (NULL == pBuffer)
	{
		fclose(f);
		return ;
	}
	
	fwrite(pBuffer,nsize,1,f);
	fclose(f);
	AfxMessageBox("创建成功，您可用Excel打开并编辑这个配置文件!",MB_OK|MB_ICONINFORMATION);
}

void CDlgConfig::LoadItem(CModDec *pdec)
{
	m_ListItem.SetRedraw(FALSE);
	m_ListItem.DeleteAllItems();
	
	int i,nIndex=0,n = pdec->GetItems();
	CString szt;
	CModItem *pi;
	for(i=0;i<n;i++)
	{
		pi = pdec->GetAt(i);
		if(pi == NULL)
			continue;
		nIndex = m_ListItem.InsertItem(nIndex + 1,pi->m_sname);
		szt.Format("%d",pi->m_ucorder);
		m_ListItem.SetItemText(nIndex,1,szt);

		szt.Format("%d",pi->m_waddr);
		m_ListItem.SetItemText(nIndex,2,szt);

		m_ListItem.SetItemText(nIndex,3,pi->GetType());//值类型
		m_ListItem.SetItemText(nIndex,4,pi->m_szdes);//描述
		m_ListItem.SetItemText(nIndex,5,pi->m_szunit);//单位
	}
	m_ListItem.SetRedraw(TRUE);
	SetDlgItemText(IDC_STATIC_DEC,pdec->m_szname);
}

void CDlgConfig::OnDblclkListDec(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int nIndex = pNMListView->iItem;
	if(nIndex == -1)
	{
		*pResult = 0;
		return;
	}
	CString szaddr;
	CModDec *pdec=NULL;
	szaddr = m_ListDec.GetItemText(nIndex,1);
	pdec = m_pcfg->GetDecByAddr((BYTE)atoi(szaddr));
	if(pdec == NULL)
	{
		m_ListItem.DeleteAllItems();
		return;
	}
	LoadItem(pdec);
	*pResult = 0;
}

void CDlgConfig::OnButtonExport() 
{
	// TODO: Add your control notification handler code here
	int  nIndex =-1;
	nIndex = m_ListDec.GetNextItem(-1,LVNI_SELECTED);
	if(nIndex == -1)
	{
		AfxMessageBox(IDS_PLSSELMODDEC);//"请选择一个modbus设备!");
		return;
	}
	CString szid="";
	szid = m_ListDec.GetItemText(nIndex,1);

	CModDec *pdec=NULL;
	pdec = m_pcfg->GetDecByAddr((BYTE)atoi(szid));
	if(pdec == NULL)
		return;

	/*
	CFileDialog	dlg(FALSE,_T(".csv"),NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("CSV(逗号分隔)文件(*.csv)|*.csv||"),
		NULL);
	dlg.m_ofn.Flags &= ~OFN_EXPLORER; //必须加上这一句，否则不能释放退出
	dlg.m_ofn.lpstrTitle=_T("导出modbus设备配置文件"); 
	if(IDOK!=dlg.DoModal())
		return;

	CString szPath=dlg.GetPathName();
	*/	
	CString szPathName;
	CString szFileName;
	if (!CFileExplorer::ShowFileDialog(szFileName, szPathName, FALSE, _T(".csv"),
		NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("DDC Modbus配置文件(*.csv)|*.csv||"), 
		_T("导出modbus设备配置文件"), this))
		return;

	FILE *f = fopen (szPathName, "wt");
	if(f==NULL)
	{
		MessageBox(_T("文件不能写！"),_T("提示"),MB_OK|MB_ICONWARNING);
		return;
	}
	CString szline = "modbus,,,,,\n";
	fwrite(szline,szline.GetLength(),1,f);

	szline = "设备名,设备地址,SEQCRC,SEQ2,SEQ4,SEQF\n";
	fwrite(szline,szline.GetLength(),1,f);

	szline.Format("%s,%d,%d,%d,%d,%d\n",pdec->m_szname,pdec->m_ucaddr,
		pdec->m_ncrc16seq,pdec->m_nwordseq,pdec->m_ndwordseq,pdec->m_nfloatseq);
	fwrite(szline,szline.GetLength(),1,f);
	
	szline = "点名,功能码,地址,值类型,描述,单位\n";
	fwrite(szline,szline.GetLength(),1,f);

	//开始写数据项
	CModItem *pi;
	int i,n = pdec->GetItems();
	for(i=0;i<n;i++)
	{
		pi = pdec->GetAt(i);
		if(pi == NULL)
			continue;
		szline.Format("%s,%d,%d,%s,%s,%s\n",pi->m_sname,
			pi->m_ucorder,pi->m_waddr,pi->GetType(),
			pi->m_szdes,pi->m_szunit);
		fwrite(szline,szline.GetLength(),1,f);
	}
	fclose(f);
}
