#pragma once

#include "AuthorityMgr_global.h"


#ifdef AUTHORITYMGR_EXPORTS
#define AUTHORITYMGR_API __declspec(dllexport)
#else
#define AUTHORITYMGR_API __declspec(dllimport)
#endif

// 此类是从 AuthorityMgr.dll 导出的
class AUTHORITYMGR_API CAuthorityMgr 
{
private:
	CAuthorityMgr(void);
	~CAuthorityMgr();
protected:
	static CAuthorityMgr*	instance;
	HINSTANCE				m_hInstance;	//DLL
	DWORD					m_handle;

	iOPT_Create			OPT_Create;
	iOPT_Destroy		OPT_Destroy;
	iOPT_MkDefault		OPT_MkDefault;
	iOPT_SetOptFile		OPT_SetOptFile;
	iOPT_LoadOptFile	OPT_LoadOptFile;
	iOPT_Login			OPT_Login;
	iOPT_ModifyPass		OPT_ModifyPass;
	iOPT_OptMan			OPT_OptMan;
	iOPT_LoginNoWin		OPT_LoginNoWin;
	iOPT_CheckLicense	OPT_CheckLicense;
	iOPT_GetLicenseInfo	OPT_GetLicenseInfo;
	iOPT_GetMachineInfo	OPT_GetMachineInfo;
	iOPT_ShowLicDlg		OPT_ShowLicDlg;
	iOPT_GetLicProperty	OPT_GetLicProperty;
public:
	static CAuthorityMgr* GetInstance();
	BOOL	LoadDrv(LPCTSTR lpszDllFile,CString &szErr);//读入驱动
	DWORD MakeDefault();
	DWORD SetOptFile(LPCTSTR lpszFile);
	DWORD LoadOptFile(LPCTSTR lpszFile);
	DWORD Login(); //登录;
	DWORD LoginNoWin(const char* suser,const char* spass,DWORD *pdwPower ); //登录
	DWORD ModifyPass(LPCTSTR lpszName); //修改密码
	DWORD OptMan(LPCTSTR lpszName,LPCTSTR lpszPass); //管理账号
	void	Logout();
	CString GetLoginMsg();
	DWORD CheckLicense();
	DWORD GetLicenseInfo(CString &szLicInfo);
	CString GetMachineInfo();
	DWORD ShowLicDlg();
	BOOL PropertyAsBool(LPCTSTR lpszPropertyName, BOOL bDefault = FALSE);
	long PropertyAsLong(LPCTSTR lpszPropertyName, long lDefault = 0);
	__int64 PropertyAsInt64(LPCTSTR lpszPropertyName, __int64 nDefault = FALSE);
	float PropertyAsFloat(LPCTSTR lpszPropertyName, float fDefault = 0.0);
	double PropertyAsDouble(LPCTSTR lpszPropertyName, double dblDefault = 0.0);
	CString PropertyAsString(LPCTSTR lpszPropertyName, LPCTSTR lpszDefault = NULL);
public:
	CString m_szOpt;
	CString m_szGroup;
	CString m_szPass;
	DWORD	m_dwPower;
	CString m_szProductName;

protected:
	DWORD Random2LicStatus(DWORD dwRandom);
	DWORD GetLicProperty(const char *lpszPropertyName, int nDataType, void *pData, int *pnSize);
};
