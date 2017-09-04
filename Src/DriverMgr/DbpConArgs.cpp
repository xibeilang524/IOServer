#include "StdAfx.h"
#include "DbpConArgs.h"

CDbpConArgs::CDbpConArgs()
{
	m_nsize = 0;
	m_szfile = "";
}
CDbpConArgs::~CDbpConArgs()
{
}
BOOL CDbpConArgs::LoadConArgs()//读入配置参数
{
	CFile fl;
	if(!fl.Open(m_szfile,CFile::modeRead))
		return FALSE;
	int i,n=0;
	long lflag = 0;//0XDF1279AC;
	try
	{
		fl.Read(&lflag,4);
		if ((lflag != 0xDF1279AC) && (lflag != 0xDF1279AD))
			return FALSE;

		m_nsize = 0;
		fl.Read(&n,4);
		for(i=0;i<n && i < MAX_DBP_CONARGS;i++)
		{
			memset(&m_args[m_nsize],0,sizeof(TDBPARG));
			if(sizeof(TDBPARG) != fl.Read(&m_args[m_nsize],sizeof(TDBPARG) ) )
				break;
			m_args[m_nsize].sip[15] = 0;
			m_args[m_nsize].spass[15] = 0;
			m_args[m_nsize].suser[15] = 0;

			// 对于老版本，强制使用TCP模式
			if (lflag == 0xDF1279AC)
			{
				m_args[m_nsize].shNetMode = NET_MODE_TCP;
			}
			m_nsize++;
		}
	}
	catch ( CFileException *ex ) 
	{
		ex->Delete();
		return FALSE;
	}
	return TRUE;
}
BOOL CDbpConArgs::SaveConArgs()//保存配置参数
{
	CFile fl;
	if(!fl.Open(m_szfile,CFile::modeCreate|CFile::modeWrite))
		return FALSE;
	int i;
	long lflag = 0xDF1279AD; //0xDF1279AC;
	try
	{
		fl.Write(&lflag,4);
		fl.Write(&m_nsize,4);
		for(i=0;i<m_nsize;i++)
			fl.Write(&m_args[i],sizeof(TDBPARG));
	}
	catch ( CFileException *ex ) 
	{
		ex->Delete();
		return FALSE;
	}
	return TRUE;
}
