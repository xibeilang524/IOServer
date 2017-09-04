#pragma once

class CIniConfig : public CReadIni
{
public:
	CIniConfig(void);
	~CIniConfig(void);
public:
	CString m_szProjectName;
	BOOL	m_bAutoRun;		//启动时自动运行驱动实例
	BOOL	m_bSaveLog;		//保存日志
	BOOL	m_bExitPass;	//退出密码验证
	BOOL	m_bCashe;		//是否使用cashe
	BOOL	m_bSkipSameTime;//放弃相同时标快照更新
	int		m_nTimeZero;	//显示时区-12,12
	WORD	m_wheartport;	//心跳端口
	CString m_szHeartIP;	//心跳IP,空表示本机是服务器
	int		m_nLogLevel;	//日志级别，0-错误、1-警告、2-提示、3-调试
	CString m_szTitle;		//DDClient附加显示标题，以区分DDClient的功能
	BOOL	m_bWaitTcpAck;	//TCP是否有响应包，部分网闸是不允许有响应包的
	int		m_nUdpSendDelay;//UDP发包延时
	int		m_nupthreads;	//上传线程数
	//计算获得
	long	m_lAdjustSec;	//显示调整间秒,即m_nTimeZero * 3600 + TIME_ZONE_INFORMATION.Bias * 60
protected:
	virtual void OnDoKeyVal(LPCTSTR lpszBlkName,LPCTSTR lpszKeyName,LPCTSTR lpszKeyVal);
};

