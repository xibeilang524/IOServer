// DriverMgr.h : DriverMgr DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号

#include "DecValMan.h"
#include "DbpConArgs.h"
#include "DDSConnect.h"
#include "WriteDDS.h"
#include "Define.h"
#include "HeartClient.h"
#include "HeartServer.h"
#include "cashe.h "
#include "DdsTagTab.h"
#include "DriverMgr_global.h"

typedef struct _out_file_cfg
{
	CString szPath;
	CString szName;
	long	lInterval;
	BOOL	bReplace;
}T_OutFileCfg, *LPT_OutFileCfg;

#define MAXDDSCONS 10

class _DRIVERMGR_CLASS CDriverMgr
{
private:
	CDriverMgr(void);
	~CDriverMgr(void);
	static CDriverMgr* Instance;
public:
	static CDriverMgr* GetInstance(void);
	//CMap<CString,LPCTSTR,CDrvItem*,CDrvItem*> m_mapDec;//驱动实例设备表
	CNoCaseMap<CDrvItem*,CDrvItem*> m_mapDec;//驱动实例设备表
	CDecValMan			m_DecValMan;//设备数据
	CDbpConArgs			m_dbpconargs; //DDS连接参数
	int					m_nWDdsReals;//写实时线程数
	CDDSConnect*		m_ddsConnects[MAXDDSCONS];	//DDS连接
	CWriteDDS*			m_WriteDDSs[MAXDDSCONS];	//写DDS
	CDDSConnect			m_ddsConCache;	//DDS连接
	CWriteDDS			m_writeDdsCache;//写DDS 缓存  
	CLocalCashe			m_cashe;	//本地缓存
	CDdsTagTab			m_ddsTagTab;//传送给DDS的标签表,带锁的
	int					m_nbakddcconnum;	//备DDC到DDS的已连接数
	long				m_lHeartTime;		//上次收到心跳的时间
	BYTE				m_ucWorkOrder;		//主发来的工作命令
	long				m_lfileflag; //文件标识 
	BOOL				m_bRefresh;

	//原ini参数
	CString				m_szProjectName;
	static BOOL			m_bAutoRun;		//启动时自动运行驱动实例
	static BOOL			m_bSaveLog;		//保存日志
	static BOOL			m_bExitPass;	//退出密码验证
	static BOOL			m_bCashe;		//是否使用cashe
	static BOOL			m_bSkipSameTime;//放弃相同时标快照更新
	static int			m_nTimeZero;	//显示时区-12,12
	static WORD			m_wHeartPort;	//心跳端口
	static CString		m_szHeartIP;	//心跳IP,空表示本机是服务器
	static int			m_nLogLevel;	//日志级别，0-错误、1-警告、2-提示、3-调试
	static CString		m_szTitle;		//DDClient附加显示标题，以区分DDClient的功能
	static BOOL			m_bWaitTcpAck;	//TCP是否有响应包，部分网闸是不允许有响应包的
	static int			m_nUdpSendDelay;//UDP发包延时
	static int			m_nUpThreads;	//上传线程数
	//计算获得
	static long			m_lAdjustSec;	//显示调整间秒,即m_nTimeZero * 3600 + TIME_ZONE_INFORMATION.Bias * 60
	//end
	T_OutFileCfg		m_tOutFileCfg;
	BOOL				m_bUseFileOutput;
	BOOL				m_bOutFileThreadRun;
	long				m_tmLastSave;
protected:
	long				m_lDrvNextID;
	CString				m_szres;  //保留
	CString				m_szPrjName;	//工程名
	CString				m_szRootPath;	//DDC根目录,后带/
	CHeartServer		m_HeartSrv;//双前置心跳服务
	CHeartClient		m_HeartClient;//心跳客户方



private:
	void		CreateDrvInst(LPCTSTR lpszdrvpath);//初始化化驱动，创建驱动实例
	void		Serialize(CArchive& ar);	//存盘

public:
	void		AddLogContent(DWORD dwLoglevel, LPCTSTR szLog);
	void		SetLogLevel(DWORD dwLevel);
	void		DelOneDec(LPCTSTR lpszDecName);//删除设备,一并从树中删除
	CString		GetDDsStatus();	//取DDS状态
	BOOL		IsDecExist(LPCTSTR sdecname);
	BOOL		OpenProject(LPCTSTR szRoot,LPCTSTR szProj); //打开工程
	BOOL		SavePrjFile(); //保存工程文件
	int			DDSConnectedNum(); //到DDS的写实时连接好的连接个数
	long		GetDrvNextID(); //取下一个ID号
	BOOL		IsDrvInstRun(LPCTSTR lpszName);//驱动实例是否运行
	CDrvItem*	GetDrvInst(LPCTSTR lpszName);
	int			GetDecStatus(LPCTSTR lpszDecName);
	void		Start(BOOL bAutoRun);	//启动
	void		StartDec(BOOL bAutoRun);//初始化驱动,创建驱动加入树中
	void		Stop();		//停止
	BOOL		SaveConnectData();
	BOOL		IsCanWork();//是否可工作
	CString		GetDDCWorkInfo(); //取主备状态
	int			AddDrv(CString szPath, CString szdrvname,CString szDecName);
	int			GetDrvInfo(CString szName, CString szPath, T_DRVINFO1& tDRVINFO);
	int			QueryTag(CString strDecName, CSortObArray* pSortArray,LPCTSTR stagname,LPCTSTR sproxytagname,LPCTSTR sdes,LPCTSTR sdrvtagname,int ntype);
	BOOL		GetDrvTagDef(CString szDrvName, int iIndex, T_DRVTAG& tDRVTAG);
	int			AddTag2Drv(CString szDecName, T_DRVTAG& tDRVTAG, DWORD dwFlag);
	BOOL		AddTag2Drv(CString szDecName, CTagItem& tTagItem, DWORD dwFlag);
	// 检查表达式
	int			CheckExp(const CString& szExp, CString szTagName, CString& szErrMsg, int& nTagCount);
	BOOL		DelDrvTag(CString szDecName, CString szTagName);
	BOOL		UpdateTag(CString szDecName, CTagItem& tagItem);
	BOOL		ImportTags(CString szDecName,CCSVFile& csvFile,int &nUpdate,int &nInsert,int &nAll,DWORD dwflag);

	BOOL		LoadPrjFile(LPCTSTR lpszFile);		//读入工程文件	
	int			GetDrvInstances(vecDrivers&  vecDatas);
	BOOL		Start(CString szModule, CString szDecName);
	BOOL		Stop(CString szDecName);

	BOOL		ConfigDrv(CString szDrvName);
	void		CreateDrvInst(LPCTSTR lpszPath, LPCTSTR szDecName);
	int			SetDBPConnectArgs(const CDBPArgs& dbpArgs);
	int			GetDBPConnectArgs(CDBPArgs& dbpArgs);
	int			GetDrvTags(DWORD dwFlag, CString szDrvName, CDrvTags& DrvTags, int &nNew, int& nUpdate);
	int			QueryTag(LPCTSTR szDecName, 
		CSortObArray& pSortArray,
		LPCTSTR szTagName,
		LPCTSTR szProxyTagName,
		LPCTSTR szDes,
		LPCTSTR szDrvTagName,
		int nType);
	BOOL		DelTag(CString szDecName, CString szTagName);
	void		ExitServer(void);
	BOOL		GetTagValue(LPCTSTR lpszDecName, LPCTSTR lpszTagName, CVarVal& varValue);
	void		GetOutFileCfig(BOOL& bUseFileOutput, T_OutFileCfg& tOutFileCfg);
	void		SetOutFileCfig(BOOL bUseFileOutput, const T_OutFileCfg& tOutFileCfg);

	void		ProcessOutFile(void);
	static  UINT threadOutFile(LPVOID pParam);
public://static
	static  DWORD WINAPI Fun_OnDataOk(	//数据回调函数
		DRHANDLE handle,	//句柄
		T_DRREC recs[],		//记录集
		int nrecs			//记录个数
		);
	static	DWORD WINAPI Fun_OnLogOk( //日志回调函数
		DRHANDLE handle,	//句柄
		const char *plog,			//日志字符串，0结束的字符串
		DWORD dwLogLevel,	//日志级别
		DWORD dwLogOptType	//日志操作类型
		);
	static	DWORD WINAPI Fun_OnSoeOk( //SOE事件回调函数,每次回调一个事件
		DRHANDLE handle,	//句柄
		LPSOE psoe			//事件
		);

	// 历史数据回调
	static DWORD WINAPI Fun_OnWriteHistoryCB(
		DRHANDLE handle,		// 句柄
		T_DRREC  recs[],		// 记录集
		int nrecs				// 记录个数
		);

	// 通用数据报文回调
	static DWORD WINAPI Fun_OnStreamMessageCB(
		DRHANDLE handle,		// 句柄
		const char* lpszTagName,// 标签名称
		DWORD	dwCommandID,	// 报文指令ID
		BYTE*	pBuffer,		// 报文数据缓存
		int		nSize			// 报文长度
		);
};



// CDriverMgrApp
// 有关此类实现的信息，请参阅 DriverMgr.cpp
//
#include "Log.h"
#include "../AuthorityMgr/AuthorityMgr.h"

using namespace KIMI;
class CDriverMgrApp : public CWinApp
{
public:
	CDriverMgrApp();
	CLog*	m_logger;
	CAuthorityMgr* m_pAuthorityMgr;
	CString GetOperator();
	//CIniConfig	m_Config;		//配置
	// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};



