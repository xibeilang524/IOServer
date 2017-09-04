//cashe.cpp
#include "stdafx.h"
#include "cashe.h"
#include <math.h>

//void g_AddLog(long ltime,LPCTSTR lpszTxt);
static long g_lLastDiskErrTime = 0;

#include  <io.h>

////////////////////////////////////////////////////////////////////////

CTvObj::CTvObj()
{
	m_szname="";//标签名
	m_ntype = DT_VOID; //数据类型
	m_ncomptype = TA_COMPNO;//压缩类型
	m_fcompval = 0;//压缩精度
}
CTvObj::~CTvObj()
{

}

////////////////////////////////////////////////////////////////////////
CTv_digital::CTv_digital()
{
	m_ntype = DT_DIGITAL;
	m_valreal.ResetType(m_ntype);

	m_llastsave = 0;//上次存储
	m_bdataok = FALSE; //数据有效
}

CTv_digital::~CTv_digital()
{

}

BOOL CTv_digital::AddReal(LPVVAL pval,CBaseVal* pbaseval)
{
	CVarVal *pv = (CVarVal *)pval;
	if(m_valreal.ltime == 0)
	{
		m_valreal = *pv;
		pbaseval->Clear();
		pbaseval->m_szName = m_szname;
		pbaseval->m_val = m_valreal;
		m_llastsave = pval->ltime;
		m_bdataok = FALSE;//不需要再存该数据了
		return TRUE;
	}

	if(m_valreal.ltime >= pval->ltime)
		return FALSE;	
	if((pval->snqa == m_valreal.snqa) && (pv->GetInt() == m_valreal.GetInt())) //值和质量相同
	{
		//更改时间
		m_valreal.ltime = pval->ltime;
		if((m_valreal.ltime - m_llastsave) > 600) //10分钟存盘一次
		{
			pbaseval->Clear();
			pbaseval->m_szName = m_szname;
			pbaseval->m_val = m_valreal;
			m_llastsave = pval->ltime;
			m_bdataok = FALSE;//不需要再存该数据了
			return TRUE;
		}
		m_bdataok = TRUE; //数据有效，暂时未存
		return FALSE;
	}
	//不同
	m_valreal = *pv;//复制
	pbaseval->Clear();
	pbaseval->m_szName = m_szname;
	pbaseval->m_val = m_valreal;
	m_llastsave = pval->ltime;
	m_bdataok = FALSE;//不需要再存该数据了
	return TRUE;
}

////////////////////////////////////////////////////////////////////////
CTv_int32::CTv_int32()
{
	m_ntype = DT_INT32;
	m_valreal.ResetType(m_ntype);

	m_llastsave = 0;//上次存储
	m_bdataok = FALSE; //数据有效
}

CTv_int32::~CTv_int32()
{

}

BOOL CTv_int32::AddReal(LPVVAL pval,CBaseVal* pbaseval)//添加实时值,该压缩的要压缩
{
	CVarVal *pv = (CVarVal *)pval;
	if(m_valreal.ltime == 0)
	{
		m_valreal = *pv;
		pbaseval->Clear();
		pbaseval->m_szName = m_szname;
		pbaseval->m_val = m_valreal;
		m_llastsave = pval->ltime;
		m_bdataok = FALSE;//不需要再存该数据了
		return TRUE;
	}

	if(m_valreal.ltime >= pval->ltime)
		return FALSE;	
	if((pval->snqa == m_valreal.snqa) && (pv->GetInt() == m_valreal.GetInt())) //值和质量相同
	{
		//更改时间
		m_valreal.ltime = pval->ltime;
		if((m_valreal.ltime - m_llastsave) > 600) //10分钟存盘一次
		{
			pbaseval->Clear();
			pbaseval->m_szName = m_szname;
			pbaseval->m_val = m_valreal;
			m_llastsave = pval->ltime;
			m_bdataok = FALSE;//不需要再存该数据了
			return TRUE;
		}
		m_bdataok = TRUE; //数据有效，暂时未存
		return FALSE;
	}
	//不同
	m_valreal = *pv;//复制
	pbaseval->Clear();
	pbaseval->m_szName = m_szname;
	pbaseval->m_val = m_valreal;
	m_llastsave = pval->ltime;
	m_bdataok = FALSE;//不需要再存该数据了
	return TRUE;
}

////////////////////////////////////////////////////////////////////////
CTv_int64::CTv_int64()
{
	m_ntype = DT_INT64;
	m_valreal.ResetType(m_ntype);

	m_llastsave = 0;//上次存储
	m_bdataok = FALSE; //数据有效
}

CTv_int64::~CTv_int64()
{

}

BOOL CTv_int64::AddReal(LPVVAL pval,CBaseVal* pbaseval)//添加实时值,该压缩的要压缩
{
	CVarVal *pv = (CVarVal *)pval;
	if(m_valreal.ltime == 0)
	{
		m_valreal = *pv;
		pbaseval->Clear();
		pbaseval->m_szName = m_szname;
		pbaseval->m_val = m_valreal;
		m_llastsave = pval->ltime;
		m_bdataok = FALSE;//不需要再存该数据了
		return TRUE;
	}

	if(m_valreal.ltime >= pval->ltime)
		return FALSE;	
	if((pval->snqa == m_valreal.snqa) && (pv->GetInt64() == m_valreal.GetInt64())) //值和质量相同
	{
		//更改时间
		m_valreal.ltime = pval->ltime;
		if((m_valreal.ltime - m_llastsave) > 600) //10分钟存盘一次
		{
			pbaseval->Clear();
			pbaseval->m_szName = m_szname;
			pbaseval->m_val = m_valreal;
			m_llastsave = pval->ltime;
			m_bdataok = FALSE;//不需要再存该数据了
			return TRUE;
		}
		m_bdataok = TRUE; //数据有效，暂时未存
		return FALSE;
	}
	//不同
	m_valreal = *pv;//复制
	pbaseval->Clear();
	pbaseval->m_szName = m_szname;
	pbaseval->m_val = m_valreal;
	m_llastsave = pval->ltime;
	m_bdataok = FALSE;//不需要再存该数据了
	return TRUE;
}

////////////////////////////////////////////////////////////////////////
CTv_string::CTv_string()
{
	m_ntype = DT_STRING;
	m_valreal.ResetType(m_ntype);

}

CTv_string::~CTv_string()
{

}

BOOL CTv_string::AddReal(LPVVAL pval,CBaseVal* pbaseval)//添加实时值,该压缩的要压缩
{
	if((m_valreal.ltime >= pval->ltime)||(pval->vt != DT_STRING))
		return FALSE;

	CVarVal *pv = (CVarVal *)pval;
	m_valreal = *pv;//复制
	pbaseval->Clear();
	pbaseval->m_szName = m_szname;
	pbaseval->m_val = m_valreal;
	return TRUE;
}

////////////////////////////////////////////////////////////////////////
CTv_blob::CTv_blob()
{
	m_ntype = DT_BLOB;
	m_valreal.ResetType(m_ntype);

}

CTv_blob::~CTv_blob()
{

}

BOOL CTv_blob::AddReal(LPVVAL pval,CBaseVal* pbaseval)//添加实时值,该压缩的要压缩
{
	if((m_valreal.ltime >= pval->ltime)||(pval->vt != DT_BLOB))
		return FALSE;

	CVarVal *pv = (CVarVal *)pval;
	m_valreal = *pv;//复制
	pbaseval->Clear();
	pbaseval->m_szName = m_szname;
	pbaseval->m_val = m_valreal;
	return TRUE;
}

////////////////////////////////////////////////////////////////////////
CTv_float32::CTv_float32()
{
	m_ntype = DT_FLOAT32;
	m_valreal.ResetType(m_ntype);

	int i;
	for(i=0;i<6;i++)
		m_valtmp[i].Init(DT_FLOAT32); //初始化类型

	m_nrecs = 0;
}

CTv_float32::~CTv_float32()
{

}

BOOL CTv_float32::GetLastHisVal(CBaseVal* pbaseval)//取最后的历史值
{
	if(m_nrecs < 3)
		return FALSE;
	pbaseval->Clear();
	pbaseval->m_szName = m_szname;
	pbaseval->m_val = m_valtmp[m_nrecs - 1];

	return TRUE;
}

BOOL CTv_float32::AddReal(LPVVAL pval,CBaseVal* pbaseval)//添加实时值,该压缩的要压缩
{
	if((m_valreal.ltime >= pval->ltime)||(pval->vt != DT_FLOAT32))
		return FALSE;

	CVarVal *pv = (CVarVal *)pval;
	m_valreal = *pv;//复制

	//压缩
	if(m_ncomptype == TA_COMPNO)//不压缩
	{
		pbaseval->Clear();
		pbaseval->m_szName = m_szname;
		pbaseval->m_val = m_valreal;
		m_llastsave = pval->ltime;
		return TRUE;
	}
	//加入压缩区
	m_valtmp[m_nrecs] = m_valreal;
	m_nrecs++;

	if(m_nrecs < 3)
	{
		if(m_nrecs == 1) //第一个点无条件存盘
		{
			pbaseval->Clear();
			pbaseval->m_szName = m_szname;
			pbaseval->m_val = m_valreal;
			m_llastsave = pval->ltime;
			return TRUE;
		}
		return FALSE; //不可压缩
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
			pbaseval->Clear();
			pbaseval->m_szName = m_szname;
			pbaseval->m_val = m_valtmp[m_nrecs - 2];
			m_llastsave = pval->ltime;

			//重新开始压缩,将已存盘点作为起点
			m_valtmp[0] = m_valtmp[m_nrecs - 2];
			m_valtmp[1] = m_valtmp[m_nrecs - 1];
			m_nrecs = 2;

			return TRUE; //返回要存盘的样本值
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
	if(((m_valreal.ltime - m_llastsave) > 600) && (m_nrecs > 2)) //10分钟存盘一次
	{
		//存盘m_nrecs - 2点
		pbaseval->Clear();
		pbaseval->m_szName = m_szname;
		pbaseval->m_val = m_valtmp[m_nrecs - 2];
		m_llastsave = pval->ltime;

		//重新开始压缩,将已存盘点作为起点
		m_valtmp[0] = m_valtmp[m_nrecs - 2];
		m_valtmp[1] = m_valtmp[m_nrecs - 1];
		m_nrecs = 2;

		return TRUE;
	}
	return FALSE;
}
////////////////////////////////////////////////////////////////////////
CTv_float64::CTv_float64()
{
	m_ntype = DT_FLOAT64;
	m_valreal.ResetType(m_ntype);

	int i;
	for(i=0;i<6;i++)
		m_valtmp[i].Init(DT_FLOAT64); //初始化类型

	m_nrecs = 0;
}

CTv_float64::~CTv_float64()
{

}

BOOL CTv_float64::GetLastHisVal(CBaseVal* pbaseval)//取最后的历史值
{
	if(m_nrecs < 3)
		return FALSE;
	pbaseval->Clear();
	pbaseval->m_szName = m_szname;
	pbaseval->m_val = m_valtmp[m_nrecs - 1];

	return TRUE;
}


BOOL CTv_float64::AddReal(LPVVAL pval,CBaseVal* pbaseval)//添加实时值,该压缩的要压缩
{

	if((m_valreal.ltime >= pval->ltime)||(pval->vt != DT_FLOAT64))
		return FALSE;

	CVarVal *pv = (CVarVal *)pval;
	m_valreal = *pv;//复制

	//压缩
	if(m_ncomptype == TA_COMPNO)//不压缩
	{
		pbaseval->Clear();
		pbaseval->m_szName = m_szname;
		pbaseval->m_val = m_valreal;
		m_llastsave = pval->ltime;
		return TRUE;
	}
	//加入压缩区
	m_valtmp[m_nrecs] = m_valreal;
	m_nrecs++;

	if(m_nrecs < 3)
	{
		if(m_nrecs == 1) //第一个点无条件存盘
		{
			pbaseval->Clear();
			pbaseval->m_szName = m_szname;
			pbaseval->m_val = m_valreal;
			m_llastsave = pval->ltime;
			return TRUE;
		}
		return FALSE; //不可压缩
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
			pbaseval->Clear();
			pbaseval->m_szName = m_szname;
			pbaseval->m_val = m_valtmp[m_nrecs - 2];
			m_llastsave = pval->ltime;

			//重新开始压缩,将已存盘点作为起点
			m_valtmp[0] = m_valtmp[m_nrecs - 2];
			m_valtmp[1] = m_valtmp[m_nrecs - 1];
			m_nrecs = 2;

			return TRUE; //返回要存盘的样本值
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
	if(((m_valreal.ltime - m_llastsave) > 600) && (m_nrecs > 2)) //10分钟存盘一次
	{
		//存盘m_nrecs - 2点
		pbaseval->Clear();
		pbaseval->m_szName = m_szname;
		pbaseval->m_val = m_valtmp[m_nrecs - 2];
		m_llastsave = pval->ltime;

		//重新开始压缩,将已存盘点作为起点
		m_valtmp[0] = m_valtmp[m_nrecs - 2];
		m_valtmp[1] = m_valtmp[m_nrecs - 1];
		m_nrecs = 2;

		return TRUE;
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////
CCompCashe::CCompCashe()
{
	m_map.InitHashTable(77777);
}
CCompCashe::~CCompCashe()
{
	Clear();
}

void CCompCashe::Clear()//清空
{
	CString szkey;
	CTvObj* pi;
	POSITION pos = m_map.GetStartPosition();
	while(pos)
	{
		m_map.GetNextAssoc(pos,szkey,pi);
		delete pi;
	}
	m_map.RemoveAll();
}

BOOL CCompCashe::AddReal(LPCTSTR sname,int ncomptype,double dblcompval,LPVVAL pval,CBaseVal* pbaseval)//添加实时值
{
	//先查找m_map中是否有标签
	CTvObj* ptv = NULL;
	if(!m_map.Lookup(sname,ptv)) //没有，建立一个
	{
		if(pval->vt == DT_DIGITAL)
			ptv = new CTv_digital;
		else if(pval->vt == DT_INT32)
			ptv = new CTv_int32;
		else if(pval->vt == DT_INT64)
			ptv = new CTv_int64;
		else if(pval->vt == DT_FLOAT32)
			ptv = new CTv_float32;
		else if(pval->vt == DT_FLOAT64)
			ptv = new CTv_float64;
		else 
			return FALSE;

		if(ptv == NULL)
			return FALSE;

		ptv->m_szname = sname;
		m_map.SetAt(sname,ptv);//保存到MAP中
	}
	//以下两个参数可更改
	ptv->m_ncomptype = ncomptype;
	ptv->m_fcompval = dblcompval;

	return ptv->AddReal(pval,pbaseval);
}

void CCompCashe::SaveLastVal(CMemCashe* pmc)//存储最后一个值，退出时使用
{
	CBaseVal bv;
	CString szkey;
	CTvObj* pi;
	POSITION pos = m_map.GetStartPosition();
	while(pos!=NULL)
	{
		m_map.GetNextAssoc(pos,szkey,pi);
		if(pi->GetLastHisVal(&bv))
			pmc->AddTagVal(&bv);
	}
}


//////////////////////////////////////////////////////////////////////////
CMemCashe::CMemCashe()
{
	m_pvals = NULL;
	m_dwNextID = 1;
	m_nsize = 0;
	m_nGetPos = 0;
	m_pvals = (struct t_caval*)calloc(MEM_CASHE_SIZE,sizeof(struct t_caval)); //分配内存

	m_mapname.InitHashTable(77777);
	m_mapid.InitHashTable(77777);

	m_szDiskCasheFile = "";//磁盘缓存文件
}

CMemCashe::~CMemCashe()
{
	if(m_pvals)
		free(m_pvals);
	m_pvals = NULL;

	ClearMap();
}

void CMemCashe::ClearMap()//清空MAP
{
	DWORD dwid;
	LPNAMEID pid = NULL;
	POSITION pos = m_mapid.GetStartPosition();
	while(pos)
	{
		m_mapid.GetNextAssoc(pos,dwid,pid);
		delete pid;
	}
	m_mapid.RemoveAll();
	m_mapname.RemoveAll();
}

DWORD CMemCashe::GetID(LPCTSTR lpszName)
{
	LPNAMEID pid = NULL;
	if(m_mapname.Lookup(lpszName,pid))
		return pid->dwid;
	return 0;
}

CString  CMemCashe::GetName(DWORD dwid)
{
	CString szname = "";
	LPNAMEID p=NULL;
	if(m_mapid.Lookup(dwid,p))
		szname = p->sname;
	return szname;
}

DWORD CMemCashe::MakeID(LPCTSTR lpszName)
{
	DWORD dwid = GetID(lpszName);
	if(dwid != 0)
		return dwid;
	LPNAMEID pid = new TNAMEID;
	if(pid == NULL)
		return 0;
	memset(pid,0,sizeof(TNAMEID));
	strncpy(pid->sname,lpszName,79);
	pid->dwid = m_dwNextID++;

	m_mapname.SetAt(pid->sname,pid);
	m_mapid.SetAt(pid->dwid,pid);
	return pid->dwid;
}

BOOL CMemCashe::IsFull()
{
	if(!m_pvals)
		return TRUE;
	return (m_nsize >= MEM_CASHE_SIZE);
}

BOOL CMemCashe::AddTagVal(CBaseVal* pbaseval)//添加样本值
{
	if(IsFull())
		return FALSE;
	DWORD dwid = MakeID(pbaseval->m_szName);
	if(dwid == 0)
		return FALSE;

	m_pvals[m_nsize].dwid = dwid;
	m_pvals[m_nsize].wtype = (WORD)pbaseval->m_val.vt; //类型
	m_pvals[m_nsize].llval = pbaseval->m_val.llval; //值
	m_pvals[m_nsize].ltime = pbaseval->m_val.ltime; //时标
	m_pvals[m_nsize].snqa = pbaseval->m_val.snqa;	//质量
	m_pvals[m_nsize].dwres = 0;	//保留
	m_nsize++;
	return TRUE;
}

void CMemCashe::Reset()		//复位，重新初始化
{
	ClearMap();
	m_dwNextID = 1;
	m_nsize = 0;
	m_nGetPos = 0;
	m_szDiskCasheFile = "";
}

BOOL CMemCashe::SaveToDisk(LPCTSTR lpszPath) //保存到磁盘
{
	CString szdisk="";
	szdisk = lpszPath;
	szdisk = szdisk.Left(3);
	if(Utility::GetDiskSpace(szdisk) < 500)  //磁盘空间不够
	{
		long ltime = (long)::time(NULL);
		if((ltime - g_lLastDiskErrTime) > 300) //5分钟报告一次
		{
			GetLogger()->AddError("磁盘空间小于500M，终止缓存写盘!");
			g_lLastDiskErrTime = ltime;
		}
		return FALSE;
	}


	CString szFile="",szf="";
	SYSTEMTIME syst;
	::GetLocalTime(&syst);
	szFile.Format("%04d%02d%02d-%02d%02d%02d-%03d.cah",syst.wYear,syst.wMonth,syst.wDay,
		syst.wHour,syst.wMinute,syst.wSecond,syst.wMilliseconds);
	szf = lpszPath;
	szf += szFile;

	if(-1 == _access( lpszPath, 0 ))//目录不存在
	{
		TRACE("CMemCashe::SaveToDisk() 目录不存在!\n");
		if(!Utility::CreateDir(lpszPath))
		{
			TRACE("CMemCashe::SaveToDisk() 创建目录失败!\n");
			return FALSE;//创建目录失败!
		}
	}

	

	CFile fl;
	//创建文件 
	if(!fl.Open(szf,CFile::modeCreate|CFile::modeWrite))
			return FALSE;
	TCAFHEAD th;
	th.dwflag = 0xDF23AD79;
	th.ldovals = 0;
	th.lnameids = m_mapid.GetCount();
	th.lvals = m_nsize;
	try
	{
		
		fl.Write(&th,sizeof(TCAFHEAD)); //写入头部

		//写入name-id表
		DWORD dwid;
		LPNAMEID pid = NULL;
		POSITION pos = m_mapid.GetStartPosition();
		while(pos)
		{
			m_mapid.GetNextAssoc(pos,dwid,pid);
			fl.Write(pid,sizeof(TNAMEID));
		}
		//写入数据记录
		int i;
		for(i=0;i<m_nsize;i++)
			fl.Write(&m_pvals[i],sizeof(struct t_caval));
	}
	catch ( CFileException *ex ) 
	{
	   ex->Delete();
	   fl.Close(); //关闭文件
	   ::DeleteFile(szf);//删除错误的文件
	   return FALSE;
	}
	GetLogger()->AddInfoFmt("写入缓存<%s>,lnameids = %d,lvals = %d",szf,th.lnameids,th.lvals);

	return TRUE;
}

BOOL CMemCashe::LoadFromDisk(LPCTSTR lpszPath)//从磁盘读取最新的,读后删除，当做当前文件
{
	if(m_nsize > 0) //还有内存缓存
		return FALSE;

	if(!m_szDiskCasheFile.IsEmpty())
	{
		GetLogger()->AddInfoFmt("<%s>发送完成!",m_szDiskCasheFile);
	}
	Reset(); //复位
	int nfiles = 0;
	CFileFind ff;
    CString strDir, strFile,szret="";

    strDir = lpszPath;
    strDir += "*.cah";

    BOOL bFind = ff.FindFile(strDir);
    while ( bFind )
    {
        bFind = ff.FindNextFile();
        if ( ff.IsDots() )
            continue;
		if ( ff.IsDirectory() )
			continue;

		nfiles++;
		strFile = ff.GetFileName ();
        if(szret.IsEmpty())
		{
			szret = strFile;
			continue;
		}
		if(_stricmp(strFile,szret) > 0) //新
			szret = strFile;
    }
    ff.Close();
    if(szret.IsEmpty())
		return FALSE;

	//strFile 待打开的全路径文件
	strFile = lpszPath;
	strFile += szret;

	CFile fl;
	//打开文件 
	if(!fl.Open(strFile,CFile::modeRead))
			return FALSE;

	m_szDiskCasheFile = strFile;
	
	GetLogger()->AddInfoFmt("开始发送缓存<%s>",m_szDiskCasheFile);

	TCAFHEAD th;
	memset(&th,0,sizeof(TCAFHEAD));

	int i;
	try
	{
		
		if(sizeof(TCAFHEAD) != fl.Read(&th,sizeof(TCAFHEAD)) )//读入头部
		{
			fl.Close();
			::DeleteFile(strFile);
			return FALSE;
		}
		if(th.dwflag != 0xDF23AD79)
		{
			fl.Close();
			::DeleteFile(strFile);
			return FALSE;
		}
		//读入name-id表
		TNAMEID tid,*ptid;
		for(i=0;i<th.lnameids;i++)
		{
			if(sizeof(TNAMEID) != fl.Read(&tid,sizeof(TNAMEID)) )
			{
				fl.Close();
				::DeleteFile(strFile);
				Reset();
				return FALSE;
			}
			ptid = new TNAMEID;
			if(ptid)
			{
				memcpy(ptid,&tid,sizeof(TNAMEID));
				m_mapid.SetAt(ptid->dwid,ptid);
				m_mapname.SetAt(ptid->sname,ptid);
			}
		}

		//读入数据记录
		int i;
		for(i=0;i<th.lvals && MEM_CASHE_SIZE;i++)
		{
			if(sizeof(struct t_caval) != fl.Read(&m_pvals[i],sizeof(struct t_caval)) )
				break;
			m_nsize++;
		}
	}
	catch ( CFileException *ex ) 
	{
	   ex->Delete();
	   fl.Close(); //关闭文件
	   ::DeleteFile(strFile);//删除错误的文件
	   return FALSE;
	}
	fl.Close(); //关闭文件
	::DeleteFile(strFile);//删除文件
	return TRUE;
}

int  CMemCashe::GetLastRecs(CTagVal* lprecs,int nsize)//取最新的cashe数据用于cashe发送,返回个数
{
	int i,n=0;
	CString szname = "";
	struct t_caval*  ptca = NULL;
	for(i = m_nsize-1; i>=0 && n < nsize;i--)
	{
		szname = GetName(m_pvals[i].dwid);
		if(szname.IsEmpty())
			continue;
	
		lprecs[n].m_szNamedb = szname; //目标服务器标签名
		lprecs[n].m_nType  = m_pvals[i].wtype;
		lprecs[n].m_nCompType = 0;//cashe数据不需在压缩，直接填写即可
		lprecs[n].m_dblcompval = 0;//cashe数据不需在压缩，直接填写即可
		lprecs[n].m_val.ResetType(m_pvals[i].wtype);
		lprecs[n].m_val.llval = m_pvals[i].llval;
		lprecs[n].m_val.ltime = m_pvals[i].ltime;
		lprecs[n].m_val.snqa = m_pvals[i].snqa;
		lprecs[n].m_val.snerr = 0;
		n++;
	}
	m_nGetPos = i;
	if(m_nGetPos < 0)
		m_nGetPos = 0;
	return n;
}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
CPfCashe::CPfCashe()
{
	m_szPath = "";
}
CPfCashe::~CPfCashe()
{

}

CString CPfCashe::GetLastFile(int &n)//获取最新的页面文件名，磁盘文件检索
{
	n = 0;
	CFileFind ff;
    CString strDir, strFile,szret="";

    strDir = m_szPath;
    strDir += "*.pgf";

    BOOL bFind = ff.FindFile(strDir);
    while ( bFind )
    {
        bFind = ff.FindNextFile();
        if ( ff.IsDots() )
            continue;
		if ( ff.IsDirectory() )
			continue;

		n++;
		strFile = ff.GetFileName ();
        if(szret.IsEmpty())
		{
			szret = strFile;
			continue;
		}
		if(_stricmp(strFile,szret) > 0) //新
			szret = strFile;
    }
    ff.Close();
    return szret;
}

BOOL CPfCashe::Init(LPCTSTR lpszPath)//初始化，打开最新文件，没有则创建
{
	int nfiles = 0;
	m_szPath = lpszPath;
	Utility::MakePath(m_szPath);
	Utility::CreateDir(m_szPath); //创建目录
	CString szFile = GetLastFile(nfiles);
	if(szFile.IsEmpty())
	{
		SYSTEMTIME syst;
		::GetLocalTime(&syst);
		szFile.Format("%04d%02d%02d-%02d%02d%02d.pgf",syst.wYear,syst.wMonth,syst.wDay,
			syst.wHour,syst.wMinute,syst.wSecond);
		if(PFR_OK != m_pf.Create(m_szPath + szFile)) //床架页面文件
			return FALSE;
		return TRUE;
	}
	if(PFR_OK != m_pf.Open(m_szPath + szFile)) //打开页面文件
		return FALSE;
	GetLogger()->AddInfoFmt("当前可变类型页面文件<%s>!",m_szPath + szFile);
	return TRUE;
}	

//void g_AddLog(long ltime,LPCTSTR lpszTxt);
BOOL CPfCashe::SaveRec(CPfRecVal* pvar)//写入
{
	CString szdisk="";
	szdisk = m_szPath;
	szdisk = szdisk.Left(3);
	if(Utility::GetDiskSpace(szdisk) < 500)  //磁盘空间不够
	{
		long ltime = (long)::time(NULL);
		if((ltime - g_lLastDiskErrTime) > 300) //5分钟报告一次
		{
			GetLogger()->AddError("磁盘空间小于500M，终止缓存写盘!");
			g_lLastDiskErrTime = ltime;
		}
		return FALSE;
	}

	if(PFR_OK == m_pf.WriteRec(pvar)) //成功
		return TRUE;

	m_pf.Close(); //关闭

	//创建新页面文件
	CString szFile;
	SYSTEMTIME syst;
	::GetLocalTime(&syst);
	szFile.Format("%04d%02d%02d-%02d%02d%02d.pgf",syst.wYear,syst.wMonth,syst.wDay,
		syst.wHour,syst.wMinute,syst.wSecond);
	if(PFR_OK != m_pf.Create(m_szPath + szFile)) //床架页面文件
		return FALSE;

	//写入到新的
	if(PFR_OK == m_pf.WriteRec(pvar)) //成功
		return TRUE;

	return FALSE;
}

void CPfCashe::OpenNew()
{
	int nfiles = 0;
	CString szFile = GetLastFile(nfiles);
	if(nfiles == 1)
	{
		m_pf.EmptyFile();
		
		return; //只有当前的一个
	}
	m_pf.CloseAndDelFile(); //关闭和删除当前

	// 找新的最后一个文件
	szFile = GetLastFile(nfiles);
	if (szFile.IsEmpty())
	{
		SYSTEMTIME syst;
		::GetLocalTime(&syst);
		szFile.Format("%04d%02d%02d-%02d%02d%02d.pgf",syst.wYear,syst.wMonth,syst.wDay,
			syst.wHour,syst.wMinute,syst.wSecond);
	}

	m_pf.Open(m_szPath + szFile);
	GetLogger()->AddInfoFmt("打开新的可变类型页面文件<%s>!",m_szPath + szFile);
}

////////////////////////////////////////////////////////////////////////
CLocalCashe::CLocalCashe()
{
	m_szPath = "";
}

CLocalCashe::~CLocalCashe()
{
	m_compcashe.SaveLastVal(&m_memcashe); //存储最后一个,即压缩缓冲区中不够样本值条件的强制变成样本指

	if(!m_memcashe.IsEmpty())
	{
		m_memcashe.SaveToDisk(m_szPath); //存储到磁盘
		m_memcashe.Reset();
	}
}

void CLocalCashe::AddRealVal(LPCTSTR sname,int ncomptype,double dblcompval,LPVVAL pval)//添加基本类型实时值
{
	CSafeLock lock(&m_lock); //加锁

	CBaseVal bv;
	if(m_compcashe.AddReal(sname,ncomptype,dblcompval,pval,&bv))
	{
		m_memcashe.AddTagVal(&bv);
		if(m_memcashe.IsFull())
		{
			m_memcashe.SaveToDisk(m_szPath);
			m_memcashe.Reset();
		}
	}
}

void CLocalCashe::AddRealVar(LPCTSTR sname,CVarVal *pval)//写String,Blob 到cashe
{
	CSafeLock lock(&m_lock); //加锁

	if((pval->vt != DT_STRING)&&(pval->vt != DT_BLOB))
		return;
	CPfRecVal rv;
	rv.m_ltime = pval->ltime;
	rv.m_ntype = (pval->vt == DT_STRING)? PFILE_STRING : PFILE_BLOB;
	rv.m_snqa = pval->snqa;
	rv.m_szdes.Format("%s",sname);
	rv.SetData(pval->pstr,pval->lvlen);

	if(!m_pfcashe.SaveRec(&rv))
		TRACE("CLocalCashe::AddRealVar Err!\n");
}

void CLocalCashe::AddSoe(CSoe *psoe)//写SOE到cashe
{
	CSafeLock lock(&m_lock); //加锁

	CPfRecVal rv;
	rv.m_ltime = psoe->m_ltime;
	rv.m_ntype = PFILE_SOE;
	rv.m_snqa = (short)psoe->m_lmsec; //质量保存的毫秒
	rv.m_dwres = psoe->m_lclsid;	  // m_dwres保存的分类

	rv.m_szdes.Format("%s",psoe->m_ssrc); //描述保存的来源
	
	CMyByteArray ua;
	ua.PutData(psoe,sizeof(TSOE) - 4);
	ua.PutData(psoe->m_pbinargs,psoe->m_lbinarglen);

	int n=0;
	BYTE *pb = ua.GetBuf(n);
	rv.SetData(pb,n);

	if(!m_pfcashe.SaveRec(&rv))
		TRACE("CLocalCashe::AddSoe Err!\n");
}
void CLocalCashe::SetPath(LPCTSTR lpszPath){
	m_szPath = lpszPath;
	m_pfcashe.Init(m_szPath + "PageFile\\"); //页面文件
}; //设置目录


int  CLocalCashe::GetLastRecs(CTagVal* lprecs,int nsize)//取最新的cashe数据用于cashe发送,返回个数
{
	CSafeLock lock(&m_lock); //加锁
	return m_memcashe.GetLastRecs(lprecs,nsize);
}

void CLocalCashe::GetPosOk()//确认去发送缓存
{
	CSafeLock lock(&m_lock); //加锁
	m_memcashe.GetPosOk();
}

//h用于读后发送成功后删除记录用
int	CLocalCashe::GetLastPfRec(PF_HANDLE &h, CPfRecVal* prec)
{
	CSafeLock lock(&m_lock); //加锁
	return m_pfcashe.GetLastRec(h,prec);
};

int	CLocalCashe::GetFirstPfRec(PF_HANDLE &h,CPfRecVal* prec)
{
	CSafeLock lock(&m_lock); //加锁
	return m_pfcashe.GetFirstRec(h,prec);
}
void CLocalCashe::DeletePfRec(PF_HANDLE h)
{
	CSafeLock lock(&m_lock); //加锁
	m_pfcashe.DeleteRec(h);//删除记录
}

//打开新的页面文件,用于缓存页面文件发送完成后打开一个新的
void CLocalCashe::OpenNewPgeFile()
{
	CSafeLock lock(&m_lock); //加锁
	m_pfcashe.OpenNew();
}

//从磁盘读取最新的
BOOL CLocalCashe::LoadFromDisk()
{
	CSafeLock lock(&m_lock); //加锁
	return m_memcashe.LoadFromDisk(m_szPath);
}