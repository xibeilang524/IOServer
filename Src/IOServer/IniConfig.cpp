#include "StdAfx.h"
#include "IniConfig.h"



CIniConfig::CIniConfig()
{
	m_szProjectName = "RTDBProj";//工程名
	m_bAutoRun = FALSE;
	m_bSaveLog = TRUE;
	m_bExitPass = TRUE;

	m_bCashe = TRUE;
	m_bSkipSameTime = FALSE;
	//默认是主DDC，服务端口12092
	m_wheartport = 12092;
	m_szHeartIP = "";

	m_nTimeZero = 8;
	m_lAdjustSec = 0;

	m_nupthreads = 4;
	m_nLogLevel = LOG_LEVEL_INFO;
	m_bWaitTcpAck = TRUE;
	m_nUdpSendDelay = 1;
}
CIniConfig::~CIniConfig()
{

}

void CIniConfig::OnDoKeyVal(LPCTSTR lpszBlkName,LPCTSTR lpszKeyName,LPCTSTR lpszKeyVal)
{
	//local
	if(0 == _stricmp(lpszBlkName,"local"))
	{
		if(0 == _stricmp(lpszKeyName,"projectname"))
		{
			m_szProjectName = lpszKeyVal;
		}
		else if(0 == _stricmp(lpszKeyName,"autorun"))
		{
			if(0 == _stricmp(lpszKeyVal,"yes"))
				m_bAutoRun = TRUE;
			else
				m_bAutoRun = FALSE;
		}
		else if(0 == _stricmp(lpszKeyName,"savelog"))
		{
			if(0 == _stricmp(lpszKeyVal,"yes"))
				m_bSaveLog = TRUE;
			else
				m_bSaveLog = FALSE;
		}
		else if(0 == _stricmp(lpszKeyName,"exitpassword"))
		{
			if(0 == _stricmp(lpszKeyVal,"yes"))
				m_bExitPass = TRUE;
			else
				m_bExitPass = FALSE;
		}	

		else if(0 == _stricmp(lpszKeyName,"cashe"))
		{
			if(0 == _stricmp(lpszKeyVal,"yes"))
				m_bCashe = TRUE;
			else
				m_bCashe = FALSE;
		}
		else if(0 == _stricmp(lpszKeyName,"skipsametime"))
		{
			if(0 == _stricmp(lpszKeyVal,"yes"))
				m_bSkipSameTime = TRUE;
			else
				m_bSkipSameTime = FALSE;
		}	

		else if(0 == _stricmp(lpszKeyName,"timezero"))//时区
		{
			m_nTimeZero = atoi(lpszKeyVal);
			if((m_nTimeZero < -12) || (m_nTimeZero > 12))
				m_nTimeZero = 0;

			//计算
			TIME_ZONE_INFORMATION   tzi;  
			GetTimeZoneInformation(&tzi);  //获取本地时区
			m_lAdjustSec = m_nTimeZero * 3600 + tzi.Bias * 60; //计算调整时间秒
		}
		else if(0 == _stricmp(lpszKeyName,"upthread")) //
		{
			m_nupthreads = atoi(lpszKeyVal);
			if((m_nupthreads < 1) || (m_nupthreads > 10))
				m_nupthreads = 1;
		}
		else if(0 == _stricmp(lpszKeyName,"loglevel")) //
		{
			m_nLogLevel = atoi(lpszKeyVal);
			if (m_nLogLevel < LOG_LEVEL_ERROR)
				m_nLogLevel = LOG_LEVEL_ERROR;
			if (m_nLogLevel > LOG_LEVEL_DEBUG)
				m_nLogLevel = LOG_LEVEL_DEBUG;
		}
		else if(0 == _stricmp(lpszKeyName,"title")) //
		{
			m_szTitle = lpszKeyVal;
		}
		else if(0 == _stricmp(lpszKeyName,"wait_tcp_ack")) //
		{
			if(0 == _stricmp(lpszKeyVal,"yes"))
				m_bWaitTcpAck = TRUE;
			else
				m_bWaitTcpAck = FALSE;
		}
		else if(0 == _stricmp(lpszKeyName,"udp_send_delay")) //
		{
			m_nUdpSendDelay = atoi(lpszKeyVal);
			if (m_nUdpSendDelay < 0)
				m_nUdpSendDelay = 0;
			if (m_nUdpSendDelay > 50)
				m_nUdpSendDelay = 50;
		}		
	}
	else if(0 == _stricmp(lpszBlkName,"heart"))
	{
		if(0 == _stricmp(lpszKeyName,"heartport"))
		{
			m_wheartport = (WORD)atoi(lpszKeyVal);
			if(m_wheartport == 0)
				m_wheartport = 12092;
		}	
		else if(0 == _stricmp(lpszKeyName,"heartip"))
		{
			if(0 == _stricmp(lpszKeyVal,"NULL"))
				m_szHeartIP = "";
			else
				m_szHeartIP = lpszKeyVal;
		}	
	}
}
