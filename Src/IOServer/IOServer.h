
// IOServer.h : IOServer 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号
#include "DriverMgr.h"
#include "IniConfig.h"
#include "CLicenseAPI.h"
#include "../AuthorityMgr/AuthorityMgr.h"


// CIOServerApp:
// 有关此类的实现，请参阅 IOServer.cpp
//
//class CLicenseMgr;
class CDriverMgr;
class CIOServerApp : public CWinAppEx
{
public:
	CIOServerApp();

	void	Logon(void);
	void	LogOff(void);
	void	UserIDMgr(void);
	void	UserIDModify(void);
	void	SetDBConnectInfo(void);
	BOOL	IsLogin(void);
	DWORD	GetUserPower(void);
	BOOL	CheckPower(DWORD dwPower);
	void	AddDrv(void);
	BOOL	GetDrvInst(CString szDrvName);
	BOOL	GetDrvInfo(CString szName, CString szPath, T_DRVINFO1& tDRVINFO);
	int		QueryTag(LPCTSTR strDecName, 
		CSortObArray& pSortArray,
		LPCTSTR szTagName,
		LPCTSTR szProxyTagName,
		LPCTSTR szDes,
		LPCTSTR szDrvTagName,
		int nType);
	int		GetDecStatus(CString szDecName);
	BOOL	GetDrvTagDef(CString szDrvName, int iIndex, T_DRVTAG& tDRVTAG);
	int		AddTag2Drv(CString szDecName, T_DRVTAG& tDRVTAG, DWORD dwFlag);
	BOOL	AddTag2Drv(CString szDecName, CTagItem& tTagItem, DWORD dwFlag);
	int		Save(void);
	int		CheckExp(const CString& szExp, CString szTagName, CString& szErrMsg, int& nTagCount);
	int		DelDrvTag(CString szDecName, CString szTagName);
	BOOL	UpdateTag(CString szDecName, CTagItem& tagItem);
	BOOL	ImportTags(CString szDecName,CCSVFile& csvFile,int &nUpdate,int &nInsert,int &nAll,DWORD dwflag);
	int		InitDriveMgr(void);
	int		GetDrvInstances(vecDrivers& vecDatas);
	BOOL	StartDrv(CString szModule, CString szDrvName);
	BOOL	StopDrv(CString szDrvName);
	BOOL	ConfigDrv(CString szDrvName);
	BOOL	DeleteDrv(CString szDevName);
	void	SetAdditionTitle(LPCTSTR lpszTitle);
	BOOL	OpenDecVarView(CString szDecName);
	void	CloseDecVarView(CString szDecName);
	int		GetDrvTags(DWORD dwFlag, CString szDrvName, CDrvTags& DrvTags, int &nNew, int& nUpdate);
	BOOL	DelTag(CString szDecName, CString szTagName);
	BOOL	IsSaveLog(void);
	BOOL	IsCanExit(void);
	void	ExitServer(void);
	BOOL	GetTagValue(LPCTSTR lpszDecName, LPCTSTR lpszTagName, CVarVal& varValue);
	BOOL	GetTagDef(LPCTSTR lpszDecName, int i, T_DRVTAG& tDRVTAG);
	CLog*   GetLog(void);
	CString GetOperator();
	void	GetOutFileCfig(BOOL& bUseFileOutput, T_OutFileCfg& tOutFileCfg);
	void	SetOutFileCfig(BOOL bUseFileOutput, const T_OutFileCfg& tOutFileCfg);
protected:
	HANDLE		m_hMutex;//单实例
	CMultiDocTemplate* m_pDecVarTemplate;
	CLicenseMgr m_LicenseMgr;	//授权管理
// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSysLogon();
	afx_msg void OnSysLogoff();
	afx_msg void OnPwdModify();
	afx_msg void OnIdmgr();
	afx_msg void OnServerPara();
	afx_msg void OnServerLocal();
	afx_msg void OnAppAbout();


public:
	CString		m_szAppDir;
	BOOL		m_bHiColorIcons;
	// 实现
	CIniConfig	m_Config;		//配置
	UINT		m_nAppLook;
	BOOL		m_bAuthority;

protected:
	int			m_nOptType;		//操作员类型-1;没登录，0系统员，1值班员
	CLog*		m_pLog;
	CAuthorityMgr *m_pAuthorityMgr;		//权限管理
	CDriverMgr *m_pDriverMgr;
};

extern CIOServerApp theApp;
