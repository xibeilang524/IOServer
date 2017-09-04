#pragma once
#include "Define.h"


class CDDSConnect //DDS服务器的链接,内部一个连接线程，外部调用写的线程
{
public:
	CDDSConnect();
	~CDDSConnect();
	CString m_szfile;
	BOOL m_breal; //实时连接
public:
	void 	SetConnect(BOOL bconnect);//设置
	void	SetArgs(LPDBPARG,int n);//设置参数
	void	ClearConArg();//清除连接参数
	void	DoConnect();//连接过程

	void	Start();
	void	Stop();

	BOOL	SetBlockingMode(BOOL bBlockingMode);
	BOOL	IsConnect(); //是否连接
	BOOL	DoWrite(void* pdata,int nsize,int ntimeoutmsec,BYTE &ucret,BYTE &ddsorder);	

	void	GetInfo(CString &szinfo);//取运行信息
	int		GetDDSNum(){return m_ndds;} //取DDS服务器个数
	int		GetNetMode() {	return m_nNetMode;}
protected:
	CCriticalSection m_Lock;

	//连接参数
	int		m_ndds;	//DDS配置个数
	int		m_ncurdds;//当前连接位置
	TDBPARG m_ddscon[MAX_DBP_CONARGS];

	//连接管理
	WORD	m_wPort; //当前连接的端口
	CString m_szSvrIP;//当前连接的IP
	int		m_nNetMode;

	BOOL	m_bConnected;//是否已连接
	SOCKET	m_socket;	//套接字

	BOOL	m_bBlockingMode;
	// 某种情况下，可能会出现select()查询时无可读数据，但实际上数据已经到达，从而会千万代理上的客户端连接
	// 会不断的断开重连情况发生，通过该状态值可自动切换数据可读状态的查询方式，可解决该问题
	BOOL	m_bMsgPeekMode;
	//线程管理
	BOOL	m_bRun; //运行
	BOOL	m_bKill;//结束线程

	BOOL	Select(BOOL* pbReadReady, BOOL* pbWriteReady, BOOL* pbExceptFlag, DWORD dwTimeout);
	int		PeekBuffer(BYTE* pBuffer, int nBufferSize, BOOL bBlockingMode = FALSE);
	BOOL	WaitForData(DWORD dwTimeout);
	BOOL	GetNextConDef(LPDBPARG pcon); //获取下一个连接定义	
	BOOL	ConnectSvr();//连接
	void	SetTcpNoDelay();//禁止Nagle优化
	static UINT	ThreadConnectDDS(LPVOID lparam);//连接DDS的线程
	FORCEINLINE void ThreadSleep(DWORD dwTimeout) {
		Sleep(dwTimeout);

		/*
		HANDLE hHandle = ::GetCurrentProcess();
		while(1)
		{
			MSG __msg;

			DWORD dwResult = ::MsgWaitForMultipleObjects(1, &hHandle, FALSE, dwTimeout, QS_ALLINPUT);

			switch(dwResult)
			{
			case WAIT_OBJECT_0:
				break;
			case WAIT_TIMEOUT:
				break;
			default:
				while(::PeekMessage(&__msg, NULL, 0, 0, PM_REMOVE))
				{
					::TranslateMessage(&__msg);
					::DispatchMessage(&__msg);
				}
			}
		}
		*/
	}

	int		Read(BYTE *pbuf,int nsize,int ntimeout);
	int		Send(BYTE *pbuf,int nsize,int ntimeout);
	int		WriteDDS(void* pdata,int nsize,int ntimeoutmsec,BYTE *pread); //写到DDS,返回0表示正确,<0错误，大于0接收的返回字节数
	BOOL	WriteDDS_NoAck(void* pdata,int nsize);

};