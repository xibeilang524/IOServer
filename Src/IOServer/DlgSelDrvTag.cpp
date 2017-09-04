// DlgSelDrvTag.cpp : implementation file
//

#include "stdafx.h"
#include "IOServer.h"
#include "DlgSelDrvTag.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDrvTagItem::CDrvTagItem()
{
	m_ntype = DRT_FLOAT32;	//数据类型
	m_szName="";	//标签名
	m_szdes="";		//描述
	m_szunit="";	//单位

	m_bSel  = FALSE;
	
}

CDrvTagItem::~CDrvTagItem()
{

}

void		CDrvTagItem::CopyTo(CTagItem *pt)
{
	pt->m_ntype = m_ntype;	
	pt->m_szName=m_szName;	
	pt->m_szdes=m_szdes;		
	pt->m_szunit=m_szunit;	
}

int		CDrvTagItem::Compare(CSortItem *pOb)	//排序比较
{
	int nret = 0;
	if(pOb == NULL)
		return nret;
	CTagItem *pi = (CTagItem *)pOb;
	
	switch(m_nSortWho)
	{
	case 0:  
		nret = strcmp(m_szName,pi->m_szName);
		break;
	case 1: 
		nret = strcmp(m_szdes,pi->m_szdes);
		break;
	case 2: 
		nret = strcmp(m_szunit,pi->m_szunit);
		break;
	case 3: 
		if(m_ntype > pi->m_ntype)
			nret = 1;
		else if(m_ntype < pi->m_ntype)
			nret = -1;
		break;

	}
	if(!m_bAsc)
		nret *= -1;
	return nret;	
}



void	CDrvTagItem::Serialize(CArchive& ar)	//存盘
{
	CString szcom = "";
	if(ar.IsLoading())
	{
		ar >> m_ntype;	
		ar >> m_szName;	
		ar >> m_szdes;	
		ar >> m_szunit;	

	}
	else
	{
		ar << m_ntype;	
		ar << m_szName;	
		ar << m_szdes;	
		ar << m_szunit;	
	
	}
}

CString CDrvTagItem::GetTypeStr(int ntype)
{
	if(ntype == DRT_DIGITAL)
		return  CString("digital");
	else if(ntype == DRT_INT32)
		return  CString("int32");
	else if(ntype == DRT_FLOAT32)
		return  CString("float32");
	else if(ntype == DRT_INT64)
		return  CString("int64");
	else if(ntype == DRT_FLOAT64)
		return  CString("float64");
	else if(ntype == DRT_STRING)
		return  CString("string");
	else if(ntype == DRT_BLOB)
		return  CString("blob");
	return CString("void");
}

int CDrvTagItem::GetTypeVal(LPCTSTR lpszType)
{
	if(!_stricmp("digital",lpszType))
		return DRT_DIGITAL;
	else if(!_stricmp("int32",lpszType))
		return DRT_INT32;
	else if(!_stricmp("float32",lpszType))
		return DRT_FLOAT32;
	else if(!_stricmp("int64",lpszType))
		return DRT_INT64;
	else if(!_stricmp("float64",lpszType))
		return DRT_FLOAT64;
	else if(!_stricmp("string",lpszType))
		return DRT_STRING;
	else if(!_stricmp("blob",lpszType))
		return DRT_BLOB;
	return DRT_VOID;
}
/*
class CWinDrvTagList : public CWinList //标签信息窗口
{
public:
	CWinDrvTagList();
	~CWinDrvTagList();
public:
	CDrvItem* m_pdrv;

	CSortObArray	m_array;	//标签数组CDrvTagItem* 数组,用于保存查询结果

public:
	int		GetNextSelTag(int nstart);
	int		GetNextSelected(int nstart);//返回选择

	CString GetText(int nr,int nc);


protected:
	virtual CString GetCellText(int nrow,int ncol);//取显示字符串
	virtual CString GetHeadText(int ncol);//取显示字符串
	virtual void	GetRowColorS(int nrow,int ncol,COLORREF &clrTxt,COLORREF &clrBk);//取行颜色
	virtual void	OnClickHeadCol(int ncol);//鼠标点击表头

	virtual void	OnCurSelChange(int nrow);
	virtual void	OnSelChange(int nrowstart,int nrowend);
	virtual void	OnCtrlSelChange(int nrow);//CTRL按住时的选择


	BOOL	IsSel(int nIndex);
	int		GetItemErrStatus(int nIndex);
	BOOL	SetSelXor(int nIndex); //相反设置
	BOOL	SetSel(int nIndex,BOOL bSel);
};
*/

CWinDrvTagList::CWinDrvTagList()
{
	m_array.SetGrown(8192);
	m_nRowH = 18;//行高
	m_nTxtSize = 12;
	m_bGridLineH = TRUE;

	m_nCols = 4;
	SetColW(0,160); 
	SetColW(1,160); 
	SetColW(2,80); 
	SetColW(3,80);	
	

	SetCellFmt(0,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	SetCellFmt(1,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	SetCellFmt(2,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	SetCellFmt(3,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

}
CWinDrvTagList::~CWinDrvTagList()
{

}
CString CWinDrvTagList::GetText(int nrow,int ncol)
{
	CDrvTagItem *pi = (CDrvTagItem*)m_array.GetAt(nrow);
	if(!pi)
		return CString("");
	CString szt= "";
	switch(ncol)
	{
	case 0:
		szt = pi->m_szName;
		break;
	case 1:
		szt = pi->m_szdes;
		break;
	case 2:
		szt = pi->m_szunit;
		break;
	case 3:
		szt = pi->GetTypeStr(pi->m_ntype);
		break;
	default:
		break;
	}
	return szt;
}
int		CWinDrvTagList::GetNextSelTag(int nstart)
{
	int i,ns = nstart,n = m_array.GetSize();
	if(ns < 0) ns = 0;
	for(i=ns;i<n;i++)
	{
		if(IsSel(i))
			return i;
	}
	return -1;
}
CString CWinDrvTagList::GetCellText(int nrow,int ncol)//取显示字符串
{
	return GetText(nrow,ncol);
}
CString CWinDrvTagList::GetHeadText(int ncol)//取显示字符串
{
	CString szt= "";
	switch(ncol)
	{
	case 0:
		szt = "标签名";
		break;
	case 1:
		szt = "描述";
		break;
	case 2:
		szt = "单位";
		break;
	case 3:
		szt = "类型";
		break;
	case TGF_TYPE:
		szt = "类型";
		break;
	default:
		break;
	}
	return szt;
}

void CWinDrvTagList::ReDraw()
{
	SetRows(m_array.GetSize());
	this->Invalidate();
}

void	CWinDrvTagList::RedrawList()
{
	m_array.Qsort();
	SetRowScrollTop(m_array.GetSize());
}

void	CWinDrvTagList::GetRowColorS(int nrow,int ncol,COLORREF &clrTxt,COLORREF &clrBk)//取行颜色
{
	if(IsSel(nrow))
	{
		clrTxt = RGB(255,255,255);
		clrBk = RGB(0,0,128);
		if(this != GetFocus())
		{
			clrBk = ::GetSysColor(COLOR_BTNFACE);
			clrTxt = RGB(0,0,0);
		}
		return;
	}
	else
	{
		clrTxt = RGB(0,0,0);
		clrBk =  RGB(255,255,255);
	}
}
void	CWinDrvTagList::OnClickHeadCol(int ncol)//鼠标点击表头
{
	if(ncol == m_nSortCol)
		m_bSortAsc = !m_bSortAsc;
	m_nSortCol = ncol;
	m_array.SetSortType(m_nSortCol,m_bSortAsc);//设置排序
	m_array.Qsort();

	SetSortCol(m_nSortCol,m_bSortAsc);//设置表头显示
	Invalidate();
}
void	CWinDrvTagList::OnCurSelChange(int nrow)
{
	int i,n = m_array.GetSize();
	for(i=0;i<n;i++)
	{
		if(i != nrow) 
			SetSel(i,FALSE);
		else
			SetSel(i,TRUE);
	}
	m_nCurSel = nrow;
	this->Invalidate();
}
void	CWinDrvTagList::OnSelChange(int nrowstart,int nrowend)
{
	int i,n = m_array.GetSize();
	for(i=0;i<n;i++)
	{
		if((i<nrowstart) || (i>nrowend))
			SetSel(i,FALSE);
	
		else
			SetSel(i,TRUE);
	}
	this->Invalidate();
}
void	CWinDrvTagList::OnCtrlSelChange(int nrow)//CTRL按住时的选择
{
	if(SetSelXor(nrow))
		this->Invalidate();
}

BOOL	CWinDrvTagList::IsSel(int nIndex)
{
	CDrvTagItem *pi = (CDrvTagItem*)m_array.GetAt(nIndex);
	if(!pi)
		return FALSE;
	return pi->m_bSel;
}

BOOL	CWinDrvTagList::SetSelXor(int nIndex) //相反设置
{
	CDrvTagItem *pi = (CDrvTagItem*)m_array.GetAt(nIndex);
	if(pi)
	{
		pi->m_bSel = !pi->m_bSel;
		return TRUE;
	}
	return FALSE;
}
BOOL	CWinDrvTagList::SetSel(int nIndex,BOOL bSel)
{
	CDrvTagItem *pi = (CDrvTagItem*)m_array.GetAt(nIndex);
	if(pi)
	{
		pi->m_bSel = bSel;
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
// CDlgSelDrvTag dialog


CDlgSelDrvTag::CDlgSelDrvTag(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelDrvTag::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSelDrvTag)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_szTagName = "";
}


void CDlgSelDrvTag::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSelDrvTag)
	DDX_Control(pDX, IDC_STATIC_STFRM, m_stFrm);
	//}}AFX_DATA_MAP
}


void CDlgSelDrvTag::InitTag(CString szDrvName)
{
	m_wintag.m_array.DeleteObjects();
	CDrvTagItem* pi;
	int i=0,n=0;
	T_DRVTAG tag;
	while(!theApp.GetTagDef(szDrvName, i,tag))
	{
		pi = new CDrvTagItem;
		if(pi)
		{
			pi->m_szName = tag.sname;
			pi->m_szdes = tag.sdes;
			pi->m_szunit = tag.sunit;
			pi->m_ntype = tag.wtype;
			m_wintag.m_array.Add(pi);
			n++;
		}
		i++;
	}
}

BEGIN_MESSAGE_MAP(CDlgSelDrvTag, CDialog)
	//{{AFX_MSG_MAP(CDlgSelDrvTag)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WINLIST_LBDBLCLK, OnDblClkWinList)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSelDrvTag message handlers

BOOL CDlgSelDrvTag::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CRect rtc;
	m_stFrm.GetWindowRect(&rtc);
	ScreenToClient(&rtc) ;  
	rtc.DeflateRect(1,1,1,1);

	m_wintag.Create(NULL,NULL, WS_CHILD|WS_VISIBLE, rtc, this, -1);//标签表
	m_wintag.ReDraw();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelDrvTag::OnOK() 
{
	// TODO: Add extra validation here
	int nIndex = m_wintag.GetNextSelTag(0);
	if(nIndex < 0)
	{
		AfxMessageBox(IDS_SELECT_TAG_OR_CANCEL);
		return;
	}
	m_szTagName = m_wintag.GetText(nIndex,0);
	CDialog::OnOK();
}

void CDlgSelDrvTag::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

LRESULT	CDlgSelDrvTag::OnDblClkWinList(WPARAM wParam, LPARAM lParam) //双击消息
{
	int nIndex = (int)lParam;
	if(nIndex < 0)
		return 0;

	m_szTagName = m_wintag.GetText(nIndex,0);
	CDialog::OnOK();

	return 0;
}
