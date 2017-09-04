// reglic.h: interface for the CRegLic class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGLIC_H__6EFDD500_2C33_47B6_AB5C_ACAC3C4B003A__INCLUDED_)
#define AFX_REGLIC_H__6EFDD500_2C33_47B6_AB5C_ACAC3C4B003A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PropertyManager.h"

#define LIC_STATUS_FACTOR	36353 // 授权状态因数

#define LIC_STATUS_FAIL		131		// 未注册
#define LIC_STATUS_EXPIRED	163		// 试用版过期
#define LIC_STATUS_DEMO		197		// 试用版
#define LIC_STATUS_REG		239		// 正式版

class CRegLic  
{
public:
	CRegLic();
	virtual ~CRegLic();
public:
	CString m_szProductName;
	CString m_szMachineNo;
	CLicensePropertyManager m_lpm;

	DWORD	CheckLicense(LPCTSTR lpszProductName, BOOL bShowMsgBox = TRUE);
	CString	GetLicInfo(LPCTSTR lpszProductName);

protected:
	BOOL	LoadLicense(LPCTSTR lpszProductName, BOOL bShowMsgBox);

	// 授权状态码转换为随机值
	DWORD LicStatus2Random(DWORD dwStatus);
	// 随机值转换为授权状态码
	DWORD Random2LicStatus(DWORD dwRandom);

private:
	BOOL	m_bLoad;
};

#endif // !defined(AFX_REGLIC_H__6EFDD500_2C33_47B6_AB5C_ACAC3C4B003A__INCLUDED_)
