
// IOServerView.cpp : CIOServerView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "IOServer.h"
#endif

#include "IOServerDoc.h"
#include "IOServerView.h"
#include "DlgLoadDrvTag.h"
#include "DlgQueryTag.h"
#include "DlgImport.h"
#include "DlgTagAttr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CIOServerView

IMPLEMENT_DYNCREATE(CIOServerView, CView)

BEGIN_MESSAGE_MAP(CIOServerView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CIOServerView::OnFilePrintPreview)
	ON_COMMAND(ID_GET_DEVICE_TAG, &CIOServerView::OnGetDeviceTag)
	ON_COMMAND(ID_QUERY_TAG, &CIOServerView::OnQueryTag)
	ON_COMMAND(ID_BTN_QUERY, &CIOServerView::OnQueryTag)
	ON_COMMAND(ID_BTN_REFRESH, &CIOServerView::OnReFresh)
	ON_COMMAND(ID_EXPORT_TAG, &CIOServerView::OnExportTag)
	ON_COMMAND(ID_EXPORT_SELECT_TAG, &CIOServerView::OnExportSelectTag)
	ON_COMMAND(ID_IMPORT_TAG, &CIOServerView::OnImportTag)
	ON_COMMAND(ID_DEL_TAG, &CIOServerView::OnDelTag)
	ON_COMMAND(ID_ADD_TAG, &CIOServerView::OnAddTag)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CIOServerView 构造/析构

CIOServerView::CIOServerView()
{
	// TODO: 在此处添加构造代码
	m_uTimer = 0;
	m_fntText.CreateFont(12, 0, 0, 0, 400, 0, 0, 0, GB2312_CHARSET, 0, 0, PROOF_QUALITY, VARIABLE_PITCH | FF_ROMAN, "宋体");
}

CIOServerView::~CIOServerView()
{
}

BOOL CIOServerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CIOServerView 绘制

void CIOServerView::OnDraw(CDC* pDC)
{
	CIOServerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CString szq = GetQueryInfo();

	CString szMsg;
	szMsg.Format("条件: %s   结果:%d",szq, m_wndTagTable.m_array.GetSize());

	CFont *oldfnt = pDC->SelectObject(&m_fntText);
	pDC->TextOut(140,10,szMsg);
	pDC->SelectObject(oldfnt);
	// TODO: 在此处为本机数据添加绘制代码
}


// CIOServerView 打印


void CIOServerView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CIOServerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CIOServerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CIOServerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CIOServerView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CIOServerView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	//theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CIOServerView 诊断

#ifdef _DEBUG
void CIOServerView::AssertValid() const
{
	CView::AssertValid();
}

void CIOServerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CIOServerDoc* CIOServerView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CIOServerDoc)));
	return (CIOServerDoc*)m_pDocument;
}
#endif //_DEBUG


// CIOServerView 消息处理程序

CString CIOServerView::GetQueryInfo()
{
	CString szQuery = m_szQuery;

	CString sname="",sdbname = "",sdbtagname="",sdes="",stype = "";
	int i,pos=0,nl = szQuery.GetLength();
	if(nl==0)
		return CString("全部");
	char c;
	for(i=0;i<nl;i++)
	{
		c = szQuery[i];
		if(c == '\n')
			pos++;
		else
		{
			if(pos == 0)
				sname+=c;
			else if(pos == 1)
				sdbname+=c;
			else if(pos == 2)
				sdbtagname+=c;
			else if(pos == 3)
				sdes+=c;
			else if(pos == 4)
				stype+=c;
		}
	}
	int ntype = atoi(stype);//zaqxsw19971015

	CString szi = "";
	if(!sname.IsEmpty())
	{
		szi += "标签名=";
		szi += sname;
	}
	if(!sdbname.IsEmpty())
	{
		if(!szi.IsEmpty())
			szi += " AND ";
		szi += "数据库=";
		szi += sdbname;
	}
	if(!sdbtagname.IsEmpty())
	{
		if(!szi.IsEmpty())
			szi += " AND ";
		szi += "数据库标签=";
		szi += sdbtagname;
	}
	if(!sdes.IsEmpty())
	{
		if(!szi.IsEmpty())
			szi += " AND ";
		szi += "描述=";
		szi += sdes;
	}
	if(ntype > 0)
	{
		if(!szi.IsEmpty())
			szi += " AND ";
		szi += "类型=";
		//szi += CTagItem::GetTypeStr(ntype);
	}
	return szi;
}

void	CIOServerView::Query()
{
	CString szQuery = m_szQuery;
	CString sname="",sdbname = "",sdbtagname="",sdes="",stype = "";
	int i,pos=0,nl = szQuery.GetLength();
	char c;
	for(i=0;i<nl;i++)
	{
		c = szQuery[i];
		if(c == '\n')
			pos++;
		else
		{
			if(pos == 0)
				sname+=c;
			else if(pos == 1)
				sdbname+=c;
			else if(pos == 2)
				sdbtagname+=c;
			else if(pos == 3)
				sdes+=c;
			else if(pos == 4)
				stype+=c;
		}
	}
	int ntype = atoi(stype);
	if(ntype == 0)
		ntype = -1;

	m_wndTagTable.QueryTag(sname,sdbname,sdbtagname,sdes,ntype);
	m_wndTagTable.RedrawList();
	CRect rt(0,0,1024,29);
	InvalidateRect(&rt);
}

int CIOServerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	// TODO: Add your specialized creation code here
	CString szContent;
	szContent.LoadStringA(IDS_QUERY);
	m_btnQuery.Create(szContent,WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,CRect(0,0,0,0),this,ID_BTN_QUERY);
	szContent.LoadStringA(IDS_REFRESH);
	m_btnRefresh.Create(szContent,WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,CRect(0,0,0,0),this,ID_BTN_REFRESH);

	m_btnQuery.SetFont(&m_fntText);
	m_btnRefresh.SetFont(&m_fntText);

	CIOServerDoc* pDoc = GetDocument();

	m_wndTagTable.m_szDecName = pDoc->m_szDecName;
	//m_wndTagTable.m_pvalman = &(theApp.m_pDriverMgr->m_DecValMan);

	CRect rtc(0,0,0,0);	
	m_wndTagTable.Create(NULL,NULL, WS_CHILD|WS_VISIBLE, rtc, this, -1);//标签表
	m_wndTagTable.QueryTag(NULL,NULL,NULL,NULL,-1);//查询全部
	
	m_wndTagTable.ReDraw();

	SetTimer(1, 1000, NULL);
	return 0;
}


void CIOServerView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if(m_btnQuery.GetSafeHwnd() != NULL)
		m_btnQuery.MoveWindow(8,5,50,20);
	if(m_btnRefresh.GetSafeHwnd() != NULL)
		m_btnRefresh.MoveWindow(70,5,50,20);

	if(m_wndTagTable.GetSafeHwnd() != NULL)
		m_wndTagTable.MoveWindow(CRect(0,30,cx,cy));
}


void CIOServerView::OnGetDeviceTag()
{
	// TODO: 在此添加命令处理程序代码

	if(!theApp.CheckPower(0XFFFFFFFF))
		return ;

	CIOServerDoc* pDoc = GetDocument();

	if(DEC_STATUS_RUN == theApp.GetDecStatus(pDoc->m_szDecName))
	{
		AfxMessageBox(IDS_MSG_DEVICESTOP,MB_OK|MB_ICONINFORMATION);
		return ;
	}

	CDlgLoadDrvTag dlg;
	if(dlg.DoModal() != IDOK)
		return;
	if(dlg.m_dwflag == 0)
	{
		AfxMessageBox(IDS_MSG_CANCEL);
		return;
	}

	CDrvTags drvTags;
	int nNew = 0, nUpdate = 0;
	int nCount = theApp.GetDrvTags(dlg.m_dwflag, pDoc->m_szDecName, drvTags, nNew, nUpdate);
	if(nNew || nUpdate)
	{
		Query(); //重新查询
		theApp.Save();//保存
	}
	CString szMsg;
	CString szTemp;
	szTemp.LoadString(IDS_MSG_LOADDEV);
	szMsg.Format(szTemp, nCount, nNew, nUpdate);
	AfxMessageBox(szMsg,MB_OK|MB_ICONINFORMATION);
}


void CIOServerView::OnQueryTag()
{
	// TODO: 在此添加命令处理程序代码
	m_szQuery;//查询字符串,name\n 代理标签名 \n 驱动标签名\n 描述\n 类型\n

	CString szQuery = m_szQuery;
	CDlgQueryTag dlg;
	CString sname="",sdbname = "",sdbtagname="",sdes="",stype = "";
	int i,pos=0,nl = szQuery.GetLength();
	char c;
	for(i=0;i<nl;i++)
	{
		c = szQuery[i];
		if(c == '\n')
			pos++;
		else
		{
			if(pos == 0)
				sname+=c;
			else if(pos == 1)
				sdbname+=c;
			else if(pos == 2)
				sdbtagname+=c;
			else if(pos == 3)
				sdes+=c;
			else if(pos == 4)
				stype+=c;
		}
	}
	dlg.m_sname = sname;
	dlg.m_spname = sdbname;
	dlg.m_sdname = sdbtagname;
	dlg.m_sdes = sdes;
	dlg.m_ntype = atoi(stype);
	if(dlg.m_ntype < 0)
		dlg.m_ntype = 0;
	if(dlg.DoModal()!= IDOK)
		return;
	int ntype = dlg.m_ntype;
	if(ntype == 0)
		ntype = -1;

	szQuery.Format("%s\n%s\n%s\n%s\n%d\n",dlg.m_sname,dlg.m_spname,dlg.m_sdname,dlg.m_sdes,ntype);
	m_szQuery = szQuery;
	m_wndTagTable.QueryTag(dlg.m_sname,dlg.m_spname,dlg.m_sdes,dlg.m_sdname,ntype);
	m_wndTagTable.ReDraw();//刷新
	CRect rt(0,0,1024,29);
	InvalidateRect(&rt);
}




void CIOServerView::OnExportTag()
{
	// TODO: 在此添加命令处理程序代码
	if(!theApp.CheckPower(1))
		return ;
	CIOServerDoc* pDoc = GetDocument();
	if(theApp.GetDecStatus(pDoc->m_szDecName) == DEC_STATUS_RUN)
	{
		MessageBox("请停止设备后导入!","提示",MB_OK|MB_ICONINFORMATION);
		return ;
	}

	CString szPathName;
	CString szFileName;
	if (!CFileExplorer::ShowFileDialog(szFileName, szPathName, FALSE, _T(".csv"),
		NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("标签文件(*.csv)|*.csv||"), 
		_T("导出标签文件"), this))
		return;

	try
	{
		CCSVFile file(szPathName, CCSVFile::modeWrite);

		int ntags = 0;
		m_wndTagTable.ExportTags(&file,&ntags);
		file.Close();

		CString szMsg;
		szMsg.Format("导出完成!\n\n%d标签",ntags);
		MessageBox(szMsg,"提示",MB_OK|MB_ICONINFORMATION);
	}
	catch (...)
	{
		MessageBox(_T("文件不能写！"),_T("提示"),MB_OK|MB_ICONWARNING);
		return;
	}
}


void CIOServerView::OnExportSelectTag()
{
	// TODO: 在此添加命令处理程序代码
	if(!theApp.CheckPower(1))
		return ;

	CIOServerDoc* pDoc = GetDocument();
	if(theApp.GetDecStatus(pDoc->m_szDecName) == DEC_STATUS_RUN)
	{
		MessageBox("请停止设备后导入!","提示",MB_OK|MB_ICONINFORMATION);
		return ;
	}
	if(m_wndTagTable.GetNextSelTag(0) == -1)
	{
		MessageBox("没有选择标签!","提示",MB_OK|MB_ICONINFORMATION);
		return;
	}

	CString szPathName;
	CString szFileName;
	if (!CFileExplorer::ShowFileDialog(szFileName, szPathName, FALSE, _T(".csv"),
		NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("标签文件(*.csv)|*.csv||"), 
		_T("导出标签"), this))
		return;

	try
	{
		CCSVFile file(szPathName, CCSVFile::modeWrite);

		int ntags = 0;
		m_wndTagTable.ExportSelTags(&file, &ntags);
		file.Close();

		CString szMsg;
		szMsg.Format("完成!\n\n%d标签",ntags);
		MessageBox(szMsg,"提示",MB_OK|MB_ICONINFORMATION);	
	}
	catch (...)
	{
		MessageBox(_T("文件不能写！"),_T("提示"),MB_OK|MB_ICONWARNING);
		return;
	}
}


void CIOServerView::OnImportTag()
{
	// TODO: 在此添加命令处理程序代码
	if(!theApp.CheckPower(0XFFFFFFFF))
		return ;

	CIOServerDoc* pDoc = GetDocument();

	if(theApp.GetDecStatus(pDoc->m_szDecName) == DEC_STATUS_RUN)
	{
		MessageBox("请停止设备后导入!","提示",MB_OK|MB_ICONINFORMATION);
		return ;
	}

	CString szPathName;
	CString szFileName;
	if (!CFileExplorer::ShowFileDialog(szFileName, szPathName, TRUE, _T(".csv"),
		NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("标签文件(*.csv)|*.csv||"), 
		_T("导入标签定义文件"), this))
		return;

	CDlgImport dlg;
	if(dlg.DoModal() != IDOK)
		return;
	if(dlg.m_dwflag == 0) //没有策略
	{
		MessageBox("更新0个，添加0个","提示",MB_OK|MB_ICONINFORMATION);
		return;
	}

	try
	{
		CCSVFile csvfile(szPathName, CCSVFile::modeRead);

		int nUpdate = 0, nAdd=0, nAll = 0;
		if(!theApp.ImportTags(pDoc->m_szDecName, csvfile,nUpdate,nAdd,nAll,dlg.m_dwflag))
		{
			csvfile.Close();
			AfxMessageBox("文件格式不对!");
			return;
		}
		csvfile.Close();

		Query(); //重新查询
		theApp.Save();//保存工程

		CString szMsg;
		szMsg.Format("导入完成!\n\n更新%d个，添加%d个",nUpdate,nAll);
		MessageBox(szMsg,"提示",MB_OK|MB_ICONINFORMATION);
	}
	catch (...)
	{
		AfxMessageBox("不能打开文件!");
		return ;
	}	
}


void CIOServerView::OnDelTag()
{
	// TODO: 在此添加命令处理程序代码
	if(!theApp.CheckPower(0XFFFFFFFF))
		return ;

	CIOServerDoc* pDoc = GetDocument();

	if(theApp.GetDecStatus(pDoc->m_szDecName) == DEC_STATUS_RUN)
	{
		MessageBox("请停止设备后导入!","提示",MB_OK|MB_ICONINFORMATION);
		return ;
	}

	if(m_wndTagTable.GetNextSelTag(0) == -1)
	{
		MessageBox("没有选择标签!","提示",MB_OK|MB_ICONINFORMATION);
		return;
	}
	if(IDYES != MessageBox("删除后不可恢复，是否要删除选择的标签?","提示",MB_YESNOCANCEL|MB_ICONINFORMATION))
		return;
	int ntags = DelSelTags(pDoc->m_szDecName); 

	Query(); //重新查询

	theApp.Save();//保存工程

	CString szMsg;
	szMsg.Format("删除选择的标签完成!\n\n%d标签",ntags);
	MessageBox(szMsg,"提示",MB_OK|MB_ICONINFORMATION);
}


int	CIOServerView::DelSelTags(CString szDecName)//删除选择的标签
{
	int i = 0,n=0;
	CTagItem *pi;

	// 禁止日志最大化限制
	//theApp.m_runlog.SetMaxLimit(FALSE);

	while(i<m_wndTagTable.m_array.GetSize())
	{
		pi = (CTagItem*)m_wndTagTable.m_array.GetAt(i);
		if(!pi->m_bSel)
			i++;
		else
		{
			theApp.DelTag(szDecName, pi->m_szName);
			m_wndTagTable.m_array.RemoveAt(i);

			GetLogger()->AddWarningFmt(LOG_OPT_STORAGE, LOG_DELETE_TAG_FORMAT, LOG_LOGIN_USER, pi->m_szName);

			delete pi;

			n++;
		}
	}
	//theApp.m_runlog.SetMaxLimit(TRUE);
	return n;
}
void CIOServerView::OnAddTag()
{
	// TODO: 在此添加命令处理程序代码
	if(!theApp.CheckPower(0XFFFFFFFF))
		return ;

	CIOServerDoc* pDoc = GetDocument();

	if(theApp.GetDecStatus(pDoc->m_szDecName) == DEC_STATUS_RUN)
	{
		MessageBox("请停止设备后导入!","提示",MB_OK|MB_ICONINFORMATION);
		return ;
	}

	CDlgTagAttr dlg;
	//dlg.m_pdrv = pdrv;
	dlg.m_szDrvName = m_wndTagTable.m_szDecName;
	dlg.m_bAdd = TRUE;
lpredo:
	if(dlg.DoModal() != IDOK)
		return;

	if(!dlg.m_szexp.IsEmpty())
	{
		/*
		CString szbh="",szErr;
		if(0 != g_Mid2Behind(dlg.m_szexp,szbh,szErr))//中置->后置 ret=0 ok
		{
			AfxMessageBox("转换公式语法错误!");
			goto lpredo;
		}
		if(!g_CheckBehindParse(szbh))
		{
			AfxMessageBox("转换公式语法错误!");
			goto lpredo;
		}
		*/
		//CExpressionEngine::RemoveAll();
		//CExpressionEngine engine(FALSE); // 创建一个用于检查表达式是否正确的计算引擎
		//engine.SetExpr(dlg.m_szexp);
		//if (!engine.Check(dlg.m_sztagname))
		//{
		//	AfxMessageBox("转换公式语法错误，请检查输入的转换公式是否正确！");
		//	goto lpredo;
		//}

		//if (CExpressionEngine::GetTagCount() == 0)
		//{
		//	if (AfxMessageBox(_T("转换公式中未发现有参与计算的标签，可能转换公式有问题，请检查输入的转换公式是否正确！"), MB_YESNO) == IDNO)
		//		goto lpredo;
		//}
	}
	//加入标签表,不成功则报错跳转到
	CTagItem tag;
	tag.m_szName = dlg.m_sztagname;
	tag.m_sznamedb=dlg.m_sztagdbname;
	tag.m_szdes=dlg.m_szdes;
	tag.m_szunit = dlg.m_szunit;
	tag.m_sznamedrv=dlg.m_sztagdrvname;
	tag.m_ntype = dlg.m_ndatatype + 1;
	tag.m_nCompType=dlg.m_ncomptype;
	tag.m_szCompVal=dlg.m_szcompval;
	tag.m_szexpcp=dlg.m_szexp;

	if(theApp.AddTag2Drv(pDoc->m_szDecName, tag, dlg.m_bAdd))
	{
		// 禁止日志最大化限制
		//theApp.m_runlog.SetMaxLimit(FALSE);
		//theApp.m_logger.AddWarningFmt(LOG_OPT_STORAGE, LOG_INSERT_TAG_FORMAT, LOG_LOGIN_USER, tag.m_szname);
		//theApp.m_runlog.SetMaxLimit(TRUE);

		theApp.Save();
		Query(); //重新查询
	}
	else
	{
		AfxMessageBox(IDS_TAG_EXIST);
		goto lpredo;
	}
}


void CIOServerView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (1 == nIDEvent)
	{
		m_wndTagTable.Invalidate();
		return;
	}
	CView::OnTimer(nIDEvent);
}

void CIOServerView::OnReFresh()
{
	Query();
}