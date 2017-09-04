// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            //  从 Windows 头文件中排除极少使用的信息
#endif

#include <afx.h>
#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <iostream>
// Windows 头文件:
#include <windows.h>
//
//typedef OPTHANDLE (WINAPI *iOPT_Create)();
//typedef  DWORD (WINAPI *iOPT_Destroy)(OPTHANDLE dwHandle); //销毁
//typedef  DWORD (WINAPI *iOPT_MkDefault)(OPTHANDLE dwHandle); //创建默认管理员
//typedef  DWORD (WINAPI *iOPT_SetOptFile)(OPTHANDLE dwHandle,LPCTSTR lpszFile); //设置操作员文件
//typedef  DWORD (WINAPI *iOPT_LoadOptFile)(OPTHANDLE dwHandle,LPCTSTR lpszFile); //读入文件
//typedef  DWORD (WINAPI *iOPT_Login)(OPTHANDLE dwHandle,char sName[64],char sGroup[64],char sPass[64],DWORD *pdwPower); //登录
//typedef  DWORD (WINAPI *iOPT_ModifyPass)(OPTHANDLE dwHandle,LPCTSTR lpszName); //修改密码
//typedef  DWORD (WINAPI *iOPT_OptMan)(OPTHANDLE dwHandle,LPCTSTR lpszName,LPCTSTR lpszPass); //管理账号
//typedef  DWORD (WINAPI *iOPT_LoginNoWin)(DWORD dwHandle,const char* suser,const char* spass,DWORD *pdwPower);
//
//typedef  DWORD (WINAPI *iOPT_CheckLicense)(DWORD dwHandle, const char *lpszProductName);
//typedef  DWORD (WINAPI *iOPT_GetLicenseInfo)(DWORD dwHandle, const char *szProductName, char *szLicInfo, int *nSize);
//typedef  DWORD (WINAPI *iOPT_GetMachineInfo)(char *szMacCode);
//typedef	 DWORD (WINAPI *iOPT_ShowLicDlg)(const char *lpszProductName);
//typedef	 DWORD (WINAPI *iOPT_GetLicProperty)(DWORD dwHandle, const char *lpszProductName, const char *lpszPropertyName, int nDataType, void *pData, int *pnSize);


// TODO: 在此处引用程序需要的其他头文件
