#include "stdafx.h"
#include <process.h>
#include "KimiCore.h"
#include "Thread\Thread.h"


_KIMI_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////
// CThread
//////////////////////////////////////////////////////////////////////

CThread::CThread(LPCTSTR lpszName/* = NULL*/)
			: m_hThread(INVALID_HANDLE_VALUE),
			m_nThreadID(0),
			m_bAutoDelete(FALSE),
			m_hShutdownEvent(::CreateEvent(NULL, TRUE, FALSE, NULL)),
			m_szThreadName( _tcsdup( (lpszName == NULL) ? _T("") : lpszName ) ),
			m_nThreadStatus(tsThreadInit),
			m_dwUserData(0)
{
}

CThread::~CThread()
{
	if (m_hThread != INVALID_HANDLE_VALUE)
		::CloseHandle(m_hThread);

	if (m_hShutdownEvent != INVALID_HANDLE_VALUE)
		::CloseHandle(m_hShutdownEvent);

	if (m_szThreadName)
		free(m_szThreadName);
	m_szThreadName = NULL;
}

BOOL CThread::Start(void *pSecurity/* = NULL*/, DWORD dwStackSize/* = NULL*/)
{
	if (m_hThread == INVALID_HANDLE_VALUE)
	{
		m_nThreadStatus = tsThreadFree;
		m_hThread = (HANDLE)::_beginthreadex(pSecurity, dwStackSize, ThreadFunction, (void*)this, 0, &m_nThreadID);

		if (m_hThread == INVALID_HANDLE_VALUE)
		{
			m_nThreadStatus = tsThreadInit;
			return FALSE;
		}
		else
			return TRUE;
	}
	else
	{
		return TRUE;
	}
}

void CThread::Shutdown()
{
	if (m_hThread != INVALID_HANDLE_VALUE)
	{
		m_nThreadStatus = tsThreadClosing;
		::SetEvent(m_hShutdownEvent);
	}
}

BOOL CThread::WaitForShutdown(DWORD dwTimeoutMillis/* = INFINITE*/)
{
	if (m_hThread == INVALID_HANDLE_VALUE)
		return TRUE;

	if (::WaitForSingleObject(m_hThread, dwTimeoutMillis) == WAIT_OBJECT_0)
	{
		Close();
		return TRUE;
	}
	else
		return FALSE;
}

void CThread::Close()
{
	if (m_hThread != INVALID_HANDLE_VALUE)
		::ResetEvent(m_hShutdownEvent);

	m_nThreadID = 0;

	if (m_hThread != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(m_hThread);
		m_hThread = INVALID_HANDLE_VALUE;
	}

	m_nThreadStatus = tsThreadClose;
	m_dwUserData = 0;
}

BOOL CThread::Terminate(DWORD dwExitCode /* = 0 */)
{
	if (::TerminateThread(m_hThread, dwExitCode))
	{
		Close();
		return TRUE;
	}
	else
		return FALSE;
}

int CThread::Run()
{
	if (OnInitThread(this))
	{
		while (TRUE)
		{
			// 是否需要退出线程？
			if (IsShutdownning())
			{
				break;
			}

			if (!OnRunThread(this))
				break;
		}

		OnShutdownThread(this);
	}

	return 0;
}

unsigned int __stdcall CThread::ThreadFunction(void *pParam)
{
	int dwResult = 0;

	CThread* pThis = (CThread*)pParam;

	if (pThis)
	{
		try
		{
			dwResult = pThis->Run();

			if (pThis->m_bAutoDelete) 
				delete pThis;
		}
		catch(...)
		{
		}
	}

	return dwResult;
}

_KIMI_NAMESPACE_END