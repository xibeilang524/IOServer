#ifndef __KIMI_KERNEL_IOCP_THREAD_POOL_H__
#define __KIMI_KERNEL_IOCP_THREAD_POOL_H__

// 使用IOCP实现的线程池，效率远高于普通的线程池

_KIMI_NAMESPACE_BEGIN

#define SHUTDOWN_FLAG	((DWORD_PTR)-1)

class CIOCPThreadPool;
class CProcessorThread;

/// 线程池请求类
class KIMI_CORE_CLASS CThreadRequest
{
public:
	CThreadRequest();
	virtual ~CThreadRequest();

	CIOCPThreadPool* GetThreadPool() {
		return m_pThreadPool;
	}
	void SetThreadPool(CIOCPThreadPool* pThreadPool) {
		m_pThreadPool = pThreadPool;
	}

	CProcessorThread* GetThread() {
		return m_pThread;
	}
	void SetThread(CProcessorThread* pThread) {
		m_pThread = pThread;
	}
	CThreadRequest* GetNext() {
		return m_pNext;
	}
	void SetNext(CThreadRequest* pThreadRequest) {
		m_pNext = pThreadRequest;
	}
	CThreadRequest* GetPrev() {
		return m_pPrev;
	}
	void SetPrev(CThreadRequest* pThreadRequest) {
		m_pPrev = pThreadRequest;
	}

public:
	// 线程池工作函数，继承这个方法填写自己的线程代码
	virtual void Execute() = 0;

protected:
private:
	CIOCPThreadPool*	m_pThreadPool;
	CProcessorThread*	m_pThread;
	CThreadRequest*		m_pNext;
	CThreadRequest*		m_pPrev;
};

/// 线程池的工作线程
class CProcessorThread : public CThread
{
public:
	CProcessorThread(CIOCPThreadPool* pThreadPool);
	virtual ~CProcessorThread();

	CIOCPThreadPool* GetThreadPool() {
		return m_pThreadPool;
	}
	void SetThreadPool(CIOCPThreadPool* pThreadPool) {
		m_pThreadPool = pThreadPool;
	}

public:
	/// 线程运行之前初始化事件
	BOOL OnInitThread(CThread *pThread);
	/// 线程退出事件
	void OnShutdownThread(CThread *pThread);
	/// 线程运行函数，如果OnRunThread返回FALSE 线程将退出
	BOOL OnRunThread(CThread *pThread);

protected:
private:
	CIOCPThreadPool*	m_pThreadPool;
};

/// 基于IOCP的线程池类
class KIMI_CORE_CLASS CIOCPThreadPool
{
public:
	friend CProcessorThread;

public:
	CIOCPThreadPool(int nNumberOfThreads = 0);
	virtual ~CIOCPThreadPool();

	HANDLE GetHandle() {
		return m_hHandle;
	}
	BOOL IsShutdowned() {
		return m_bShutdowned;
	}
	long GetPendingRequest() {
		return m_nPendingRequest;
	}
	int GetMaxFreeRequest() {
		return m_nMaxFreeRequest;
	}
	void SetMaxFreeRequest(int nMaxFreeRequest) {
		m_nMaxFreeRequest = nMaxFreeRequest;
	}
	int GetFreeRequest() {
		return m_nFreeRequest;
	}

	CThreadRequest* AllocThreadRequest();

	virtual BOOL AddRequest(CThreadRequest* pThreadRequest);
	BOOL Startup();
	void Shutdown();

protected:
	void RemoveRequest(CThreadRequest* pThreadRequest);
	void ReleaseThreadRequest(CThreadRequest* pThreadRequest);

	// 创建CThreadRequest的函数
	virtual CThreadRequest* CreateThreadRequest() = 0;

	// 线程启动事件
	virtual void OnThreadStart(CProcessorThread* pThread) = 0;

	// 线程结束事件
	virtual void OnThreadExit(CProcessorThread* pThread) = 0;
private:
	HANDLE					m_hHandle;
	int						m_nNumberOfThreads;
	int						m_nNumberOfWorkThreads;
	CProcessorThread**		m_pWorkThreads;
	HANDLE*					m_pThreadHandles;
	CThreadRequest*			m_pRequestHead;
	CThreadRequest*			m_pRequestTail;
	volatile long			m_nPendingRequest;
	BOOL					m_bShutdowned;
	CRITICAL_SECTION		m_csLock;

private:
	int						m_nMaxFreeRequest;
	int						m_nFreeRequest;
	CThreadRequest*			m_pFreeRequest;
	CRITICAL_SECTION		m_csFreeRequest;
};

_KIMI_NAMESPACE_END

#endif // end of __KIMI_KERNEL_IOCP_THREAD_POOL_H__