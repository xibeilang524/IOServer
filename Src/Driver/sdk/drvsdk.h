//drsdk.h 新驱动开发包
#pragma once
#include "drvdef.h"

// 日志信息级别
#define LOG_LEVEL_ERROR		0x00	// 错误级别
#define LOG_LEVEL_WARNING	0x01	// 警告级别
#define LOG_LEVEL_INFO		0x02	// 提示级别
#define LOG_LEVEL_DEBUG		0x03	// Debug级别

// 日志操作类型
#define LOG_OPT_DISPLAY		0x01	// 在窗口中显示日志
#define LOG_OPT_STORAGE		0x02	// 写日志到日志文件
#define LOG_OPT_ALL			(LOG_OPT_DISPLAY | LOG_OPT_STORAGE)

//驱动对象基类,凡是虚函数，继承的具体驱动对象都应重载
class CDrvObj
{
public:
	CDrvObj();
	virtual ~CDrvObj();

	DRHANDLE m_handle; //句柄
	void SetBlockingMode(BOOL bMode) {
		m_bBlockingMode = bMode;
	}
protected:
	DRCB_OnDataOk	m_cbOnDataOk;	//数据回调函数
	DRCB_OnLogOk	m_cbOnLogOk;	//日志回调函数
	DRCB_OnSoeOk	m_cbOnSoeOk;	//事件回调函数
	BOOL	m_bRun;					//运行状态
	BOOL	m_bKillThread;			//终止线程标志

	DRCB_OnWriteHistoryCB m_cbWriteHistory;
	DRCB_OnStreamMessageCB m_cbStreamMessage;

	BOOL	m_bBlockingMode;

public:
	DRVRET	Create(
		void *pConfigData,
		int nConfigSize,
		DRCB_OnDataOk	cbondataok,	//数据回调函数
		DRCB_OnLogOk	cbonlogok,  //日志回调函数
		DRCB_OnSoeOk	cbOnSoeOk	//事件回调函数
		);
	int		GetConfigSize();		//取配置数据长度
	DRVRET	GetConfigData(void *pbuf,int nbufsize,int *pnactsize); //取配置数据
	DRVRET  Start();	//启动
	DRVRET  Stop();		//停止

	DRVRET  Config(int *pnModify);	//配置
	BOOL	IsRun(){return m_bRun;};
	BOOL	IsKillThread(){return m_bKillThread;};

	void SetWriteHistoryCB(DRCB_OnWriteHistoryCB pWriteHistoryCB) {
		m_cbWriteHistory = pWriteHistoryCB;
	}
	void SetStreamMessageCB(DRCB_OnStreamMessageCB pStreamMessageCB) {
		m_cbStreamMessage = pStreamMessageCB;
	}

public:
	void AddDebug(LPCTSTR lpszLogMessage);
	void AddDebug(DWORD dwLogOptType, LPCTSTR lpszLogMessage);
	void AddDebugFmt(LPCTSTR lpszLogFormat, ...);
	void AddDebugFmt(DWORD dwLogOptType, LPCTSTR lpszLogFormat, ...);

	void AddInfo(LPCTSTR lpszLogMessage);
	void AddInfo(DWORD dwLogOptType, LPCTSTR lpszLogMessage);
	void AddInfoFmt(LPCTSTR lpszLogFormat, ...);
	void AddInfoFmt(DWORD dwLogOptType, LPCTSTR lpszLogFormat, ...);

	void AddWarning(LPCTSTR lpszLogMessage);
	void AddWarning(DWORD dwLogOptType, LPCTSTR lpszLogMessage);
	void AddWarningFmt(LPCTSTR lpszLogFormat, ...);
	void AddWarningFmt(DWORD dwLogOptType, LPCTSTR lpszLogFormat, ...);

	void AddError(LPCTSTR lpszLogMessage);
	void AddError(DWORD dwLogOptType, LPCTSTR lpszLogMessage);
	void AddErrorFmt(LPCTSTR lpszLogFormat, ...);
	void AddErrorFmt(DWORD dwLogOptType, LPCTSTR lpszLogFormat, ...);

protected:
	static	UINT	RunThread(void *pParam);// 运行线程

//以下需要重载
public: //虚函数
	virtual DRVRET  GetErrMsg(CString& szMsg,DRVRET dwcode);			//获取错误信息
	virtual DRVRET  WriteVar(LPDRREC lpval){return DRERR_OK;};			//写变量
	virtual DRVRET  GetTagDef(int nindex,LPDRVTAG  lptag){return DRERR_NOMORETAG;};
	virtual short	ToDBPQuality(short nQuality) { return nQuality; } // 将由驱动采集的质量转换为目标服务器标准质量
					
protected: //纯虚函数
	virtual BOOL	OnConfig() = 0;					//配置和配置界面,返回TRUE表示修改了配置
	virtual void	Serialize(CArchive& ar) = 0;	//串行化保存，读取配置
	virtual DRVRET	OnStart() = 0;					//准备运行
	virtual void	OnStop() = 0;					//停止运行后的清理工作
	virtual void	OnDisconnect() {};				//断开连接，如果不是阻塞模式，则可以不实现该函数
	virtual UINT	DoRun() = 0;					//正真的运行过程，被RunThread中调用
};

class CDrvFactory //驱动工场,这是一个静态类,每个驱动都要修改,里面的两个实现函数
{
public:
	static CDrvObj*	NewDrv(); //创建驱动
	static DRVRET GetDriverInfo(LPDRVINFO lpDrvInfo);	//获取驱动信息，指驱动程序的信息，非具体实例的信息
};
