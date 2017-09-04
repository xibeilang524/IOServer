#include "StdAfx.h"
#include "DecVal.h"
#include "Expression/ExpressionTagNameParser.h"

CDecVal::CDecVal(void)
{
	int nHashSize = ::CalcHashTableSize(100000); // 按10w标签点的规模设计
	m_map.InitHashTable( nHashSize );
	m_mapd2t.InitHashTable( nHashSize );
	m_array.SetSize(0,4096);
	m_nposval = 0;
	m_nposvar = 0;
	m_id = 0;
	m_szDecName = "";
	m_bvardataok = FALSE;
};

CDecVal::CDecVal(long lid,int ntags)
{
	int nHashSize = ::CalcHashTableSize(ntags);
	m_map.InitHashTable(nHashSize);
	m_mapd2t.InitHashTable(nHashSize);//漏了这一句，导致DDC负荷很重
	m_array.SetSize(0,ntags);
	m_id = lid;
	m_nposval = 0;
	m_nposvar = 0;

	m_szDecName = "";
};
CDecVal::~CDecVal(void)
{
	//删除标签表
	m_map.RemoveAll();
	int i,n = m_array.GetSize();
	for(i=0;i<n;i++)
		delete m_array.GetAt(i);
	m_array.RemoveAll();

	//删除映射
	CString szkey;
	CDec2TagVal *pd2t, *pd2tNext;
	POSITION pos = m_mapd2t.GetStartPosition();
	while(pos != NULL)
	{
		m_mapd2t.GetNextAssoc(pos,szkey,pd2t);

		while(pd2t)
		{
			pd2tNext = pd2t->m_pNext;
			delete pd2t;

			pd2t = pd2tNext;
		}
	}
	m_mapd2t.RemoveAll();
}
CTagVal* CDecVal::GetTagVal(LPCTSTR lpszName)//取标签值指针
{
	CTagVal* pt = NULL;
	if(m_map.Lookup(lpszName,pt))
		return pt;
	return NULL;
}; //只在计算回调中使用
void CDecVal::Init(CDrvItem* pdec)//初始化
{
	//逐个加入标签
	CString szkey="",sztags = "",sztg="";
	//LPCTSTR pc;
	CDec2TagVal* pd2t;
	CTagVal* ptv;
	CTagItem* ptag=NULL,*ptg = NULL;

	m_szDecName = pdec->m_szDecName;//设备名称
	int i,ntags = pdec->m_array.GetSize();

	//先扫描没有公式的标签
	for(i=0;i<ntags;i++)
	{
		ptag = (CTagItem*)pdec->m_array.GetAt(i);
		//查找是否存在
		if(m_map.Lookup(ptag->m_szName,ptv))
			continue;
		ptag->m_szexpcp.TrimLeft();
		ptag->m_szexpcp.TrimRight();

		if(!ptag->m_szexpcp.IsEmpty()) //有公式
			continue;

		//新建CTagVal
		ptv = new CTagVal(ptag->m_szName,ptag->m_sznamedb,ptag->m_szexpcp,ptag->m_ntype,ptag->m_nCompType,ptag->m_szCompVal);
		if(ptv == NULL)
			continue;
		
		//加入CDecVal
		m_map.SetAt(ptv->m_szName,ptv);
		m_array.Add(ptv);

		//加入映射，第一个映射总是自身
		pd2t = NULL;
		if(!m_mapd2t.Lookup(ptag->m_sznamedrv,pd2t))
		{
			pd2t = new CDec2TagVal(ptag->m_sznamedrv);
			if(pd2t)
			{
				pd2t->m_tab.Add(ptv);
				m_mapd2t.SetAt(ptag->m_sznamedrv,pd2t);
			}
		}
		else if (pd2t) { // 已经存在
			CDec2TagVal* pd2t2 = new CDec2TagVal(ptag->m_sznamedrv);
			if(pd2t2)
			{
				pd2t2->m_tab.Add(ptv);
				pd2t->AddItem(pd2t2);
			}
		}
	}

	//再扫描有公式的
	for(i=0;i<ntags;i++)
	{
		ptag = (CTagItem*)pdec->m_array.GetAt(i);
		//查找是否存在
		if(m_map.Lookup(ptag->m_szName,ptv))
			continue;
		ptag->m_szexpcp.TrimLeft();
		ptag->m_szexpcp.TrimRight();

		if(ptag->m_szexpcp.IsEmpty()) //无公式，前面已经处理
			continue;

		//新建CTagVal
		ptv = new CTagVal(ptag->m_szName,ptag->m_sznamedb,ptag->m_szexpcp,ptag->m_ntype,ptag->m_nCompType,ptag->m_szCompVal);
		if(ptv == NULL)
			continue;
		
		//加入CDecVal
		m_map.SetAt(ptv->m_szName,ptv);
		m_array.Add(ptv);

		//加入映射，第一个映射总是自身
		pd2t = NULL;
		if(!m_mapd2t.Lookup(ptag->m_sznamedrv,pd2t))
		{
			pd2t = new CDec2TagVal(ptag->m_sznamedrv);
			if(pd2t)
			{
				pd2t->m_tab.Add(ptv);
				m_mapd2t.SetAt(ptag->m_sznamedrv,pd2t);
			}
		}
		else if (pd2t) { // 已经存在
			CDec2TagVal* pd2t2 = new CDec2TagVal(ptag->m_sznamedrv);
			if(!pd2t2)
				continue;

			pd2t2->m_tab.Add(ptv);
			pd2t->AddItem(pd2t2);			

			pd2t = pd2t2;
		}

		CExpressionTagNameParser parser;
		parser.Parse(ptag->m_szexpcp);
		for (int j = 0; j < parser.GetCount(); j++)
		{
			sztg = parser.GetAt(j);

			// 是不是标签自己？
			if (sztg == _T("__this__"))
				sztg = ptag->m_szName;

			if(pdec->m_map.Lookup(sztg,ptg))
			{
				//加入映射，从第二个开始
				/*
				if(!m_mapd2t.Lookup(ptg->m_sznamedrv,pd2t))
				{
					pd2t = new CDec2TagVal(ptg->m_sznamedrv);
					if(pd2t)
						m_mapd2t.SetAt(ptg->m_sznamedrv,pd2t);
				}
				if(pd2t)
					pd2t->m_tab.Add(ptv);
				*/
				if(pd2t)
					pd2t->m_tab.Add(ptv);
			}
		}
		//解析出公式中的标签名
		/*
		sztags = g_parsetvtagname(ptag->m_szexpcp);
		pc = sztags;
		if(pc)
		{
			sztg = "";
			while(*pc != 0)
			{
				if(*pc != '\n')
					sztg += *pc;//sztg是标签名，不是驱动标签名
				else
				{
					//找到驱动标签名作为触发条件
					if(pdec->m_map.Lookup(sztg,ptg))
					{
						//加入映射
						if(!m_mapd2t.Lookup(ptg->m_sznamedrv,pd2t))
						{
							pd2t = new CDec2TagVal(ptg->m_sznamedrv);
							if(pd2t)
								m_mapd2t.SetAt(ptg->m_sznamedrv,pd2t);
						}
						if(pd2t)
							pd2t->m_tab.Add(ptv);
					}
					sztg = ""; //这里少了一句
				}
				pc++;
			}
		}
		*/
	}

#if 0
	POSITION pos = m_mapd2t.GetStartPosition();
	while(pos)
	{
		m_mapd2t.GetNextAssoc(pos, szkey, pd2t);
		if (pd2t->m_tab.GetSize() > 1)
		{
			for (int i = 1; i < pd2t->m_tab.GetSize(); i++)
			{
				GetLogger()->AddInfoFmt(_T("发现触发标签%s -> %s"), pd2t->m_szname, pd2t->m_tab[i]->m_szNamedb);
			}
		}
	}
#endif
}

void CDecVal::OnDecDataOK(T_DRREC recs[],int nsize) //添加设备值，用于在回调函数中使用，加锁
{
	//先找到CDec2TagVal
	CDec2TagVal *pd2t;
	int i;
	CString szdrvname;
	for(i=0;i<nsize;i++)
	{
		recs[i].sname[79] = 0;
		if(!m_mapd2t.Lookup(recs[i].sname,pd2t))
			continue;

#if 0
		if ((strcmp(recs[i].sname, "REAL_MW") == 0) && ((recs[i].val.fval < 100.0) || (recs[i].val.fval > 400.0)))
		{
			CTime tf(recs[i].val.ltime);
			GetLogger()->AddInfoFmt(_T("CDecVal::OnDecDataOK(%d, %d)收到标签<REAL_MW>：值 = %f, 时标 = %s"), nsize, i, (double)recs[i].val.fval, tf.Format(_T("%Y-%m-%d %H:%M:%S")));
		}

#endif

		while(pd2t)
		{
			pd2t->OnDrvTagDataOk(this,&recs[i]);
			pd2t = pd2t->GetNext();
		}
	}
}

DWORD CDecVal::OnWriteHistoryCB(T_DRREC recs[],int nsize)
{
	return 0;
}

int  CDecVal::GetSaveRecs(CTagVal* lprecs,int nsize)//取存盘数据
{
	int n = m_array.GetSize();
	if(n == 0)
		return 0;
	if(m_nposval >= n)
		m_nposval = 0;
	int ndo=0,nh = m_nposval;
	CTagVal* pv=NULL;
	long ltime = (long)::time(NULL);
	int nCounter = 0;
	do{
		++nCounter;
		if ((nCounter % 10000) == 0)
		{
			Sleep(1);
			nCounter = 0;
		}
		pv = m_array.GetAt(m_nposval);

		if(pv->m_val.ltime > 0 )
		{
			if((pv->m_nType >DT_VOID) && (pv->m_nType < DT_STRING)) 
			{
				if(pv->m_nUpdate)
				{
					*lprecs = *pv;
					pv->m_nUpdate = 0;

					pv->m_lsendtime = ltime;
					lprecs->m_bresend = FALSE;

					ndo++;
					lprecs++;
				}
				else
				{
					if((pv->m_lsendtime > 0)&&((ltime - pv->m_lsendtime) > 300) )//5分钟强制重发
					{
						pv->m_lsendtime = ltime;
						*lprecs = *pv;
						lprecs->m_bresend = TRUE;
						ndo++;
						lprecs++;
					}
				}
			}	
		}
		m_nposval = (m_nposval + 1) % n;
	}while((nh != m_nposval) && (ndo < nsize));
	return ndo;
}

BOOL CDecVal::GetSaveVarVal(CTagVal* lprecs)//取存盘的可变数据，string blob
{
	if(!m_bvardataok)
		return FALSE;
	int n = m_array.GetSize();
	if(n == 0)
		return FALSE;
	if(m_nposvar >= n)
		m_nposvar = 0;
	int nh = m_nposvar;
	CTagVal* pv=NULL;
	long ltime = (long)::time(NULL);
	int nCounter = 0;
	do{
		++nCounter;
		if ((nCounter % 10000) == 0)
		{
			Sleep(1);
			nCounter = 0;
		}
		pv = m_array.GetAt(m_nposvar);
		if(pv->m_val.ltime > 0 )
		{
			if((pv->m_nType == DT_STRING) || (pv->m_nType == DT_BLOB))
			{
				if(pv->m_nUpdate)
				{
					*lprecs = *pv;
					pv->m_nUpdate = 0;

					pv->m_lsendtime = ltime;
					lprecs->m_bresend = FALSE;

					m_nposvar = (m_nposvar + 1) % n;
					return TRUE;
				}
				else
				{
					if((pv->m_lsendtime > 0)&&((ltime - pv->m_lsendtime) > 300) )//5分钟强制重发
					{
						pv->m_lsendtime = ltime;
						*lprecs = *pv;
						lprecs->m_bresend = TRUE;

						m_nposvar = (m_nposvar + 1) % n;
						return TRUE;
					}
				}
			}	
		}
		m_nposvar = (m_nposvar + 1) % n;
	}while(nh != m_nposvar);

	m_bvardataok = FALSE;
	return FALSE;
}