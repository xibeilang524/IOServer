//odbcobj.cpp
#include "stdafx.h"
#include <string.h>
#include "odbcdrv.h"
#include "odbcobj.h"
#include <math.h>
#include <stdio.h>

#include "DbCfgDlg.h"
#include "..\sdk\DrvVersion.h"

#include "DlgConfig.h"

#define DRVCFG_VERSION		(DRVCFG_VERSION_USER + 1)	// 第一版

/////////////////////////////////////////////////////////////////
//实现静态对象类工厂
CDrvObj*	CDrvFactory::NewDrv()
{
	return new COdbcDrvObj();
}

DRVRET CDrvFactory::GetDriverInfo(LPDRVINFO lpDrvInfo)
{
	memset(lpDrvInfo,0,sizeof(T_DRVINFO));
	strcpy(lpDrvInfo->sname,"DDC odbc Driver");
	strcpy(lpDrvInfo->sdes,"CopyRight By 西安热工研究院有限公司");
	strcpy(lpDrvInfo->sver,"2.0.0.1"); //程序版本
	lpDrvInfo->dwver = 0x0200;//规范版本
	lpDrvInfo->dwres = 0;
	return DRERR_OK;
}

//辅助对象
CString  CTagItem::GetText(int nField)//取数据字符串
{
	if(nField == FIELD_NAME)
		return m_szName;
	else if(nField == FIELD_ALIASNAME)
		return m_szAliasName;
	else if(nField == FIELD_DES)
		return m_szDes;
	else if(nField == FIELD_UNIT)
		return m_szUnit;
	else if(nField == FIELD_TYPE)
		return GetTypeStr(m_nType);

	return CString("");
}

void CTagItem::CopyTo(CTagItem *pt)//复制
{
	pt->m_szName = m_szName;
	pt->m_szAliasName = m_szAliasName;
	pt->m_szDes = m_szDes;
	pt->m_szUnit = m_szUnit;
	pt->m_nType = m_nType;
}

void	CTagItem::CopyToDrTag(LPDRVTAG lpdrtag)//复制到接口格式标签定义
{
	lpdrtag->sname[79]=  0;
	lpdrtag->sdes[79] = 0;
	lpdrtag->sunit[15] = 0;
	strncpy(lpdrtag->sname,m_szAliasName,79); // 这里拷贝的是标签别名，而不是m_szName
	strncpy(lpdrtag->sdes,m_szDes,79);
	strncpy(lpdrtag->sunit,m_szUnit,15);

	lpdrtag->wtype = m_nType;
	lpdrtag->waccess = 0;
	lpdrtag->cr[0] = 0;
	lpdrtag->cr[1] = 0;
	lpdrtag->cr[2] = 0;
	lpdrtag->cr[3] = 0;
}

void    CTagItem::Serialize(CArchive& ar)
{
	if(ar.IsLoading())
	{
		ar >> m_szName;
		ar >> m_szAliasName;
		ar >> m_szDes;
		ar >> m_szUnit;
		ar >> m_nType;
	}
	else
	{
		ar << m_szName;
		ar << m_szAliasName;
		ar << m_szDes;
		ar << m_szUnit;
		ar << m_nType;
	}
}

//排序比较重载
int CTagItem::Compare(CSortItem *pOb)
{
	int nret = 0;
	if(pOb == NULL)
		return nret;
	CTagItem *pi = (CTagItem *)pOb;
	
	switch(m_nSortWho)
	{
	case 0:  //TagName
		nret = strcmp(m_szName,pi->m_szName);
		break;
	case 1:
		nret = strcmp(m_szAliasName,pi->m_szAliasName);
	case 2: //Des
		nret = strcmp(m_szDes,pi->m_szDes);
		break;
	case 3: //unit
		nret = strcmp(m_szUnit,pi->m_szUnit);
		break;
	case 4: //datatype
		if(m_nType > pi->m_nType)
			nret = 1;
		else if(m_nType < pi->m_nType)
			nret = -1;
		break;
	}
	if(!m_bAsc)
		nret *= -1;
	return nret;	
}


CString CTagItem::GetTypeStr(int ntype)
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

int CTagItem::GetTypeVal(LPCTSTR lpszType)
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

/////////////////////////////////////////////////////////////////
//CTagDefTab
void CTagDefTab::SetSel(int nIndex,BOOL bSel)
{
	CTagItem *pi = (CTagItem *)m_oaTag.GetAt(nIndex);
	if(pi)
		pi->m_bSel = bSel;
}
BOOL CTagDefTab::IsSelected(int nIndex)
{
	CTagItem *pi = (CTagItem *)m_oaTag.GetAt(nIndex);
	if(pi)
		return pi->m_bSel;
	return FALSE;
}

BOOL CTagDefTab::DelSelect()//删除选择的标签
{
	int i=0,n = m_oaTag.GetSize();
	CTagItem *pi;
	BOOL bDel = FALSE;
	while(i<n)
	{
		pi = (CTagItem *)m_oaTag.GetAt(i);
		if(pi->m_bSel)
		{
			m_mapTab.RemoveKey(pi->m_szName);
			m_oaTag.RemoveAt(i);
			delete pi;
			n = m_oaTag.GetSize();
			bDel = TRUE;
			continue;
		}
		i++;
	}
	return bDel;
}
CString CTagDefTab::GetText(int nIndex,int nField)
{
	CTagItem *pi = (CTagItem *)m_oaTag.GetAt(nIndex);
	if(pi == NULL)
		return CString("");
	return pi->GetText(nField);
}
BOOL		CTagDefTab::GetAt(int nIndex,CTagItem *ptag) //复制方式取
{
	CTagItem* pt = (CTagItem *)m_oaTag.GetAt(nIndex);
	if(pt == NULL)
		return FALSE;
	pt->CopyTo(ptag);
	return TRUE;
}

CTagItem*	CTagDefTab::GetTag(LPCTSTR lpszName) //返回标签对象
{
	CTagItem	*ptag;
	if(m_mapTab.Lookup(lpszName,ptag))
		return ptag; //已存在
	return NULL;
};
BOOL		CTagDefTab::AddTag(CTagItem *pTag) //添加标签定义，复制方式
{
	CTagItem	*pt;
	if(m_mapTab.Lookup(pTag->m_szName,pt))
		return FALSE; //已存在
	pt = new CTagItem;
	if(pt == NULL)
		return FALSE;
	pTag->CopyTo(pt);
	m_mapTab.SetAt(pt->m_szName,pt);
	m_oaTag.Add(pt); //存储相同指针
	return TRUE;
}
BOOL		CTagDefTab::IsExist(LPCTSTR lpszName,long *ptime)//是否存在
{
	CTagItem *pt = GetTag(lpszName);
	if(!pt)
		return FALSE;
	if(ptime)
		*ptime = pt->m_ldatatime;
	return TRUE;
}

void    CTagDefTab::Serialize(CArchive& ar)
{
	if(ar.IsLoading())
	{
		m_mapTab.RemoveAll();
		m_oaTag.DeleteObjects();

		long ltags = 0;
		ar >> ltags;

		UINT nHashSize = (UINT)(ltags * 1.20);
		if (nHashSize < 1000)
			nHashSize = 1000;
		nHashSize = ::GetHashTablePrime(nHashSize);
		m_mapTab.InitHashTable(nHashSize);

		int i;
		CTagItem tag;
		for(i=0;i<ltags;i++)
		{
			tag.Serialize(ar);
			AddTag(&tag);
		}
	}
	else
	{
		long ltags = m_oaTag.GetSize();
		ar << ltags;
		int i;
		CTagItem *pt;
		for(i=0;i<ltags;i++)
		{
			pt = (CTagItem *)m_oaTag.GetAt(i);
			pt->Serialize(ar);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
// CWinTagInfo

CWinTagInfo::CWinTagInfo()
{
	m_nRowH = 18;//行高
	m_nTxtSize = 12;
	m_bGridLineH = TRUE;

	m_nCols = 5;
	SetColW(0,160); //FIELD_NAME
	SetColW(1,160); //FIELD_NAME
	SetColW(2,200); //FIELD_DES
	SetColW(3,80); //FIELD_UNIT
	SetColW(4,80);//FIELD_TYPE

	SetCellFmt(0,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	SetCellFmt(1,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	SetCellFmt(2,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	SetCellFmt(3,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	SetCellFmt(4,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
}

CWinTagInfo::~CWinTagInfo()
{

}

CString CWinTagInfo::GetHeadText(int ncol)//取显示字符串
{
	CString szt= "";
	switch(ncol)
	{
	case 0://FIELD_NAME
		szt = "标签名/ID值";
		break;
	case 1://FIELD_ALIASNAME
		szt = "标签别名";
		break;
	case 2://FIELD_DES
		szt = "描述";
		break;
	case 3://FIELD_UNIT
		szt = "单位";
		break;
	case 4://FIELD_TYPE
		szt = "类型";
		break;
	default:
		break;
	}
	return szt;
}

void CWinTagInfo::ReDraw()
{
	SetRows(m_pdrv->m_tagtab.GetSize());
	this->Invalidate();
}

void	CWinTagInfo::GetRowColorS(int nrow,int ncol,COLORREF &clrTxt,COLORREF &clrBk)//取行颜色
{
	if(m_pdrv->m_tagtab.IsSelected(nrow))
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

CString CWinTagInfo::GetCellText(int nrow,int ncol)//取显示字符串
{
	return m_pdrv->m_tagtab.GetText(nrow,ncol);
}

void	CWinTagInfo::OnClickHeadCol(int ncol)//鼠标点击表头
{

	if(ncol == m_nSortCol)
		m_bSortAsc = !m_bSortAsc;
	m_nSortCol = ncol;
	m_pdrv->m_tagtab.m_oaTag.SetSortType(m_nSortCol,m_bSortAsc);//设置排序
	m_pdrv->m_tagtab.m_oaTag.Qsort();

	SetSortCol(m_nSortCol,m_bSortAsc);//设置表头显示
	Invalidate();
}

int	 CWinTagInfo::GetNextSelected(int nstart)//返回选择
{
	CTagItem *pi;
	int i,ns = nstart,n = m_pdrv->m_tagtab.m_oaTag.GetSize();
	if(ns < 0) ns = 0;
	for(i=ns;i<n;i++)
	{
		pi = (CTagItem	*)m_pdrv->m_tagtab.m_oaTag.GetAt(i);
		if(pi->m_bSel)
			return i;
	}
	return -1;
}

void	CWinTagInfo::OnCurSelChange(int nrow)
{
	CTagItem *pi;
	int i,n = m_pdrv->m_tagtab.m_oaTag.GetSize();
	for(i=0;i<n;i++)
	{
		pi = (CTagItem	*)m_pdrv->m_tagtab.m_oaTag.GetAt(i);
		if(i != nrow) 
			m_pdrv->m_tagtab.SetSel(i,FALSE);
		else
			m_pdrv->m_tagtab.SetSel(i,TRUE);
	}
	m_nCurSel = nrow;
	this->Invalidate();
}
void	CWinTagInfo::OnCtrlSelChange(int nrow)//CTRL按住时的选择
{
	CTagItem *pi;
	pi = (CTagItem *)m_pdrv->m_tagtab.m_oaTag.GetAt(nrow);
	if(!pi)
		return;
	pi->m_bSel = !(pi->m_bSel);
	this->Invalidate();
}

void	CWinTagInfo::OnSelChange(int nrowstart,int nrowend)
{
	CTagItem *pi;
	int i,n = m_pdrv->m_tagtab.m_oaTag.GetSize();
	for(i=0;i<n;i++)
	{
		pi = (CTagItem	*)m_pdrv->m_tagtab.m_oaTag.GetAt(i);
		if((i<nrowstart) || (i>nrowend))
			pi->m_bSel = FALSE;
		else
			pi->m_bSel = TRUE;
	}
	this->Invalidate();
}

void	CWinTagInfo::RedrawList()
{
	m_pdrv->m_tagtab.m_oaTag.Qsort();
	SetRowScrollTop(m_pdrv->m_tagtab.m_oaTag.GetSize());
}

///////////////////////////////////////////////////////////
COdbcDrvObj::COdbcDrvObj()
{
	m_ntabstyle = 0;
	m_ntimestyle = 0; 
	m_szconstr = _T("");
	m_szfname = _T("");
	m_szqa = _T("");
	m_szftime = _T("");
	m_szfval = _T("");
	m_sztabname = _T("");

	m_szres = "";
	m_nrecsize = 0;
}
COdbcDrvObj::~COdbcDrvObj()
{

}

DRVRET  COdbcDrvObj::GetErrMsg(CString& szMsg,DRVRET dwcode)			//获取错误信息
{
	if(dwcode < 100)
		return CDrvObj::GetErrMsg(szMsg,dwcode);

	//>100是驱动自定义的，这里没有自定义错误信息，直接返回无此错误代码
	return DRERR_NOERRCODE;
}

DRVRET  COdbcDrvObj::WriteVar(LPDRREC lpval)			//写变量
{
	return DRERR_OK;//本版不支持写
}

DRVRET  COdbcDrvObj::GetTagDef(int nindex,LPDRVTAG  lptag)
{
	CTagItem tag;
	if(m_tagtab.GetAt(nindex,&tag))
	{
		tag.CopyToDrTag(lptag);
		return DRERR_OK;
	}
	return DRERR_NOMORETAG;
}

BOOL	COdbcDrvObj::OnConfig()					//配置和配置界面,返回TRUE表示修改了配置
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); //资源切换
	
	CDbCfgDlg dlg;
	BOOL bmodify = FALSE;
	dlg.m_pdrv = this;
	dlg.m_objConnStr.SetConnectString(m_szconstr);
	dlg.m_dlgTableCfg.m_szTableName = m_sztabname;
	dlg.m_dlgTableCfg.m_szFieldName = m_szfname;
	dlg.m_dlgTableCfg.m_szQualityField = m_szqa;
	dlg.m_dlgTableCfg.m_szTimeField = m_szftime;
	dlg.m_dlgTableCfg.m_szValField = m_szfval;
	dlg.m_dlgTableCfg.m_nStorageType = m_ntabstyle;

	if(dlg.DoModal() != IDOK)
		return FALSE;
	CString szConnStr = dlg.m_objConnStr.GetConnectString();
	if(szConnStr != m_szconstr)
	{
		m_szconstr = szConnStr;
		bmodify = TRUE;
	}

	if ((dlg.m_dlgTableCfg.m_szTableName != m_sztabname) ||
		(dlg.m_dlgTableCfg.m_szFieldName != m_szfname) ||
		(dlg.m_dlgTableCfg.m_szQualityField != m_szqa) ||
		(dlg.m_dlgTableCfg.m_szTimeField != m_szftime) ||
		(dlg.m_dlgTableCfg.m_szValField != m_szfval) ||
		(dlg.m_dlgTableCfg.m_nStorageType != m_ntabstyle))
	{
		m_sztabname = dlg.m_dlgTableCfg.m_szTableName;
		m_szfname = dlg.m_dlgTableCfg.m_szFieldName;
		m_szqa = dlg.m_dlgTableCfg.m_szQualityField;
		m_szftime = dlg.m_dlgTableCfg.m_szTimeField;
		m_szfval = dlg.m_dlgTableCfg.m_szValField;
		m_ntabstyle = dlg.m_dlgTableCfg.m_nStorageType;
		bmodify = TRUE;
	}
	
	return bmodify || dlg.m_bModified;

	/*
	CDlgConfig dlg; //标签配置
	CDlgDbSet dbset; //全局参数配置

	BOOL bmodify = FALSE;

	dbset.m_ntabstyle = m_ntabstyle;
	dbset.m_szconstr = m_szconstr;
	dbset.m_szfname = m_szfname;
	dbset.m_szqa = m_szqa;
	dbset.m_szftime = m_szftime;
	dbset.m_szfval = m_szfval;
	dbset.m_sztabname = m_sztabname;
lpredo:
	dbset.m_bNext = FALSE;
	if(dbset.DoModal() != IDOK)
		return bmodify;
	
	bmodify = TRUE;
	m_ntabstyle = dbset.m_ntabstyle;
	m_szconstr = dbset.m_szconstr;
	m_szfname = dbset.m_szfname;
	m_szqa = dbset.m_szqa;
	m_szftime = dbset.m_szftime;
	m_szfval = dbset.m_szfval;
	m_sztabname = dbset.m_sztabname;

	if(!dbset.m_bNext)
		return bmodify;

	dlg.m_wintag.m_pdrv = this;
	dlg.m_bUp = FALSE;
	dlg.DoModal();
	if(dlg.m_bModified)
		bmodify = TRUE;
	if(dlg.m_bUp)
		goto lpredo;
	
	return bmodify;
	*/
}

void	COdbcDrvObj::Serialize(CArchive& ar)	//串行化保存，读取配置
{
	if(ar.IsLoading())
	{/*
		long lflag = 0;
		ar >> lflag;
		if(lflag != 71022808)
			return;*/
			
		// 增加版本号信息
		DWORD dwDrvFlag;
		ar >> dwDrvFlag;
		if (dwDrvFlag != DRVCFG_FLAG_ODBC) // 是否为OPC驱动配置
		return;
		WORD wVersion;
		ar >> wVersion;
		if (wVersion != DRVCFG_VERSION)
			return;
		
		//存入全局配置参数
		ar >> m_ntabstyle;
		ar >> m_ntimestyle;
		ar >> m_szconstr;
		ar >> m_szfname;
		ar >> m_szqa;
		ar >> m_szftime;
		ar >> m_szfval;
		ar >> m_sztabname;

		ar >> m_szres;
		m_tagtab.Serialize(ar);
	}
	else
	{
		// 增加版本号信息
		DWORD dwDrvFlag = DRVCFG_FLAG_ODBC;
		ar << dwDrvFlag;
		WORD wVersion = DRVCFG_VERSION;
		ar << wVersion;

		ar << m_ntabstyle;
		ar << m_ntimestyle;
		ar << m_szconstr;
		ar << m_szfname;
		ar << m_szqa;
		ar << m_szftime;
		ar << m_szfval;
		ar << m_sztabname;

		ar << m_szres;
		m_tagtab.Serialize(ar);
	}
}

DRVRET	COdbcDrvObj::OnStart()					//准备运行
{
	m_tagtab.Initdatatime();
	return DRERR_OK;
}

void	COdbcDrvObj::OnStop()					//停止运行后的清理工作
{
	
}

CString COdbcDrvObj::GetFieldList(LPCTSTR lpszTableName/* = NULL*/)
{
	CString szFieldList;
	
	if (NULL == lpszTableName || *lpszTableName == '\0')
		szFieldList = m_szfname;
	else {
		szFieldList = lpszTableName;
		szFieldList += _T(".") + m_szfname;
	}

	if (!m_szftime.IsEmpty()) {
		if (NULL == lpszTableName || *lpszTableName == '\0')
			szFieldList += _T(",") + m_szftime;
		else {
			szFieldList += _T(",");
			szFieldList += lpszTableName;
			szFieldList += _T(".") + m_szftime;
		}
	}
	if (!m_szqa.IsEmpty())
	{
		if (NULL == lpszTableName || *lpszTableName == '\0')
			szFieldList += _T(",") + m_szqa;
		else {
			szFieldList += _T(",");
			szFieldList += lpszTableName;
			szFieldList += _T(".") + m_szqa;
		}
	}
	
	if (NULL == lpszTableName || *lpszTableName == '\0')
		szFieldList += _T(",") + m_szfval;
	else {
		szFieldList += _T(",");
		szFieldList += lpszTableName;
		szFieldList += _T(".") + m_szfval;
	}

	return szFieldList;
}

UINT	COdbcDrvObj::DoRun()					//正真的运行过程，被RunThread中调用
{
	m_bRun = TRUE; //不能少这一句
	
	int ntags = m_tagtab.GetSize();//默认标签个数，用于历史表取数，后面用SQL语句查询具体个数
	CString szSQL;

	CConnStringParser parser;
	parser.SetConnectString(m_szconstr);
	int nDbType = parser.GetDBType();

	long ltimep = 0,ltime = (long)::time(NULL);

	CTimeCount timer;
	int nerr = 0;
	int nolderr = 0;
	int nRecord;
	CString szErr="";
	while(!m_bKillThread)
	{
		//每秒读一次数据
		if(ltime == ltimep)
		{
			Sleep(10);
			ltime = (long)::time(NULL);
			continue;
		}
		ltimep = ltime;

		timer.start();
		//连接数据库
		if(!m_db.IsOpen())
		{
			if(!Connectdb(szErr))
			{
				nerr = 1;
				if(nolderr != nerr)
				{
					AddError(szErr);
					nolderr = nerr;
				}
				continue;
			}
			nerr = 0;
			nolderr = nerr;

			/*
			if(m_ntabstyle != TAB_REAL)
			{
				//读一次标签数
				CRecordset rs(&m_db);
				CString szt;
				szt.Format("select count(*) as tagnum from (select %s from %s group by %s) cttab",
					m_szfname,m_sztabname,m_szfname);
				try
				{
					if(!rs.Open(AFX_DB_USE_DEFAULT_TYPE,szt,CRecordset::readOnly))
						continue;
					
					if(!rs.IsEOF())
					{
						rs.GetFieldValue("tagnum",szt);
						ntags = atol(szt);
					}
				}
				catch (CDBException *e)
				{
					if(IsNeedReConnect(e->m_strStateNativeOrigin))
					{
						m_db.Close();
						continue;
					}
					szErr="ODBC错误:";
					szErr+=e->m_strError;
					e->Delete();
					TRACE("%s\n",szErr);

					nerr = 3;
					if(nolderr != nerr)
					{
						OnLog(szErr);
						nolderr = nerr;
					}
				}
			}
			*/
		}

		//打开记录集,读取数据
		{
			CString szval;
			CRecordset rs(&m_db);
			CDBVariant varname,vartime,varqa,varval;

			if(m_ntabstyle == TAB_REAL) //实时表
				szSQL.Format("select %s from %s", GetFieldList(), m_sztabname);
			else {//历史表
				switch(nDbType)
				{
				case DBT_MYSQL:
					szSQL.Format(_T("select %s from (select * from %s order by %s desc) cttab group by %s"),
						GetFieldList(),m_sztabname,m_szftime, m_szfname);
					break;
				case DBT_MSSQL:
					szSQL.Format(_T("select %s from %s a, (select %s, max(%s) as m_ltime from %s group by %s) b where a.%s = b.%s and a.%s = b.m_ltime order by a.%s"), 
						GetFieldList(_T("a")), m_sztabname, 
						m_szfname, m_szftime, m_sztabname, m_szfname, 
						m_szfname, m_szfname, m_szftime, m_szfname);
					break;
				case DBT_ORACLE:
					/* 支持mssql和oracle
					select * from (select id,gid,col1,col2, ROW_NUMBER( ) over(partition by gid order by col2 DESC) as bz from ZW1..t2v group by id,gid,col1,col2) a  
where bz <=3
					*/
				case DBT_SYBASE11:
				case DBT_SYBASE12:
				case DBT_DB2:
					//szSQL.Format("select top %d %s,%s,%s,%s from %s order by %s desc",
					//	ntags,m_szfname,m_szftime,m_szqa,m_szfval,m_sztabname,m_szftime);
					szSQL.Format(_T("select %s from %s a, (select %s, max(%s) as m_ltime from %s group by %s) b where a.%s = b.%s and a.%s = b.m_ltime order by a.%s"), 
						GetFieldList(_T("a")), m_sztabname, 
						m_szfname, m_szftime, m_sztabname, m_szfname, 
						m_szfname, m_szfname, m_szftime, m_szfname);
					break;
				}
			}

			try
			{
				szSQL.MakeUpper(); // Oracle需要大写
				if(!rs.Open(CRecordset::snapshot,szSQL,CRecordset::readOnly))
					continue;
				
				CString szfname = m_szfname;
				szfname.MakeUpper();
				CString szftime = m_szftime;
				szftime.MakeUpper();
				CString szqa = m_szqa;
				szqa.MakeUpper();
				CString szval = m_szfval;
				szval.MakeUpper();

				nRecord = 0;
				while((!rs.IsEOF()) && (!m_bKillThread))
				{
					rs.GetFieldValue(szfname, varname);//名称
					if (szftime.IsEmpty())
					{
						vartime.Clear();
						vartime.m_dwType = DBVT_LONG;
						vartime.m_lVal = (long)::time(NULL);
					}
					else {
						rs.GetFieldValue(szftime, vartime);//时标
					}
					if (szqa.IsEmpty())
					{
						varqa.Clear();
						varqa.m_dwType = DBVT_LONG;
						varqa.m_lVal = 0;
					}
					else
						rs.GetFieldValue(szqa, varqa);//质量
					rs.GetFieldValue(szval, varval);  //值

					OnDataOk(varname,vartime,varqa,varval);
					nRecord++;
					rs.MoveNext(); //移到下一个记录
				}
				rs.Close();
				OnUploadData();
			}
			catch (CDBException *e)
			{
				if(IsNeedReConnect(e->m_strStateNativeOrigin))
					m_db.Close();

				szErr="ODBC错误:";
				szErr+=e->m_strError;
				e->Delete();
				TRACE("%s\n",szErr);

				nerr = 2;
				if(nolderr != nerr)
				{
					AddError(szErr);
					nolderr = nerr;
				}
			}
		}
		timer.end();
		OutputDebug(_T("从ODBC抽取 %d 条记录，耗时 %f ms"), nRecord, timer.time_milli());
		ltime = (long)::time(NULL);
	}
	if(m_db.IsOpen())
		m_db.Close(); //关闭数据库

	m_bRun = FALSE; //不能少这一句

	return DRERR_OK;
}

BOOL COdbcDrvObj::Connectdb(CString &szerr)//连接数据库
{
	if(m_db.IsOpen())
		m_db.Close();
	m_db.SetLoginTimeout(8); // 登录秒超时，默认15s
	//m_db.SetQueryTimeout(20); // 查询秒超时，默认15s
	BOOL bret = FALSE;
	try
	{
		bret = m_db.OpenEx(m_szconstr,CDatabase::noOdbcDialog); 
	}
	catch (CDBException *e)
	{
		CString szt="ODBC连接错误:";
		szt+=e->m_strError;
		e->Delete();
		TRACE("%s\n",szt);
		szerr = szt;
		return FALSE;
	}
	return bret;
}

BOOL COdbcDrvObj::IsNeedReConnect(CString &szstatus)//是否需要重新连接
{
	//取index为6-10
	CString szval = szstatus.Left(11);
	szval = szval.Right(5);
	/*
	08001	无法连接到数据源,一般在Open DB时的错误信息
	08003	连接未打开
	08007	在执行事务的过程中连接失败
	08S01	通讯链接失败，一般在打开记录集或调用直接执行SQL语句时出错。
	*/
	if((szval == "08S01")||
		(szval == "08001")||
		(szval == "08003")||
		(szval == "08007")
		)
		return TRUE;
	return FALSE;
}

void			COdbcDrvObj::OnDataOk(CDBVariant &varname,CDBVariant &vartime,CDBVariant &varqa,CDBVariant &varval)
{
	//解析标签名
	CString sztag = "";
	if(varname.m_dwType == DBVT_LONG)
		sztag.Format("%d",varname.m_lVal);
	else if(varname.m_dwType == DBVT_SHORT)
		sztag.Format("%d",varname.m_iVal);
	else if(varname.m_dwType == DBVT_STRING)
	{
		sztag = *(varname.m_pstring);
		sztag.TrimRight();
	}
	else
		return; //不能解析标签名

	CTagItem *pItem = m_tagtab.GetTag(sztag);
	if (NULL == pItem)
		return;

	// 将标签名转换成标签别名
	sztag = pItem->m_szAliasName;

	//解析时间
	long ltime = 0;
	if(vartime.m_dwType == DBVT_LONG)
	{
		ltime = vartime.m_lVal;
	}
	else if(vartime.m_dwType == DBVT_DATE)
	{
		TIMESTAMP_STRUCT *pt;
		pt = vartime.m_pdate;
		CTime ct(pt->year,pt->month,pt->day,pt->hour,pt->minute,pt->second);
		ltime = (long)ct.GetTime();
		if(ltime <= 0)
			return;//时间错误
	}
	else if(vartime.m_dwType == DBVT_STRING)
	{
		CString sztime = *(vartime.m_pstring);
		sztime.TrimRight();
		
		BOOL bIsLong = TRUE;
		// 如果为Oracle数据库，则可能会将整形的时间值读成字符串类型的
		for(int i = 0; i < sztime.GetLength(); i++)
		{
			if (!isdigit(sztime.GetAt(i)))
			{
				bIsLong = FALSE;
				break;
			}
		}
		if (bIsLong)
		{
			ltime = _ttol(sztime);
		}
		else {
			COleDateTime olet;
			olet.ParseDateTime(sztime);
			if(olet.m_status != 0)
				return ;//时间错误
			CTime ct(olet.GetYear(),olet.GetMonth(),olet.GetDay(),olet.GetHour(),olet.GetMinute(),olet.GetSecond());
			ltime = (long)ct.GetTime();
		}
		if(ltime <= 0)
			return;//时间错误
	}
	//解析质量
	short nqa = 0;
	if(varqa.m_dwType == DBVT_LONG)
		nqa = (short)varqa.m_lVal;
	else if(varqa.m_dwType == DBVT_SHORT)
		nqa = varqa.m_iVal;
	else if(varqa.m_dwType == DBVT_BOOL)
	{
		if(varqa.m_boolVal)
			nqa = 0;
		else
			nqa = 1;
	}
	else if(varqa.m_dwType == DBVT_STRING)
	{
		CString sznqa = *(varqa.m_pstring);
		sznqa.TrimRight();
		nqa = _ttoi(sznqa);
	}
	else 
		return; //不能解析质量

	CVarVal val;
	//解析值
	long lval = 0;
	double fval = 0;
	CString szval = "";
	int nvaltype = -1;
	if(varval.m_dwType == DBVT_LONG)
		val = (long)varval.m_lVal;
	
	else if(varval.m_dwType == DBVT_SHORT)
		val = (long)varval.m_iVal;

	else if(varval.m_dwType == DBVT_SINGLE)
		 val = (float)varval.m_fltVal;

	else if(varval.m_dwType == DBVT_DOUBLE)
		val = varval.m_dblVal;

	else if(varval.m_dwType == DBVT_STRING)
	{
		szval = *(varval.m_pstring);
		val = szval;
	}
	else
		return;

	//查找标签是否定义
	//long llasttime = 0;
	//if(!m_tagtab.IsExist(sztag,&llasttime))
	//	return;
	if(pItem->m_ldatatime >= ltime)
		return;
	pItem->m_ldatatime = ltime;
	//提交数据
	val.ltime=ltime;
	val.snqa=nqa;
	val.snerr=0;
	m_recs[m_nrecsize].SetName(sztag);
	m_recs[m_nrecsize].SetVal(&val);
	m_nrecsize++;

	if(m_nrecsize >= UPDATEVALTAGS)
		OnUploadData();

}
void			COdbcDrvObj::OnUploadData()
{
	if(m_nrecsize == 0)
		return;
	if(m_cbOnDataOk)
		m_cbOnDataOk(m_handle,m_recs,m_nrecsize );
	m_nrecsize = 0;
}

