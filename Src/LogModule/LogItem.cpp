#include "StdAfx.h"
#include "LogItem.h"

using namespace KIMI;

//////////////////////////////////////////////////////////////////////////
// CLogItem - 日志信息项
//////////////////////////////////////////////////////////////////////////
CLogItem::CLogItem()
	: m_dwLogLevel(LOG_LEVEL_DEBUG)
	, m_dwLogOptType(LOG_OPT_ALL)
	//, m_szLogMessage(_T(""))
	, m_pNext(NULL)
	, m_nLogSize(0)
	, m_szLogMessage(NULL)
{

}

CLogItem::~CLogItem()
{
	if (m_szLogMessage)
		delete[] m_szLogMessage;
	m_szLogMessage = NULL;
	m_nLogSize = 0;
}

BOOL CLogItem::AllocBuffer(size_t nSize)
{
	if (m_nLogSize >= nSize)
		return (m_szLogMessage != NULL);

	if (m_szLogMessage)
		delete[] m_szLogMessage;

	m_nLogSize = (nSize + 31) >> 5 << 5; // 32字节对齐分配
	m_szLogMessage = new TCHAR[m_nLogSize];
	if (m_szLogMessage)
	{
		m_szLogMessage[0] = _T('\0');
		return TRUE;
	}
	else {
		m_nLogSize = 0;
		return FALSE;
	}
}

void CLogItem::SetLogMessage(LPCTSTR lpszLogMsg, DWORD dwLogLevel/* = LOG_LEVEL_INFO*/, DWORD dwLogOptType/* = LOG_OPT_ALL*/)
{
	m_dwLogLevel = dwLogLevel;
	if (m_dwLogLevel > LOG_LEVEL_DEBUG)
		m_dwLogLevel = LOG_LEVEL_DEBUG;

	m_dwLogOptType = dwLogOptType;
	if (m_dwLogOptType > LOG_OPT_ALL)
		m_dwLogOptType = LOG_OPT_ALL;

	SYSTEMTIME syst;
	::GetLocalTime(&syst);

	TCHAR szLogLevel[8];
	switch(m_dwLogLevel)
	{
	case LOG_LEVEL_ERROR:
		_tcscpy(szLogLevel, _T("错误"));
		break;
	case LOG_LEVEL_WARNING:
		_tcscpy(szLogLevel, _T("警告"));
		break;
	case LOG_LEVEL_INFO:
		_tcscpy(szLogLevel, _T("提示"));
		break;
	case LOG_LEVEL_DEBUG:
		_tcscpy(szLogLevel, _T("调试"));
		break;
	}

	// 当发生大量日志时，直接使用CString::Format()会造成内存不足错误
	// 这里为日志格式化使用独立的Heap，可以保证CString::Format()安全性

// 	m_szLogMessage.Format("[%04d-%02d-%02d %02d:%02d:%02d.%03d] [%s] %s",
// 		syst.wYear, syst.wMonth, syst.wDay, syst.wHour, syst.wMinute, syst.wSecond, syst.wMilliseconds,
// 		szLogLevel, lpszLogMsg);

	// 33为日志和报警级别字符总长度
	size_t nLen = 33 + _tcslen(lpszLogMsg) + 1;
	if (!AllocBuffer( nLen ))
		return;

	_stprintf_s(m_szLogMessage, m_nLogSize, _T("[%04d-%02d-%02d %02d:%02d:%02d.%03d] [%s] "),  
		syst.wYear, syst.wMonth, syst.wDay, syst.wHour, syst.wMinute, syst.wSecond, syst.wMilliseconds,	szLogLevel);
	_tcscat_s(m_szLogMessage, m_nLogSize, lpszLogMsg);

	/*
	CWin32Heap strHeap( HEAP_NO_SERIALIZE, 0, 0 );
	CAtlStringMgr strMgr( &strHeap );
	CString szLogMsg(&strMgr );

	szLogMsg.Format("[%04d-%02d-%02d %02d:%02d:%02d.%03d] [%s] %s",
		syst.wYear, syst.wMonth, syst.wDay, syst.wHour, syst.wMinute, syst.wSecond, syst.wMilliseconds,
		szLogLevel, lpszLogMsg);

	if (!AllocBuffer( szLogMsg.GetLength() + 1 ))
		return;

	strcpy_s(m_szLogMessage, m_nLogSize, szLogMsg);
	*/
}