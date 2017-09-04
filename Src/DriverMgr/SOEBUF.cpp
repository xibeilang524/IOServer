#include "StdAfx.h"
#include "SOEBUF.h"


CSOEBUF::CSOEBUF(void)
{
	m_nhead = 0;
	m_ntail = 0;
	memset(m_soe,0,SOEBUF_SIZE * sizeof(TSOE)); //初始化
}

CSOEBUF::~CSOEBUF(void)
{
	int i;
	for(i=0;i<SOEBUF_SIZE;i++)
		((CSoe*)&m_soe[i])->Clear();
}

BOOL CSOEBUF::Add(LPSOE psoe)
{
	if( ((m_ntail + 1) % SOEBUF_SIZE) == m_nhead)
		return FALSE; //满
	//加入尾部
	*((CSoe*)&m_soe[m_ntail]) = *((CSoe*)psoe);//赋值
	m_ntail = (m_ntail + 1) % SOEBUF_SIZE;
	return TRUE;
};
BOOL CSOEBUF::Get(CSoe* psoe)
{
	if(m_nhead == m_ntail) //空
		return FALSE;
	//从头出
	*psoe = *((CSoe*)&m_soe[m_nhead]);
	m_nhead = (m_nhead + 1) % SOEBUF_SIZE;
	return TRUE;
};