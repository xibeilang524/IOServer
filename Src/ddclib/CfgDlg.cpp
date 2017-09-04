// CfgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ddclib.h"
#include "CfgDlg.h"
#include "DlgComset.h"
#include "DlgItemAttr.h"
#include "ProgressThread.h"
#include "FileExplorer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCfgDlg dialog

CCfgDlg::CCfgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCfgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCfgDlg)
	m_szConnectString = _T("");
	//}}AFX_DATA_INIT
	m_pDrvObj = NULL;
}


void CCfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCfgDlg)
	DDX_Control(pDX, IDC_STATIC_FRAME, m_ctrlFrame);
	DDX_Text(pDX, IDC_EDIT_COM, m_szConnectString);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCfgDlg, CDialog)
	//{{AFX_MSG_MAP(CCfgDlg)
	ON_BN_CLICKED(IDC_BUTTON_COM, OnButtonCom)
	ON_BN_CLICKED(IDC_BUTTON_COMSET, OnButtonComset)
	ON_BN_CLICKED(IDC_BUTTON_EXAMPLE, OnButtonExample)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, OnButtonExport)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_ADDONE, OnButtonAddone)
	ON_BN_CLICKED(IDC_BUTTON_EDITTAG, OnButtonEdittag)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCfgDlg message handlers

void CCfgDlg::OnSysCommand(UINT nID, LONG lParam)
{
	if (nID == SC_CLOSE)
	{
		if (m_config.IsModified() || m_tagtab.IsModified())   
		{
			int nret = AfxMessageBox(_T("标签表或配置信息已经被修改，是否保存？"), 
				MB_YESNOCANCEL | MB_ICONQUESTION);
			switch(nret)
			{
			case IDYES:
				CDialog::OnOK();
				return;
			case IDNO:
				break;
			case IDCANCEL:
				return;
			}
		}   
	}   
	CDialog::OnSysCommand(nID, lParam); 
}

BOOL CCfgDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
		
	CRect rtc;
	m_ctrlFrame.GetWindowRect(&rtc);
	ScreenToClient(&rtc) ;  
	rtc.DeflateRect(1,1,1,1);
	
	m_winTagInfo.m_pTagtab = &m_tagtab;
	m_winTagInfo.Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rtc, this, -1);//标签表
	m_winTagInfo.ReDraw();
	m_szConnectString = m_config.GetConnectString();
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCfgDlg::OnCancel() 
{
	if (m_config.IsModified() || m_tagtab.IsModified())   
	{
		int nret = AfxMessageBox(_T("标签表或配置信息已经被修改，是否保存？"), 
			MB_YESNOCANCEL | MB_ICONQUESTION);
		switch(nret)
		{
		case IDYES:
			CDialog::OnOK();
			return;
		case IDNO:
			break;
		case IDCANCEL:
			return;
		}
	}   
	
	CDialog::OnCancel();
}

void CCfgDlg::OnButtonCom() 
{
	CDlgComset dlg;
	dlg.m_pDrvObj = m_pDrvObj;
	dlg.SetConfig(m_config);
	dlg.m_wComTypes = m_wComTypes;
	if (dlg.DoModal() == IDOK)
	{
		CConfigInfo info;
		dlg.GetConfig(info);
		if (m_config != info)
		{
			m_config = info;
			m_config.m_bModified = TRUE;
		}

		m_szConnectString = m_config.GetConnectString();
		UpdateData(FALSE);
	}
}

void CCfgDlg::OnButtonComset() 
{
	OnButtonCom();
}

void CCfgDlg::OnButtonExample() 
{
	/*
	CFileDialog	dlg(FALSE,_T(".csv"),NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("CSV(逗号分隔)文件(*.csv)|*.csv||"),
		NULL);
	dlg.m_ofn.lpstrTitle=_T("保存例子设备配置文件"); 
	if(IDOK!=dlg.DoModal())
		return;
	
	CString szPathName=dlg.GetPathName();
	*/
	
	CString szPathName;
	CString szFileName;
	if (!CFileExplorer::ShowFileDialog(szFileName, szPathName, FALSE, _T(".csv"),
		NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("CSV(逗号分隔)文件(*.csv)|*.csv||"), 
		_T("保存例子设备配置文件"), this))
		return;
	
	FILE *f = fopen (szPathName, "wt");
	if(f==NULL)
	{
		AfxMessageBox(_T("文件不能写！"), MB_OK | MB_ICONWARNING);
		return;
	}
	
	CWaitCursor cursor;
	//CProgressThread *pThread = CProgressThread::StartThread();
	//pThread->SetRange(0, 100);
	//pThread->SetText(_T("正在导出标签配置文件..."));
	
	int i;
	// 生成表头
	CString szline = CTagItem::GetCSVFormatName();
	szline += _T(",,,,,");
	for(i = 0; i < CTagItem::GetCustomFieldCount(); i++)
		szline += _T(",");
	szline += _T("\n");
	
	fwrite(szline,szline.GetLength(),1,f);
	
	// 生成列名
	szline = "ID,标签名,描述,单位,类型";
	for(i = 0; i < CTagItem::GetCustomFieldCount(); i++)
	{	
		CCustomField &field = CTagItem::GetCustomField(i);
		szline += _T(",") + field.m_szDisplayName;
	}
	szline += _T(",备注\n");
	fwrite(szline, szline.GetLength(), 1, f);
		
	// 第一个示例标签
	szline = _T("1,TESTPOINT1,测试点1,c,float32");
	for(i = 0; i < CTagItem::GetCustomFieldCount(); i++)
	{	
		CCustomField &field = CTagItem::GetCustomField(i);
		szline += _T(",");
		szline += field.DefaultValueAsString();
	}
	szline += _T(",备注1\n");
	fwrite(szline, szline.GetLength(), 1, f);
	
	// 第二个示例标签
	szline = _T("2,TESTPOINT2,测试点2,c,float32");
	for(i = 0; i < CTagItem::GetCustomFieldCount(); i++)
	{	
		CCustomField &field = CTagItem::GetCustomField(i);
		szline += _T(",");
		szline += field.DefaultValueAsString();
	}
	szline += _T(",备注2\n");
	fwrite(szline, szline.GetLength(), 1, f);
	
	// 第三个示例标签
	szline = _T("3,TESTPOINT3,测试点3,c,int32");
	for(i = 0; i < CTagItem::GetCustomFieldCount(); i++)
	{	
		CCustomField &field = CTagItem::GetCustomField(i);
		szline += _T(",");
		szline += field.DefaultValueAsString();
	}
	szline += _T(",备注3\n");
	fwrite(szline, szline.GetLength(), 1, f);
	
	// 第四个示例标签
	szline = _T("4,TESTPOINT4,测试点4,c,int32");
	for(i = 0; i < CTagItem::GetCustomFieldCount(); i++)
	{	
		CCustomField &field = CTagItem::GetCustomField(i);
		szline += _T(",");
		szline += field.DefaultValueAsString();
	}
	szline += _T(",备注4\n");
	fwrite(szline, szline.GetLength(), 1, f);
	
	// 第五个示例标签
	szline = _T("5,TESTPOINT5,测试点5,c,digital");
	for(i = 0; i < CTagItem::GetCustomFieldCount(); i++)
	{	
		CCustomField &field = CTagItem::GetCustomField(i);
		szline += _T(",");
		szline += field.DefaultValueAsString();
	}
	szline += _T(",备注5\n");
	fwrite(szline, szline.GetLength(), 1, f);
	
	// 第六个示例标签
	szline = _T("6,TESTPOINT6,测试点6,c,string");
	for(i = 0; i < CTagItem::GetCustomFieldCount(); i++)
	{	
		CCustomField &field = CTagItem::GetCustomField(i);
		szline += _T(",");
		szline += field.DefaultValueAsString();
	}
	szline += _T(",备注6\n");
	fwrite(szline, szline.GetLength(), 1, f);

	fclose(f);

	AfxMessageBox("创建成功，您可用Excel打开并编辑这个配置文件!",MB_OK|MB_ICONINFORMATION);
}

void CCfgDlg::OnButtonAdd() 
{
	/*
	CFileDialog dlg(TRUE, NULL,NULL,OFN_HIDEREADONLY | 	OFN_OVERWRITEPROMPT,
		"数据项文件(*.csv)|*.csv||",NULL);

	dlg.m_ofn.lpstrTitle = "导入数据项文件";
	if(IDOK != dlg.DoModal())
		return ;	

	CString szPathName = dlg.GetPathName();
	CString szFileName = dlg.GetFileName();
	*/
	
	CString szPathName;
	CString szFileName;
	if (!CFileExplorer::ShowFileDialog(szFileName, szPathName, TRUE, _T(".csv"),
		NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("CSV(逗号分隔)文件(*.csv)|*.csv||"), 
		_T("导入标签配置文件"), this))
		return;

	FILE* pf=NULL;
	pf = fopen(szPathName, "rt");//文本模式打开
	if (pf == NULL)
	{
		AfxMessageBox("不能打开文件!");
		return ;
	}

	CWaitCursor cursor;
	//CProgressThread *pThread = CProgressThread::StartThread();
	//pThread->SetRange(0, 100);
	//pThread->SetText(_T("正在导入标签..."));
	//CString szInfo1;
	//szInfo1.Format("正在读入标签......");
	// 计算文件大小
	//fseek(pf, 0, SEEK_END);
	//long fsize = ftell(pf);
	//fseek(pf, 0, SEEK_SET);

	//第一行,第一格
	TCHAR c;
	int nrow = 0;
	int ncol = 0;
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
	if(_stricmp(sz0, CTagItem::GetCSVFormatName()) != 0)		
	{	
		//pThread->Close();
		AfxMessageBox("不是要求的数据项格式文件");
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
		//pThread->Close();
		AfxMessageBox("文件无参数！");
		fclose(pf);
		return;
	}
	//读数据项
	int nUpdate = 0,nInsert = 0;

	CTagItem TagItem;

	int i;
	ncol = 0;
	CStringArray arrFields;
	arrFields.SetSize(32);
	int nDataType = 6;//float
	while((c = fgetc( pf)) != EOF)
	{
		if(c == ',')
		{
			ncol++;
			continue;
		}
		else if(c=='\n')
		{
			int nColCount = ncol + 1;
			ncol = 0;
			nrow++;
			if (nColCount < FIELD_LAST + CTagItem::GetCustomFieldCount() + 1)
			{
				for(i = 0; i < arrFields.GetSize(); i++)
					arrFields[i] = _T("");
				continue;
			}
			for(i = 0; i < nColCount; i++)
			{
				arrFields[i].TrimLeft();
				arrFields[i].TrimRight();
			}
			
			if(arrFields[1].IsEmpty())
			{
				for(i = 0; i < arrFields.GetSize(); i++)
					arrFields[i] = _T("");
				continue;
			}
			TagItem.m_szID		= arrFields[0];
			TagItem.m_szName	= arrFields[1];
			TagItem.m_szDes		= arrFields[2];
			TagItem.m_szUnit	= arrFields[3];
			TagItem.SetTypeVal(arrFields[4]);
			for(i = 0; i < CTagItem::GetCustomFieldCount(); i++)
			{
				TagItem.SetCustomField(i, arrFields[FIELD_LAST + i]);
			}
			TagItem.m_szNote	= arrFields[FIELD_LAST + CTagItem::GetCustomFieldCount()];

			CTagItem *pTemp = m_tagtab.GetTag(TagItem.m_szName);
			if (pTemp)
			{
				TagItem.CopyTo(pTemp);
				m_tagtab.SetModified(TRUE);

				nUpdate++;
			}
			else {
				nInsert++;
				m_tagtab.AddTag(&TagItem);
			}

			// 通过计算文件的读位置来计算处理进度
			//pThread->SetPos((int)(ftell(pf) / (double)fsize * 100));
			
			for(i = 0; i < arrFields.GetSize(); i++)
				arrFields[i] = _T("");
			continue;
		}
		arrFields[ncol] += c;
	}
	fclose(pf);
	//pThread->Close();

	//m_tagtab.Qsort();
	m_winTagInfo.ReDraw();
	//cursor.Restore();
	
	CString szInfo;
	szInfo.Format("新增标签%d个，更新标签%d个。", nInsert, nUpdate);
	AfxMessageBox(szInfo);
}

void CCfgDlg::OnButtonExport() 
{
	int nsize = m_tagtab.GetSize();
	if (nsize == 0)
	{
		AfxMessageBox("没有可导出的标签点！");
		return;
	}
	/*
	CFileDialog	dlg(FALSE,_T(".csv"),NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("CSV(逗号分隔)文件(*.csv)|*.csv||"),
		NULL);
	dlg.m_ofn.lpstrTitle=_T("导出标签配置文件"); 
	if(IDOK!=dlg.DoModal())
		return;	
	
	CString szPathFile=dlg.GetPathName();
	*/
	
	CString szPathName;
	CString szFileName;
	if (!CFileExplorer::ShowFileDialog(szFileName, szPathName, FALSE, _T(".csv"),
		NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("CSV(逗号分隔)文件(*.csv)|*.csv||"), 
		_T("导出标签配置文件"), this))
		return;
	
	FILE *f = fopen (szPathName, "wt");
	if(f==NULL)
	{
		AfxMessageBox(_T("文件不能写！"), MB_OK | MB_ICONWARNING);
		return;
	}
	
	CWaitCursor cursor;
	//CProgressThread *pThread = CProgressThread::StartThread();
	//pThread->SetRange(0, 100);
	//pThread->SetText(_T("正在导出标签配置文件..."));
	
	int i;
	// 生成表头
	CString szline = CTagItem::GetCSVFormatName();
	szline += _T(",,,,,");
	for(i = 0; i < CTagItem::GetCustomFieldCount(); i++)
		szline += _T(",");
	szline += _T("\n");

	fwrite(szline,szline.GetLength(),1,f);
	
	// 生成列名
	szline = "ID,标签名,描述,单位,类型";
	for(i = 0; i < CTagItem::GetCustomFieldCount(); i++)
	{	
		CCustomField &field = CTagItem::GetCustomField(i);
		szline += _T(",") + field.m_szDisplayName;
	}
	szline += _T(",备注\n");
	fwrite(szline, szline.GetLength(), 1, f);
	
	int nNum = 0;
	CTagItem *pItem;
	CString szErr;
	CSortObArray &tagList = m_tagtab.GetSortObArray();
	for(i = 0; i < tagList.GetSize(); i++)
	{
		pItem = (CTagItem *)tagList.GetAt(i);
		
		if (pItem == NULL)
			continue;
		
		if (pItem->m_nError ==0)
			szErr = "";
		else if (pItem->m_nError == 1)
			szErr = "无此标签";
		else
			szErr = "类型错误";
		
		szline.Format("%s,%s,%s,%s,%s", pItem->m_szID, pItem->m_szName, pItem->m_szDes,
			pItem->m_szUnit, pItem->GetTypeStr());
		for(int j = 0; j < CTagItem::GetCustomFieldCount(); j++)
		{	
			CCustomField &field = CTagItem::GetCustomField(j);
			szline += _T(",");
			szline += pItem->GetText(j + FIELD_LAST);
		}
		szline += _T(",");
		szline += pItem->m_szNote;
		if (!szErr.IsEmpty())
		{
			szline += _T(" - (");
			szline += szErr;	// 错误信息显示以最后
			szline += _T(")");
		}
		szline += _T("\n");
		fwrite(szline,szline.GetLength(),1,f);
		nNum++;
		//pThread->SetPos((int)(nNum / (double)nsize * 100));
	}
	fclose(f);
	
	//pThread->Close();
	
	szline.Format("成功导出%d条标签记录!", nNum);
	MessageBox(szline, _T("提示"), MB_OK | MB_ICONINFORMATION);
}

void CCfgDlg::OnButtonDel() 
{
	if(IDYES != AfxMessageBox("删除后不可恢复，是否要删除选择的标签?",MB_YESNO | MB_ICONQUESTION))
		return;
	
	int nDel = m_tagtab.DelSelect();
	m_winTagInfo.ReDraw();
	CString szt;
	szt.Format(_T("总共删除%d个数据项！"), nDel);
	AfxMessageBox(szt, MB_OK | MB_ICONINFORMATION);
}

void CCfgDlg::OnButtonAddone() 
{	
	CDlgItemAttr dlg;
	dlg.m_bAdd = TRUE;
	CString szError;
	
_loop_show_dlg:
	if(dlg.DoModal()==IDOK)
	{
		
		if(dlg.m_bAdd)
		{
			if(dlg.m_szTagname.IsEmpty())
			{
				AfxMessageBox("标签名不能为空!", MB_OK | MB_ICONERROR);
				return;
			}
			CTagItem tagItem;
			tagItem.m_szID	=	dlg.m_szID;
			tagItem.m_szName = dlg.m_szTagname;
			tagItem.m_szDes = dlg.m_szDesc;
			tagItem.m_szUnit = dlg.m_szUnit;
			tagItem.m_szNote = dlg.m_szNote;
			tagItem.m_wType = dlg.GetDataType();
			tagItem.m_bSel = FALSE;
			
			if (m_tagtab.GetTag(dlg.m_szTagname))
			{
				
				szError.Format("%s<%s>重名!","错误:数据项", dlg.m_szTagname);//错误:数据项,重名
				AfxMessageBox(szError, MB_OK | MB_ICONERROR);
				goto _loop_show_dlg;
			}
			m_tagtab.AddTag(&tagItem);
			//m_tagtab.Qsort();
			m_winTagInfo.ReDraw();
		}
	}	
}

void CCfgDlg::OnButtonEdittag() 
{
	int nSel = m_winTagInfo.GetCurSel();
	
	if(nSel == -1)
	{
		AfxMessageBox("请选择一个数据项!", MB_OK | MB_ICONWARNING);
		return;
	}
	CTagItem *pTagItem = m_tagtab.GetTag(nSel);
	if (!pTagItem)
	{
		AfxMessageBox("请选择一个数据项!", MB_OK | MB_ICONWARNING);
		return;
	}
	
	CString szError;
	CDlgItemAttr dlg;
	dlg.m_bAdd = FALSE;
	dlg.m_szID		= pTagItem->m_szID;
	dlg.m_szOldName   = pTagItem->m_szName;
	dlg.m_szTagname   = pTagItem->m_szName;
	dlg.m_szDesc      = pTagItem->m_szDes;
	dlg.m_szUnit      = pTagItem->m_szUnit;
	dlg.SetDataType(pTagItem->m_wType);
	dlg.m_szNote      = pTagItem->m_szNote;
	
_loop_show_dlg:
	if(dlg.DoModal()==IDOK)
	{
		if ((dlg.m_szTagname != dlg.m_szOldName) && m_tagtab.GetTag(dlg.m_szTagname))
		{
			szError.Format(_T("标签名“%s”已经存在！"), dlg.m_szTagname);
			AfxMessageBox(szError, MB_OK | MB_ICONERROR);
			goto _loop_show_dlg;
		}
		
		pTagItem->m_szID	= dlg.m_szID;
		pTagItem->m_szName	= dlg.m_szTagname;
		pTagItem->m_szDes	= dlg.m_szDesc;
		pTagItem->m_szUnit	= dlg.m_szUnit;
		pTagItem->m_szNote	= dlg.m_szNote;
		pTagItem->m_wType	= dlg.GetDataType();
		pTagItem->m_bSel = FALSE;
		
		m_tagtab.ModifyItem(pTagItem, dlg.m_szOldName);
		//m_tagtab.Qsort();
		m_winTagInfo.ReDraw();
	}	
}
