// readini.cpp
#include "stdafx.h"
#include "GeneralAPI.h"
#include "General\readini.h"

TRUNK_NAMESPACE_BEGIN

CReadIni::CReadIni()
{
	m_szLine = "";	//行
	m_szBlkName = ""; //块名
}
CReadIni::~CReadIni()
{

}

void CReadIni::DoLine()
{
	m_szLine.TrimLeft();
	if(m_szLine.IsEmpty())
		return;
	char c = m_szLine[0];
	if(c == ';')
		return; //注释行
	int i,n=m_szLine.GetLength();
	if(c == '[') //块
	{
		m_szBlkName = "";
		for(i=1;i<n;i++)
		{
			c = m_szLine[i];
			if(c == ']')
				break;
			else 
				m_szBlkName += c;
		}
		if(c != ']')
			m_szBlkName = "";
		return;
	}
	
	for(i=0;i<n;i++) //去掉行中注释
	{
		if(m_szLine[i] == ';')
		{
			m_szLine = m_szLine.Left(i);
			break;
		}
	}
	//键名和键值
	int npos = 0;
	CString szKeyName = "",szKeyVal = "";
	n=m_szLine.GetLength();
	for(i=0;i<n;i++)
	{
		c = m_szLine[i];
		if(c == '=')
			npos++;
		else
		{
			if(npos == 0)
				szKeyName += c;
			else
				szKeyVal += c;
		}
	}
	szKeyName.TrimLeft();
	szKeyName.TrimRight();
	szKeyVal.TrimLeft();
	szKeyVal.TrimRight();
	if((npos > 0)&&(!m_szBlkName.IsEmpty()))
		OnDoKeyVal(m_szBlkName,szKeyName,szKeyVal); //处理键值
}

BOOL CReadIni::ReadIniFile(LPCTSTR lpszFile)
{
	FILE* pf=NULL;
	pf = fopen(lpszFile,"rt");//文本模式打开
	if(pf == NULL)//不能打开文件!
		return FALSE;
	char c;
	m_szLine = "";
	while((c = fgetc( pf)) != EOF)
	{
		if(c != '\n')
			m_szLine += c;
		else
		{
			DoLine();
			m_szLine = "";
		}
	}
	if(!m_szLine.IsEmpty())
		DoLine();

	fclose(pf);

	return TRUE;
}


TRUNK_NAMESPACE_END