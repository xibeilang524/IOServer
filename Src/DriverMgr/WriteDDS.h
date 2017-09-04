#pragma once
#include "DDSConnect.h"
#include "DecValMan.h"
#include "tool/frmtool.h"

//标记 wddscache 
#define WDDS_BUF_SIZE		2000			//写DDS的基本数据类型数据缓冲区大小
#define WDDSCACHE_BUF_SIZE (WDDS_BUF_SIZE/2)//写缓存基本数据类型缓冲大小
#define WDDSTIMEOUT			25000			//25秒超时

#define UDP_WDDS_BUF_SIZE		100
#define UDP_WDDSCACHE_BUF_SIZE	100


class CWriteDDS
{
protected:
	class CMKVALC //基本数据类型发送包组织
	{
	public:
		CMKVALC();
		~CMKVALC();
		char m_sbuf[256];
		int m_nsize;
		void MakeCell(CTagVal* ptv);
	};
public:
	CWriteDDS();
	~CWriteDDS();

	BOOL m_breal;//是否是实时, wddscache
public:
	CDDSConnect* m_pddscon;//DDS连接
	CDecValMan* m_pDecValMan;//设备数据管理
public:
	void Start();
	void Stop(BOOL bWaitForThreadExit);
	UINT DoRun();//线程运行
protected:
	CTagVal		m_wrec[WDDS_BUF_SIZE];	//写库用记录缓冲区,基本数据类型
	CTagVal		m_varval;//string 和BLOB的缓冲区，一次写一个，故只需要一个缓冲区即可
	CSoe		m_soe;	 //soe缓冲区，一次写一个。
	DWORD		m_dwPingTick;

	//线程管理
	BOOL	m_bRun; //运行
	BOOL	m_bKill;//结束线程

	//通信缓冲
	CMakeFrm	m_sendfrm;//发送报文
	CMyByteArray m_batmp;//临时数据段

protected:
	//以下<0 表示故障，>=0表示写的记录数
	int		WriteRealVal(BYTE &ucret,BYTE &ddsorder); //写基本数据类型实时
	int		WriteCasheVal(BYTE &ucret,BYTE &ddsorder);//写基本数据类型缓存

	int		WriteRealVar(BYTE &ucret,BYTE &ddsorder);	//写可变长记录string,blob 实时

	int		WriteCasheVar(BYTE &ucret,BYTE &ddsorder);	//写可变长记录string,blob 缓存

	int		WriteStreamMessage(BYTE &ucret,BYTE &ddsorder);	//写流消息

	int		WriteRealSoe(BYTE &ucret,BYTE &ddsorder);	//写SOE

	static  UINT ThreadDDS(LPVOID lparam); //线程

	void	SendDdsTags();//传送标签

	int    SendEmpty(BYTE &ucret,BYTE &ddsorder); //发送空命令
};

