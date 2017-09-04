#include "StdAfx.h"
#include "DdsTagTab.h"



//////////////////////////////////////////////////////////////
CDdsTagTab::CDdsTagTab()
{
	m_map.InitHashTable(77777);
	m_array.SetSize(0,10000);
}

CDdsTagTab::~CDdsTagTab()
{
	Delmap();
	Delarray();
}

void CDdsTagTab::Delmap()
{
	CString szkey;
	CDDSTI* ptag;
	POSITION pos = m_map.GetStartPosition();
	while(pos)
	{
		m_map.GetNextAssoc(pos,szkey,ptag);
		delete ptag;
	}
	m_map.RemoveAll();
}

void CDdsTagTab::Delarray()
{
	int i,n = m_array.GetSize();
	for(i=0;i<n;i++)
		delete m_array.GetAt(i);
	m_array.RemoveAll();
}

BOOL CDdsTagTab::IsExistInMap(LPCTSTR lpszname)
{
	CDDSTI* ptag;
	return m_map.Lookup(lpszname,ptag);
}

void CDdsTagTab::ReMap(LPVOID pObject) //重新建立map,当保存和读入工程时调用，用于和ECD工程中的标签表同步
{
	CSafeLock lock(&m_Lock);

	Delmap();

	CDDSTI* ptag;

	CDriverMgr* pDriverMgr  = (CDriverMgr*)pObject;
	CDrvItem* pdec = NULL;

	CTagItem* pi;
	int i,n;
	CString szkey="";
	POSITION pos = pDriverMgr->m_mapDec.GetStartPosition();
	while(pos)
	{
		pDriverMgr->m_mapDec.GetNextAssoc(pos,szkey,pdec);
		n = pdec->m_array.GetSize();
		for(i=0;i<n;i++)
		{
			pi = (CTagItem*)pdec->m_array.GetAt(i);
			if(IsExistInMap(pi->m_sznamedb))
				continue;
			ptag = new CDDSTI();
			if(ptag == NULL)
				continue;

			ptag->m_szname = pi->m_sznamedb;
			ptag->m_szdes = pi->m_szdes;
			ptag->m_szunit = pi->m_szunit;
			ptag->m_szcompval = pi->m_szCompVal;
			ptag->m_wcomptye = (WORD)pi->m_nCompType;
			ptag->m_wtype = (WORD)pi->m_ntype;
			m_map.SetAt(ptag->m_szname,ptag);
		}
	}
}

void CDdsTagTab::ReMakeArray()
{
	Delarray(); //清空数组

	CString szkey;
	CDDSTI* ptag,*pt;
	POSITION pos = m_map.GetStartPosition();
	while(pos)
	{
		m_map.GetNextAssoc(pos,szkey,ptag);
		pt = new CDDSTI;
		if(pt == NULL)
			continue;
		pt->m_szcompval = ptag->m_szcompval;
		pt->m_szdes = ptag->m_szdes;
		pt->m_szname = ptag->m_szname;
		pt->m_szunit = ptag->m_szunit;
		pt->m_wcomptye = ptag->m_wcomptye;
		pt->m_wtype = ptag->m_wtype;
		m_array.Add(pt);
	}
}

BOOL CDdsTagTab::GetSendData(CMyByteArray &uca,int &nindex) //取发送标签数据段
{
	CSafeLock lock(&m_Lock);

	uca.ClearData();//清除数据
	int n = m_array.GetSize();
	if((nindex >= n) && (nindex > 0))
	{
		Delarray(); //清空数组
		return FALSE;
	}
	if(nindex <= 0)
	{
		ReMakeArray();
		nindex = 0;
		n = m_array.GetSize();
	}

	//每次取500个标签
	int ng=0;
	CMKDDSVTAG vtag;
	while((nindex < n) && (ng < 500))
	{
		vtag.MakeCell(m_array.GetAt(nindex));
		uca.PutData(vtag.m_sbuf,vtag.m_nsize);
		ng++;
		nindex++;
	}
	return TRUE;
}


