#include "StdAfx.h"
#include "HeartClient.h"
#include "define.h"

CHeartClient::CHeartClient()
{
	m_socket = INVALID_SOCKET;

	m_bSrvExist = TRUE;		//服务器是否存在

	m_bRun = FALSE; //运行
	m_bKill = FALSE;//结束线程

	m_wport = 0; //连接的端口
	m_szip = "";//连接的IP
}
CHeartClient::~CHeartClient()
{
	Stop();
}
void CHeartClient::SetArgs(LPCTSTR lpszip,WORD wport)//设置参数
{
	m_wport = wport; //连接的端口
	m_szip = lpszip;//连接的IP
}

BOOL	CHeartClient::IsSrvExist() //心跳服务器是否存在
{
	return m_bSrvExist;
}

void CHeartClient::Start()
{
	if(m_bRun)
		return;
	AfxBeginThread(ThreadRun,this,THREAD_PRIORITY_NORMAL);//启动DDS连接线程
}

void CHeartClient::Stop()
{
	m_bKill = TRUE;
	while(m_bRun)
		Sleep(100);

	if(m_socket != INVALID_SOCKET) //先断开
	{
		::closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
	m_bKill = FALSE;
}

BOOL	CHeartClient::ConnectSvr()//连接
{
	if(m_szip.IsEmpty())
		return FALSE;
	if(m_szip == "0.0.0.0")
		return FALSE; //无效IP

	if(m_socket != INVALID_SOCKET) //先断开
	{
		::closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}

	struct sockaddr_in ServerHostAddr;//调度主站或终端主机地址
	ServerHostAddr.sin_family=AF_INET;
	ServerHostAddr.sin_port=::htons(m_wport);

	ServerHostAddr.sin_addr.s_addr=::inet_addr(m_szip);

	SOCKET s_socket = INVALID_SOCKET;
	s_socket=::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	if(s_socket==INVALID_SOCKET)
		return FALSE;

	long ul=1;
	if(SOCKET_ERROR == ioctlsocket(s_socket, FIONBIO, (unsigned long*)&ul))//设置非阻塞方式连接
	{
		::closesocket(s_socket);
		s_socket = INVALID_SOCKET;
		return FALSE;
	}

	connect(s_socket,(sockaddr *)(&ServerHostAddr),sizeof(SOCKADDR));

	int i,nret=0;
	for(i=0;i<21;i++) //40 * 100 = 4秒 
	{
		if(m_bKill) //要求终止线程
		{
			::closesocket(s_socket);
			return FALSE;
		}
		TIMEVAL tv01 = {0,200 * 1000};//100豪秒钟延迟
		FD_SET fdw = {1, s_socket};

		nret = ::select(0, NULL, &fdw, NULL, &tv01);
		if(nret == 0)
			continue; //超时
		break;
	}
	if(nret <= 0)
	{
		::closesocket(s_socket);
		s_socket = INVALID_SOCKET;
		return FALSE;
	}
	ul=0;
	if(SOCKET_ERROR == ioctlsocket(s_socket, FIONBIO, (unsigned long*)&ul))//设置回阻塞方式连接
	{
		::closesocket(s_socket);
		s_socket = INVALID_SOCKET;
		return FALSE;
	}
	m_socket = s_socket;
	SetTcpNoDelay();//禁止Nagle优化

	//发送登录命令

	CMakeFrm frmsend;
	SYSTEMTIME syst;
	::GetLocalTime(&syst);
	BYTE sendbuf[32];
	memcpy(sendbuf,&syst,sizeof(SYSTEMTIME));
	DWORD *pdw = (DWORD*)(sendbuf + 16);
	*pdw  = Utility::GetCRC32(sendbuf,16);

	frmsend.MakeFrm(OD_DDC_LOGIN,0,0,0x00,(BYTE*)sendbuf,20);//组登录包

	CMyByteArray baread;
	BYTE *pb = NULL;
	int nsize;

	pb = frmsend.GetFrm(&nsize); //取发送报文

	nret =  WriteSrv(pb,nsize,2000,&baread); //写到DDS,返回0表示正确,<0错误，大于0接收的返回字节数
	if(nret == SOCKET_ERROR) //错误
	{
		::closesocket(m_socket);
		m_socket = INVALID_SOCKET;
		return FALSE;
	}
	pb = baread.GetBuf(nsize);
	if((pb == NULL)||(nsize != 16))
	{
		::closesocket(m_socket);
		m_socket = INVALID_SOCKET;
		return FALSE;
	}

	if((pb[8] != OD_DDC_HEART) && (pb[11] != 0))//登录失败
	{
		::closesocket(m_socket);
		m_socket = INVALID_SOCKET;
		return FALSE;
	}
	return TRUE;
}

int		CHeartClient::ReadFrm(CMyByteArray *puca,int ntimeout)//读一个指定命令完整包,返回-1,0,>0字节数
{
	puca->ClearData();
	//int nt = (ntimeoutsec * 1000 )/200 + 1; //转成毫秒
	int nt = ntimeout / 200 + 1; //转成毫秒
	int nret = 0;
	for(int i=0;i<nt;i++)
	{
		nret = Read(m_ucTmp,8192,200);//每次读8K,没200毫秒超时读一次
		if(nret >0 )
			break;
		if(nret == SOCKET_ERROR)
			return SOCKET_ERROR;
	}
	if(nret <= 0) //错误或超时，断开服务器连接
		return SOCKET_ERROR;


	int nst;
	BYTE *pb;
	BOOL bbreak = FALSE;
	while(nret > 0) //有数据
	{
		nst = m_FrmParse.PutData(m_ucTmp,nret);
		pb = m_FrmParse.m_Frm.GetBuf();
		while(nst == FRMP_FRMOK)
		{
			puca->PutData(pb,m_FrmParse.m_Frm.GetDataSize());
			m_FrmParse.m_Frm.ClearData();//解析后重新初始化
			m_FrmParse.SetStatus(FRMP_NOHEAD);

			return puca->GetDataSize(); //返回读的完整包字节数
		}
		nret = Read(m_ucTmp,8192,1000);//每次读8K
	}
	m_FrmParse.m_Frm.ClearData();//解析后重新初始化
	m_FrmParse.SetStatus(FRMP_NOHEAD);

	return SOCKET_ERROR;
}

int CHeartClient::WriteSrv(void* pdata,int nsize,int ntimeoutm,CMyByteArray *puread) //写到DDS,返回0表示正确,<0错误，大于0接收的返回字节数
{
	int nret = Send((BYTE*)pdata,nsize,1000);
	if(nret <= 0)
		return -1;
	return ReadFrm(puread,ntimeoutm);
}

UINT	CHeartClient::DoRun()//线程运行
{
	m_bKill = FALSE;
	m_bRun = TRUE;

	SYSTEMTIME syst;
	BYTE sendbuf[32];
	DWORD *pdw = (DWORD*)(sendbuf + 16);
	CMyByteArray baread;
	BYTE *pb = NULL;
	int nsize;
	CMakeFrm sendfrm;

	int i,nret;
	while(!m_bKill)
	{

		if(m_socket == INVALID_SOCKET)
		{
			if(!ConnectSvr())
			{
				m_bSrvExist = FALSE;
				Sleep(100);
				continue;
			}
			m_bSrvExist = TRUE;
		}
		for(i=0;i<10;i++) //1秒钟发送一次
		{
			if(m_bKill)
				break;
		}

		::GetLocalTime(&syst);

		memcpy(sendbuf,&syst,sizeof(SYSTEMTIME));
		*((long *)sendbuf) = gGetDriveMgr()->DDSConnectedNum(); //填写到DDS的已连接个数

		*pdw  = Utility::GetCRC32(sendbuf,16);
		sendfrm.MakeFrm(OD_DDC_HEART,0,0,0x00,(BYTE*)sendbuf,20);//组织心跳包

		pb = sendfrm.GetFrm(&nsize); //取发送报文
		nret =  WriteSrv(pb,nsize,2000,&baread); //写到DDS,返回0表示正确,<0错误，大于0接收的返回字节数
		if(nret == SOCKET_ERROR)
		{
			::closesocket(m_socket);
			m_socket = INVALID_SOCKET;
			m_bSrvExist = FALSE;
		}
		else
		{
			pb = baread.GetBuf(nsize);
			if((pb == NULL)||(nsize != 16))
			{
				m_bSrvExist = FALSE;
				gGetDriveMgr()->m_ucWorkOrder = 0;
			}
			else
			{
				if(pb[8] != OD_DDC_HEART)
					m_bSrvExist = FALSE;
				else
				{
					m_bSrvExist = TRUE; //主存在
					if(pb[11] == 0xFE) //不需要工作
						gGetDriveMgr()->m_ucWorkOrder = 0xFE;
					else
						gGetDriveMgr()->m_ucWorkOrder = 0x00;

				}
			}
		}
		Sleep(1);
	}

	m_bKill = FALSE;
	m_bRun = FALSE;	
	TRACE("CWriteDDS Thread Exit\n");
	return 0;
}

UINT CHeartClient::ThreadRun(LPVOID lparam) //线程
{
	CHeartClient* pwd = (CHeartClient*)lparam;
	return pwd->DoRun();
}
void	CHeartClient::SetTcpNoDelay()//禁止Nagle优化
{
	int bNodelay = 1; 
	if(m_socket==INVALID_SOCKET)
		return;
	setsockopt( 
		m_socket,
		IPPROTO_TCP,
		TCP_NODELAY,
		(char *)&bNodelay,
		sizeof(bNodelay));//不采用延时算法 
}

int	CHeartClient::Read(BYTE *pbuf,int nsize,int ntimeout)
{
	if(m_socket == INVALID_SOCKET)
		return SOCKET_ERROR;
	TIMEVAL tv01 = {ntimeout/1000,1000 *(ntimeout%1000)};
	FD_SET fdr;
	fdr.fd_count = 1;
	fdr.fd_array[0] = m_socket;

	int nRet=::select(0, &fdr, NULL, NULL, &tv01);
	if(SOCKET_ERROR == nRet)//发生网络错误
		return SOCKET_ERROR;

	if(nRet==0)//超时发生，无可读数据
		return 0;

	nRet=::recv(m_socket, (char*)pbuf, nsize , 0);
	if(nRet == SOCKET_ERROR)
		return SOCKET_ERROR;
	return nRet;
} //读

int		CHeartClient::Send(BYTE *pbuf,int nsize,int ntimeout)
{
	if(m_socket == INVALID_SOCKET)
		return SOCKET_ERROR;

	TIMEVAL tv01 = {ntimeout/1000,1000 * (ntimeout%1000)};//1秒钟延迟
	FD_SET fdw;
	fdw.fd_count = 1;
	fdw.fd_array[0] = m_socket;

	int nRet = ::select(0, NULL, &fdw, NULL, &tv01);//测试可写
	if(SOCKET_ERROR == nRet)//发生网络错误
		return SOCKET_ERROR;

	if(nRet==0)//超时发生，不能发送数据
		return 0;
	nRet = ::send(m_socket, (char*)pbuf,nsize,0);
	if(SOCKET_ERROR == nRet)//发生网络错误
		return SOCKET_ERROR;
	return nRet;
} //发送

