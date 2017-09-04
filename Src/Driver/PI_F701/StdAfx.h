// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__23C54F40_72AB_47AB_A6FC_31AA40152DAC__INCLUDED_)
#define AFX_STDAFX_H__23C54F40_72AB_47AB_A6FC_31AA40152DAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if _MSC_VER > 1200
#define   WINVER   0x0502
#define _CRT_SECURE_NO_WARNINGS	1	// 关闭VC6以上版本的CRT安全警告
#endif

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

// 编译PI SDK 1.3.5时需要
#include <winsock2.h>

#include <afxsock.h>		// MFC socket extensions

#include "TPRICommonAPI.h"
using namespace TPRI::COMMON;
//#define _crtdbg_map_alloc
//#include "stdlib.h"
//#include "crtdbg.h"

//
//typedef struct _weight_data
//{
//	int		iID;
//	int		ltime;
//	int		iQuality;
//	float	fDefineValue;
//	float	fTotalValue;
//	float	fRealValue;
//	int		iTotalNum;
//	char	chMechineName[50];
//	char	chChenName[50];
//}T_WeightData, *LPT_WeightData;

//
//typedef struct _weight_data
//{
//	int		iID;
//	int		ltime;
//	int		iQuality;
//	float	fDefineValue;
//	float	fTotalValue;
//	float	fRealValue;
//	int		iTotalNum;
//	char	chMechineName[50];
//	char	chChenName[50];
//	BYTE	btFlag;
//}T_WeightData, *LPT_WeightData;


typedef struct _weight_data
{
	int		iID;
	int		ltime;
	int		iQuality;
	float	fDefineValue;
	float	fTotalValue;
	float	fRealValue;
	int		iTotalNum;
	int		iUpper;
	char	chMechineName[50];
	char	chChenName[50];
	BYTE	btFlag;
}T_WeightData, *LPT_WeightData;


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__23C54F40_72AB_47AB_A6FC_31AA40152DAC__INCLUDED_)
