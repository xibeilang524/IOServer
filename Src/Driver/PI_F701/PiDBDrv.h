// PiDBDrv.h: interface for the CPiDBDrv class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PIDBDRV_H__FBA770FC_637F_4AD7_B9D8_A05A0A3410E1__INCLUDED_)
#define AFX_PIDBDRV_H__FBA770FC_637F_4AD7_B9D8_A05A0A3410E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define CACHE_ITEM_MAX		1000 // 最大缓存标签值
#define FLUSH_DELAY_TIME	0.2 // 单位为秒，如果该延时时间值内还未达到最大可批量提交条件，也会被强制批量提交

// 如果需要SDK的类型检查，则定义该宏
#define DATATYPE_CHECKED	1

#pragma warning(disable : 4819) // warning C4819: 该文件包含不能在当前代码页(936)中表示的字符。请将该文件保存为 Unicode 格式以防止数据丢失

#include "..\sdk\dbsdk.h"
#include "..\sdk\dbdef.h"
#include <piapix.h>
//#include <pilogex.h>
#include "..\..\Common\CriticalSection.h"
#include "..\sdk\DbSummary.h"
#include "..\..\Common\DBDrvConnectString.h"

// 是否使用安全锁，对于多连接的PI，必须使用多线程
#define USE_SAFELOCK

#ifdef USE_SAFELOCK
	#define SAFELOCK(x) TPRI::CCriticalSection::Owner csLock(x)
	// 是否需要使用全局线程安全锁
	#define USE_STATIC_SAFELOCK		1
#else
	#define SAFELOCK(x) (void)0
#endif


typedef struct t_tagval2		//标签值
{
	long	lid;		//标签ID
	PIvaluetype pitype;
	TVVAL	m_val;		//值
	
}TTAGVAL2,*LPTAGVAL2;

class CPIServerItem
{
public:
	CPIServerItem(){

	}
	CPIServerItem(const CPIServerItem &other) {
		this->operator =(other);
	}
	~CPIServerItem() {

	}

	CPIServerItem &operator=(const CPIServerItem &other) {
		if (this != &other)
		{
			m_szIP = other.m_szIP;
			m_szPort = other.m_szPort;
			m_szUserName = other.m_szUserName;
			m_szPassword = other.m_szPassword;
			m_nConnTimeout = other.m_nConnTimeout;
			m_nDATimeout = other.m_nDATimeout;
		}

		return *this;
	}

public:
	CString		m_szIP;		//RDB IP地址
	CString	    m_szPort;	//服务端口
	CString     m_szUserName ;  //用户名称
	CString     m_szPassword ;  //用户密码
	int	        m_nConnTimeout;	//连接超时
	int	        m_nDATimeout;	//DA连接超时
};


typedef CArray<CPIServerItem> CPIServerList;


typedef struct _server_info
{
	CString szServer;
	UINT uiPort;
	CString szDBName;
	CString szTable;
	CString szUser;
	CString szPsw;
}T_ServerInfo;

#define PI_CONN_MAXTRY			1	//20，不作重连接尝试，交自代理服务器自己处理
#define PI_STRING_BUFFER		1024
#define PI_STRING_BUFFER_MAX	4096 // 最大允许读取字符串长度

class CSqlServerConnect;

typedef CMap<int,int, int,int> CMapCount;

class CPiDBDrv : public CDrvObj  
{
public:
	CPiDBDrv();
	virtual ~CPiDBDrv();
	
#ifdef USE_SAFELOCK
#ifdef USE_STATIC_SAFELOCK
	static TPRI::CCriticalSection m_csLock;
#else
	TPRI::CCriticalSection m_csLock;
#endif
#endif

public:
	DWORD ErrRetCode(DWORD dwcode); //返回值错误码转换，PI错误码转换为驱动错误码
	DWORD ErrDataCode(DWORD dwcode);//数据错误代码装换，PI错误码转换为驱动错误码
	DWORD DataTypeFrom(PIvaluetype pttype); //实时库类型转换为驱动类型
	PIvaluetype DataTypeTo(DWORD dwtype);	//驱动类型转换为实时库类型
	void Time2PiTimestamp(long lTime, PITIMESTAMP *pTimestamp);
	long PiTimestamp2Time(PITIMESTAMP *pTimestamp);
	short StateToQuality(int32 nIstat);
	int32 QualityToState(short nQuality);
	BOOL SetTVVAL(LPVVAL pvval, int32 digicode, int32 pttype, float64 drval, int32 ival, 
		void *bval, uint32 bsize, int32 istat, int16 flags, long ltime, short ierror);//wzq 20101128
	BOOL SetTVVAL(LPVVAL pvval, int32 digicode, int32 pttype, float rval, int32 istat, long ltime);
	BOOL SetTVVAL(LPVVAL pvval, short snerr);//wzq
	//驱动使用的protected属性
protected:
	int				m_nPIServerIndex;
	CPIServerList	m_arrPIServerList;
	T_ServerInfo	m_tServerInfo;

	TVVAL		m_vStrValue;	// 用于String\Blob类型访问
	CArray<char, char>	m_arrStrValue;	// m_vStrValue的动态缓冲区

	CArray<int32, int32> m_arrPoint;
	CArray<float64, float64> m_arrDrval;
	CArray<int32, int32> m_arrIval;
	CArray<int32, int32> m_arrIstat;
	CArray<int16, int16> m_arrFlags;
	CArray<PITIMESTAMP, PITIMESTAMP> m_arrTimestamp;
	CArray<int32, int32> m_arrErrors;
	CArray<int, int> m_arrIndex;
	CArray<void *, void *> m_arrBval;
	CArray<uint32, uint32> m_arrBsize;

	CArray<float, float> m_arrHisFRvals;
	CArray<int32, int32> m_arrHisTimestamp;
	CArray<int32, int32> m_arrHisIstat;

	CDbSummary	m_summary;
	CMapCount  m_mapCount;

	
	void LoadSQLConfig(CDBDrvConnectString &objConnect, T_ServerInfo& tServerInfo);
	void SaveSQLConfig(CDBDrvConnectString &objConnect, const T_ServerInfo& tServerInfo);

	void LoadConfig(CDBDrvConnectString &objConnect, CPIServerList *pPIServerList = NULL);
	void SaveConfig(CDBDrvConnectString &objConnect, CPIServerList *pPIServerList = NULL);
	CPIServerItem &GetPIServer() 
	{
		return m_arrPIServerList[m_nPIServerIndex];
	}

	BOOL CheckValueType(PIvaluetype pttype, DWORD dwtype);
	int32 InitWriteTagValues(TTAGVAL vals[], int nsize);
	int32 InitWriteTagValues(TTAGVAL2 vals[], int nsize);
	int DoDeleteHisVal(int32 nPointID, long ltimes, long ltimee);
	//连接数据库;
	BOOL ConnectSQL();
	int IsConnectSQL();
	int WriteSQL(LPT_WeightData pWeightData);
	int UpdateFlag(int szDevice, long ltime);
protected:
	BOOL SetActiveServer() {
		/*
		  piut_setservernode - 需要连接PI服务器
		  pilg_setservernode - 仅对现有的PI服务器连接进行切换

		  但是使用pilg_setservernode()函数需要链接pilog32.lib，并保证sdkreg.dll和pilog32.dll版本对应
		 */
		//return (pilg_setservernode(GetPIServer().m_szIP) == 0);
		return (piut_setservernode(GetPIServer().m_szIP) == 0);
	}
	
protected:
	CSqlServerConnect* m_pSQLServer;
	BOOL		m_bWritePIServer;
	BOOL		m_bWriteSQLServer;

	BOOL		m_bKillThread;
	CWinThread	*m_pThread;
	int			m_nStrRealVals;
	TTAGVAL2	m_arrStrRealVals[CACHE_ITEM_MAX];
	int			m_nStrHisVals;
	TTAGVAL2	m_arrStrHisVals[CACHE_ITEM_MAX];
	TPRI::CCriticalSection m_csBatchLock;

	void InitBatchParams();
	void ClearTagVals(LPTAGVAL2 pvals, int nsize);
	DWORD AddBatchTag(LPTAGVAL pval, BOOL bReal = TRUE);
	DWORD FlushBatchTags(BOOL bReal = TRUE);
	void StartBatchThread();
	void StopBatchThread();
	static	UINT RunBatchThread(void *pParam);// 运行线程

	//重载函数
public:
	void CheckConnect();

	virtual DWORD ExecCommand(
		DWORD dwCommand,		// 命令代码
		void *pParams,		// 命令参数数据指针
		DWORD dwParamSize,	// 命令参数大小
		void *pResults,		// 命令执行结果数据指针，由驱动分配内存，并和代理协商保证大小满足命令执行结果要求
		DWORD dwResultSize	// 命令执行结果数据大小
		);
	//管理
	virtual	void  OnDestroy(); //删除前调用
	virtual DWORD Connect(const char* sConnectStr);
	virtual DWORD DisConnect();
	virtual DWORD Config(const char* sConnectStr,char* sBuf, int  nBufLen); //配置
	
	//标签
	virtual DWORD GetTagAttr(LPDRVTAGATTR ptag); //取属性
	virtual DWORD AddTag(
			TDRVTAGATTR* ptag
		) ; //添加标签
	virtual DWORD DelTag(
			const char* stagname
		) ; //删除标签
	virtual DWORD EditTag(
			TDRVTAGATTR* ptag
		) ; //修改标签,已标签名为索引

	//写实时
	virtual DWORD WriteReal(
		TTAGVAL vals[],
		int nsize
		); //写实时
	virtual DWORD WriteHis(
		TTAGVAL vals[],
		int nsize
		);	//写历史
	virtual DWORD WriteSoe(
		TSOE    	soes[], //事件结构数组
		int			 nsize  //事件个数
	);
	
	//读实时
	virtual DWORD GetRealVal(
		TTAGVAL vals[],
		int		nsize
		) ;//读实时,只适合基本数据类型
	virtual DWORD GetRealStr(
		LPCTSTR sTagName,
		TVVAL** ppv
		) ; //取实时字符串类型,标签名在ptv->m_sname中
	virtual DWORD GetRealBlob(
		LPCTSTR sTagName,
		TVVAL** ppv
		) ;//取实时blob类型,标签名在ptv->m_sname中

	//读历史
	virtual DWORD GetHisVal(
		const char* stagname,	//标签名
		long ltimes,			//开始时标
		long ltimee,			//结束时标
		long ldsec,				//间隔时间秒
		long lflag,				//实时库标志，
		TVVAL vals[],			//数据回存空间
		int	 nsize,				//空间大小
		int* pnvals				//实际数据个数
		) ;  //读一个标签的历史，只针对基本数据类型
	/*
	virtual DWORD GetHisVal2(
		TTAGVAL vals[],		//标签名和回填值空间
		int		nsize,		//标签个数
		long	ltime		//时标
		) ;  //读多标签同一时刻历史，只针对基本数据类型
	*/
	virtual DWORD GetHisStr(
		DWORD dwrst,
		LPCTSTR sTagName,
		long ltimes,
		TVVAL** ppv
		) ; //取一条字符串类型标签历史值
	virtual DWORD GetHisBlob(
		DWORD dwrst,
		LPCTSTR sTagName,
		long ltimes,
		TVVAL** ppv
		) ;//取一条blob类型标签历史值
	
	//其他读
	virtual DWORD GetSoe(
		DWORD dwrst,
		long	ltimes, //开始时间
		long    lmsecs,	//开始毫秒
		const char* ssrc, //来源,NULL表示全部
		const char* sdec, //DDClient设备,NULL表示全部
		long lclsid,       //事件分类,0表示全部
		TSOE**  lppSoe
	); //读下一个SOE
	virtual DWORD GetSumVal(
		const char* stagname,	//标签名
		long ltimes,			//开始时标
		long ltimee,			//结束时标
		int  nSumType,			//统计方式
		long ldsec,				//分段时间，填0表示全区间
		TVVAL vals[],			//数据回存空间
		int	 nsize,				//空间大小
		int* pnvals				//实际个数
	);  //读统计值，只针对基本数据类型

	//删除
	virtual DWORD  DelSoe(
		long ltimes, //开始时标
		long lmsecs, //开始毫秒
		long timee,  //结束时标
		long lmsece, // 结束毫秒
		const char* ssrc, //来源,NULL表示全部
		const char* sdec, //DDClient设备,NULL表示全部
		long lclsid       //事件分类,0表示全部
	 ) ;//删除SOE事件
	virtual DWORD DelHisVal(
		const char* stagname, //标签名
		long	 ltimes,    //开始时标
		long	 ltimee		//结束时间
		) ;
	virtual DWORD  DelHisVal2(
		const char stagnames[][80], //标签名数组
		long	ltimes[],			//对应每个标签的时刻
		int		nerrcodes[],			//错误码，回填
		int		nsize				//记录个数
		) ;	
	virtual DWORD  GetMultiPointHisVal(
		char sTagNames[][80], 
		long lTimes[], 
		long lMode,
		TVVAL tvs[], 
		int nsize
		);	
	virtual DWORD  GetDiSumVal(
		LPCTSTR lpszTagName, 
		long lts, 
		long lte, 
		long lds, 
		long lval,
		long lpSumCount[], 
		long lpSumTimes[], 
		int nsize, 
		int* pnrecsize
		);	
	virtual DWORD GetHisValFilter(
		const char* stagname,	//标签名
		long ltimes,			//开始时标
		long ltimee,			//结束时标
		long ldsec,				//间隔时间秒
		long lflag,				//实时库标志，
		const char* expression,	//过滤条件字符串
		TVVAL vals[],			//数据回存空间
		int	 nsize,				//空间大小
		int* pnvals				//实际数据个数
		);//带条件过滤的读历史

	virtual DWORD TagSearch(
		const char* szTagNameMask,			// 标签名掩码，*表示全部，?表示任意一个字符
		const char* szTagDescrMask,			// 标签描述掩码，*表示全部，?表示任意一个字符
		int nDataTypeMask,					// 数据类型掩码，-1表示全部
		LPTAGSEARCHRESULT pSearchResults,	// 返回标签搜索结果的标签属性数组
		DWORD* pdwSearchResultCount,		// 传递pSearchResults数组大小，并返回标签搜索结果个数，如果该数组放满，则可能需要继续搜索，否则表示搜索完成，
											// 如果*pdwSearchResultCount==0，仅返回驱动是否支持标签搜索，DBERR_OK表示支持，DBERR_NOTSUPPORT表示不支持
		DWORD_PTR* pdwSearchSession,		// 初次传入0，返回标签搜索Session
		float* pfltPerProgress				// 返回搜索完成百分率，有效值0.0-100.0，如果返回值小于0，表示不支持搜索进度
		);

	virtual DWORD WriteStreamMessage(
		const char* lpszDeviceName,			// DDClient设备实例名
		const char* lpszTagName,			// 数据库标签名
		DWORD dwCommandID,					// 消息指令ID
		BYTE* pBuffer,						// 流消息块
		int nSize							// 流消息块大小
		);
};

#endif // !defined(AFX_PIDBDRV_H__FBA770FC_637F_4AD7_B9D8_A05A0A3410E1__INCLUDED_)
