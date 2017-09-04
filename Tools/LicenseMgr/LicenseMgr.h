
// LicenseMgr.h : LicenseMgr 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif


#ifdef _LICENSE_VIEW_APP
#	include "../LicenseView/resource.h"
#else
#	include "resource.h"       // 主符号
#endif


// CLicenseMgrApp:
// 有关此类的实现，请参阅 LicenseMgr.cpp
//

class CLicenseMgrApp : public CWinAppEx
{
public:
	CLicenseMgrApp();


// 重写
public:
	virtual BOOL InitInstance();

// 实现
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CLicenseMgrApp theApp;
extern CLicenseFile theLicenseFile;
