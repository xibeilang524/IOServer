//SimdrvExobj.cpp
#include "stdafx.h"
#include <string.h>
#include "SimdrvEx.h"
#include "SimdrvExobj.h"
#include <math.h>
#include <stdio.h>

//辅助对象
CString  CTagItem::GetText(int nField)//取数据字符串
{
	if(nField == FIELD_NAME)
		return m_szName;
	else if(nField == FIELD_DES)
		return m_szDes;
	else if(nField == FIELD_UNIT)
		return m_szUnit;
	else if(nField == FIELD_TYPE)
		return GetTypeStr(m_nType);
	else if(nField == FIELD_SIMTYPE)
		return GetSimTypeStr(m_nSimtype);
	else if(nField == FIELD_BASEVAL)
		return m_szBaseVal;
	else if(nField == FIELD_PERCENT)
	{
		CString szt;
		szt.Format("%.1f",m_fPercent);
		return szt;
	}
	return CString("");
}

void CTagItem::CopyTo(CTagItem *pt)//复制
{
	pt->m_szName = m_szName;
	pt->m_szDes = m_szDes;
	pt->m_szUnit = m_szUnit;
	pt->m_nType = m_nType;
	pt->m_nSimtype = m_nSimtype;
	pt->m_szBaseVal = m_szBaseVal;
	pt->m_fPercent = m_fPercent;
}

void	CTagItem::CopyToDrTag(LPDRVTAG lpdrtag)//复制到接口格式标签定义
{
	lpdrtag->sname[79]=  0;
	lpdrtag->sdes[79] = 0;
	lpdrtag->sunit[15] = 0;
	strncpy(lpdrtag->sname,m_szName,79);
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
		ar >> m_szDes;
		ar >> m_szUnit;
		ar >> m_nType;
		ar >> m_nSimtype;
		ar >> m_szBaseVal;
		ar >> m_fPercent;
	}
	else
	{
		ar << m_szName;
		ar << m_szDes;
		ar << m_szUnit;
		ar << m_nType;
		ar << m_nSimtype;
		ar << m_szBaseVal;
		ar << m_fPercent;
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
	case 1: //Des
		nret = strcmp(m_szDes,pi->m_szDes);
		break;
	case 2: //unit
		nret = strcmp(m_szUnit,pi->m_szUnit);
		break;
	case 3: //datatype
		if(m_nType > pi->m_nType)
			nret = 1;
		else if(m_nType < pi->m_nType)
			nret = -1;
		break;
	case 4: // simtype
		if(m_nSimtype > pi->m_nSimtype)
			nret = 1;
		else if(m_nSimtype < pi->m_nSimtype)
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

CString CTagItem::GetSimTypeStr(int ntype)
{
	if(ntype == SIMTYPE_LINE)
		return CString("line");
	else if(ntype == SIMTYPE_SIN)
		return CString("sin");
	else if(ntype == SIMTYPE_RAND)
		return CString("rand");//wzq
	else
		return CString("line");
}

int CTagItem::GetSimTypeVal(LPCTSTR lpszType)
{
	if(!_stricmp("line",lpszType))
		return SIMTYPE_LINE;
	else if(!_stricmp("sin",lpszType))
		return SIMTYPE_SIN;
	else if(!_stricmp("rand",lpszType))
		return SIMTYPE_RAND; // wzq
	else
		return SIMTYPE_LINE;
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

BOOL CTagDefTab::UpdateTag(CTagItem *pTag) //更新标签，如果未存在，则添加
{
	CTagItem	*pt;
	if(m_mapTab.Lookup(pTag->m_szName,pt))
	{
		pTag->CopyTo(pt);
		return TRUE;
	}
	pt = new CTagItem;
	if(pt == NULL)
		return FALSE;
	pTag->CopyTo(pt);
	m_mapTab.SetAt(pt->m_szName,pt);
	m_oaTag.Add(pt); //存储相同指针
	return TRUE;
}

BOOL		CTagDefTab::IsExist(LPCTSTR lpszName)//是否存在
{
	return (GetTag(lpszName) != NULL);
}

void    CTagDefTab::Serialize(CArchive& ar)
{
	if(ar.IsLoading())
	{
		m_mapTab.RemoveAll();
		m_oaTag.DeleteObjects();

		long ltags = 0;
		ar >> ltags;
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
/////////////////////////////////////////////////////////////////
//CTagVal
CTagVal::CTagVal()
{

}

CTagVal::CTagVal(CTagItem *pTag)
{
	m_val.ReSetType(pTag->m_nType);
	m_val.SetName(pTag->m_szName);

	m_val.val.snqa = DBP_QUALITY_BAD;
	m_nSimtype = pTag->m_nSimtype;
	m_fPercent = pTag->m_fPercent;
	if((m_val.val.vt == DRT_FLOAT32) || (m_val.val.vt == DRT_FLOAT64))
		bv.fval = (float)atof(pTag->m_szBaseVal);
	else
		bv.lval = atol(pTag->m_szBaseVal);

}
CTagVal::~CTagVal()
{

}

void CTagVal::Simulate(long ltime,CRdRec* lprv)//模拟，结果复制到lprv中
{
	m_val.val.ltime = ltime;
	m_val.val.snqa = DBP_QUALITY_GOOD; //先置无效
	if(m_val.val.vt == DRT_DIGITAL)
	{
		if (m_fPercent == 0)
			m_val.val.lval = bv.lval;
		else {
			srand(::GetTickCount());
			m_val.val.lval = rand() % 2;
		}
		/*
		if((ltime % 240) > 120)
			m_val.val.lval = 1;
		else
			m_val.val.lval = 0;*/
	}
	else
	{
		if(m_nSimtype == SIMTYPE_LINE) //锯齿波
		{
			double dblxs=0;
			long lzq  = ltime/60;
			long lyu = ltime%60;

			if((lzq % 4) == 0)
				dblxs = lyu;
			else if((lzq % 4) == 1)
				dblxs = 60 - lyu;
			else if((lzq % 4) == 2)
				dblxs = -lyu;
			else
				dblxs = lyu - 60;
			dblxs = (m_fPercent / 100.0) * dblxs / 60.0;

			if(m_val.val.vt == DRT_INT32)
				m_val.val.lval = bv.lval + (long)(bv.lval *  dblxs);
			
			else if(m_val.val.vt == DRT_INT64)
				m_val.val.llval = bv.lval + (__int64)(bv.lval *  dblxs);

			else if(m_val.val.vt == DRT_FLOAT32)
				m_val.val.fval = bv.fval + (float)(bv.fval * dblxs );

			else if(m_val.val.vt == DRT_FLOAT64)
				m_val.val.dblval = bv.fval + (bv.fval * dblxs) ;
			else  //其他类型暂无
			{
				GetVarVal()->ClearData(); 
				m_val.val.ltime = 0;
				m_val.val.snqa = DBP_QUALITY_BAD;
			}
		}
		else if(m_nSimtype == SIMTYPE_SIN) //正玄波
		{
			double dblxs =  ( 6.2831852 * (ltime % 300)) / 300;
			dblxs = sin(dblxs) * (m_fPercent/100.0);

			if(m_val.val.vt == DRT_INT32)
				m_val.val.lval = bv.lval + (long)(bv.lval *  dblxs );
			
			else if(m_val.val.vt == DRT_INT64)
				m_val.val.llval = bv.lval + (__int64)(bv.lval *  dblxs);

			else if(m_val.val.vt == DRT_FLOAT32)
				m_val.val.fval = bv.fval + (float)(bv.fval *  dblxs );

			else if(m_val.val.vt == DRT_FLOAT64)
				m_val.val.dblval = bv.fval + (bv.fval *   dblxs ) ;
			else 
			{
				GetVarVal()->ClearData(); 
				m_val.val.ltime = 0;
				m_val.val.snqa = DBP_QUALITY_BAD;
			}
		}
		else if(m_nSimtype == SIMTYPE_RAND) //随机 wzq
		{
			double dblxs=0;
			srand(::GetTickCount());
			//int nx = (int)m_fPercent;
			dblxs= rand()/(double)RAND_MAX *  m_fPercent;//rand() % nx + rand()/(double)RAND_MAX;
			dblxs /= 100;

			if(m_val.val.vt == DRT_INT32)
				m_val.val.lval = bv.lval + (long)(bv.lval *  dblxs);
			
			else if(m_val.val.vt == DRT_INT64)
				m_val.val.llval = bv.lval + (__int64)(bv.lval *  dblxs);
			
			else if(m_val.val.vt == DRT_FLOAT32)
				m_val.val.fval = bv.fval + (float)(bv.fval * dblxs );
			
			else if(m_val.val.vt == DRT_FLOAT64)
				m_val.val.dblval = bv.fval + (bv.fval * dblxs) ;
			else  //其他类型暂无
			{
				GetVarVal()->ClearData(); 
				m_val.val.ltime = 0;
				m_val.val.snqa = DBP_QUALITY_BAD;
			}
		}
		else 
		{
			m_val.val.ltime = 0;
			m_val.val.snqa = DBP_QUALITY_BAD;
		}
	}
	lprv->SetName(m_val.sname);
	lprv->SetVoid();// 2011.3.7 debug,修复总是变成整数的错误
	lprv->SetVal(&m_val.val);//赋值
//	*((CVarVal*)&lprv->val) = *GetVarVal();
}

void CRunTags::Init(CTagDefTab *pTagDefTag) //初始化
{
	m_oaItems.DeleteObjects();
	int i,n = pTagDefTag -> m_oaTag.GetSize();
	CTagItem *pt;
	CTagVal* pv;
	for(i=0;i<n;i++)
	{
		pt = (CTagItem *)pTagDefTag -> m_oaTag.GetAt(i);
		if(!pt)
			continue;
		pv = new CTagVal(pt);
		if(pv)
		{
			strncpy(pv->m_val.sname,pt->m_szName,79);
			pv->m_nSimtype = pt->m_nSimtype;
			pv->m_fPercent = pt->m_fPercent;
			if((pt->m_nType == DRT_FLOAT32) || (pt->m_nType == DRT_FLOAT64))
				pv->bv.fval = (float)atof(pt->m_szBaseVal);
			else
				pv->bv.lval = atol(pt->m_szBaseVal);
			pv->m_val.val.vt = pt->m_nType;

			m_oaItems.Add(pv);
		}
	}
}

void CRunTags::Simulate(long ltime,CRdRec rec[],int nsize,DRCB_OnDataOk FunOnDataOk,DRHANDLE h) //模拟
{
	int i,npos = 0,n = m_oaItems.GetSize();
	CTagVal *pv = NULL;

	for(i=0;i<n;i++)
	{
		pv = (CTagVal * )m_oaItems.GetAt(i);
		if(pv)
		{
			pv->Simulate(ltime,&rec[npos]);
			npos++;
			if(npos == nsize)
			{
				FunOnDataOk(h,rec,nsize);
				npos = 0;
			}
		}
	}
	if(npos > 0)
		FunOnDataOk(h,rec,npos);
}

/////////////////////////////////////////////////////////////////
//实现静态对象类工厂
CDrvObj*	CDrvFactory::NewDrv()
{
	return new CSimDrvObj();
}

DRVRET CDrvFactory::GetDriverInfo(LPDRVINFO lpDrvInfo)
{
	memset(lpDrvInfo,0,sizeof(T_DRVINFO));
	strcpy(lpDrvInfo->sname,"Simulate Driver for IOServer");
	strcpy(lpDrvInfo->sdes,"CopyRight By 服务科技");
	strcpy(lpDrvInfo->sver,"0.1.0.0"); //程序版本
	lpDrvInfo->dwver = 0x0200;//规范版本
	lpDrvInfo->dwres = 0;
	return DRERR_OK;
}

/////////////////////////////////////////////////////////////////
//CSimDrvObj
CSimDrvObj::CSimDrvObj()
{

}
CSimDrvObj::~CSimDrvObj()
{

}


DRVRET  CSimDrvObj::GetErrMsg(CString& szMsg,DRVRET dwcode)			//获取错误信息
{
	if(dwcode < 100)
		return CDrvObj::GetErrMsg(szMsg,dwcode);

	//>100是驱动自定义的，这里没有自定义错误信息，直接返回无此错误代码
	return DRERR_NOERRCODE;
}

DRVRET  CSimDrvObj::WriteVar(LPDRREC lpval)			//写变量
{
	return DRERR_OK;//本版不支持写
}

DRVRET  CSimDrvObj::GetTagDef(int nindex,LPDRVTAG  lptag)
{
	CTagItem tag;
	if(m_tagtab.GetAt(nindex,&tag))
	{
		tag.CopyToDrTag(lptag);
		return DRERR_OK;
	}
	return DRERR_NOMORETAG;
}
				
#include "DlgConfig.h"
BOOL	CSimDrvObj::OnConfig()					//配置和配置界面,返回TRUE表示修改了配置
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); //资源切换

	CDlgConfig dlg;
	dlg.m_wintag.m_pdrv = this;
	dlg.m_pcfg = &(m_simcfg);
	dlg.DoModal();
	if(dlg.m_bModified)
		return TRUE; //修改了
	return FALSE;
}

void	CSimDrvObj::Serialize(CArchive& ar)	//串行化保存，读取配置
{
	if(ar.IsLoading())
	{
		long lflag = 0;
		ar >> lflag;
		if((lflag != 71022801) && (lflag != 71022802))
			return;
		m_tagtab.Serialize(ar);
		if (lflag > 71022801)
			m_simcfg.Serialize(ar, lflag);
	}
	else
	{
		long lflag = 71022802;
		ar << lflag;
		m_tagtab.Serialize(ar);
		m_simcfg.Serialize(ar, lflag);
	}
}

DRVRET	CSimDrvObj::OnStart()					//准备运行
{
	m_runtags.Init(&m_tagtab);
	return DRERR_OK;
}

void	CSimDrvObj::OnStop()					//停止运行后的清理工作
{
	m_runtags.Clear();
}

UINT	CSimDrvObj::DoRun()					//正真的运行过程，被RunThread中调用
{
	m_bRun = TRUE; //不能少这一句
	CSoe timesoe;
	SYSTEMTIME syst;
	CTime   StartTime(m_simcfg.m_starttime.GetYear(), 
		              m_simcfg.m_starttime.GetMonth(), 
					  m_simcfg.m_starttime.GetDay(), 
					  m_simcfg.m_starttime.GetHour(),
					  m_simcfg.m_starttime.GetMinute(), 
					  m_simcfg.m_starttime.GetSecond());

	CTime   EndTime(m_simcfg.m_endtime.GetYear(), 
					m_simcfg.m_endtime.GetMonth(), 
					m_simcfg.m_endtime.GetDay(), 
					m_simcfg.m_endtime.GetHour(),
					m_simcfg.m_endtime.GetMinute(), 
					m_simcfg.m_endtime.GetSecond());

	long stime = (long)StartTime.GetTime() , etime = (long)EndTime.GetTime();
	bool binfinite = stime > etime ? false : true;
	long ltimep = 0,ltime;
	if (m_simcfg.m_bAllowAcce)
	{
		ltime = stime; 
	}
	else {
		ltime = (long)::time(NULL);
	}
	while(!m_bKillThread)
	{
		if (m_simcfg.m_bAllowAcce)
		{
			ltime  += m_simcfg.m_timeincr ;
			if(binfinite)
				if( ltime > etime)
					break;
		}
		else 
		{
			//每秒产生一次数据
			if(ltime == ltimep)
				//if(ltime < ltimep + 10) // 10s一次
			{
				if (m_bKillThread)
					break;

				Sleep(10);
				ltime = (long)::time(NULL);
				continue;
			}
			ltimep = ltime;
		}
		//if(ltime == ltimep)//每秒产生一次数据 //if(ltime < ltimep + 10) // 10s一次
		//{
		//	Sleep(100);
		//	ltime = (long)::time(NULL);
		//	continue;
		//}
		//ltimep = ltime;
		if(m_cbOnDataOk)
			m_runtags.Simulate(ltime,m_recs,UPDATEVALTAGS,m_cbOnDataOk,m_handle);
		if(((ltime % 10) == 0)&&(m_cbOnSoeOk != NULL)) //模拟SOE事件,每10分钟产生一次
		{
			::GetLocalTime(&syst);

			timesoe.m_ltime = ltime;
			timesoe.m_lmsec = syst.wMilliseconds;
			//timesoe.m_ssrc 不填写，由DDC填写驱动实例名
			sprintf(timesoe.m_sdec,"simudrv");
			sprintf(timesoe.m_sdes,"设备报时事件!");
			sprintf(timesoe.m_stxtargs,"%04d-%02d-%02d %02d:%02d:%02d.%03d",syst.wYear,syst.wMonth,syst.wDay,
				syst.wHour,syst.wMinute,syst.wSecond,syst.wMilliseconds);
			timesoe.m_lclsid = 0;
			//timesoe.m_lbinarglen = 0; 如果没有二进制参数，则需要本行
			timesoe.SetBinArgs(&syst,sizeof(SYSTEMTIME));//二进制参数为SYSTEMTIME结构

			m_cbOnSoeOk(m_handle,&timesoe); //回调函数
		}
		if (m_simcfg.m_bAllowAcce)
			Sleep(m_simcfg.m_span);
	}
	m_bRun = FALSE; //不能少这一句

	return DRERR_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////
// CWinTagInfo

CWinTagInfo::CWinTagInfo()
{
	m_nRowH = 18;//行高
	m_nTxtSize = 12;
	m_bGridLineH = TRUE;

	m_nCols = 7;
	SetColW(0,160); //FIELD_NAME
	SetColW(1,200); //FIELD_DES
	SetColW(2,80); //FIELD_UNIT
	SetColW(3,80);//FIELD_TYPE
	SetColW(4,60); //FIELD_SIMTYPE
	SetColW(5,80); //FIELD_BASEVAL
	SetColW(6,80); //FIELD_PERCENT


	SetCellFmt(0,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	SetCellFmt(1,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	SetCellFmt(2,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	SetCellFmt(3,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	SetCellFmt(4,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	SetCellFmt(5,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	SetCellFmt(6,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);

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
		szt = "标签名";
		break;
	case 1://FIELD_DES
		szt = "描述";
		break;
	case 2://FIELD_UNIT
		szt = "单位";
		break;
	case 3://FIELD_TYPE
		szt = "类型";
		break;
	case 4://FIELD_SIMTYPE
		szt = "模拟方式";
		break;
	case 5://FIELD_BASEVAL
		szt = "基准值";
		break;
	case 6://FIELD_PERCENT
		szt = "幅度(%)";
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
	if(ncol > FIELD_SIMTYPE)
		return;
	
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
