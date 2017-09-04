#pragma once

////////////////////////////////////////////////////////////////////////////////
//以下为双前置TCP心跳服务
#define OD_DDC_ERROR 0x07 //错误命令
#define OD_DDC_LOGIN 0x08 //登录命令
#define OD_DDC_HEART 0x09 //心跳命令
class CHeartClient
{
public:
	CHeartClient(void);
	~CHeartClient(void);
	void SetArgs(LPCTSTR lpszip,WORD wport);//设置参数
	void Start();
	void Stop();

	BOOL	IsSrvExist(); //心跳服务器是否存在
	UINT	DoRun();//线程运行		
protected:
	int		Read(BYTE *pbuf,int nsize,int ntimeout);
	int		Send(BYTE *pbuf,int nsize,int ntimeout);
	int		ReadFrm(CMyByteArray *puca,int ntimeout);
	int		WriteSrv(void* pdata,int nsize,int ntimeoutm,CMyByteArray *puca); //写到DDS,返回0表示正确,<0错误，大于0接收的返回字节数
	BOOL	ConnectSvr();//连接
	void	SetTcpNoDelay();//禁止Nagle优化
	BOOL	DoWrite(void* pdata,int nsize,int ntimeoutmsec,BYTE &ucret,BYTE &ddsorder);	
	static UINT	ThreadRun(LPVOID lparam);//运行线程
	//线程管理
	BOOL	m_bRun; //运行
	BOOL	m_bKill;//结束线程

	CFrmParse		m_FrmParse;		//接收报文分析
	//连接参数
	CString m_szip;
	WORD	m_wport;

	BOOL	m_bSrvExist;//服务器是否存在
	BYTE	m_ucTmp[8192];
	SOCKET	m_socket;	//套接字
};

