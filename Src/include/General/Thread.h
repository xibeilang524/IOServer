#ifndef __THREAD_H__
#define __THREAD_H__

TRUNK_NAMESPACE_BEGIN

class CThread;

/// 线程运行器
class _GENERAL_CLASS IThreadWorker
{
public:
	friend CThread;

protected:
	/// 线程运行之前初始化事件
	virtual BOOL OnInitThread(CThread *pThread) = 0;
	/// 线程退出事件
	virtual void OnShutdownThread(CThread *pThread) = 0;
	/// 线程运行函数，如果OnRunThread返回FALSE 线程将退出
	virtual BOOL OnRunThread(CThread *pThread) = 0;
};

/// 线程类封装
class _GENERAL_CLASS CThread
{
public:
	friend IThreadWorker;

	enum {
		tsThreadInit,		// 线程初始化状态，还未运行
		tsThreadFree,		// 线程空闲状态
		tsThreadSuspend,	// 线程暂停状态 
		tsThreadBusy,		// 线程忙碌状态
		tsThreadClosing,	// 线程正在关闭
		tsThreadClose		// 线程关闭状态
	};

public:
	CThread(LPCTSTR lpszName = NULL, IThreadWorker *pThreadWorker = NULL);
	~CThread();

	/// 获取线程句柄
	HANDLE GetHandle() const { 
		return m_hThread; 
	}
	/// 获取线程ID
	UINT GetThreadID() const { 
		return m_nThreadID; 
	}
	/// 获取线程名称，如果创建了多个线程实例，则可以通过设置不同的线程名称来进行区分
	LPCTSTR GetThreadName() const {
		return m_szThreadName;
	}
	/// 设置线程名称，如果lpszName==NULL，则会将线程名称设置为空
	void SetThreadName(LPCTSTR lpszName) {
		m_szThreadName = lpszName;
	}
	/// 读取线程的当前状态
	int GetThreadStatus() { return m_nThreadStatus; }
	/// 设置线程的当前状态
	void SetThreadStatus(int nThreadStatus) { m_nThreadStatus = nThreadStatus; }
	/*
	*	Priority                        Meaning
	*
	*   THREAD_PRIORITY_ABOVE_NORMAL	Priority 1 point above the priority class. 
	*   THREAD_PRIORITY_BELOW_NORMAL	Priority 1 point below the priority class. 
	*   THREAD_PRIORITY_HIGHEST			Priority 2 points above the priority class. 
	*   THREAD_PRIORITY_IDLE			Base priority of 1 for IDLE_PRIORITY_CLASS, BELOW_NORMAL_PRIORITY_CLASS, NORMAL_PRIORITY_CLASS, ABOVE_NORMAL_PRIORITY_CLASS, or HIGH_PRIORITY_CLASS processes, and a base priority of 16 for REALTIME_PRIORITY_CLASS processes. 
	*   THREAD_PRIORITY_LOWEST			Priority 2 points below the priority class. 
	*   THREAD_PRIORITY_NORMAL			Normal priority for the priority class. 
	*   THREAD_PRIORITY_TIME_CRITICAL	Base priority of 15 for IDLE_PRIORITY_CLASS, BELOW_NORMAL_PRIORITY_CLASS, NORMAL_PRIORITY_CLASS, ABOVE_NORMAL_PRIORITY_CLASS, or HIGH_PRIORITY_CLASS processes, and a base priority of 31 for REALTIME_PRIORITY_CLASS processes. 
	 */
	/// 获取线程优先级别
	int GetThreadPriority( ) { 
		return ::GetThreadPriority( m_hThread ); 
	}
	/// 设置线程优先级别
	BOOL SetThreadPriority(int nPriority) { 
		return ::SetThreadPriority( m_hThread, nPriority );
	}
	/// 是否自动删除线程对象
	BOOL IsAutoDelete() const { 
		return m_bAutoDelete; 
	}
	/// 设置是否自动删除线程对象，如果为TRUE，则线程退出时，线程对象CThread会自动被删除
	void SetAutoDelete(BOOL bAutoDelete) { 
		m_bAutoDelete = bAutoDelete; 
	}
	/// 获取用户数据
	DWORD GetUserData() {
		return m_dwUserData;
	}
	/// 设置用户数据
	void SetUserData(DWORD dwUserData) {
		m_dwUserData = dwUserData;
	}

public:
	/// 启动线程并执行
	BOOL Start(IThreadWorker *pThreadWorker = NULL, void *pSecurity = NULL, DWORD dwStackSize = NULL);
	/// 暂停线程执行
	BOOL SuspendThread() {
		if (m_hThread != INVALID_HANDLE_VALUE)
			return (::SuspendThread(m_hThread) != 0);
		else
			return FALSE;
	}
	/// 恢复线程执行
	BOOL ResumeThread() {
		if (m_hThread != INVALID_HANDLE_VALUE)
			return (::ResumeThread(m_hThread) != 0);
		else
			return FALSE;
	}
	/// 通知线程退出，但线程并不会马上退出，该函数只是设置了一个线程退出的事件信号
	void Shutdown();
	/// 等待线程退出，如果该函数返回为TRUE，表示线程退出成功，否则线程还未真正退出
	BOOL WaitForShutdown(DWORD dwTimeoutMillis = INFINITE);
	/// 关闭线程句柄和相关退出通知信号，如果WaitForShutdown()函数返回为TRUE，则会自动调用该函数，不需要再调用
	void Close();
	/// 强制退出线程
	BOOL Terminate(DWORD dwExitCode = 0);


protected:
	/// 查询退出线程事件信号是否被设置
	inline BOOL IsShutdownning() {
		if (::WaitForSingleObject(m_hShutdownEvent, 0) == WAIT_OBJECT_0)
			return TRUE;
		else
			return FALSE;
	}

private:
	/// 线程运行函数
	int Run();

	/// 线程回调函数
	static unsigned int __stdcall ThreadFunction(void *pParam);

	/// 线程句柄
	HANDLE			m_hThread;
	/// 线程ID
	UINT			m_nThreadID;
	/// 线程实例名称
	CString			m_szThreadName;
	/// 是否自动删除线程实例
	BOOL			m_bAutoDelete;
	/// 线程退出事件句柄
	HANDLE			m_hShutdownEvent;
	/// 线程工作器指针
	IThreadWorker	*m_pThreadWorker;
	/// 线程工作状态
	int				m_nThreadStatus;
	/// 用户数据
	DWORD			m_dwUserData;

	// No copies do not implement
	CThread(const CThread &rhs);
	CThread &operator=(const CThread &rhs);
};

// 线程池管理类
class _GENERAL_CLASS CThreadPool
{
public:
	typedef CList<CThread*, CThread*> CThreadList;

public:
	/// 构造函数，dwMaxThreadCount==0表示没有最大线程数限制
	CThreadPool(DWORD dwMaxThreadCount = 0);
	~CThreadPool();

public:
	DWORD GetThreadCount() { return (DWORD)m_listThreads.GetCount(); }
	DWORD GetMaxThreadCount() { return m_dwMaxThreadCount; }
	DWORD GetThreadStackSize() { return m_dwStackSize; }
	void SetThreadStackSize(DWORD dwStackSize) { m_dwStackSize = dwStackSize; }

public:
	/// 开始线程池
	void Start(LPCTSTR lpszThreadName, IThreadWorker *pThreadWorker, DWORD dwMaxThreadCount = 0, DWORD dwInitCount = 10);

	/// 新加一个线程
	CThread *AddThread(LPCTSTR lpszThreadName, IThreadWorker *pThreadWorker);

	/// 删除一个线程，如果pThread == NULL，则从列表最后删除一个线程
	BOOL RemoveThread(CThread *pThread = NULL);

	/// 暂停线程池
	BOOL SuspendThreadPool();

	/// 恢复线程池执行
	BOOL ResumeThreadPool();

	/// 通知线程退出，但线程并不会马上退出，该函数只是通知线程线程
	void Shutdown();

	/// 等待线程退出，如果该函数返回为TRUE，表示线程退出成功，否则线程还未真正退出
	BOOL WaitForShutdown(DWORD dwTimeoutMillis = INFINITE);

protected:

private:
	/// 线程安全锁
	CCriticalSection m_csLock;

	/// 最大允许线程数
	DWORD m_dwMaxThreadCount;

	/// 线程池列表
	CThreadList m_listThreads;

	/// 线程堆栈大小
	DWORD m_dwStackSize;
};

TRUNK_NAMESPACE_END

#endif // end of __THREAD_H__