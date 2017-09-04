//异步TCP服务
#pragma once
#include <afxsock.h>
#include <afxtempl.h> //模版
#include "../tool/frmtool.h"


//////////////////////////////////////////////////////////////////////////
// CAsynConnect 异步连接
class CAsynConnect 
{
public:
	CAsynConnect();
	virtual ~CAsynConnect();

public:
	DWORD		m_dwID;				//连接ID,唯一标示一个客户连接,不用m_hSocket做ID的原因是可能从新分配
	SOCKET		m_hSocket;			//=INVALID_SOCKET;
	WORD		m_wPort;			//客户端口
	SYSTEMTIME	m_DateTime;			//连接开始时间
	LONGLONG 	m_l64Read;			//接收字节
	LONGLONG    m_l64Write;			//发送字节
	char		m_sIp[16];			//客户ip
	CString		m_szUser;			//用户
	CString		m_szActor;			//角色
protected:
	CFrmParse	m_frmparse;			//解析报文
public:
	int	writesocket(BYTE *pbuf,int nsize,int ntimeout);//写
	BOOL OnData(char *pdata,int nsize,CMyByteArray &ba);//数据到达
};

class CAsynConInfo : public CSortItem//异步连接信息
{
public:
	CAsynConInfo();
	virtual ~CAsynConInfo();
public:
	virtual		int Compare(CSortItem *pOb);//排序比较
	CAsynConInfo& operator=(const CAsynConnect* pcon); //重载=符号
public:
	DWORD		m_dwID;				//端口
	char		m_sIp[16];			//IP
	char		m_sTime[32];		//连接时间
	char		m_sUser[16];		//用户
	char		m_sActor[16];		//角色
	LONGLONG 	m_l64Read;			//接收字节
	LONGLONG    m_l64Write;			//发送字节

	BOOL		m_bSel;					//选择
};

//////////////////////////////////////////////////////////////////////////
//CAsynConPool 异步连接池

#define MAX_CONNECT_NUM   FD_SETSIZE //最大同时连接数

class CAsynConPool
{
public:
	CAsynConPool();
	~CAsynConPool();

protected:
	CMap <DWORD,DWORD, CAsynConnect*, CAsynConnect*> m_mapcon;	//异步连接表
	DWORD m_dwNextID; //下一个ID
	CCriticalSection m_Lock;//锁
public:
	BOOL IsFull();//连接池是否满
	BOOL AddConnect(SOCKET s,WORD wport,const char* sip,LPCTSTR sUser,LPCTSTR sActor);
	void AddReadByteNum(DWORD dwid,long lbytes);//加接收字节数
	BOOL WriteSocket(DWORD dwid,BYTE *pbuf,int nsize,int ntimeout); //发送数据

	int  Makefdsets(fd_set *fd); //填写fd_set
	DWORD DelCon(SOCKET s);//删除连接

	BOOL OnReadData(SOCKET s,char *pdata,int nsize,CMyByteArray &frm,DWORD& dwid);//数据到达，返回连接ID,报文存放在frm中

	void OnStop();//停止后清理

	void GetConInfo(CSortObArray* pa);//取连接信息

	BOOL GetActor(DWORD dwid,CString &sActor);//取某个连接的角色
	int	 GetConnectNum();//获取连接数

protected:
	void Clear(); //清空map表
	void DelOneCon(DWORD dwID);//删除一个连接
	CAsynConnect* GetConnect(DWORD dwID);
	CAsynConnect* GetConBySocket(SOCKET s);

};

//////////////////////////////////////////////////////////////////////////
//CJobList 作业队列
class CJobItem //服务作业
{
public:
	CJobItem(){
		m_dwid = 0;
		m_bin.SetGrown(1024 * 32);//32K
	};
	CJobItem(DWORD dwid){
		m_dwid = dwid;
		m_bin.SetGrown(1024 * 32);//32K
	};
	~CJobItem(){};
public:
	DWORD			m_dwid;
	CMyByteArray	m_bin;
};

#define MAX_JOBS    1024
typedef CJobItem*  PJOB;
class CJobList  //作业队列
{
public:
	CJobList();
	~CJobList();
protected:
	PJOB  m_jobs[MAX_JOBS];
	int	  m_nHead,m_nTail;

	CCriticalSection m_Lock;//锁
public:
	PJOB Get(); //取一个PJOB,返回 NULL 表示空
	BOOL Put(PJOB pj); //加入一个JOB，返回TRUE表示成功，FALSE失败
	void Del(DWORD dwjobid); //删除
	
protected:
	PJOB GetIn();
	void Clear();	//
	BOOL DelOne(DWORD dwjobid);
};

//////////////////////////////////////////////////////////////////////////
//CAsynThread 线程类
class CAsynThread //线程池中线程类,从该类继承一个类即可
{
public:
	CAsynThread();
	virtual ~CAsynThread();
protected:
	BOOL*	m_pbKill;		//终止线程
	BOOL*	m_pbRun;		//状态

	CJobList* m_pjoblist;	//作业队列
	CAsynConPool* m_pconpool;//连接池
	DWORD  m_dwThreadID;
public:
	BOOL Start(BOOL* pbKill,BOOL* pbRun,CJobList* pjoblist,CAsynConPool* pconpool);//启动服务

protected:
	static  UINT ThreadProcess(LPVOID pParam);
	UINT	ProcessRun();

	//需要重载的
protected:
	virtual int DoOneJob(CJobItem* pjob); //服务一个作业
};


//////////////////////////////////////////////////////////////////////////
//CAsynTcpServer TCP异步服务类

#define	TCP_READBUFSIZE		16384   //16K大小的缓冲
#define MAX_SERVER_THREADS	10		//最大服务线程数
class CAsynTcpServer  //代理TCP服务
{
public:
	CAsynTcpServer();
	virtual ~CAsynTcpServer();
protected:
	CAsynConPool	m_conpool;	//连接池
	CJobList		m_joblist;	//作业池

	WORD				m_wPort;				//端口号，默认值12084
	SOCKET				m_hSocket;				//=INVALID_SOCKET;
	struct	sockaddr_in m_HostAddr;				//服务器主机地址

	char	m_readbuf[TCP_READBUFSIZE];			//接收缓冲
	
	BOOL			m_bKillThread; //终止线程

	//固定线程
	BOOL	m_bTcpAcceptRun;	//侦听线程运行标志
	BOOL	m_bTcpEventRun;		//TCP事件线程标志

	//JOB线程池
	int				m_nThreadNum; //线程数
	CAsynThread*	m_threadpool[MAX_SERVER_THREADS];		//线程池
	BOOL			m_bPoolThreadRun[MAX_SERVER_THREADS];	//线程状态
	
public:
	BOOL			Start(int nthreadnum,WORD wPort,CString &szErr);//启动
	void			Stop();
	void			GetConInfo(CSortObArray* pa)//取连接信息
	{
		m_conpool.GetConInfo(pa);
	};
	BOOL GetActor(DWORD dwid,CString &sActor)//取某个连接的角色
	{
		return m_conpool.GetActor(dwid,sActor);
	};
protected:
	UINT			ProcessTcpAccept();
	UINT			ProcessTcpEvent();

	//线程函数
	static UINT		ThreadTcpAccept(LPVOID pParam);	//侦听线程
	static UINT		ThreadTcpEvent(LPVOID pParam);	//TCP事件线程

	//需要重载的
protected:
	virtual CAsynThread* CreateThread(){return NULL;};
	virtual BOOL	OnLogin(SOCKET s,LPCTSTR szip,CString &szUser,CString &szActor){return TRUE;}; //Accept后的登录连接
};
