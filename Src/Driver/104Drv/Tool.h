#pragma once

#include <afxmt.h>

#define  _DELETE(x)   { delete x; x= NULL;}

class CTimeSpanDif
{
public:
	FILETIME		m_ftStartTime;				// 开始时间
	FILETIME		m_ftEndTime;				// 终止时间
	int                     m_iStatus;                   // 状态
public:

	CTimeSpanDif()
	{
		::CoFileTimeNow(&m_ftStartTime);;
	}

	long  GetElapsedTime()
	{
		ULONGLONG qst = *((ULONGLONG*)(&m_ftStartTime));
		ULONGLONG qet = *((ULONGLONG*)(&m_ftEndTime));
		return (long)((qet - qst) / 10000);
	}

	BOOL CheckUpdatePeriod(UINT lTime)
	{	
		::CoFileTimeNow(&m_ftEndTime);
		int iTime = GetElapsedTime();
		if (GetElapsedTime() < lTime)
		{
			Sleep(100);
			return TRUE;
		}
		memcpy(&m_ftStartTime, &m_ftEndTime, sizeof(FILETIME));
		return FALSE;
	}

	BOOL CheckUpdatePeriod(UINT lTime, int iStatus)
	{	
		::CoFileTimeNow(&m_ftEndTime);
		if (GetElapsedTime() < lTime|| m_iStatus != iStatus)
		{
			Sleep(100);
			return FALSE;
		}
		memcpy(&m_ftStartTime, &m_ftEndTime, sizeof(FILETIME));
		return TRUE;
	}
};

