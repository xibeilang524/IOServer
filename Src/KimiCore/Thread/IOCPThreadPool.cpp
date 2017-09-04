#include "stdafx.h"
#include "KimiCore.h"
#include "Thread\IOCPThreadPool.h"


_KIMI_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////
/// CThreadRequest - 线程池请求类
//////////////////////////////////////////////////////////////////////////

CThreadRequest::CThreadRequest()
	: m_pThreadPool(NULL)
	, m_pThread(NULL)
	, m_pNext(NULL)
	, m_pPrev(NULL)
{
}

CThreadRequest::~CThreadRequest()
{

}

//////////////////////////////////////////////////////////////////////////
/// CProcessorThread - 线程池的工作线程
//////////////////////////////////////////////////////////////////////////

CProcessorThread::CProcessorThread(CIOCPThreadPool* pThreadPool)
	: m_pThreadPool(pThreadPool)
{
}

CProcessorThread::~CProcessorThread()
{

}

BOOL CProcessorThread::OnInitThread(CThread *pThread)
{
	m_pThreadPool->OnThreadStart(this);
	return TRUE;
}

void CProcessorThread::OnShutdownThread(CThread *pThread)
{
	m_pThreadPool->OnThreadExit(this);
}

BOOL CProcessorThread::OnRunThread(CThread *pThread)
{
	DWORD dwBytes = 0;
	ULONG_PTR dwCompKey = 0;
	CThreadRequest* pThreadRequest = NULL;

	if (!::GetQueuedCompletionStatus(m_pThreadPool->GetHandle(), &dwBytes, &dwCompKey, (LPOVERLAPPED*)&pThreadRequest, INFINITE))
		return FALSE;

	// 不是有效的请求，忽略
	if (dwCompKey != (ULONG_PTR)m_pThreadPool)
		return TRUE;

	// 收到线程退出标志，跳出循环
	if ((ULONG_PTR)pThreadRequest == SHUTDOWN_FLAG)
		return FALSE;

	if (pThreadRequest != NULL)
	{
		pThreadRequest->SetThreadPool(m_pThreadPool);
		pThreadRequest->SetThread(this);
		pThreadRequest->Execute();

		m_pThreadPool->RemoveRequest(pThreadRequest);
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
/// CIOCPThreadPool - 基于IOCP的线程池类
//////////////////////////////////////////////////////////////////////////

CIOCPThreadPool::CIOCPThreadPool(int nNumberOfThreads/* = 0*/)
	: m_nNumberOfThreads(nNumberOfThreads)
	, m_nNumberOfWorkThreads(0)
	, m_hHandle(0)
	, m_nPendingRequest(0)
	, m_bShutdowned(TRUE)
	, m_pWorkThreads(NULL)
	, m_pThreadHandles(NULL)
	, m_pRequestHead(NULL)
	, m_pRequestTail(NULL)
	, m_nMaxFreeRequest(0)
	, m_nFreeRequest(0)
	, m_pFreeRequest(NULL)
{
	//::InitializeCriticalSection(&m_crit);
	::InitializeCriticalSectionAndSpinCount(&m_csLock, 0x80000400);
	::InitializeCriticalSectionAndSpinCount(&m_csFreeRequest, 0x80000400);
}

CIOCPThreadPool::~CIOCPThreadPool()
{
	Shutdown();
	::DeleteCriticalSection(&m_csLock);
	::DeleteCriticalSection(&m_csFreeRequest);
}

CThreadRequest* CIOCPThreadPool::AllocThreadRequest()
{
	CThreadRequest* pThreadRequest = NULL;

	::EnterCriticalSection(&m_csFreeRequest);

	if (m_pFreeRequest)
	{
		pThreadRequest = m_pFreeRequest;
		m_pFreeRequest = m_pFreeRequest->GetNext();
		m_nFreeRequest--;
	}
	else {
		pThreadRequest = CreateThreadRequest();
	}

	::LeaveCriticalSection(&m_csFreeRequest);

	pThreadRequest->SetNext(NULL);
	pThreadRequest->SetPrev(NULL);

	return pThreadRequest;
}

void CIOCPThreadPool::ReleaseThreadRequest(CThreadRequest* pThreadRequest)
{
	::EnterCriticalSection(&m_csFreeRequest);

	if (m_nFreeRequest >= m_nMaxFreeRequest)
	{
		delete pThreadRequest;
	}
	else {
		pThreadRequest->SetThreadPool(NULL);
		pThreadRequest->SetThread(NULL);
		pThreadRequest->SetNext( m_pFreeRequest );
		m_pFreeRequest = pThreadRequest;
		m_nFreeRequest++;
	}

	::LeaveCriticalSection(&m_csFreeRequest);
}

void CIOCPThreadPool::RemoveRequest(CThreadRequest* pThreadRequest)
{
	// 待处理任务队列使用双向链表，可减少删除任务的时间花销
	::EnterCriticalSection(&m_csLock);

	CThreadRequest* pNext = pThreadRequest->GetNext();
	CThreadRequest* pPrev = pThreadRequest->GetPrev();

	if (pNext)
	{
		if (pPrev)
		{
			// 中间的某个节点
			pPrev->SetNext(pNext);
			pNext->SetPrev(pPrev);
		}
		else {
			// 是首个节点
			pNext->SetPrev(NULL);
			m_pRequestHead = pNext;
		}
	}
	else {
		if (pPrev)
		{
			// 是最后一个节点
			pPrev->SetNext(NULL);
			m_pRequestTail = pPrev;
		}
		else {
			// 是当前双向链表的唯一节点
			m_pRequestHead = NULL;
			m_pRequestTail = NULL;
		}
	}

	::LeaveCriticalSection(&m_csLock);

	InterlockedDecrement(&m_nPendingRequest);

	// 将该任务对象放到可用列表队列中
	ReleaseThreadRequest(pThreadRequest);
}

BOOL CIOCPThreadPool::AddRequest(CThreadRequest* pThreadRequest)
{
	if (m_hHandle == 0)
		return FALSE;

	::EnterCriticalSection(&m_csLock);
	if (m_pRequestHead)
	{
		pThreadRequest->SetNext(m_pRequestHead);
		pThreadRequest->SetPrev(NULL);
		m_pRequestHead->SetPrev(pThreadRequest);

		m_pRequestHead = pThreadRequest;
	}
	else {
		pThreadRequest->SetNext(NULL);
		pThreadRequest->SetPrev(NULL);
		m_pRequestHead = pThreadRequest;
		m_pRequestTail = pThreadRequest;
	}

	::LeaveCriticalSection(&m_csLock);


	BOOL bResult = ::PostQueuedCompletionStatus(m_hHandle, 0, (ULONG_PTR)this, (LPOVERLAPPED)pThreadRequest);
	if (bResult)		
		InterlockedIncrement(&m_nPendingRequest);

	return bResult;
}

BOOL CIOCPThreadPool::Startup()
{
	if (m_hHandle != 0)
		return FALSE;

	int nNumberOfWorkThreads = 0;
	if (m_nNumberOfThreads == 0)
	{
		SYSTEM_INFO sysinfo;
		::GetSystemInfo(&sysinfo);

		nNumberOfWorkThreads = sysinfo.dwNumberOfProcessors * 2;
	}
	else {
		nNumberOfWorkThreads = min(m_nNumberOfThreads, 64); // WaitForMultipleObjects()最多可同时等待64个对象
	}

	// 创建完成端口
	// NumberOfConcurrentThreads = 0 表示每个CPU保持一个并发线程
	m_hHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	if (m_hHandle == INVALID_HANDLE_VALUE)
	{
		// IocpThreadPool创建IOCP对象失败
		return FALSE;
	}

	m_pWorkThreads = new CProcessorThread*[nNumberOfWorkThreads];
	memset(m_pWorkThreads, 0, sizeof(CProcessorThread*) * nNumberOfWorkThreads);

	m_pThreadHandles = new HANDLE[nNumberOfWorkThreads];
	memset(m_pThreadHandles, 0, sizeof(HANDLE*) * nNumberOfWorkThreads);

	m_nNumberOfWorkThreads = 0;
	// 创建所有工作线程
	for (int i = 0; i < nNumberOfWorkThreads; i++)
	{
		CProcessorThread *pThread = new CProcessorThread(this);
		if (pThread)
		{
			m_pWorkThreads[m_nNumberOfWorkThreads] = pThread;
			m_pThreadHandles[m_nNumberOfWorkThreads] = pThread->GetHandle();
			m_nNumberOfWorkThreads++;

			pThread->Start();
		}
	}

	m_bShutdowned = FALSE;

	return TRUE;
}

void CIOCPThreadPool::Shutdown()
{
	if (m_hHandle == 0)
		return;

	m_bShutdowned = TRUE;

	// 给所有工作线程发送退出命令
	for (int i = 0; i < m_nNumberOfWorkThreads; i++)
	{
		::PostQueuedCompletionStatus(m_hHandle, 0, (ULONG_PTR)this, (LPOVERLAPPED)SHUTDOWN_FLAG);
		::SleepEx(10, TRUE);
	}

	// 等待工作线程结束
	::WaitForMultipleObjects(m_nNumberOfWorkThreads, m_pThreadHandles, TRUE, INFINITE);

	// 释放线程对象
	for (int i = 0; i < m_nNumberOfWorkThreads; i++)
	{
		delete m_pWorkThreads[i];
	}
	delete[] m_pWorkThreads;
	m_pWorkThreads = NULL;

	delete[] m_pThreadHandles;
	m_pThreadHandles = NULL;

	// 关闭完成端口句柄
	::CloseHandle(m_hHandle);
	m_hHandle = 0;

	// 释放线程池的任务
	::EnterCriticalSection(&m_csLock);
	CThreadRequest* pThreadRequest = m_pRequestHead;
	CThreadRequest* pNext = NULL;
	while(pThreadRequest)
	{
		pNext = pThreadRequest->GetNext();
		delete pThreadRequest;
		pThreadRequest = pNext;
	}
	m_pRequestHead = NULL;
	m_pRequestTail = NULL;
	::LeaveCriticalSection(&m_csLock);

	::EnterCriticalSection(&m_csFreeRequest);
	pThreadRequest = m_pFreeRequest;
	while(pThreadRequest)
	{
		pNext = pThreadRequest->GetNext();
		delete pThreadRequest;
		pThreadRequest = pNext;
	}
	m_pFreeRequest = NULL;
	m_nFreeRequest = 0;
	::LeaveCriticalSection(&m_csFreeRequest);
}

_KIMI_NAMESPACE_END