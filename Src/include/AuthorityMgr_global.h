#pragma once

#define OPTDLL_OK			0
#define OPTDLL_ERRHANDLE	1 
#define OPTDLL_ERRPASS		2	//密码错误
#define OPTDLL_ERRNAME		3	//无此账号
#define OPTDLL_CANCEL		4	//取消
#define OPTDLL_NOPOWER		5	//无此权限
#define OPTDLL_UNREG		6   //未注册 //wzq 20090731 add
#define OPTDLL_MOMERY		7	//内存错误

#define LIC_STATUS_FACTOR	36353 // 授权状态因数

#define LIC_STATUS_FAIL		131		// 未注册
#define LIC_STATUS_EXPIRED	163		// 试用版过期
#define LIC_STATUS_DEMO		197		// 试用版
#define LIC_STATUS_REG		239		// 正式版

/// 授权属性数据类型
#define LIC_DT_BOOL			0		// BOOL类型
#define LIC_DT_NUMBER		1		// 数字类型，可为int32或int64
#define LIC_DT_FLOAT		2		// 实数类型，可为float或double
#define LIC_DT_STRING		3		// 字符串类型

typedef DWORD	OPTHANDLE;	

typedef OPTHANDLE (WINAPI *iOPT_Create)();
typedef  DWORD (WINAPI *iOPT_Destroy)(OPTHANDLE dwHandle); //销毁
typedef  DWORD (WINAPI *iOPT_MkDefault)(OPTHANDLE dwHandle); //创建默认管理员
typedef  DWORD (WINAPI *iOPT_SetOptFile)(OPTHANDLE dwHandle,LPCTSTR lpszFile); //设置操作员文件
typedef  DWORD (WINAPI *iOPT_LoadOptFile)(OPTHANDLE dwHandle,LPCTSTR lpszFile); //读入文件
typedef  DWORD (WINAPI *iOPT_Login)(OPTHANDLE dwHandle,char sName[64],char sGroup[64],char sPass[64],DWORD *pdwPower); //登录
typedef  DWORD (WINAPI *iOPT_ModifyPass)(OPTHANDLE dwHandle,LPCTSTR lpszName); //修改密码
typedef  DWORD (WINAPI *iOPT_OptMan)(OPTHANDLE dwHandle,LPCTSTR lpszName,LPCTSTR lpszPass); //管理账号
typedef  DWORD (WINAPI *iOPT_LoginNoWin)(DWORD dwHandle,const char* suser,const char* spass,DWORD *pdwPower);

typedef  DWORD (WINAPI *iOPT_CheckLicense)(DWORD dwHandle, const char *lpszProductName);
typedef  DWORD (WINAPI *iOPT_GetLicenseInfo)(DWORD dwHandle, const char *szProductName, char *szLicInfo, int *nSize);
typedef  DWORD (WINAPI *iOPT_GetMachineInfo)(char *szMacCode);
typedef	 DWORD (WINAPI *iOPT_ShowLicDlg)(const char *lpszProductName);
typedef	 DWORD (WINAPI *iOPT_GetLicProperty)(DWORD dwHandle, const char *lpszProductName, const char *lpszPropertyName, int nDataType, void *pData, int *pnSize);
