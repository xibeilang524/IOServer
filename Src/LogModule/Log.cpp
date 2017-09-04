#include "StdAfx.h"
#include "Log.h"

//////////////////////////////////////////////////////////////////////////
// CLog - 日志管理器
//////////////////////////////////////////////////////////////////////////

using namespace KIMI;

CLog* CLog::instance = NULL;

CLog* CLog::GetInstance()
{
	if(NULL == instance)
		instance = new CLog;
	return instance;
}
CLog::CLog()
:	m_nMaxLogCount(0),
	m_nLogCount(0),
	m_pHead(NULL),
	m_pTail(NULL),
	m_nMaxLogFreeCount(DEFAULT_MAX_LOG_FREE_COUNT),
	m_nLogFreeCount(0),
	m_pFreeHead(NULL),
	m_dwAllowLogLevel(LOG_LEVEL_INFO)
{
}

CLog::~CLog()
{
	//CSafeLock lock(&m_csLock);

	CLogItem *pNext = NULL;
	/*
	CLogItem *pItem = m_pHead;
	while(pItem)
	{
		pNext = pItem->m_pNext;
		delete pItem;
		pItem = pNext;
	}
	m_pHead = NULL;
	m_pTail = NULL;
	m_nLogCount = 0;
	*/
	CLogItem *pItem = GetLogItem();
	while(pItem)
	{
		FreeLogItem(pItem);
		pItem = GetLogItem();
	}

	pItem = m_pFreeHead;
	while(pItem)
	{
		pNext = pItem->m_pNext;
		delete pItem;
		pItem = pNext;
	}
	m_pFreeHead = NULL;
	m_nLogFreeCount = 0;
}

void CLog::AddSeparator(LPCTSTR lpszText/* = NULL*/)
{
	AddSeparator(LOG_OPT_ALL, lpszText);
}

void CLog::AddSeparator(DWORD dwLogOptType, LPCTSTR lpszText/* = NULL*/)
{
	CLogItem *pItem = GetFreeLogItem();
	ASSERT(pItem != NULL);
	if (!pItem)
		return;

	size_t nLen = (lpszText == NULL) ? 1 : _tcslen(lpszText) + 1;
	if (!pItem->AllocBuffer( nLen ))
		return;

	if (lpszText == NULL)
		pItem->m_szLogMessage[0] ='\0';
	else
		_tcscpy(pItem->m_szLogMessage, lpszText);

	pItem->m_dwLogLevel = LOG_LEVEL_ERROR;
	pItem->m_dwLogOptType = dwLogOptType;
	pItem->m_pNext = NULL;

	CSafeLock lock(&m_csLock);
	/*
	if (m_pTail)
	{
		m_pTail->m_pNext = pItem;
		m_pTail = pItem;
	}
	else {
		m_pHead = pItem;
		m_pHead->m_pNext = NULL;
		m_pTail = m_pHead;
	}
	*/

	pItem->m_pNext = NULL;
	if (!m_pHead)
	{
		m_pHead = pItem;
		m_pTail = pItem;
	}
	else 
	{
		m_pTail->m_pNext = pItem;
		m_pTail = pItem;
	}
	m_nLogCount++;
}

void CLog::AddLogMessage(LPCTSTR lpszLogMsg, DWORD dwLogLevel, DWORD dwLogOptType, BOOL bCheckLogLevel/* = TRUE*/)
{
	if ((m_nMaxLogCount > 0) && (m_nLogCount >= m_nMaxLogCount))
		return;

	if (bCheckLogLevel && (m_dwAllowLogLevel < dwLogLevel))
		return;

	CLogItem *pItem = GetFreeLogItem();
	ASSERT(pItem != NULL);
	if (!pItem)
		return;

	pItem->SetLogMessage(lpszLogMsg, dwLogLevel, dwLogOptType);

	CSafeLock lock(&m_csLock);
	/*
	if (m_pTail)
	{
		m_pTail->m_pNext = pItem;
		m_pTail = pItem;
	}
	else {
		m_pHead = pItem;
		m_pHead->m_pNext = NULL;
		m_pTail = m_pHead;
	}
	*/
	pItem->m_pNext = NULL;
	if (!m_pHead)
	{
		m_pHead = pItem;
		m_pTail = pItem;
	}
	else {
		m_pTail->m_pNext = pItem;
		m_pTail = pItem;
	}
	m_nLogCount++;
}

CLogItem *CLog::GetLogItem()
{
	CSafeLock lock(&m_csLock);

	CLogItem *pItem = NULL;
	if (m_pHead)
	{
		pItem = m_pHead;
		m_pHead = m_pHead->m_pNext;
		pItem->m_pNext = NULL;

		m_nLogCount--;
		if ((m_nLogCount == 0) || (m_pHead == NULL))
		{
			m_pHead = NULL;
			m_pTail = NULL;
			m_nLogCount = 0;
		}
	}

	return pItem;
}

void CLog::AddDebug(LPCTSTR lpszLogMessage)
{
	AddLogMessage(lpszLogMessage, LOG_LEVEL_DEBUG, LOG_OPT_ALL);
}

void CLog::AddDebugFmt(LPCTSTR lpszLogFormat, ...)
{
	TCHAR buf[MAX_LOG_MESSAGE_LEN];

	va_list args;
	va_start(args, lpszLogFormat);
	_vstprintf_s(buf, MAX_LOG_MESSAGE_LEN, lpszLogFormat, args);
	va_end(args);

	AddLogMessage(buf, LOG_LEVEL_DEBUG, LOG_OPT_ALL);
}

void CLog::AddDebug(DWORD dwLogOptType, LPCTSTR lpszLogMessage)
{
	AddLogMessage(lpszLogMessage, LOG_LEVEL_DEBUG, dwLogOptType);
}

void CLog::AddDebugFmt(DWORD dwLogOptType, LPCTSTR lpszLogFormat, ...)
{
	TCHAR buf[MAX_LOG_MESSAGE_LEN];

	va_list args;
	va_start(args, lpszLogFormat);
	_vstprintf_s(buf, MAX_LOG_MESSAGE_LEN, lpszLogFormat, args);
	va_end(args);

	AddLogMessage(buf, LOG_LEVEL_DEBUG, dwLogOptType);
}

void CLog::AddInfo(LPCTSTR lpszLogMessage)
{
	AddLogMessage(lpszLogMessage, LOG_LEVEL_INFO, LOG_OPT_ALL);
}

void CLog::AddInfoFmt(LPCTSTR lpszLogFormat, ...)
{
	TCHAR buf[MAX_LOG_MESSAGE_LEN];

	va_list args;
	va_start(args, lpszLogFormat);
	_vstprintf_s(buf, MAX_LOG_MESSAGE_LEN, lpszLogFormat, args);
	va_end(args);

	AddLogMessage(buf, LOG_LEVEL_INFO, LOG_OPT_ALL);
}

void CLog::AddInfo(DWORD dwLogOptType, LPCTSTR lpszLogMessage)
{
	AddLogMessage(lpszLogMessage, LOG_LEVEL_INFO, dwLogOptType);
}

void CLog::AddInfoFmt(DWORD dwLogOptType, LPCTSTR lpszLogFormat, ...)
{
	TCHAR buf[MAX_LOG_MESSAGE_LEN];

	va_list args;
	va_start(args, lpszLogFormat);
	_vstprintf_s(buf, MAX_LOG_MESSAGE_LEN, lpszLogFormat, args);
	va_end(args);

	AddLogMessage(buf, LOG_LEVEL_INFO, dwLogOptType);
}

void CLog::AddWarning(LPCTSTR lpszLogMessage)
{
	AddLogMessage(lpszLogMessage, LOG_LEVEL_WARNING, LOG_OPT_ALL);
}

void CLog::AddWarningFmt(LPCTSTR lpszLogFormat, ...)
{
	TCHAR buf[MAX_LOG_MESSAGE_LEN];

	va_list args;
	va_start(args, lpszLogFormat);
	_vstprintf_s(buf, MAX_LOG_MESSAGE_LEN, lpszLogFormat, args);
	va_end(args);

	AddLogMessage(buf, LOG_LEVEL_WARNING, LOG_OPT_ALL);
}

void CLog::AddWarning(DWORD dwLogOptType, LPCTSTR lpszLogMessage)
{
	AddLogMessage(lpszLogMessage, LOG_LEVEL_WARNING, dwLogOptType);
}

void CLog::AddWarningFmt(DWORD dwLogOptType, LPCTSTR lpszLogFormat, ...)
{
	TCHAR buf[MAX_LOG_MESSAGE_LEN];

	va_list args;
	va_start(args, lpszLogFormat);
	_vstprintf_s(buf, MAX_LOG_MESSAGE_LEN, lpszLogFormat, args);
	va_end(args);

	AddLogMessage(buf, LOG_LEVEL_WARNING, dwLogOptType);
}

void CLog::AddError(LPCTSTR lpszLogMessage)
{
	AddLogMessage(lpszLogMessage, LOG_LEVEL_ERROR, LOG_OPT_ALL);
}

void CLog::AddErrorFmt(LPCTSTR lpszLogFormat, ...)
{
	TCHAR buf[MAX_LOG_MESSAGE_LEN];

	va_list args;
	va_start(args, lpszLogFormat);
	_vstprintf_s(buf, MAX_LOG_MESSAGE_LEN, lpszLogFormat, args);
	va_end(args);

	AddLogMessage(buf, LOG_LEVEL_ERROR, LOG_OPT_ALL);
}

void CLog::AddError(DWORD dwLogOptType, LPCTSTR lpszLogMessage)
{
	AddLogMessage(lpszLogMessage, LOG_LEVEL_ERROR, dwLogOptType);
}

void CLog::AddErrorFmt(DWORD dwLogOptType, LPCTSTR lpszLogFormat, ...)
{
	TCHAR buf[MAX_LOG_MESSAGE_LEN];

	va_list args;
	va_start(args, lpszLogFormat);
	_vstprintf_s(buf, MAX_LOG_MESSAGE_LEN, lpszLogFormat, args);
	va_end(args);

	AddLogMessage(buf, LOG_LEVEL_ERROR, dwLogOptType);
}

void CLog::FreeLogItem(CLogItem *pItem)
{
	if (!pItem)
		return;

	CSafeLock lock(&m_csLock);

	if ((m_nMaxLogFreeCount > 0) && (m_nLogFreeCount >= m_nMaxLogFreeCount))
		delete pItem;
	else 
	{
		pItem->m_pNext = m_pFreeHead;
		m_pFreeHead = pItem;
		m_nLogFreeCount++;
	}
}

CLogItem *CLog::GetFreeLogItem()
{
	CSafeLock lock(&m_csLock);

	CLogItem *pItem = NULL;
	if (m_pFreeHead)
	{
		pItem = m_pFreeHead;
		m_pFreeHead = m_pFreeHead->m_pNext;
		pItem->m_pNext = NULL;
		m_nLogFreeCount--;
	}
	else 
	{
		pItem = new CLogItem();
	}

	return pItem;
}

CString CLog::InitLogPath(LPCTSTR lpszPath, LPCTSTR lpszModule)
{
	if(!lpszPath)
		return FALSE;
	m_szLogPath.Format(_T("%s\\logs\\"), lpszPath);
	m_writelog.Init(m_szLogPath, lpszModule);
	return m_szLogPath;
}
BOOL CLog::Write2Disk(CString &szContent)
{
	return m_writelog.WriteStr(szContent);
}