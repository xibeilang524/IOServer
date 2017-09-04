#ifndef __TPRI_DDCLIB_H__
#define __TPRI_DDCLIB_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxsock.h>		// MFC socket extensions
#include <afxmt.h>
#include <afxtempl.h> //模版支持

#include <drvdef.h>
#include <drvsdk.h>
#include <sdktool.h>
#include <SortObArray.h>
#include <WinList.h>
#include <TagItem.h>

#ifndef _DDC_LIB
#ifdef _DEBUG
#pragma comment( lib, "ddclib_debug.lib")
#else
#pragma comment( lib, "ddclib.lib")
#endif

//#pragma comment( lib, "ddclib.res")
#endif
//#include <afxdao.h>

// 通讯协议定义
#define COM_TYPE_COM		0x01	// 串行口
#define COM_TYPE_UDP		0x02	// UDP Client协议
#define COM_TYPE_UDPSERV	0x04	// UDP Server协议
#define COM_TYPE_TCP		0x08	// TCP Client协议
#define COM_TYPE_TCPSERV	0x10	// TCP Server协议
#define COM_TYPE_DB		    0x20	// ODBC DB
#define COM_TYPE_FILE		0x40	// 文件类型
///////////////////////////////////////////////////////////////////

#define BUF_LENGTH_SERIAL	4096
#define BUF_LENGTH_TCP		8192
#define BUF_LENGTH_UDP		65536

#define UPDATEVALTAGS		2000	// 每次最大更新标签数

// 根据系统错误码，获得格式化的错误信息，dwErrorCode一般由GetLastError()函数获得
CString GetSystemErrorMessage(DWORD dwErrorCode);

class CDDCDrvObj;

class CConfigInfo
{
public:
	friend CDDCDrvObj;
public:
	CConfigInfo();
	virtual ~CConfigInfo();
	void Serialize(CArchive& ar, DWORD dwVersion);
	
public:	
	BOOL IsModified() { return m_bModified; }

	CConfigInfo& operator=(const CConfigInfo& config);
	BOOL operator==( const CConfigInfo& config);
	BOOL operator!=( const CConfigInfo& config);
	CString GetConnectString();

	void CalcTimeOffset() {
		m_nTimeOffset = m_nTimeBias * 3600;
	}
public:
	static CString m_szHelpFile;	// 驱动配置帮助文件名称，不带路径名，该文件和驱动动态库放在一起，如果为 res://1234 格式，表示从资源中加载资源ID为1234的TEXT资源显示给用户
	static CString m_szParamDisplayName; // 连接参数显示名
	static CString m_szParamHintText;	// 连接参数的提示文本

	static BOOL	m_bSelectFileName;		// 配置文件路径还是文件名？？
	static CString m_szSelectFileFilter;		// 打开文件过滤器
	static CString m_szSelectFileHint;	// 打开目录提示文本

	BOOL		m_bModified;
	
	WORD		m_wComType;			// 通讯协议类型

	// TCP/UDP通讯参数
	CString		m_szIP;				// TCP/UDP服务器IP地址，如果是COM_TYPE_TCPSERV方式，则该项不用
	WORD	    m_wPort;			// TCP/UDP服务端口
	CString		m_szTcpUdpParam;	// TCP/UDP连接参数，除非特殊的通讯协议，一般不用
	
	// 串行口通讯参数
	CString		m_szComPort;		// 端口名，如"\\\\.\\com1","\\\\.\\com2"
	DWORD		m_dwBaudRate;		// 波特率,CBR_1200,CBR_2400,CBR_4800,CBR_9600,
									// CBR_14400,CBR_19200,CBR_38400,CBR_57600,CBR_115200,
									// CBR_14400,CBR_19200,CBR_38400,CBR_57600,CBR_115200,
	BYTE		m_cByteSize;		// 数据位 number of bits/byte, 4-8
	BYTE		m_cParity;			// 0-4对应no,odd,even,mark,space 
	BYTE		m_cStopBits;		// 0,1,2 对应 1, 1.5, 2
	CString		m_szComParam;		// 串行口连接参数，除非特殊的通讯协议，一般不用
   
	//odbc参数
	CString		m_szID;
	CString		m_szPsw;
	CString		m_szDataSource;

	// File格式参数
	CString		m_szFileName;		// 文件名
	BOOL		m_bAutoDeleteFile;	// 是否在读取数据后自动删除文件？？
	CString		m_szFileParam;		// 可扩展文件参数

	// 驱动配置参数
	int			m_nReadTimeout;		//读超时
	int			m_nWriteTimeout;	//读超时
	int			m_nUpdateSec;       //采集周期，毫秒
	int			m_nTimeBias;		//时区偏移量
	BOOL		m_bDebug;           //调试信息记录
	BOOL		m_bLocalTime;		//是否使用本地时标，如果驱动协议中不包含时标，则忽略该配置

protected:	
	long		m_nTimeOffset;
	CDDCDrvObj *m_pdrv;
};


/*
CRC32计算函数
szData:数据
dwSize:数据字节数
*/
//DWORD	TL_GetCRC32(unsigned char *szData, unsigned long dwSize);	// 计算CRC32

class CCheckAlgorithm
{
public:
	CCheckAlgorithm(){};
	~CCheckAlgorithm(){};	
public:
	/*
	CRC32计算函数
	szData:数据
	dwSize:数据字节数
	*/
	static DWORD TL_GetCRC32(unsigned char *szData, unsigned long dwSize);	// 计算CRC32

	// 创建查表数组
	static void  Init_CRC32_Table();

	static DWORD Reflect(unsigned long ref, char ch);
//CRC16
private:
	static unsigned long crc32_table[256]; 
	static BOOL		b_crc32IninTab;
};
/*------------------------------------------------------------------------------------
																	
字节动态数组CMyByteArray，用于保存数据报文或待分析数据
																	  
*/
#define MAX_BYTEARRAYSIZE	(1024 * 2048)	// 最大字节动态数组大小2M

class CMyByteArray //字节动态数组，用于保存数据报文或待分析数据
{
public:
	CMyByteArray();
	~CMyByteArray();
public:
	BOOL PutData(BYTE *pBuf,int nSize);
	
	BOOL PutStr(LPCTSTR lpszData,int nSize);
	
	BYTE *GetBuf();
	int  GetDataSize();
	void ClearData();
	void SetGrown(int ngrown);
	void LeftMove(int n);
	void CopyTo(CMyByteArray *pba); //复制到另一个字节数组尾部.
protected:
	BYTE *m_pData;		//数据段
	int  m_nSize;		//数据大小
	int  m_nBufSize;	//空间大小
	int	 m_nGrown;		//增长增量
};

/*------------------------------------------------------------------------------------

  报文解析 CFrmParse,每个客户连接后就创建一个该对象
  
*/

// 解决分包返回状态
#define FRAME_STATUS_NO_HEAD			0x00	// 没找到报头标志
#define FRAME_STATUS_HEAD_NO_ENOUGH		0x01	// 找到报头标志,长度不够无法验证报头
#define FRAME_STATUS_DATA_NO_ENOUGH		0x02	// 报头校验完成,数据长度不够
#define FRAME_STATUS_OK					0x03	// 报文完成

class CFrmParse //报文解析,只是分包
{
public:
	CFrmParse();
	virtual ~CFrmParse();

public:
	CMyByteArray	m_Frm;

	int		GetFrmLen() { return m_nFrmLen; }
	void*	GetBuffer() { return m_buf.GetBuf(); }

	void	Clear() { m_buf.ClearData(); }

	int		PutData(BYTE *buf,int nsize);//放入未解析数据,nsize < 256K
	int		GetStatus(){return m_nStatus;};
	void	SetStatus(int nst){m_nStatus = nst;};
	BOOL	CheckCrc32(BYTE *pd);//按照16字节报头校验
	BYTE*	FindFlag(BYTE *pDataBuffer, int nDataSize, BYTE *pFlag, int nFlagSize);

public:
	virtual int	Parse();//分包,返回m_nStatus

protected:
	int				m_nFrmLen;		//报文长度
	int				m_nStatus;		//0:无，1：报头开始标志找到，2：报头完成，3：报文完成
	CMyByteArray	m_buf;			//待处理字节	
};

/*
获取程序运行时间
*/
class CTimeCount   
{   
public:
	CTimeCount(){};
	~CTimeCount(){};
	
	//开始计时
	void start() 
	{
		QueryPerformanceFrequency(&litmp);
		dfFreq = (double)litmp.QuadPart;//   获得计数器的时钟频率
		QueryPerformanceCounter(&litmp);
		QPart1 = litmp.QuadPart;//   获得初始值
	};

	//停止计时 
	void end()   
	{
		QueryPerformanceCounter(&litmp);
		QPart2	= litmp.QuadPart;//获得中止值
		dfMinus	= (double)(QPart2-QPart1);
		dfTim	= dfMinus / dfFreq;//   获得对应的时间值，单位为秒 
	}; 
	
	//以毫秒精度输出所用时间 
	double time_milli() 
	{
		return dfTim * 1000.0;
	};
	   
private:   
	LARGE_INTEGER   litmp;     
    LONGLONG   QPart1,QPart2;   
    double   dfMinus,   dfFreq,   dfTim;     
}; 
  
//锁类
class CSafeLock  
{
public:
	CSafeLock(CCriticalSection *pLock){
		m_pLock = pLock;
		m_pLock->Lock();
	};
	virtual ~CSafeLock() {
		m_pLock->Unlock();
	};
private:
	CCriticalSection *m_pLock;
};

class CDDCDrvObj;

class CDDCDrvProtocol
{
public:
	friend CDDCDrvObj;
	CDDCDrvProtocol();
	virtual ~CDDCDrvProtocol();

	CDDCDrvObj *GetDrvObj() {
		return m_pDrvObj;
	}
	BOOL GetBlockingMode() {
		return m_bBlockingMode;
	}
public:
	void SetConfig(CConfigInfo &config) { m_config = config; }
	CConfigInfo &GetConfig() { return m_config; }
	
	void SetAutoUpdateSec(BOOL bUpdateSec = TRUE) { m_bAutoUpdateSec = bUpdateSec; }
	BOOL GetAutoUpdateSec() { return m_bAutoUpdateSec; }
	void WriteDebugLog(BOOL nReadMode, BYTE *pb, int nsize);

public: //纯虚函数
	//下面定义的纯虚函数，必须重载后才能用
	virtual BOOL	IsConnect()						= 0;//端口是否打开
	virtual BOOL	Connect()						= 0;
	virtual void	Disconnect()					= 0;
	virtual int		SendData(BYTE *pb, int nsize)	= 0;//发送通信包,返回发送的字节数	
	virtual int		ReadData(BYTE *pb, int nsize)   = 0;//接受通信包,返回接收的字节数

	virtual DRVRET  WriteVar(LPDRREC lpval)			= 0;
	virtual void	DoRun()							= 0;

protected:
	BOOL				m_bAutoUpdateSec;
	CConfigInfo			m_config;
	CDDCDrvObj			*m_pDrvObj;
	BOOL				m_bBlockingMode;
};

//串行口通信方式
class CSerialProtocol : public CDDCDrvProtocol
{
public:
	CSerialProtocol();
	virtual ~CSerialProtocol();

protected:
	HANDLE  m_hComDev;				//通信端口句柄
	int		m_nerrnum;				//错误次数,用于监测串口错误，自复位用
	
public:
	virtual BOOL	IsConnect();
	virtual BOOL	Connect();
	virtual void	Disconnect();
	virtual int		SendData(BYTE *pb, int nsize);
	virtual int		ReadData(BYTE *pb, int nsize);
	virtual DRVRET  WriteVar(LPDRREC lpval);
};

//UDP通信方式基类
class CUDPProtocolBase : public CDDCDrvProtocol 
{
public:
	CUDPProtocolBase();
	virtual ~CUDPProtocolBase();
	
protected:
	SOCKET	m_sClient;	
	struct sockaddr m_sockaddr;
	
	int	Read(BYTE *pbuf,int nsize,int ndelaymsec);
	int	Write(BYTE *pbuf,int nsize,int ndelaymsec);
	BOOL SetRcvBuf(int nSize);
	BOOL GetRcvBuf(int &nSize);
public:
	virtual BOOL	IsConnect();
	virtual void	Disconnect();
	virtual int		SendData(BYTE *pb, int nsize);
	virtual int		ReadData(BYTE *pb, int nsize);
	virtual DRVRET  WriteVar(LPDRREC lpval);
};

//UDP Client通信方式
class CUDPClientProtocol : public CUDPProtocolBase 
{
public:
	virtual BOOL	Connect();
};

//UDP Server通信方式
class CUDPServerProtocol : public CUDPProtocolBase 
{
public:
	CUDPServerProtocol();
	virtual BOOL	Connect();
};

/*
	CUDPProtocolBase类用select()函数实现recvfrom()阻塞函数的超时功能，但select()函数
	在某些情况下会出来查询结果不正确的问题，重写UDP通信方式基类CUDPProtocolBaseEx，直
	接使用阻塞recvfrom()，不使用select()来处理超时
 */
class CUDPProtocolBaseEx : public CDDCDrvProtocol 
{
public:
	CUDPProtocolBaseEx();
	virtual ~CUDPProtocolBaseEx();

protected:
	SOCKET	m_sClient;	
	struct sockaddr m_sockaddr;

	int	Read(BYTE *pbuf,int nsize,int ndelaymsec);
	int	Write(BYTE *pbuf,int nsize,int ndelaymsec);
	BOOL SetRcvBuf(int nSize);
	BOOL GetRcvBuf(int &nSize);
public:
	virtual BOOL	IsConnect();
	virtual void	Disconnect();
	virtual int		SendData(BYTE *pb, int nsize);
	virtual int		ReadData(BYTE *pb, int nsize);
	virtual DRVRET  WriteVar(LPDRREC lpval);
};

//UDP Client通信方式
class CUDPClientProtocolEx : public CUDPProtocolBaseEx 
{
public:
	virtual BOOL	Connect();
};

//UDP Server通信方式
class CUDPServerProtocolEx : public CUDPProtocolBaseEx 
{
public:
	CUDPServerProtocolEx();
	virtual BOOL	Connect();
};

//TCP Client通信方式

class CTCPClientProtocol : public CDDCDrvProtocol 
{
public:
	CTCPClientProtocol();
	virtual ~CTCPClientProtocol();

protected:
	SOCKET	m_sClient;
		
	int		Read(SOCKET s, BYTE *pbuf, int nsize, int ndelaymsec);
	int		Write(SOCKET s, BYTE *pbuf, int nsize, int ndelaymsec);

public:
	//{{必须重载的函数
	virtual BOOL	IsConnect();
	virtual BOOL	Connect();
	virtual void	Disconnect();
	virtual int		SendData(BYTE *pb, int nsize);
	virtual int		ReadData(BYTE *pb, int nsize);
	virtual DRVRET  WriteVar(LPDRREC lpval);
	//}}
};

// 阻塞方式的TCP Client通信方式
class CTCPClientProtocolEx : public CDDCDrvProtocol 
{
public:
	CTCPClientProtocolEx();
	virtual ~CTCPClientProtocolEx();

protected:
	SOCKET	m_sClient;

	int		Read(SOCKET s, BYTE *pbuf, int nsize, int ndelaymsec);
	int		Write(SOCKET s, BYTE *pbuf, int nsize, int ndelaymsec);

public:
	//{{必须重载的函数
	virtual BOOL	IsConnect();
	virtual BOOL	Connect();
	virtual void	Disconnect();
	virtual int		SendData(BYTE *pb, int nsize);
	virtual int		ReadData(BYTE *pb, int nsize);
	virtual DRVRET  WriteVar(LPDRREC lpval);
	//}}
};

//TCP SERVER通信方式
class CTCPServerProtocol : public CDDCDrvProtocol 
{
public:
	CTCPServerProtocol();
	virtual ~CTCPServerProtocol();

public:
	//{{必须重载的函数
	virtual BOOL	IsConnect();
	virtual BOOL	Connect();
	virtual void	Disconnect();
	virtual int		SendData(BYTE *pb, int nsize);
	virtual int		ReadData(BYTE *pb, int nsize);	
	virtual DRVRET  WriteVar(LPDRREC lpval);
	//}}

protected:
	BOOL	IsAccept();
	BOOL	ListenClient();
	void	DisconnectClient();
	int		Read(SOCKET s, BYTE *pbuf, int nsize, int ndelaymsec);
	int		Write(SOCKET s, BYTE *pbuf, int nsize, int ndelaymsec);
	
protected:
	SOCKET				m_hSocket;				//=INVALID_SOCKET;
	struct	sockaddr_in m_HostAddr;				//服务器主机地址
	
	BOOL				m_bRun;					//运行标志
	
	CCriticalSection	m_Lock;					//锁
	SOCKET				m_hConnect;             //连接的SOCKET
	struct sockaddr_in	m_addrClient;           //客户地址
	int					m_nConn;				//当前的连接个数	
};

// 阻塞方式的TCP SERVER通信方式
class CTCPServerProtocolEx : public CDDCDrvProtocol 
{
public:
	CTCPServerProtocolEx();
	virtual ~CTCPServerProtocolEx();

public:
	//{{必须重载的函数
	virtual BOOL	IsConnect();
	virtual BOOL	Connect();
	virtual void	Disconnect();
	virtual int		SendData(BYTE *pb, int nsize);
	virtual int		ReadData(BYTE *pb, int nsize);	
	virtual DRVRET  WriteVar(LPDRREC lpval);
	//}}

protected:
	BOOL	IsAccept();
	BOOL	ListenClient();
	void	DisconnectClient();
	int		Read(SOCKET s, BYTE *pbuf, int nsize, int ndelaymsec);
	int		Write(SOCKET s, BYTE *pbuf, int nsize, int ndelaymsec);

protected:
	SOCKET				m_hSocket;				//=INVALID_SOCKET;
	struct	sockaddr_in m_HostAddr;				//服务器主机地址

	BOOL				m_bRun;					//运行标志

	CCriticalSection	m_Lock;					//锁
	SOCKET				m_hConnect;             //连接的SOCKET
	struct sockaddr_in	m_addrClient;           //客户地址
	int					m_nConn;				//当前的连接个数	
};

class CDataBaseProtocol: public CDDCDrvProtocol 
{
public:
	CDataBaseProtocol(){};
	virtual ~CDataBaseProtocol(){};
public:
	virtual BOOL IsConnect() {return FALSE;};
	virtual BOOL IsNeedReConnect(const CString& szStatus)
	{
		//取index为6-10
		CString szval = szStatus.Left(11);
		szval = szval.Right(5);
		
		// 	08001	无法连接到数据源,一般在Open DB时的错误信息
		// 	08003	连接未打开
		// 	08007	在执行事务的过程中连接失败
		// 	08S01	通讯链接失败，一般在打开记录集或调用直接执行SQL语句时出错。
		
		if((szval == "08S01")||
			(szval == "08001")||
			(szval == "08003")||
			(szval == "08007")
			)
			return TRUE;
		return FALSE;
	};
	virtual BOOL DBConnect() {return FALSE;};
	virtual BOOL Connect() {return TRUE;};
	virtual void Disconnect() {};
	virtual int	SendData(BYTE *pb, int nsize) {return 0;};
	virtual int	ReadData(BYTE *pb, int nsize) {return 0;};
	virtual DRVRET  WriteVar(LPDRREC lpval) {
		return DRERR_NOTSUPPORT;
	}
};

//UDP通信方式基类
class CFileProtocol : public CDDCDrvProtocol 
{
public:
	CFileProtocol() {};
	virtual ~CFileProtocol() {};

public:
	virtual BOOL	IsConnect() {
		return TRUE;
	};
	virtual BOOL	Connect() {
		return TRUE;
	};
	virtual void	Disconnect() {
	};
	virtual int		SendData(BYTE *pb, int nsize) {
		return 0;
	};
	virtual int		ReadData(BYTE *pb, int nsize) {
		return 0;
	};
	virtual DRVRET  WriteVar(LPDRREC lpval) {
		return DRERR_NOTSUPPORT;
	};
};

class CDDCDrvObj : public CDrvObj  
{
public:
	CDDCDrvObj();
	virtual ~CDDCDrvObj();
		
public:
	CTagDefTab			m_tagtab;	//标签表
	CConfigInfo			m_config;
	CTagDefTab::CTagItemMap	m_mapTagIDs;

	void EnabledTestConnect(BOOL bEnabled = TRUE) {	m_bCanTestConnect = bEnabled; }
	BOOL CanTestConnect() { return m_bCanTestConnect; }
	DWORD GetSerializeID() { return m_dwSerializeID; }
	WORD GetVersion() { return m_wVersion; }
	void SetSerializeID(DWORD dwSerializeID) { m_dwSerializeID = dwSerializeID; }
	WORD GetComTypes() { return m_wComTypes; }
	void SetComTypes(WORD wComTypes) { 
		if ((wComTypes & COM_TYPE_COM) == COM_TYPE_COM)
			m_config.m_wComType = COM_TYPE_COM;
		else if ((wComTypes & COM_TYPE_UDP) == COM_TYPE_UDP)
			m_config.m_wComType = COM_TYPE_UDP;
		else if ((wComTypes & COM_TYPE_UDPSERV) == COM_TYPE_UDPSERV)
			m_config.m_wComType = COM_TYPE_UDPSERV;
		else if ((wComTypes & COM_TYPE_TCP) == COM_TYPE_TCP)
			m_config.m_wComType = COM_TYPE_TCP;
		else if ((wComTypes & COM_TYPE_TCPSERV) == COM_TYPE_TCPSERV)
			m_config.m_wComType = COM_TYPE_TCPSERV;
		else if ((wComTypes & COM_TYPE_DB) == COM_TYPE_DB)
			m_config.m_wComType = COM_TYPE_DB;
		else if ((wComTypes & COM_TYPE_FILE) == COM_TYPE_FILE)
			m_config.m_wComType = COM_TYPE_FILE;
		m_wComTypes = wComTypes; 
	}
	BOOL CanSerialType() { return ((m_wComTypes & COM_TYPE_COM) == COM_TYPE_COM); }
	BOOL CanUdpType() { return ((m_wComTypes & COM_TYPE_UDP) == COM_TYPE_UDP); }
	BOOL CanUdpServerType() { return ((m_wComTypes & COM_TYPE_UDPSERV) == COM_TYPE_UDPSERV); }
	BOOL CanTcpType() { return ((m_wComTypes & COM_TYPE_TCP) == COM_TYPE_TCP); }
	BOOL CanTcpServerType() { return ((m_wComTypes & COM_TYPE_TCPSERV) == COM_TYPE_TCPSERV); }
	BOOL CanDBType() { return ((m_wComTypes & COM_TYPE_DB) == COM_TYPE_DB); }
	BOOL CanFileType() { return ((m_wComTypes & COM_TYPE_FILE) == COM_TYPE_FILE); }
	// 配置信息访问
	BOOL IsDebug() { return m_config.m_bDebug; }
	BOOL IsLocalTime() { return m_config.m_bLocalTime; }
	int GetReadTimeout() { return m_config.m_nReadTimeout; }
	int GetWriteTimeout() { return m_config.m_nWriteTimeout; }
	int GetUpdateSec() { return m_config.m_nUpdateSec; }
	BOOL EnabledConnectionFailedStartup() { return m_bConnectionFailedStartup; }
	void SetConnectionFailedStartup(BOOL bConnectionFailedStartup) { m_bConnectionFailedStartup = bConnectionFailedStartup; }
	
	DWORD OnDataOk(	//数据回调函数
		T_DRREC  recs[],		//记录集
		int nrecs			//记录个数
		);
	/*
	DWORD OnLogOk( //日志回调函数
		long ltime,			//时标
		const char *plog			//日志字符串，0结束的字符串
		);*/
	
	DWORD OnSoeOk( //SOE事件回调函数,每次回调一个事件
		LPSOE psoe			//事件
		);

	void InitTagIdMap(BOOL bIgnoreCase = FALSE);	// 初始化标签ID名称MAP表，如果协议不需要ID到TagName的映射关系，则可以不用调用该函数
	CTagItem *FindByID(LPCTSTR lpszID); // 根据标签ID名称从Map表中快速查找标签
	CTagItem *FindByName(LPCTSTR lpszTagName) { return m_tagtab.GetTag(lpszTagName); } // 根据标签名称从Map表中快速查找标签

	// 根据标签的ID名称添加一个回调值
	BOOL AddValueByID(LPCTSTR lpszID, long ltime, short snqa, long val);
	BOOL AddValueByID(LPCTSTR lpszID, long ltime, short snqa, __int64 val);
	BOOL AddValueByID(LPCTSTR lpszID, long ltime, short snqa, float val);
	BOOL AddValueByID(LPCTSTR lpszID, long ltime, short snqa, double val);
	BOOL AddValueByID(LPCTSTR lpszID, long ltime, short snqa, LPCTSTR val);
	BOOL AddValueByID(LPCTSTR lpszID, long ltime, short snqa, void *val, int nsize);

	// 根据标签的名称添加一个回调值
	BOOL AddValueByName(LPCTSTR lpszTagName, long ltime, short snqa, long val);
	BOOL AddValueByName(LPCTSTR lpszTagName, long ltime, short snqa, __int64 val);
	BOOL AddValueByName(LPCTSTR lpszTagName, long ltime, short snqa, float val);
	BOOL AddValueByName(LPCTSTR lpszTagName, long ltime, short snqa, double val);
	BOOL AddValueByName(LPCTSTR lpszTagName, long ltime, short snqa, LPCTSTR val);
	BOOL AddValueByName(LPCTSTR lpszID, long ltime, short snqa, void *val, int nsize);

	BOOL FlushValue();	// 将剩余标签值回传到DDC

	DWORD WriteStreamMessageCB( const char* lpszTagName, DWORD dwCommandID, BYTE* pBuffer, int nSize );
	DWORD WriteHistoryCB( T_DRREC recs[], int nrecs );

	virtual BOOL TestConnect(CConfigInfo &config);
	
protected:
	BOOL				m_bCanTestConnect;
	DWORD				m_dwSerializeID;		// 驱动配置标识码
	WORD				m_wVersion;				// 驱动版本号
	WORD				m_wComTypes;			// 驱动协议类型
	CDDCDrvProtocol		*m_pProtocol;			// 驱动协议处理器实例指针
	CRdRec				m_recs[UPDATEVALTAGS];	// 采集值回调参数
	int					m_nRecCount;			// 采集值个数，如果达到UPDATEVALTAGS会自动提交，交重设置为0

	BOOL				m_bIgnoreCase;			// TagID是否忽略大小写，默认为FALSE
	BOOL				m_bConnectionFailedStartup;	// 连接失败后是否允许启动服务
	
	//以下重载基类的函数
public: //虚函数
	virtual DRVRET  GetErrMsg(CString& szMsg,DRVRET dwcode);			//获取错误信息
	virtual DRVRET  WriteVar(LPDRREC lpval);							//写变量
	virtual DRVRET  GetTagDef(int nindex,LPDRVTAG  lptag);
	
protected:
	virtual CDDCDrvProtocol	*CreateProtocol() = 0;	//根据配置的协议类型创建协议处理对象	
	BOOL CheckUpdateSec(BOOL bForceUpdateSec = TRUE);
	long GetValueTimestmap(long ltime)	{			// 将采样时标根据配置情况转换为最终的回调时标
		if ((ltime <= 0) || m_config.m_bLocalTime)
		{
			ltime = (long)::time(NULL);
		}
		
		ltime += m_config.m_nTimeOffset;
		return ltime;
	}

protected: //纯虚函数
	virtual void	OnInitConfig();				// 初始化配置信息
	virtual BOOL	OnConfig();					// 配置和配置界面,返回TRUE表示修改了配置
	virtual void	OnBeginConfig();			// 开始配置之前调用，重载可以检验配置值是否需要重新初始化
	virtual void	OnEndConfig();				// 配置完成，并准备保存配置信息之前调用，重载可以检验配置是否有效，如果无效，可以强制设置为默认的有效值
	virtual void	Serialize(CArchive& ar);	// 串行化保存，读取配置
	virtual DRVRET	OnStart();					// 准备运行
	virtual void	OnStop();					// 停止运行后的清理工作
	virtual void	OnDisconnect();				// 断开连接
	virtual UINT	DoRun();					// 正真的运行过程，被RunThread中调用

private:
	FILETIME		m_ftStartTime;				// 开始时间
	FILETIME		m_ftEndTime;				// 终止时间

	long GetElapsedTime() {	// 计算毫秒级时差
		ULONGLONG qst = *((ULONGLONG*)(&m_ftStartTime));
		ULONGLONG qet = *((ULONGLONG*)(&m_ftEndTime));
		return (long)((qet - qst) / 10000);
	}
};

#endif // end of __TPRI_DDCLIB_H__
