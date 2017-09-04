#include "StdAfx.h"
#include "TagVal.h"
#include "Define.h"

///////////////////////////////////////////////////////////////////
CTagVal::CTagVal()
{
	m_szName="";
	m_szNamedb = "";
	m_szExpcp = "";
	m_nCompType = CASHE_COMP_NONE;	//压缩方式
	m_szCompVal = "";	//压缩率

	m_val.Init(DT_VOID);
	m_nType = (DT_VOID);
	m_nUpdate = 0;

	m_dblcompval = 0.01;

	m_pEngine = NULL;
}
CTagVal::CTagVal(LPCTSTR lpszName,LPCTSTR lpszNameRdb,LPCTSTR lpszExpcp,int ntype,int ncomptype,LPCTSTR scompval)
{
	m_szName=lpszName;
	m_szNamedb = lpszNameRdb;
	m_szExpcp = lpszExpcp;
	m_nType = ntype;
	m_nCompType = ncomptype;
	m_szCompVal = scompval;
	m_val.Init(ntype);

	m_nUpdate = 0;
	m_lsendtime = 0;
	m_bresend = FALSE;
	m_dblcompval = atof(scompval); //先转化已被使用

	m_pEngine = NULL;
}

CTagVal::CTagVal(int ntype)
{
	m_szName="";
	m_szNamedb = "";
	m_szExpcp = "";

	m_nType = ntype;
	m_nCompType = CASHE_COMP_NONE;	//压缩方式
	m_szCompVal = "";	//压缩率
	m_val.Init(ntype);
	m_nUpdate = 0;
	m_lsendtime = 0;
	m_bresend = FALSE;

	m_dblcompval = 0.01;

	m_pEngine = NULL;
}

CTagVal& CTagVal::operator=(const CTagVal& tv) //重载=符号,完全拷贝
{
	m_szName = tv.m_szName;
	m_szNamedb = tv.m_szNamedb;
	m_szExpcp = tv.m_szExpcp;
	m_nType = tv.m_nType;
	m_nCompType = tv.m_nCompType;
	m_szCompVal = tv.m_szCompVal;
	m_nUpdate = tv.m_nUpdate;
	m_dblcompval = tv.m_dblcompval;

	m_lsendtime = tv.m_lsendtime;
	m_bresend = tv.m_bresend;

	m_val.ResetType(DT_VOID);
	m_val = tv.m_val;

	return *this;
}

CTagVal::~CTagVal()
{
	if (m_pEngine != NULL)
		delete m_pEngine;
	m_pEngine = NULL;
}