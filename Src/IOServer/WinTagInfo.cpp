#include "StdAfx.h"
#include "WinTagInfo.h"
#include "QualityCodeDef.h"
#include "IOServer.h"

const CString szDes[] = {"标签名","目标服务器标签名","描述","单位","类型","驱动标签名","采集值-工程值转换","时标","值","质量","压缩方式","压缩精度"};

long g_GetTimeAdjust();

CWinTagInfo::CWinTagInfo()
	:m_szDecName("")
{
	//m_pvalman = NULL;

	m_nRowH = 18;//行高
	m_nTxtSize = 12;
	m_bGridLineH = TRUE;

	m_nCols = 12;
	SetColW(TGF_NAME,120); 
	SetColW(TGF_NAMEDB,140); 
	SetColW(TGF_DES,160); 
	SetColW(TGF_UNIT,60);	
	SetColW(TGF_TYPE,60);	
	SetColW(TGF_NAMEDRV,120);

	SetColW(TGF_CATYPE,60);	
	SetColW(TGF_CAVAL,60);	
	
	SetColW(TGF_TIME,120); 
	SetColW(TGF_VAL,80);	
	SetColW(TGF_QA,120);	
	SetColW(TGF_EXPCP,130);
	
	

	SetCellFmt(TGF_NAME,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	SetCellFmt(TGF_NAMEDB,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	SetCellFmt(TGF_DES,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	SetCellFmt(TGF_UNIT,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	SetCellFmt(TGF_TYPE,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	SetCellFmt(TGF_NAMEDRV,DT_LEFT|DT_VCENTER|DT_SINGLELINE);

	SetCellFmt(TGF_CATYPE,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	SetCellFmt(TGF_CAVAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);

	
	SetCellFmt(TGF_TIME,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	SetCellFmt(TGF_VAL,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	SetCellFmt(TGF_QA,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	SetCellFmt(TGF_EXPCP,DT_LEFT|DT_VCENTER|DT_SINGLELINE);


	m_bmpOK.LoadBitmap(IDB_BIT_TAG_OK);
	m_bmpRDB.LoadBitmap(IDB_BIT_TAGRDBERR);
	m_bmpDrv.LoadBitmap(IDB_BIT_TAGDRVERR);
	m_bmpAll.LoadBitmap(IDB_BIT_TAGALLERR);

	m_array.SetGrown(8192);

}

CWinTagInfo::~CWinTagInfo()
{

}

CString CWinTagInfo::GetQualityText(CVarVal *pval)
{
	CString szt;

	if (!pval)
		return szt;

	if((pval->vt == DT_VOID)||(pval->ltime<=0))
	{
		return szt;
	}
	else 
	{
		CString szQualityName;

		LPDBPQUALITYITEM pItem = FindQualityItem(pval->snqa);

		if (pItem)
			szQualityName = pItem->lpszQualityName;

		szt.Format(_T("%d/%s"), pval->snqa, szQualityName);
	}

	return szt;
}

BOOL	CWinTagInfo::ExportTags(CCSVFile* pf,int *pnExport)//导出标签
{
	CString szline = "IO_Tags,,,,,,\n";
	pf->Write(szline, szline.GetLength());

	szline = "标签名,目标服务器标签名,描述,单位,类型,驱动标签名,压缩方式,压缩精度,采集值-工程值转换\n";
	pf->Write(szline, szline.GetLength());

	//开始写数据项
	CStringArray arrLines;
	arrLines.SetSize(9);

	CString szt;
	int i,n = m_array.GetSize();
	for(i=0;i<n;i++)
	{
		arrLines[0] = GetText(i,TGF_NAME);
		arrLines[1] = GetText(i,TGF_NAMEDB);
		arrLines[2] = GetText(i,TGF_DES);
		arrLines[3] = GetText(i,TGF_UNIT);
		arrLines[4] = GetText(i,TGF_TYPE);
		arrLines[5] = GetText(i,TGF_NAMEDRV);
		arrLines[6] = GetText(i,TGF_CATYPE);
		arrLines[7] = GetText(i,TGF_CAVAL);
		arrLines[8] = GetText(i,TGF_EXPCP);
		pf->WriteData(arrLines);
	}
	*pnExport = n;
	return TRUE;
}

int  CWinTagInfo::GetNextSelTag(int nstart)
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


BOOL	CWinTagInfo::ExportSelTags(CCSVFile* pf,int *pnExport)//导出选择标签
{
	CString szline = "IOServer Tag,,,,,,\n";
	pf->Write(szline, szline.GetLength());

	szline = "标签名,实时库标签名,描述,单位,类型,驱动标签名,压缩方式,压缩精度,采集值-工程值转换\n";
	pf->Write(szline, szline.GetLength());

	//开始写数据项
	CString szt;
	CStringArray arrLines;
	arrLines.SetSize(9);
	int i,n = m_array.GetSize();
	*pnExport = 0;
	for(i=0;i<n;i++)
	{
		if(!IsSel(i))
			continue;
		arrLines[0] = GetText(i,TGF_NAME);
		arrLines[1] = GetText(i,TGF_NAMEDB);
		arrLines[2] = GetText(i,TGF_DES);
		arrLines[3] = GetText(i,TGF_UNIT);
		arrLines[4] = GetText(i,TGF_TYPE);
		arrLines[5] = GetText(i,TGF_NAMEDRV);
		arrLines[6] = GetText(i,TGF_CATYPE);
		arrLines[7] = GetText(i,TGF_CAVAL);
		arrLines[8] = GetText(i,TGF_EXPCP);
		pf->WriteData(arrLines);

		(*pnExport)++;
	}
	return TRUE;
}

BOOL	CWinTagInfo::ExportRdbSelTags(CCSVFile* pf,int *pnExport,int nsel,LPCTSTR sdbname)//导出选择标签为实时库可用标签表
{
	CString szline = "dbproxytag,,,,,,\n";
	pf->Write(szline, szline.GetLength());

	szline = "标签名,数据库名,数据库标签名,描述,单位,数据类型,标签类型\n";
	pf->Write(szline, szline.GetLength());

	//开始写数据项
	CString szt;
	CStringArray arrLines;
	arrLines.SetSize(7);
	int i,n = m_array.GetSize();
	*pnExport = 0;
	for(i=0;i<n;i++)
	{
		if((!IsSel(i)) && (nsel == 1))
			continue;
		arrLines[0] = GetText(i,TGF_NAMEDB);//目标服务器标签名
		arrLines[1] = sdbname;//数据库名
		arrLines[2] = GetText(i,TGF_NAMEDB);
		arrLines[3] = GetText(i,TGF_DES);
		arrLines[4] = GetText(i,TGF_UNIT);
		arrLines[5] = GetText(i,TGF_TYPE);
		arrLines[6] = "ddctag";

		pf->WriteData(arrLines);

		(*pnExport)++;
	}
	return TRUE;
}

int		CWinTagInfo::ExportDDSTags(CCSVFile* pf,int *pnExport,int nsel)//导出DDS标签,nsel = 0表示全部
{
	CString szline = "dds2tag,,,,,\n";
	pf->Write(szline, szline.GetLength());

	//标签名,描述,单位,类型,缓存压缩,压缩精度
	szline = "标签名,描述,单位,类型,缓存压缩,压缩精度\n";
	pf->Write(szline, szline.GetLength());

	//开始写数据项
	CString szt;
	CStringArray arrLines;
	arrLines.SetSize(6);
	int i,n = m_array.GetSize();
	*pnExport = 0;
	for(i=0;i<n;i++)
	{
		if(!IsSel(i) && nsel != 0)
			continue;
		arrLines[0] = GetText(i,TGF_NAMEDB);
		arrLines[1] = GetText(i,TGF_DES);
		arrLines[2] = GetText(i,TGF_UNIT);
		arrLines[3] = GetText(i,TGF_TYPE);
		arrLines[4] = GetText(i,TGF_CATYPE);
		arrLines[5] = GetText(i,TGF_CAVAL);

		pf->WriteData(arrLines);

		(*pnExport)++;
	}
	return TRUE;
}
BOOL	CWinTagInfo::IsSel(int nIndex)
{
	CTagItem *pi = (CTagItem*)m_array.GetAt(nIndex);
	if(!pi)
		return FALSE;
	return pi->m_bSel;
}

int		CWinTagInfo::GetItemErrStatus(int nIndex)
{
	CTagItem *pi = (CTagItem*)m_array.GetAt(nIndex);
	if(pi)
		return pi->m_nErrStatus;
	return -1;
}

BOOL	CWinTagInfo::SetSel(int nIndex,BOOL bSel)
{
	CTagItem *pi = (CTagItem*)m_array.GetAt(nIndex);
	if(pi)
	{
		pi->m_bSel = bSel;
		return TRUE;
	}
	return FALSE;
}

BOOL	CWinTagInfo::SetSelXor(int nIndex) //相反设置
{
	CTagItem *pi = (CTagItem*)m_array.GetAt(nIndex);
	if(pi)
	{
		pi->m_bSel = !pi->m_bSel;
		return TRUE;
	}
	return FALSE;
}

void	CWinTagInfo::QueryTag(LPCTSTR stagname,LPCTSTR sproxytagname,LPCTSTR sdes,LPCTSTR sdrvtagname,int ntype)//查询标签
{
	m_array.DeleteObjects();//删除所有标签。

	theApp.QueryTag(m_szDecName, m_array,stagname,sproxytagname,sdes,sdrvtagname,ntype);
}

CBitmap* CWinTagInfo::GetBitMap(int nrow)
{
	int nst = GetItemErrStatus(nrow);
	if(nst == 0)
		return &m_bmpOK;
	else if(nst == 1)
		return &m_bmpRDB;
	else if(nst == 2)
		return &m_bmpDrv;
	else if(nst == 3)
		return &m_bmpAll;
	return NULL;
}

CString CWinTagInfo::GetHeadText(int ncol)//取显示字符串
{
	CString szt= "";
	switch(ncol)
	{
	case TGF_NAME:
		szt = "标签名";
		break;
	case TGF_NAMEDB:
		szt = "目标服务器标签名";
		break;
	case TGF_DES:
		szt = "描述";
		break;
	case TGF_UNIT:
		szt = "单位";
		break;
	case TGF_TYPE:
		szt = "类型";
		break;
	case TGF_NAMEDRV:
		szt = "驱动标签名";
		break;
	case TGF_EXPCP:
		szt = "采集值-工程值转换";
		break;
	case TGF_TIME:
		szt = "时标";
		break;
	case TGF_VAL:
		szt = "值";
		break;
	case TGF_QA:
		szt = "质量";
		break;

	case TGF_CATYPE:
		szt = "压缩方式";
		break;
	case TGF_CAVAL:
		szt = "压缩精度";
		break;
	default:
		break;
	}
	return szt;
}

void CWinTagInfo::ReDraw()
{
	SetRows(m_array.GetSize());
	this->Invalidate();
}

void	CWinTagInfo::GetRowColorS(int nrow,int ncol,COLORREF &clrTxt,COLORREF &clrBk)//取行颜色
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
CString CWinTagInfo::GetText(int nrow,int ncol)
{
	CTagItem *pi = (CTagItem*)m_array.GetAt(nrow);
	if(!pi)
		return CString("");
	CString szt= "";
	switch(ncol)
	{
	case TGF_NAME:
		szt = pi->m_szName;
		break;
	case TGF_NAMEDB:
		szt = pi->m_sznamedb;
		break;
	case TGF_DES:
		szt = pi->m_szdes;
		break;
	case TGF_UNIT:
		szt = pi->m_szunit;
		break;
	case TGF_TYPE:
		szt = pi->GetTypeStr(pi->m_ntype);
		break;
	case TGF_CATYPE:
		szt = pi->GetCompTypeStr(pi->m_nCompType);
		break;
	case TGF_CAVAL:
		if(pi->m_nCompType == 0)
			szt = "";
		else
			szt = pi->m_szCompVal;
		break;

	case TGF_NAMEDRV:
		szt = pi->m_sznamedrv;
		break;
	case TGF_EXPCP:
		szt = pi->m_szexpcp;
		break;
	case TGF_TIME:
		szt = ""; 
		break;
	case TGF_VAL:
		szt = ""; 
		break;
	case TGF_QA:
		szt = ""; 
		break;
	default:
		break;
	}
	return szt;
}
CString CWinTagInfo::GetCellText(int nrow,int ncol)//取显示字符串
{
	if(theApp.GetDecStatus(m_szDecName) != DRSTATUS_RUN)//没有运行则从驱动实例获取
		return GetText(nrow,ncol);

	if((ncol < TGF_TIME)||(ncol == TGF_EXPCP)) 
		return GetText(nrow,ncol);
	else
	{
		CVarVal varValue;
		CString szVal = "";
	//	if(!m_pvalman)
	//		return szVal;

		CTagItem* pt = (CTagItem*)m_array.GetAt(nrow);
		if(!pt)
			return szVal;
		
		if(!theApp.GetTagValue(m_szDecName, pt->m_szName, varValue))
			return "";
		//if(!m_pvalman->GetTagVal(m_pdrv->m_lid,pt->m_szname,val)) 
		//	return szVal;

		if(ncol == TGF_TIME)
			szVal = varValue.GetTimeStr(g_GetTimeAdjust());
		
		else if(ncol == TGF_QA)
		{
			if(varValue.vt == DT_VOID)
				return szVal; 
			//szVal.Format("%d",val.snqa);
			//{{ heqing 2011/0623
			szVal = GetQualityText(&varValue);
			//}}
		}
		else if(ncol == TGF_VAL)
		{
			if(varValue.vt == DT_VOID)
				return szVal;

			//szVal =  val.GetValStr();

			if (varValue.vt == DT_BLOB)
				szVal = _T("[BLOB]");
			else
				szVal = varValue.GetValStr();
		}

		return  szVal;
	}

	return CString("");
}

void	CWinTagInfo::OnClickHeadCol(int ncol)//鼠标点击表头
{
	if((ncol == TGF_VAL)||(ncol == TGF_CATYPE) || (ncol == TGF_CAVAL) || (ncol == TGF_EXPCP)) //modify811
		return;

	if(ncol == m_nSortCol)
		m_bSortAsc = !m_bSortAsc;
	m_nSortCol = ncol;
	m_array.SetSortType(m_nSortCol,m_bSortAsc);//设置排序
	m_array.Qsort();

	SetSortCol(m_nSortCol,m_bSortAsc);//设置表头显示
	Invalidate();
}

int	 CWinTagInfo::GetNextSelected(int nstart)//返回选择
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

BOOL	CWinTagInfo::GetTag(int nIndex,CTagItem *pi) //复制方式
{
	int n = m_array.GetSize();
	if((nIndex < 0) || (nIndex >= n))
		return FALSE;
	CTagItem *pt = (CTagItem *)m_array.GetAt(nIndex);
	//pt->CopyTo(pi);
	return TRUE;
}

BOOL	CWinTagInfo::UpdateTag(int nIndex,CTagItem *pi)
{
	int n = m_array.GetSize();
	if((nIndex < 0) || (nIndex >= n))
		return FALSE;
	CTagItem *pt = (CTagItem *)m_array.GetAt(nIndex);
	if(pt->m_szName != pi->m_szName)
		return FALSE;

	//theApp.m_runlog.SetMaxLimit(FALSE);
	//GetLogger()->AddWarningFmt(LOG_OPT_STORAGE, LOG_UPDATE_TAG_FORMAT, LOG_LOGIN_USER, pi->m_szname);
	//theApp.m_runlog.SetMaxLimit(TRUE);

	//pi->CopyTo(pt);
	Invalidate();
	return TRUE;
}


void	CWinTagInfo::OnCurSelChange(int nrow)
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
void	CWinTagInfo::OnCtrlSelChange(int nrow)//CTRL按住时的选择
{
	if(SetSelXor(nrow))
		this->Invalidate();
}

void	CWinTagInfo::OnSelChange(int nrowstart,int nrowend)
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

void	CWinTagInfo::RedrawList()
{
	m_array.Qsort();
	SetRowScrollTop(m_array.GetSize());
}
