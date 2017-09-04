#ifndef __TPRI_FIBER_H__
#define __TPRI_FIBER_H__

#include <map>
#include <list>
#include <time.h>

TPRI_NAMESPACE_BEGIN

class CFiberScheduler;
class CFiber;

/// 纤程运行器
class TPRICOMMON_CLASS IFiberWorker
{
public:
	friend CFiber;
	IFiberWorker() : m_pFiber(NULL) { }
	CFiber *GetFiber() { return m_pFiber; }
	CFiberScheduler *GetScheduler();

public:
	virtual void OnRunFiber() = 0;

private:
	CFiber *m_pFiber;
};

/// 纤程封装类
class TPRICOMMON_CLASS CFiber
{
public:
	enum
	{
		NONE,           //仅仅让出处理器
		WAIT4EVENT = 1, //等待某事件的来临
		WAIT4EVENTTIMEOUT,
		DEAD,           //纤程已死亡
		ACTIVED,        //可运行的
		SLEEP,
	};
	friend CFiberScheduler;
	friend IFiberWorker;
public:
	CFiber();
	CFiber(IFiberWorker *pFiberWorker, SIZE_T stack = 0);
	virtual ~CFiber(void);

	int GetFiberID() { return m_nFiberID; }
	BYTE GetStatus() { return m_btStatus; }
	int GetWaitEvent() { return m_nWaitEvent; }
	LPVOID GetFiber() { return m_pFiber; }

public:
	BOOL Create(IFiberWorker *pFiberWorker, SIZE_T stack = 0);
	BOOL ConvertThreadToFiber();
	void Attach(LPVOID p);
	LPVOID Detach();
	void Run();
	//等待的事件到达了，将纤程重新插入到可运行队列中
	void OnEvent(int ev);

protected:
	static void WINAPI OnRunFiber(LPVOID pvParam);

private:
	int m_nFiberID;
	BYTE m_btStatus;
	LPVOID	m_pFiber;
	int m_nWaitEvent;//一个纤程只能等待在一个事件上
	CFiberScheduler *m_pScheduler;
};

#define MAX_FIBER 1024

/// 纤程调度器

class TPRICOMMON_CLASS CFiberScheduler
{
public:
	friend CFiber;
public:
	CFiberScheduler();
	~CFiberScheduler();

public:
	void Init() {
		m_pFiberContext = ::ConvertThreadToFiber(NULL);
	}
	void Destroy();
	int Spawn(IFiberWorker *pFiberWorker, SIZE_T stack = 0);

	//选择一个纤程以进行调度
	void Schedule();

	void Sleep(time_t timeout)
	{
		if(timeout > 0)
		{
			m_pFibers[m_nCurrentUID]->m_btStatus = CFiber::SLEEP;
			time_t t = timeout + time(NULL);
			m_sleepList.push_back(std::make_pair(t,m_pFibers[m_nCurrentUID]));
		}

		::SwitchToFiber(m_pFiberContext);

	}

	//将一个纤程添加到可运行队列中
	void AddToActive(CFiber *ut)
	{
		ut->m_btStatus = CFiber::ACTIVED;
		m_pendingAdd.push_back(ut);
	}

	//将当运行权交给scheduler
	void yield()
	{
		//将运行权交给调度器
		::SwitchToFiber(m_pFiberContext);
	}

	//阻塞在ev上,timeout==0将永远等待
	int Block(int ev,time_t timeout);
protected:

private:
	std::map<PVOID,CFiber*> m_activeList;//可运行列表
	std::list<CFiber*> m_pendingAdd;//等待添加进可运行列表中的纤程
	std::list<std::pair<time_t,CFiber*> > m_sleepList;//正在睡眠的纤程，将来改成用优先队列

	PVOID m_pFiberContext; // 调度器所在纤程的上下文
	int m_nCount;
	CFiber *m_pFibers[MAX_FIBER];
	int m_nCurrentUID;           //当前正在运行的纤程的uid,==-1表示在scheduler中运行
	volatile bool m_terminate;       
};

inline CFiberScheduler *IFiberWorker::GetScheduler() { 
	return m_pFiber->m_pScheduler; 
}

#ifdef _FIBER_TEST

class CFiberTest1 : public IFiberWorker
{

public:
	void OnRunFiber()
	{
		for(int i = 0 ; i < 20; ++i)
		{
			TRACE(_T("%d\n"),i);
			TRACE("begin block\n");
			if(0 == GetScheduler()->Block(1, 1))
				TRACE("test wake me up\n");
			else
				TRACE("timeout\n");
			//Scheduler::sleep(1);
		}
		TRACE("die\n");
	}
};

class CFiberTest2 : public IFiberWorker
{
public:
	void OnRunFiber()
	{
		for(int i = 0 ; i < 10; ++i)
		{
			TRACE("%d\n",i);
			if(test1->GetFiber()->GetWaitEvent() == 1)
				test1->GetFiber()->OnEvent(1);
			GetScheduler()->yield();
		}
		TRACE("die\n");
	}
	CFiberTest1 *test1;
};

void FiberTest()
{
	CFiberScheduler fsc;
	fsc.Init();

	CFiberTest1 test1;
	CFiberTest2 test2;
	test2.test1 = &test1;
	fsc.Spawn(&test1,4096);
	fsc.Spawn(&test2,4096);

	fsc.Schedule();
}

#endif

TPRI_NAMESPACE_END

#endif // end of __TPRI_FIBER_H__