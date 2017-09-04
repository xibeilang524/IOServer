// reglic.cpp: implementation of the CRegLic class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Operator.h"
#include "reglic.h"

#include "ComputerHardwareInfo.h"

#define _USE_THUNK_CODE
#include "AsmThunkCode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*
取应用程序所在目录,后面带了\,比如c:\aap\
*/
CString TL_GetHomeDirectory()
{
	TCHAR sFilename[_MAX_PATH];
	TCHAR sDrive[_MAX_DRIVE];
	TCHAR sDir[_MAX_DIR];
	TCHAR sFname[_MAX_FNAME];
	TCHAR sExt[_MAX_EXT];
	GetModuleFileName(AfxGetInstanceHandle(), sFilename, _MAX_PATH);
	_tsplitpath(sFilename, sDrive, sDir, sFname, sExt);	
	CString rVal(CString(sDrive) + CString(sDir));
	int nLen = rVal.GetLength();
	if (rVal.GetAt(nLen-1) != _T('\\'))
		rVal += _T("\\");
	
	return rVal;
}


CRegLic::CRegLic()
:	m_bLoad(FALSE),
	m_szProductName(_T(""))
{
}

CRegLic::~CRegLic()
{	
}

BOOL CRegLic::LoadLicense(LPCTSTR lpszProductName, BOOL bShowMsgBox)
{
	//if (m_szProductName.CompareNoCase(lpszProductName) != 0)
	//{
	//	m_bLoad = FALSE;
		m_szProductName = lpszProductName;
	//}

	//if (!m_bLoad)
	{

		CString szAppDir = ::TL_GetHomeDirectory();
		CString szFile = szAppDir + "license.dat";

		if (!m_lpm.Load(szFile))
		{
			if (bShowMsgBox)
				AfxMessageBox("打开软件授权文件license.dat无效!", MB_OK | MB_ICONERROR);

			m_bLoad = FALSE;
			return FALSE;
		}

		m_bLoad = TRUE;
	}

	return m_bLoad;
}

DWORD CRegLic::CheckLicense(LPCTSTR lpszProductName, BOOL bShowMsgBox/* = TRUE*/)
{
__THUNK_CODE_0
	if (!LoadLicense(lpszProductName, bShowMsgBox))
	{
__THUNK_CODE_1
		return LicStatus2Random(LIC_STATUS_FAIL);
	}

__THUNK_CODE_2

	// 授权文件中是否有该软件的授权
	CLicenseProduct *pProduct = m_lpm.FindProduct(lpszProductName);
	if (pProduct == NULL)
		return LicStatus2Random(LIC_STATUS_FAIL);

__THUNK_CODE_3

	// 指定的软件产品是否被授权？
	CLicenseProperty *pProperty = pProduct->FindProperty(_T("授权"));
	if ((pProperty == NULL) || (!pProperty->AsBool()))
		return LicStatus2Random(LIC_STATUS_FAIL);

__THUNK_CODE_4

	// 获取当前计算机的序列号
	CComputerHardwareInfo CHInfo;
	m_szMachineNo = CHInfo.GetMachineInfo();

__THUNK_CODE_5

	// 是否是正式版
	if (m_szMachineNo.CompareNoCase(m_lpm.GetMachineNo()) == 0)
		return LicStatus2Random(LIC_STATUS_REG);

__THUNK_CODE_6

	long ltime = (long)::time(NULL);
	
	if (ltime > m_lpm.GetEndDate())
		return LicStatus2Random(LIC_STATUS_EXPIRED);

__THUNK_CODE_7

	if (ltime < m_lpm.GetStartDate())
		return LicStatus2Random(LIC_STATUS_EXPIRED);

__THUNK_CODE_8

	// 是未过期的试用版
	return LicStatus2Random(LIC_STATUS_DEMO);
}

CString	CRegLic::GetLicInfo(LPCTSTR lpszProductName)
{
	CString szLicInfo = _T("");

	CComputerHardwareInfo CHInfo;
	m_szMachineNo = CHInfo.GetMachineInfo();
	szLicInfo.Format(_T("机器码:%s\n"), m_szMachineNo);
	/*
	szLicInfo.Append(_T("产 品  名 称："));
	szLicInfo.Append(lpszProductName);
	szLicInfo.Append(_T("\n"));
	*/

__THUNK_CODE_0
	DWORD dwret = CheckLicense(lpszProductName, FALSE);
__THUNK_CODE_1
	dwret = Random2LicStatus(dwret);
__THUNK_CODE_2

	if ((dwret == LIC_STATUS_EXPIRED) || (dwret == LIC_STATUS_DEMO))
	{
		__THUNK_CODE_3
		szLicInfo += _T("授权类型:试用版\n");
		szLicInfo += _T("被授权人:");
		szLicInfo += m_lpm.GetUserName();
		szLicInfo += _T("\n");

		szLicInfo += _T("开始时间:");
		CTime ftime(m_lpm.GetStartDate());
		szLicInfo += ftime.Format(_T("%Y-%m-%d"));
		szLicInfo += _T("\n");

		szLicInfo += _T("终止时间:");
		ftime = m_lpm.GetEndDate();
		szLicInfo += ftime.Format(_T("%Y-%m-%d"));
		szLicInfo += _T("\n");

		long ltime = (long)::time(NULL);
		if (dwret == LIC_STATUS_EXPIRED)
			szLicInfo += _T("授权状态：已过期");
		else {
			long ltime = m_lpm.GetEndDate() - (long)::time(NULL); // 剩余时间
			CString szTime = _T(""), szTemp;
			if (ltime >= 86400)
			{
				szTemp.Format(_T("%d天"), ltime / 86400);
				ltime %= 86400;
				szTime += szTemp;
			}

			if (ltime >= 3600)
			{
				szTemp.Format(_T("%02d小时"), ltime / 3600);
				ltime %= 3600;
				szTime += szTemp;
			}

			if (ltime >= 60)
			{
				szTemp.Format(_T("%02d分钟"), ltime / 60);
				ltime %= 60;
				szTime += szTemp;
			}
			szTemp.Format(_T("授权状态:还剩%s到期"), szTime);
			szLicInfo += szTemp;
		}
	}
	else if (dwret == LIC_STATUS_REG)
	{
		__THUNK_CODE_4
		szLicInfo += _T("授权类型:正式版\n");
		szLicInfo += _T("被授权人:");
		szLicInfo += m_lpm.GetUserName();
		szLicInfo += _T("\n");
		szLicInfo += _T("授权状态:无限制使用");
	}
	else {
		__THUNK_CODE_5
		szLicInfo += _T("被授权人:未授权用户");
	}

	CLicenseProduct *pProduct = m_lpm.FindProduct(lpszProductName);
	if (pProduct == NULL)
		return szLicInfo;

	szLicInfo += _T("\n\n");
	szLicInfo += pProduct->GetPropertyString();

	return szLicInfo;
}

DWORD CRegLic::LicStatus2Random(DWORD dwStatus)
{
__THUNK_CODE_0
	::srand((UINT)time(NULL));

__THUNK_CODE_1

	DWORD dwret = 75431 * rand();

__THUNK_CODE_2
	dwret = (DWORD)(dwret / (RAND_MAX + 1.0));

__THUNK_CODE_3

	dwret = LIC_STATUS_FACTOR * dwret;

__THUNK_CODE_4
	dwret += dwStatus;

__THUNK_CODE_5

	return dwret;
}

DWORD CRegLic::Random2LicStatus(DWORD dwRandom)
{
	__THUNK_CODE_6
	DWORD dwret = dwRandom % LIC_STATUS_FACTOR;

	__THUNK_CODE_7
	return dwret;
}
