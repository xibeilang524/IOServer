#include "StdAfx.h"
#include <process.h>
#include "GeneralAPI.h"
#include "General\Thread.h"
#include "General\safelock.h"

TRUNK_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////
// CThread
//////////////////////////////////////////////////////////////////////

CThread::CThread(LPCTSTR lpszName/* = NULL*/, IThreadWorker *pThreadWorker/* = NULL*/)
  : m_hThread(INVALID_HANDLE_VALUE),
	m_nThreadID(0),
    m_bAutoDelete(FALSE),
	m_hShutdownEvent(::CreateEvent(NULL, TRUE, FALSE, NULL)),
	m_pThreadWorker(pThreadWorker),
	m_szThreadName((lpszName == NULL) ? _T("") : lpszName),
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
}

BOOL CThread::Start(IThreadWorker *pThreadWorker/* = NULL*/, void *pSecurity/* = NULL*/, DWORD dwStackSize/* = NULL*/)
{
	if (pThreadWorker != NULL)
		m_pThreadWorker = pThreadWorker;
	if (m_pThreadWorker == NULL)
		return FALSE;

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
	if (m_pThreadWorker == NULL)
		return -1;

	if (m_pThreadWorker->OnInitThread(this))
	{
		while (TRUE)
		{
			// 是否需要退出线程？
			if (IsShutdownning())
				break;

			if (!m_pThreadWorker->OnRunThread(this))
				break;
		}

		m_pThreadWorker->OnShutdownThread(this);
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

//////////////////////////////////////////////////////////////////////////
// CThreadPool - 线程池管理类
//////////////////////////////////////////////////////////////////////////

CThreadPool::CThreadPool(DWORD dwMaxThreadCount/* = 0*/)
:	m_dwMaxThreadCount(dwMaxThreadCount),
	m_dwStackSize(0)
{
}

CThreadPool::~CThreadPool()
{

}

void CThreadPool::Start(LPCTSTR lpszThreadName, IThreadWorker *pThreadWorker, DWORD dwMaxThreadCount/* = 0*/, DWORD dwInitCount/* = 10*/)
{
	CSafeLock lock(&m_csLock);

	// 已经初始化过了
	if (m_listThreads.GetCount() > 0)
		return;

	m_dwMaxThreadCount = dwMaxThreadCount;
	if ((dwInitCount > dwMaxThreadCount) && (dwMaxThreadCount > 0))
		dwInitCount = dwMaxThreadCount;

	CThread *pThread;

	// 创建线程池
	for (int i = 0; i < (int)dwInitCount; i++)
	{
		pThread = new CThread(lpszThreadName);
		if (pThread)
		{
			pThread->SetAutoDelete(FALSE);
			m_listThreads.AddTail(pThread);
		}
	}

	// 启动线程
	POSITION pos = m_listThreads.GetHeadPosition();
	while(pos)
	{
		pThread = m_listThreads.GetNext(pos);
		pThread->Start(pThreadWorker, NULL, m_dwStackSize);
	}
}

CThread *CThreadPool::AddThread(LPCTSTR lpszThreadName, IThreadWorker *pThreadWorker)
{
	CSafeLock lock(&m_csLock);

	if ((m_dwMaxThreadCount > 0) && ((DWORD)m_listThreads.GetCount() >= m_dwMaxThreadCount))
		return NULL;

	CThread *pThread = new CThread(lpszThreadName);
	if (pThread)
	{
		pThread->SetAutoDelete(FALSE);
		pThread->Start(pThreadWorker, NULL, m_dwStackSize);
		m_listThreads.AddTail(pThread);
	}

	return pThread;
}

BOOL CThreadPool::RemoveThread(CThread *pThread/* = NULL*/)
{
	CSafeLock lock(&m_csLock);

	if (m_listThreads.GetCount() == 0)
		return FALSE;

	POSITION pos;

	if (pThread == NULL)
	{
		pThread = m_listThreads.RemoveTail();
		pThread->Shutdown();
		pThread->WaitForShutdown();
		delete pThread;

		return TRUE;
	}
	else {
		pos = m_listThreads.GetHeadPosition();
		while(pos)
		{
			if (pThread == m_listThreads.GetNext(pos))
			{
				pThread = m_listThreads.RemoveTail();
				pThread->Shutdown();
				pThread->WaitForShutdown();
				delete pThread;
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CThreadPool::SuspendThreadPool()
{
	CSafeLock lock(&m_csLock);

	CThread *pThread;
	POSITION pos = m_listThreads.GetHeadPosition();
	while(pos)
	{
		pThread = m_listThreads.GetNext(pos);
		pThread->SuspendThread();
	}

	return TRUE;
}

BOOL CThreadPool::ResumeThreadPool()
{
	CSafeLock lock(&m_csLock);

	CThread *pThread;
	POSITION pos = m_listThreads.GetHeadPosition();
	while(pos)
	{
		pThread = m_listThreads.GetNext(pos);
		pThread->ResumeThread();
	}

	return TRUE;
}

void CThreadPool::Shutdown()
{
	CSafeLock lock(&m_csLock);

	CThread *pThread;
	POSITION pos = m_listThreads.GetHeadPosition();
	while(pos)
	{
		pThread = m_listThreads.GetNext(pos);
		pThread->Shutdown();
	}
}

BOOL CThreadPool::WaitForShutdown(DWORD dwTimeoutMillis/* = INFINITE*/)
{
	CSafeLock lock(&m_csLock);

	int nCount = (int)m_listThreads.GetCount();

	POSITION *pDelPos = new POSITION[nCount];
	CThread **pDelThreads = new CThread*[nCount];
	int nDelCount = 0;

	CThread *pThread;
	POSITION posOld;
	POSITION pos = m_listThreads.GetHeadPosition();
	while(pos)
	{
		posOld = pos;
		pThread = m_listThreads.GetNext(pos);
		if (!pThread->WaitForShutdown(dwTimeoutMillis)) // 超时
			break;
		else {
			pDelPos[nDelCount] = posOld;
			pDelThreads[nDelCount] = pThread;

			nDelCount++;
			nCount--;
		}
	}

	// 删除线程
	for (int i = 0; i < nDelCount; i++)
	{
		m_listThreads.RemoveAt(pDelPos[i]);
		delete pDelThreads[i];
	}

	delete[] pDelPos;
	delete[] pDelThreads;

	return (nCount == 0);	
}

TRUNK_NAMESPACE_END