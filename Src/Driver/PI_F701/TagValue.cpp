// TagValue.cpp: implementation of the CTagValue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "pidbdrv.h"
#include "TagValue.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


////////////////////////////////////////////////////////////////////////

CTvObj::CTvObj()
{
	m_szname="";//标签名
	m_lid = 0;	//标签ID
	m_bsave = FALSE; //是否存历史
	m_ntype = DT_VOID; //数据类型
	m_ncomptype = TA_COMPNO;//压缩类型
	m_fcompval = 0;//压缩精度

	m_bRealUpdate = FALSE;  //实时更新标志
	m_bHisUpdate = FALSE;	//历史更新标志
}
CTvObj::~CTvObj()
{

}

BOOL	CTvObj::GetRealVal(COdbcTagVal* pval)//取存盘实时值
{
	if(!m_bRealUpdate)
		return FALSE;
	pval->m_lid = m_lid;
	pval->m_lres = 0;
	CVarVal* pv = (CVarVal*)&pval->m_val;
	pv->SetVoid();
	*pv = m_valreal;
	
	m_bRealUpdate = FALSE;

	return TRUE;
}

BOOL	CTvObj::GetHisVal(COdbcTagVal* pval)//取存盘历史值
{
	if((!m_bHisUpdate)||(m_bsave == FALSE))
		return FALSE;
	pval->m_lid = m_lid;
	pval->m_lres = 0;
	CVarVal* pv = (CVarVal*)&pval->m_val;
	pv->SetVoid();
	*pv = m_valhis;
	
	m_bHisUpdate = FALSE;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////
CTv_digital::CTv_digital()
{
	m_ntype = DT_DIGITAL;
	m_valreal.ResetType(m_ntype);
	m_valhis.ResetType(m_ntype);
}

CTv_digital::~CTv_digital()
{

}

void CTv_digital::AddReal(LPVVAL pval)//添加实时值,该压缩的要压缩
{
	pval->snerr = ERRTV_OK;
	CVarVal *pv = (CVarVal *)pval;
	if(m_valreal.ltime == 0)
	{
		m_valreal = *pv;
		m_bRealUpdate = TRUE;
		if(m_bsave)//要求存盘
		{
			m_valhis = m_valreal;
			m_bHisUpdate = TRUE;
		}
		return ;
	}

	if(m_valreal.ltime >= pval->ltime)
	{
		pval->snerr = ERRTV_TIME;
		return;
	}
	
	if((pval->snqa == m_valreal.snqa) && (pv->GetInt() == m_valreal.GetInt())) //值和质量相同
	{
		//更改时间
		m_valreal.ltime = pval->ltime;
		m_bRealUpdate = TRUE;
		if(m_bsave)
		{
			if((m_valreal.ltime - m_valhis.ltime) > 600) //10分钟存盘一次
			{
				m_valhis = m_valreal;
				m_bHisUpdate = TRUE;
			}
		}
		return;
	}
	//不同
	m_valreal = *pv;//复制
	m_bRealUpdate = TRUE;
	if(m_bsave)//要求存盘
	{
		m_valhis = m_valreal;
		m_bHisUpdate = TRUE;
	}
}

////////////////////////////////////////////////////////////////////////
CTv_int32::CTv_int32()
{
	m_ntype = DT_INT32;
	m_valreal.ResetType(m_ntype);
	m_valhis.ResetType(m_ntype);
}

CTv_int32::~CTv_int32()
{

}

void CTv_int32::AddReal(LPVVAL pval)//添加实时值,该压缩的要压缩
{
	pval->snerr = ERRTV_OK;

	CVarVal *pv = (CVarVal *)pval;
	if(m_valreal.ltime == 0)
	{
		m_valreal = *pv;
		m_bRealUpdate = TRUE;
		if(m_bsave)//要求存盘
		{
			m_valhis = m_valreal;
			m_bHisUpdate = TRUE;
		}
		return ;
	}

	if(m_valreal.ltime >= pval->ltime)
	{
		pval->snerr = ERRTV_TIME;
		return;
	}

	if((pval->snqa == m_valreal.snqa) && (pv->GetInt() == m_valreal.GetInt()) ) //值和质量相同
	{
		//更改时间
		m_valreal.ltime = pval->ltime;
		m_bRealUpdate = TRUE;
		if(m_bsave)
		{
			if((m_valreal.ltime - m_valhis.ltime) > 600) //10分钟存盘一次
			{
				m_valhis = m_valreal;
				m_bHisUpdate = TRUE;
			}
		}
		return;
	}
	//不同
	m_valreal = *pv;//复制
	m_bRealUpdate = TRUE;
	if(m_bsave)//要求存盘
	{
		m_valhis = m_valreal;
		m_bHisUpdate = TRUE;
	}
}

////////////////////////////////////////////////////////////////////////
CTv_int64::CTv_int64()
{
	m_ntype = DT_INT64;
	m_valreal.ResetType(m_ntype);
	m_valhis.ResetType(m_ntype);
}

CTv_int64::~CTv_int64()
{

}

void CTv_int64::AddReal(LPVVAL pval)//添加实时值,该压缩的要压缩
{
	pval->snerr = ERRTV_OK;
	CVarVal *pv = (CVarVal *)pval;
	if(m_valreal.ltime == 0)
	{
		m_valreal = *pv;
		m_bRealUpdate = TRUE;
		if(m_bsave)//要求存盘
		{
			m_valhis = m_valreal;
			m_bHisUpdate = TRUE;
		}
		return ;
	}

	if(m_valreal.ltime >= pval->ltime)
	{
		pval->snerr = ERRTV_TIME;
		return;
	}

	if((pval->snqa == m_valreal.snqa) && (pv->GetInt64() == m_valreal.GetInt64())) //值和质量相同
	{
		//更改时间
		m_valreal.ltime = pval->ltime;
		m_bRealUpdate = TRUE;
		if(m_bsave)
		{
			if((m_valreal.ltime - m_valhis.ltime) > 600) //10分钟存盘一次
			{
				m_valhis = m_valreal;
				m_bHisUpdate = TRUE;
			}
		}
		return;
	}
	//不同
	m_valreal = *pv;//复制
	m_bRealUpdate = TRUE;
	if(m_bsave)//要求存盘
	{
		m_valhis = m_valreal;
		m_bHisUpdate = TRUE;
	}
}

////////////////////////////////////////////////////////////////////////
CTv_string::CTv_string()
{
	m_ntype = DT_STRING;
	m_valreal.ResetType(m_ntype);
	m_valhis.ResetType(m_ntype);
}

CTv_string::~CTv_string()
{

}

void CTv_string::AddReal(LPVVAL pval)//添加实时值,该压缩的要压缩
{
	pval->snerr = ERRTV_OK;
	if(m_valreal.ltime >= pval->ltime)
	{
		pval->snerr = ERRTV_TIME;
		return;
	}
	if(pval->vt != DT_STRING)
	{
		pval->snerr = ERRTV_TYPE;
		return;
	}
	CVarVal *pv = (CVarVal *)pval;
	m_valreal = *pv;//复制
	m_bRealUpdate = TRUE;

	if(m_bsave)//要求存盘
	{
		m_valhis = m_valreal;
		m_bHisUpdate = TRUE;
	}
}

////////////////////////////////////////////////////////////////////////
CTv_blob::CTv_blob()
{
	m_ntype = DT_BLOB;
	m_valreal.ResetType(m_ntype);
	m_valhis.ResetType(m_ntype);
}

CTv_blob::~CTv_blob()
{

}

void CTv_blob::AddReal(LPVVAL pval)//添加实时值,该压缩的要压缩
{
	pval->snerr = ERRTV_OK;
	if(m_valreal.ltime >= pval->ltime)
	{
		pval->snerr = ERRTV_TIME;
		return;
	}
	if(pval->vt != DT_BLOB)
	{
		pval->snerr = ERRTV_TYPE;
		return;
	}
	CVarVal *pv = (CVarVal *)pval;
	m_valreal = *pv;//复制
	m_bRealUpdate = TRUE;

	if(m_bsave)//要求存盘
	{
		m_valhis = m_valreal;
		m_bHisUpdate = TRUE;
	}
}

////////////////////////////////////////////////////////////////////////
CTv_float32::CTv_float32()
{
	m_ntype = DT_FLOAT32;
	m_valreal.ResetType(m_ntype);
	m_valhis.ResetType(m_ntype);
	int i;
	for(i=0;i<6;i++)
		m_valtmp[i].Init(DT_FLOAT32); //初始化类型

	m_nrecs = 0;
}

CTv_float32::~CTv_float32()
{

}

BOOL CTv_float32::GetLastHisVal(COdbcTagVal* pval)//取最后的历史值
{
	if(m_bsave == FALSE)
		return FALSE;
	if(m_nrecs < 3)
		return FALSE;

	pval->m_lid = m_lid;
	pval->m_lres = 0;
	CVarVal* pv = (CVarVal*)&pval->m_val;
	pv->SetVoid();
	*pv = m_valtmp[m_nrecs - 1];

	return TRUE;
}

void CTv_float32::AddReal(LPVVAL pval)//添加实时值,该压缩的要压缩
{
	pval->snerr = ERRTV_OK;
	if(m_valreal.ltime >= pval->ltime)
	{
		pval->snerr = ERRTV_TIME;
		return;
	}
	if(pval->vt != DT_FLOAT32)
	{
		pval->snerr = ERRTV_TYPE;
		return;
	}
	CVarVal *pv = (CVarVal *)pval;
	m_valreal = *pv;//复制
	m_bRealUpdate = TRUE;

	if(!m_bsave)//不存盘
		return;
	//压缩
	if(m_ncomptype == TA_COMPNO)//不压缩
	{
		m_valhis = m_valreal;
		m_bHisUpdate = TRUE;
		return;
	}
	//加入压缩区
	m_valtmp[m_nrecs] = m_valreal;
	m_nrecs++;

	if(m_nrecs < 3)
	{
		if(m_nrecs == 1) //第一个点无条件存盘
		{
			m_valhis = m_valreal;
			m_bHisUpdate = TRUE;
			return;
		}
		return ; //不可压缩
	}
	//压缩
	int i;
	double dblx = (m_valtmp[m_nrecs-1].GetFloat() - m_valtmp[0].GetFloat())/
		(m_valtmp[m_nrecs-1].ltime - m_valtmp[0].ltime);
	double dblt=0,dbltv,dbldv;
	for(i=1;i<m_nrecs - 1;i++)
	{
		dbltv = m_valtmp[i].GetFloat();
		dblt = m_valtmp[i-1].GetFloat() + dblx * (m_valtmp[i].ltime - m_valtmp[i-1].ltime);
		if(m_ncomptype == TA_COMPPER) //百分比精度
			dbldv = fabs(100 * (dblt - dbltv)/dbltv);
		else //绝对精度
			dbldv = fabs(m_valtmp[i-1].GetFloat() - dbltv); //超出精度
		if(dbldv > m_fcompval)//超出精度
		{
			//存盘m_nrecs - 2点
			m_valhis = m_valtmp[m_nrecs - 2];
			m_bHisUpdate = TRUE;
		
			//重新开始压缩,将已存盘点作为起点
			m_valtmp[0] = m_valtmp[m_nrecs - 2];
			m_valtmp[1] = m_valtmp[m_nrecs - 1];
			m_nrecs = 2;

			return ; //返回要存盘的样本值
		}
	}
	
	if(m_nrecs > 4) //5个点及以上抛弃偏离最小的点
	{
		double dblmin = 0,dbltt;
		int nminpos = -1;
		for(i=1;i<m_nrecs - 1;i++) //寻找偏移最小点位置
		{
			dbltv = m_valtmp[i].GetFloat();
			dblt = m_valtmp[i-1].GetFloat() + dblx * (m_valtmp[i].ltime - m_valtmp[i-1].ltime);
			if(m_ncomptype == TA_COMPPER) //百分比精度
				dbltt = fabs(100 * (dblt - dbltv)/dbltv);
			else //绝对精度
				dbltt = fabs(m_valtmp[i-1].GetFloat() - dbltv);
			if(i == 1)
			{
				dblmin = dbltt;
				nminpos = i;
			}
			else 
			{
				if(dblmin > dbltt)
				{
					dblmin = dbltt;
					nminpos = i;
				}
			}
		}
		//抛弃
		if(nminpos > 0)
		{
			for(i=nminpos;i<m_nrecs - 1;i++)
				m_valtmp[i] = m_valtmp[i+1];
			m_nrecs--;
		}
	}
}
////////////////////////////////////////////////////////////////////////
CTv_float64::CTv_float64()
{
	m_ntype = DT_FLOAT64;
	m_valreal.ResetType(m_ntype);
	m_valhis.ResetType(m_ntype);
	int i;
	for(i=0;i<6;i++)
		m_valtmp[i].Init(DT_FLOAT64); //初始化类型

	m_nrecs = 0;
}

CTv_float64::~CTv_float64()
{

}

BOOL CTv_float64::GetLastHisVal(COdbcTagVal* pval)//取最后的历史值
{
	if(m_bsave == FALSE)
		return FALSE;
	if(m_nrecs < 3)
		return FALSE;

	pval->m_lid = m_lid;
	pval->m_lres = 0;
	CVarVal* pv = (CVarVal*)&pval->m_val;
	pv->SetVoid();
	*pv = m_valtmp[m_nrecs - 1];
	return TRUE;
}


void CTv_float64::AddReal(LPVVAL pval)//添加实时值,该压缩的要压缩
{
	pval->snerr = ERRTV_OK;
	if(m_valreal.ltime >= pval->ltime)
	{
		pval->snerr = ERRTV_TIME;
		return;
	}
	if(pval->vt != DT_FLOAT64)
	{
		pval->snerr = ERRTV_TYPE;
		return;
	}
	CVarVal *pv = (CVarVal *)pval;
	m_valreal = *pv;//复制
	m_bRealUpdate = TRUE;

	if(!m_bsave)//不存盘
		return;
	//压缩
	if(m_ncomptype == TA_COMPNO)//不压缩
	{
		m_valhis = m_valreal;
		m_bHisUpdate = TRUE;
		return;
	}
	//加入压缩区
	m_valtmp[m_nrecs] = m_valreal;
	m_nrecs++;

	if(m_nrecs < 3)
	{
		if(m_nrecs == 1) //第一个点无条件存盘
		{
			m_valhis = m_valreal;
			m_bHisUpdate = TRUE;
			return;
		}
		return ; //不可压缩
	}
	//压缩
	int i;
	double dblx = (m_valtmp[m_nrecs-1].GetDouble() - m_valtmp[0].GetDouble())/
		(m_valtmp[m_nrecs-1].ltime - m_valtmp[0].ltime);
	double dblt=0,dbltv,dbldv;
	for(i=1;i<m_nrecs - 1;i++)
	{
		dbltv = m_valtmp[i].GetDouble();
		dblt = m_valtmp[i-1].GetDouble() + dblx * (m_valtmp[i].ltime - m_valtmp[i-1].ltime);
		if(m_ncomptype == TA_COMPPER) //百分比精度
			dbldv = fabs(100 * (dblt - dbltv)/dbltv);
		else //绝对精度
			dbldv = fabs(m_valtmp[i-1].GetDouble() - dbltv); //超出精度
		if(dbldv > m_fcompval)//超出精度
		{
			//存盘m_nrecs - 2点
			m_valhis = m_valtmp[m_nrecs - 2];
			m_bHisUpdate = TRUE;
		
			//重新开始压缩,将已存盘点作为起点
			m_valtmp[0] = m_valtmp[m_nrecs - 2];
			m_valtmp[1] = m_valtmp[m_nrecs - 1];
			m_nrecs = 2;

			return ; //返回要存盘的样本值
		}
	}
	
	if(m_nrecs > 4) //5个点及以上抛弃偏离最小的点
	{
		double dblmin = 0,dbltt;
		int nminpos = -1;
		for(i=1;i<m_nrecs - 1;i++) //寻找偏移最小点位置
		{
			dbltv = m_valtmp[i].GetDouble();
			dblt = m_valtmp[i-1].GetDouble() + dblx * (m_valtmp[i].ltime - m_valtmp[i-1].ltime);
			if(m_ncomptype == TA_COMPPER) //百分比精度
				dbltt = fabs(100 * (dblt - dbltv)/dbltv);
			else //绝对精度
				dbltt = fabs(m_valtmp[i-1].GetDouble() - dbltv);
			if(i == 1)
			{
				dblmin = dbltt;
				nminpos = i;
			}
			else 
			{
				if(dblmin > dbltt)
				{
					dblmin = dbltt;
					nminpos = i;
				}
			}
		}
		//抛弃
		if(nminpos > 0)
		{
			for(i=nminpos;i<m_nrecs - 1;i++)
				m_valtmp[i] = m_valtmp[i+1];
			m_nrecs--;
		}
	}
}

////////////////////////////////////////////////////////////////////////
CMemRdb::CMemRdb()
{
	//m_map.InitHashTable(11111);
}
CMemRdb::~CMemRdb()
{
	//清空map
	Clear();
}

void CMemRdb::RemoveTag(LPCTSTR lpszTagName)
{
	CSafeLock lock(&m_Lock);

	CTvObj* pi;
	if (m_map.Lookup(lpszTagName, pi))
	{
		delete pi;
		m_map.RemoveKey(lpszTagName);
	}
}

void CMemRdb::Clear()//清空
{
	CSafeLock lock(&m_Lock);

	CString szkey;
	CTvObj* pi;
	POSITION pos = m_map.GetStartPosition();
	while(pos!=NULL)
	{
		m_map.GetNextAssoc(pos,szkey,pi);
		delete pi;
	}
	m_map.RemoveAll();
}

void CMemRdb::AddTag(LPDRVTAGATTR ptd)
{
	CSafeLock lock(&m_Lock);

	CTvObj* pi;
	if(m_map.Lookup(ptd->sname,pi))
	{
		//pi->m_szname = ptd->sname;
		pi->m_lid = ptd->lid;
		pi->m_ntype = ptd->nDataType;
		pi->m_bsave = ptd->nsave;
		pi->m_ncomptype = ptd->ncomp;
		pi->m_fcompval = ptd->dblcompval;

		return; //已存在
	}
	if(ptd->nDataType == DT_DIGITAL)
	{
		CTv_digital* pv = new CTv_digital;
		if(pv == NULL)
			return;

		pv->m_szname = ptd->sname;
		pv->m_lid = ptd->lid;
		pv->m_ntype = ptd->nDataType;
		pv->m_bsave = ptd->nsave;
		pv->m_ncomptype = ptd->ncomp;
		pv->m_fcompval = ptd->dblcompval;
		m_map.SetAt(pv->m_szname,pv);
	}
	else if(ptd->nDataType == DT_INT32)
	{
		CTv_int32* pv = new CTv_int32;
		if(pv == NULL)
			return;

		pv->m_szname = ptd->sname;
		pv->m_lid = ptd->lid;
		pv->m_ntype = ptd->nDataType;
		pv->m_bsave = ptd->nsave;
		pv->m_ncomptype = ptd->ncomp;
		pv->m_fcompval = ptd->dblcompval;
		m_map.SetAt(pv->m_szname,pv);
	}
	else if(ptd->nDataType == DT_INT64)
	{
		CTv_int64* pv = new CTv_int64;
		if(pv == NULL)
			return;

		pv->m_szname = ptd->sname;
		pv->m_lid = ptd->lid;
		pv->m_ntype = ptd->nDataType;
		pv->m_bsave = ptd->nsave;
		pv->m_ncomptype = ptd->ncomp;
		pv->m_fcompval = ptd->dblcompval;
		m_map.SetAt(pv->m_szname,pv);
	}
	else if(ptd->nDataType == DT_FLOAT32)
	{
		CTv_float32* pv = new CTv_float32;
		if(pv == NULL)
			return;

		pv->m_szname = ptd->sname;
		pv->m_lid = ptd->lid;
		pv->m_ntype = ptd->nDataType;
		pv->m_bsave = ptd->nsave;
		pv->m_ncomptype = ptd->ncomp;
		pv->m_fcompval = ptd->dblcompval;
		m_map.SetAt(pv->m_szname,pv);
	}
	else if(ptd->nDataType == DT_FLOAT64)
	{
		CTv_float64* pv = new CTv_float64;
		if(pv == NULL)
			return;

		pv->m_szname = ptd->sname;
		pv->m_lid = ptd->lid;
		pv->m_ntype = ptd->nDataType;
		pv->m_bsave = ptd->nsave;
		pv->m_ncomptype = ptd->ncomp;
		pv->m_fcompval = ptd->dblcompval;
		m_map.SetAt(pv->m_szname,pv);
	}
	else if(ptd->nDataType == DT_STRING)
	{
		CTv_string* pv = new CTv_string;
		if(pv == NULL)
			return;

		pv->m_szname = ptd->sname;
		pv->m_lid = ptd->lid;
		pv->m_ntype = ptd->nDataType;
		pv->m_bsave = ptd->nsave;
		pv->m_ncomptype = ptd->ncomp;
		pv->m_fcompval = ptd->dblcompval;
		m_map.SetAt(pv->m_szname,pv);
	}
	else if(ptd->nDataType == DT_BLOB)
	{
		CTv_blob* pv = new CTv_blob;
		if(pv == NULL)
			return;

		pv->m_szname = ptd->sname;
		pv->m_lid = ptd->lid;
		pv->m_ntype = ptd->nDataType;
		pv->m_bsave = ptd->nsave;
		pv->m_ncomptype = ptd->ncomp;
		pv->m_fcompval = ptd->dblcompval;
		m_map.SetAt(pv->m_szname,pv);
	}
}

void CMemRdb::AddRealVal(LPCTSTR sname,LPVVAL pval) //添加标签值
{
	CSafeLock lock(&m_Lock);

	CTvObj* pi;
	if(!m_map.Lookup(sname,pi))
	{
		pval->snerr = ERRTV_NOTAG;
		return; //不存在
	}
	if (pi->m_ntype != (int)pval->vt)
	{
		pval->snerr = ERRTV_TYPE;
		return;
	}
	pi->AddReal(pval);

}

void CMemRdb::GetSaveVals(CArrayOdbcVal* par,CArrayOdbcVal* pah) //取需要存盘的实时和历史
{
	CSafeLock lock(&m_Lock);

	COdbcTagVal ov,*pv;
	CString szkey;
	CTvObj* pi;
	POSITION pos = m_map.GetStartPosition();
	while(pos!=NULL)
	{
		m_map.GetNextAssoc(pos,szkey,pi);
		ov.GetVval().SetVoid();
		if(pi->GetRealVal(&ov))
		{
			pv = new COdbcTagVal;
			if(pv)
			{
				*pv = ov;
				par->Add(pv);
			}
		}
		ov.GetVval().SetVoid();
		if(pi->GetHisVal(&ov))
		{
			pv = new COdbcTagVal;
			if(pv)
			{
				*pv = ov;
				pah->Add(pv);
			}
		}
	}
}