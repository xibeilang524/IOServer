#ifndef __KIMI_KERNEL_THREAD_H__
#define __KIMI_KERNEL_THREAD_H__

_KIMI_NAMESPACE_BEGIN

/// 线程类封装
class KIMI_CORE_CLASS CThread
{
public:

	enum {
		tsThreadInit,		// 线程初始化状态，还未运行
		tsThreadFree,		// 线程空闲状态
		tsThreadSuspend,	// 线程暂停状态 
		tsThreadBusy,		// 线程忙碌状态
		tsThreadClosing,	// 线程正在关闭
		tsThreadClose		// 线程关闭状态
	};

public:
	CThread(LPCTSTR lpszName = NULL);
	virtual ~CThread();

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
		if (m_szThreadName)
			free(m_szThreadName);
		m_szThreadName = _tcsdup( (lpszName == NULL) ? _T("") : lpszName );
	}
	/// 读取线程的当前状态
	int GetThreadStatus() { return m_nThreadStatus; }
	/// 设置线程的当前状态
	void SetThreadStatus(int nThreadStatus) { m_nThreadStatus = nThreadStatus; }

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
	/// 线程运行之前初始化事件
	virtual BOOL OnInitThread(CThread *pThread) {return TRUE;};
	/// 线程退出事件
	virtual void OnShutdownThread(CThread *pThread){return;} ;
	/// 线程运行函数，如果OnRunThread返回FALSE 线程将退出
	virtual BOOL OnRunThread(CThread *pThread) {return TRUE;};
	/// 启动线程并执行
	BOOL Start( void *pSecurity = NULL, DWORD dwStackSize = NULL);
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
	TCHAR*			m_szThreadName;
	/// 是否自动删除线程实例
	BOOL			m_bAutoDelete;
	/// 线程退出事件句柄
	HANDLE			m_hShutdownEvent;
	/// 线程工作器指针
	//IThreadWorker	*m_pThreadWorker;
	/// 线程工作状态
	int				m_nThreadStatus;
	/// 用户数据
	DWORD			m_dwUserData;
	
	// No copies do not implement
	CThread(const CThread &rhs);
	CThread &operator=(const CThread &rhs);
};

_KIMI_NAMESPACE_END

#endif // end of __KIMI_KERNEL_THREAD_H__