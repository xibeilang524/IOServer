//asyntcp.cpp 异步TCP服务
#include "stdafx.h"
#include "asyntcp.h"


/////////////////////////////////////////////////////////////////////
//CAsynConnect 异步连接
CAsynConnect::CAsynConnect()
{
	m_dwID = 0;
	m_hSocket = INVALID_SOCKET;
	m_sIp[0] = 0;
	m_wPort = 0;
	::GetSystemTime(&m_DateTime);
	m_l64Read = 0;
	m_l64Write = 0;
	m_szUser = "";
	m_szActor = "";
}

CAsynConnect::~CAsynConnect()
{
	if(m_hSocket != INVALID_SOCKET)
	{
		::closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}
}

int	CAsynConnect::writesocket(BYTE *pbuf,int nsize,int ntimeout)//写,超时为毫秒
{
	if(m_hSocket == INVALID_SOCKET)
		return -1;
	TIMEVAL tv01 = {ntimeout/1000,1000 * (ntimeout%1000)};
	FD_SET fdw;
	fdw.fd_count = 1;
	fdw.fd_array[0] = m_hSocket;

	int nRet = ::select(0, NULL, &fdw, NULL, &tv01);//测试可写
	if(SOCKET_ERROR == nRet)//发生网络错误
		return -1;

	if(nRet==0)//超时发生，不能发送数据
		return 0;
	nRet = ::send(m_hSocket, (char*)pbuf,nsize,0);
	if(nRet >0 )
		m_l64Write += nRet;
	return nRet;
}

BOOL CAsynConnect::OnData(char *pdata,int nsize,CMyByteArray &ba)//数据到达
{
	int nst = 0,nlen = 0;
	
	if((pdata == NULL)||(nsize == 0))
	{
		nst = m_frmparse.Parse();
		if(nst != FRMP_FRMOK)
			return FALSE;
		ba = m_frmparse.m_Frm;
		m_frmparse.m_Frm.ClearData();
		m_frmparse.SetStatus(FRMP_NOHEAD);
		return TRUE;
	}
	nst = m_frmparse.PutData((BYTE*)pdata,nsize);
	if(nst != FRMP_FRMOK)
		return FALSE;

	ba = m_frmparse.m_Frm;
	m_frmparse.m_Frm.ClearData();
	m_frmparse.SetStatus(FRMP_NOHEAD);
	return TRUE;
}

CAsynConInfo::CAsynConInfo()
{
	m_dwID = 0;
	m_sIp[0] = 0;
	m_sTime[0] = 0;
	m_sUser[0] = 0;
	m_sActor[0] = 0;
	m_l64Read = 0;
	m_l64Write = 0;

	m_bSel = FALSE;
}
CAsynConInfo::~CAsynConInfo()
{

}

CAsynConInfo& CAsynConInfo::operator=(const CAsynConnect* pcon) //重载=符号
{
	m_dwID = 0;
	strncpy(m_sIp,pcon->m_sIp,15);
	sprintf(m_sTime,"%04d-%02d-%02d %02d:%02d:%02d",
		pcon->m_DateTime.wYear,pcon->m_DateTime.wMonth,pcon->m_DateTime.wDay,
		pcon->m_DateTime.wHour,pcon->m_DateTime.wMinute,pcon->m_DateTime.wSecond);
	strncpy(m_sUser,pcon->m_szUser,15);
	strncpy(m_sActor,pcon->m_szActor,15);
	m_l64Read = pcon->m_l64Read;
	m_l64Write = pcon->m_l64Write;
	m_dwID = pcon->m_dwID;
	return *this;
}

int CAsynConInfo::Compare(CSortItem *pOb)//排序比较
{
	int nret = 0;
	if(pOb == NULL)
		return nret;
	CAsynConInfo *pi = (CAsynConInfo *)pOb;
	
	switch(m_nSortWho)
	{
	case 0:  
		if(m_dwID > pi->m_dwID)
			nret = 1;
		else if(m_dwID < pi->m_dwID)
			nret = -1;
		break;
	case 1: 
		nret = strcmp(m_sIp,pi->m_sIp);
		break;
	case 2:
		nret = strcmp(m_sTime,pi->m_sTime);
		break;
	case 3: 
		nret = strcmp(m_sUser,pi->m_sUser);
		break;
	case 4: 
		nret = strcmp(m_sActor,pi->m_sActor);
		break;
	case 5: 
		if(m_l64Read > pi->m_l64Read)
			nret = 1;
		else if(m_l64Read < pi->m_l64Read)
			nret = -1;
		break;
	case 6: 
		if(m_l64Write > pi->m_l64Write)
			nret = 1;
		else if(m_l64Write < pi->m_l64Write)
			nret = -1;
		break;
	}
	if(!m_bAsc)
		nret *= -1;
	return nret;	
}

/////////////////////////////////////////////////////////////////////
//CAsynConPool 异步连接池
CAsynConPool::CAsynConPool()
{
	m_dwNextID = 1;
	m_mapcon.InitHashTable(MAX_CONNECT_NUM + 7);
}

CAsynConPool::~CAsynConPool()
{
	Clear();
}
void CAsynConPool::Clear()
{
	//逐个删除对象
	DWORD		key=0;
	CAsynConnect*	pi=NULL;
	POSITION pos = m_mapcon.GetStartPosition( );
	while(pos!=NULL)
	{
		pi = NULL;
		m_mapcon.GetNextAssoc( pos, key, pi );
		if(pi != NULL)
			delete pi;
	}
	m_mapcon.RemoveAll();	//清空ID索引表	
}

void CAsynConPool::GetConInfo(CSortObArray* pa)//取连接信息
{
	CSafeLock lock(&m_Lock); //锁
	pa->DeleteObjects();
	DWORD		key=0;
	CAsynConnect*	pi=NULL;
	CAsynConInfo*	pinfo = NULL;
	POSITION pos = m_mapcon.GetStartPosition( );
	while(pos!=NULL)
	{
		m_mapcon.GetNextAssoc( pos, key, pi );
		pinfo = new CAsynConInfo;
		if(pinfo)
		{
			*pinfo = pi;
			pa->Add(pinfo);
		}
	}
}

int	 CAsynConPool::GetConnectNum()//获取连接数
{
	CSafeLock lock(&m_Lock); //锁
	return m_mapcon.GetCount();
}

void CAsynConPool::OnStop()//停止后清理
{
	CSafeLock lock(&m_Lock); //锁
	Clear();
}

void CAsynConPool::DelOneCon(DWORD dwID)//删除一个连接
{
	CAsynConnect *pcon = NULL;
	DWORD 		key=dwID;

	if(!m_mapcon.Lookup(key,pcon))
		return ;
	if(m_mapcon.RemoveKey(key))
	{
		TRACE("Disconnect %s,%d\n",pcon->m_sIp,pcon->m_wPort);
		delete pcon;
	}
}

CAsynConnect* CAsynConPool::GetConBySocket(SOCKET s)
{
	DWORD		key=0;
	CAsynConnect*	pi=NULL;
	POSITION pos = m_mapcon.GetStartPosition( );
	while(pos!=NULL)
	{
		pi = NULL;
		m_mapcon.GetNextAssoc( pos, key, pi );
		if(pi->m_hSocket == s)
			return pi;
	}
	return NULL;
}

DWORD CAsynConPool::DelCon(SOCKET s)//删除连接
{
	CSafeLock lock(&m_Lock); //锁

	DWORD dwid = 0;

	DWORD		key=0;
	CAsynConnect*	pi=NULL;
	POSITION pos = m_mapcon.GetStartPosition( );
	while(pos!=NULL)
	{
		pi = NULL;
		m_mapcon.GetNextAssoc( pos, key, pi );
		if(pi->m_hSocket == s)
		{
			dwid = pi->m_dwID;
			m_mapcon.RemoveKey(pi->m_dwID);
			delete pi;
			return dwid;
		}
	}
	return dwid;
}

BOOL CAsynConPool::IsFull()//连接池是否满
{
	CSafeLock lock(&m_Lock); //锁
	int nsize = m_mapcon.GetCount();
	if( nsize < MAX_CONNECT_NUM)
		return FALSE;
	return TRUE;
}

BOOL CAsynConPool::AddConnect(SOCKET s,WORD wport,const char* sip,LPCTSTR sUser,LPCTSTR sActor)
{
	CSafeLock lock(&m_Lock); //锁
	int nsize = m_mapcon.GetCount();
	if( nsize >= MAX_CONNECT_NUM)
		return FALSE;
	CAsynConnect* pcon = new CAsynConnect();
	if(pcon == NULL)
		return FALSE;

	pcon->m_dwID = m_dwNextID++;
	if(pcon->m_dwID == 0) //保证不为0
		pcon->m_dwID = m_dwNextID++;

	pcon->m_hSocket = s;
	pcon->m_wPort = wport;
	pcon->m_szUser = sUser;
	pcon->m_szActor = sActor;

	strncpy(pcon->m_sIp,sip,15);

	while((NULL != GetConnect(pcon->m_dwID)) || (pcon->m_dwID == 0))
		pcon->m_dwID = m_dwNextID++;

	m_mapcon.SetAt(pcon->m_dwID,pcon);

	TRACE("accept %s,%d,SOCKET = %d\n",pcon->m_sIp,pcon->m_wPort,pcon->m_hSocket);
	return TRUE;
}

CAsynConnect* CAsynConPool::GetConnect(DWORD dwID)
{
	CAsynConnect *pcon = NULL;
	DWORD 		key=dwID;

	if(!m_mapcon.Lookup(key,pcon))
		return NULL;
	return pcon;
}

void CAsynConPool::AddReadByteNum(DWORD dwid,long lbytes)//加接收字节数
{
	CSafeLock lock(&m_Lock); //锁
	CAsynConnect* pcon = GetConnect(dwid);
	if(pcon)
		pcon->m_l64Read += lbytes;
}

BOOL CAsynConPool::OnReadData(SOCKET s,char *pdata,int nsize,CMyByteArray &frm,DWORD& dwid)//数据到达，返回连接ID,报文存放在frm中
{
	CSafeLock lock(&m_Lock); //锁
	CAsynConnect* pcon = GetConBySocket(s);
	if(pcon == NULL)
	{
		frm.ClearData();
		return 0;
	}
	pcon->m_l64Read += nsize;
	dwid = pcon->m_dwID;
	return pcon->OnData(pdata,nsize,frm);
}

BOOL CAsynConPool::GetActor(DWORD dwid,CString &sActor)//取某个连接的角色
{
	CSafeLock lock(&m_Lock); //锁
	CAsynConnect* pcon = GetConnect(dwid);
	if(!pcon)
	{
		sActor = "";
		return FALSE;
	}
	sActor = pcon->m_szActor;
	return TRUE;
}

BOOL CAsynConPool::WriteSocket(DWORD dwid,BYTE *pbuf,int nsize,int ntimeout)//发送数据
{
	CSafeLock lock(&m_Lock); //锁
	CAsynConnect* pcon = GetConnect(dwid);
	if(!pcon)
		return FALSE;

	if(-1 == pcon->writesocket(pbuf,nsize,ntimeout))//发生错误,需要断开连接
	{
		if(m_mapcon.RemoveKey(dwid))
			delete pcon; //删除连接
		return FALSE; 
	}
	return TRUE;
}

int  CAsynConPool::Makefdsets(fd_set *fd) //填写fd_set
{
	CSafeLock lock(&m_Lock);
	fd->fd_count = 0;
	DWORD		key=0;
	CAsynConnect*	pi=NULL;
	POSITION pos = m_mapcon.GetStartPosition( );
	while((pos!=NULL) && (fd->fd_count < FD_SETSIZE))
	{
		pi = NULL;
		m_mapcon.GetNextAssoc( pos, key, pi );
		if(pi != NULL)
		{
			fd->fd_array[fd->fd_count] = pi->m_hSocket;
			fd->fd_count++;
		}
	}
	return fd->fd_count;
}


/////////////////////////////////////////////////////////////////////
//CJobList 作业队列
CJobList::CJobList()
{
	m_nHead = 0;
	m_nTail = 0;
}
CJobList::~CJobList()
{
	Clear();
}

PJOB CJobList::GetIn()
{
	if(m_nHead == m_nTail)
		return NULL;
	PJOB s = m_jobs[m_nHead];
	m_nHead = (m_nHead + 1)%MAX_JOBS;
	return s;
}

void CJobList::Clear()	//清除全部
{
	PJOB pj=GetIn();
	while(pj != NULL)
	{
		delete pj;
		pj=GetIn();
	}
	m_nHead = 0;
	m_nTail = 0;
}

BOOL CJobList::DelOne(DWORD dwjobid)//删除一个
{
	int nh = m_nHead;
	if(m_nHead == m_nTail)
		return FALSE; //空
	BOOL bFind = FALSE;
	while(nh  != m_nTail)
	{
		if(m_jobs[nh]->m_dwid == dwjobid)
		{
			bFind = TRUE;
			break;
		}
		nh = (nh + 1)%MAX_JOBS;
	}
	if(bFind)
	{
		delete m_jobs[nh];
		int nnext = (nh + 1)%MAX_JOBS;
		while(nnext  != m_nTail)
		{
			m_jobs[nh] = m_jobs[nnext];
			nh = nnext;
			nnext = (nh + 1)%MAX_JOBS;
		}
		m_nTail = nh;
		return TRUE;
	}
	return FALSE;
}

PJOB CJobList::Get() //取一个PJOB,返回 NULL 表示空
{
	CSafeLock lock(&m_Lock);
	return GetIn();

}
BOOL CJobList::Put(PJOB pj) //加入一个PJOB，返回TRUE表示成功，FALSE失败
{
	CSafeLock lock(&m_Lock);
	if((m_nTail + 1)%MAX_JOBS == m_nHead)
		return FALSE;
	m_jobs[m_nTail] = pj;
	m_nTail = (m_nTail + 1) % MAX_JOBS;
	return TRUE;
}
void CJobList::Del(DWORD dwjobid) //删除
{
	CSafeLock lock(&m_Lock);
	while(DelOne(dwjobid));
}

////////////////////////////////////////////////////////////////////
//CAsynThread 服务线程
CAsynThread::CAsynThread()
{
	m_pbKill = NULL;
	m_pbRun = NULL;
	m_pjoblist = NULL;
	m_pconpool = NULL;
	m_dwThreadID = 0;
}

CAsynThread::~CAsynThread()
{

}

BOOL CAsynThread::Start(BOOL* pbKill,BOOL* pbRun,CJobList* pjoblist,CAsynConPool* pconpool)//启动服务
{
	ASSERT(pbKill != NULL);
	ASSERT(pbRun != NULL);
	ASSERT(pjoblist != NULL);
	ASSERT(pconpool != NULL);

	m_pbKill = pbKill;
	m_pbRun = pbRun;
	m_pjoblist = pjoblist;
	m_pconpool = pconpool;

	
	m_dwThreadID = (DWORD)AfxBeginThread(ThreadProcess,this,THREAD_PRIORITY_NORMAL);
	return TRUE;
}

UINT CAsynThread::ThreadProcess(LPVOID pParam)
{
	CAsynThread *pserver = (CAsynThread*)pParam;

	*(pserver->m_pbRun)  = TRUE;
	pserver->ProcessRun();
	*(pserver->m_pbRun)  = FALSE;

	
	
	return 0;
}

int CAsynThread::DoOneJob(CJobItem* pjob)
{
	return 0; //基类不做任何处理
}

UINT	CAsynThread::ProcessRun()
{
	CJobItem* pj;
	while((*m_pbKill) == FALSE)
	{
		pj = m_pjoblist->Get();
		if(!pj)
		{
			Sleep(1);
			continue;
		}
		else
		{
			DoOneJob(pj);
			delete pj;
			pj = NULL;
		}
	}
	TRACE("CAsynThread(0X%08X) exit!\n",m_dwThreadID);
	return 0;
}

///////////////////////////////////////////////////////////////////
// CAsynTcpServer
CAsynTcpServer::CAsynTcpServer()
{
	m_wPort = 12084;
	m_hSocket = INVALID_SOCKET;

	m_bTcpAcceptRun = FALSE;	//侦听线程运行标志
	m_bTcpEventRun = FALSE;	//TCP事件线程标志

	m_bKillThread = FALSE;

	//初始化线程池
	for(int i=0;i<MAX_SERVER_THREADS;i++)
	{
		m_threadpool[i] = NULL;
		m_bPoolThreadRun[i] = FALSE;
	}
	m_nThreadNum = 0;
}

CAsynTcpServer::~CAsynTcpServer()
{

}

BOOL	CAsynTcpServer::Start(int nthreadnum,WORD wPort,CString &szErr)
{
	szErr = "";
	if(!m_bTcpEventRun)
			AfxBeginThread(ThreadTcpEvent,this,THREAD_PRIORITY_NORMAL);//启动事件处理线程
	if(!m_bTcpAcceptRun)
	{
		m_wPort = wPort;
		m_HostAddr.sin_family = AF_INET;
		m_HostAddr.sin_port = ::htons(m_wPort);
		m_HostAddr.sin_addr.s_addr = htonl (INADDR_ANY);

		m_hSocket = ::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		
		if(m_hSocket == INVALID_SOCKET)
		{
			szErr = "不能创建实时数据服务SOCKET,可能该端口被使用!";
			return FALSE;
		}

		if(SOCKET_ERROR==::bind(m_hSocket,(struct sockaddr *)(&(m_HostAddr)),sizeof(SOCKADDR)))
		{
			szErr.Format("绑定实时数据服务端口[%d]失败! ErrCode = %d",m_wPort,WSAGetLastError());
			return FALSE;
		}
		if(SOCKET_ERROR==::listen(m_hSocket,1)) 
		{
			szErr.Format("监听实时数据服务端口[%d]失败! ErrCode = %d",m_wPort,WSAGetLastError());
			return FALSE;
		}
		AfxBeginThread(ThreadTcpAccept,this,THREAD_PRIORITY_NORMAL);//启动Accept服务线程

		//创建线程池中线程
		m_nThreadNum = nthreadnum;
		if( m_nThreadNum > MAX_SERVER_THREADS)
			m_nThreadNum = MAX_SERVER_THREADS;
		if(m_nThreadNum <= 0)
			m_nThreadNum = 1;

		int i,ki = 0;
		CAsynThread* pth=NULL;
		for(i=0;i<m_nThreadNum;i++)
		{
			pth = CreateThread(); //创建服务线程
			if(pth)
			{
				m_threadpool[ki] = pth;
				pth->Start(&m_bKillThread,&(m_bPoolThreadRun[ki]),&m_joblist,&m_conpool);
				ki++;
			}
		}
		m_nThreadNum = ki;//准确个数
	}
	
	return TRUE;
}

void	CAsynTcpServer::Stop()
{
	m_bKillThread = TRUE;
	//两个固定线程是否退出
	while(m_bTcpEventRun || m_bTcpAcceptRun)
	{
		Sleep(100);
		continue;
	}

	//线程池中线程是否退出
	int i;
	BOOL brun = TRUE;
	while(brun)
	{
		Sleep(100);
		brun = FALSE;
		for(i=0;i<m_nThreadNum;i++)
		{
			if(m_bPoolThreadRun[i])
			{
				brun = TRUE;
				break;
			}
		}
	}
	//删除线程池线程
	for(i=0;i<m_nThreadNum;i++)
		delete m_threadpool[i];
	m_nThreadNum = 0;

	for(i=0;i<MAX_SERVER_THREADS;i++)
	{
		m_threadpool[i] = NULL;
		m_bPoolThreadRun[i] = FALSE;
	}

	m_conpool.OnStop(); //删除所有连接池连接

	m_bKillThread = FALSE;
}

UINT	CAsynTcpServer::ThreadTcpAccept(LPVOID pParam)	//侦听线程
{
	CAsynTcpServer *pserver = (CAsynTcpServer*)pParam;
	pserver->m_bTcpAcceptRun = TRUE;
	pserver->ProcessTcpAccept();
	pserver->m_bTcpAcceptRun = FALSE;
	return 0;
}
UINT	CAsynTcpServer::ThreadTcpEvent(LPVOID pParam)	//TCP事件线程
{
	CAsynTcpServer *pserver = (CAsynTcpServer*)pParam;
	pserver->m_bTcpEventRun = TRUE;
	pserver->ProcessTcpEvent();
	pserver->m_bTcpEventRun = FALSE;
	return 0;
}

void tcp_SetTcpNoDelay(SOCKET s) //禁止Nagle优化
{
	int bNodelay = 1; 
	setsockopt( 
		s,
		IPPROTO_TCP,
		TCP_NODELAY,
		(char *)&bNodelay,
		sizeof(bNodelay));//不采用延时算法 
}

UINT	CAsynTcpServer::ProcessTcpAccept()
{
	SOCKET  sConnect;//连接的SOCKET
	struct  sockaddr_in		 addrClient;//客户地址
	int		nClientAddrLen = sizeof(SOCKADDR);//客户地址长度
	CString szUser="",szActor = "";;
	DWORD	dwPower = 0;
	while(!m_bKillThread)
	{
		//等待可以连接
		while(m_conpool.IsFull())
		{
			Sleep(100);//延时100毫秒
			if(m_bKillThread) //要求终止线程
			{
				TRACE("Exit At Accept at1 !\n");
				return 0;
			}
		}
	
		//用select超时方式Accept
		TIMEVAL tv01 = {0,1000 * 100}; //100ms
		FD_SET fdr;
		
		int nRet;
		while(!m_bKillThread)
		{
			fdr.fd_count = 1;
			fdr.fd_array[0]=m_hSocket;
			nRet = ::select(0, &fdr, NULL, NULL, &tv01); //检查是否有Accept请求
			if(nRet != 1)
				continue;
			break;
		}
		if(m_bKillThread)
		{
			TRACE("Exit At Accept at2!\n");
			return 0;
		}
		sConnect=::accept(m_hSocket,(struct sockaddr *)(&addrClient),&nClientAddrLen);
		tcp_SetTcpNoDelay(sConnect);//禁止优化发送
		if(!OnLogin(sConnect,inet_ntoa(addrClient.sin_addr),szUser,szActor))//szActor实际是密码
		{
			//已经在Onlogin中发回应答包了
			::closesocket(sConnect);
			continue;
		}
		//添加到连接池
		if(!m_conpool.AddConnect(sConnect,ntohs(addrClient.sin_port),inet_ntoa(addrClient.sin_addr),szUser,szActor) )
			::closesocket(sConnect);		
	}
	TRACE("Exit At Accept at3 !\n");
	return 0;
}

UINT CAsynTcpServer::ProcessTcpEvent()
{
	int i,nRet,ns=0,nevent = 0;
	fd_set fdr,fde;
	TIMEVAL tv01 = {0,1000 * 50}; //50ms
	DWORD dwid = 0;
	CMyByteArray frm;
	while(!m_bKillThread)
	{
		ns = m_conpool.Makefdsets(&fdr);
		if(ns == 0)
		{
			Sleep(50);
			continue;
		}
		memcpy(&fde,&fdr,sizeof(fd_set));//复制到错误检查连接

		nevent  = ::select(0, &fdr, NULL, &fde, &tv01); //检查是否有事件

		if(nevent <= 0)
			continue;
		//处理读
		for(i=0;i<(int)fdr.fd_count;i++)
		{
			nRet=::recv(fdr.fd_array[i], m_readbuf, TCP_READBUFSIZE , 0);
			if(nRet <= 0)//连接关闭或发生错误
			{
				TRACE("del at rev socket = %d!\n",fdr.fd_array[i]);
				dwid = m_conpool.DelCon(fdr.fd_array[i]);//从连接池中删除
				m_joblist.Del(dwid); //从作业池中删除
			}
			else
			{
				if(m_conpool.OnReadData(fdr.fd_array[i],m_readbuf,nRet,frm,dwid))
				{
					//加入joblist
					if(!m_conpool.IsFull())
					{
						PJOB pj = new CJobItem(dwid);
						if(pj != NULL)
						{
							pj->m_bin = frm;
							if(!m_joblist.Put(pj))
								delete pj;
						}
					}
				}
				while(m_conpool.OnReadData(fdr.fd_array[i],NULL,0,frm,dwid)) //加入剩下的
				{
					//加入joblist
					if(!m_conpool.IsFull())
					{
						PJOB pj = new CJobItem(dwid);
						if(pj != NULL)
						{
							pj->m_bin = frm;
							if(!m_joblist.Put(pj))
								delete pj;
						}
					}
				}
			}
		}
		//处理错误
		for(i=0;i<(int)fde.fd_count;i++)
		{
			TRACE("del at err socket = %d!\n",fde.fd_array[i]);
			dwid = m_conpool.DelCon(fdr.fd_array[i]);//从连接池中删除
			m_joblist.Del(dwid); //从作业池中删除
		}
	}	
	TRACE("ProcessEvent Exit!\n");
	return 0;
}
