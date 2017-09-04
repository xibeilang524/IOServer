// AuthorityMgr.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "AuthorityMgr.h"
#include "AsmThunkCode.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// 初始化 MFC 并在失败时显示错误
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: 更改错误代码以符合您的需要
			_tprintf(_T("错误: MFC 初始化失败\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: 在此处为应用程序的行为编写代码。
		}
	}
	else
	{
		// TODO: 更改错误代码以符合您的需要
		_tprintf(_T("错误: GetModuleHandle 失败\n"));
		nRetCode = 1;
	}

	return nRetCode;
}

CAuthorityMgr* CAuthorityMgr::instance = NULL;

CAuthorityMgr::CAuthorityMgr(void)
{
	m_szOpt = "";
	m_szGroup = "";
	m_szPass = "";
	m_dwPower = 0;

	m_hInstance = NULL;
	m_handle = NULL;

	OPT_Create = NULL;
	OPT_Destroy = NULL;
	OPT_MkDefault = NULL;
	OPT_SetOptFile = NULL;
	OPT_LoadOptFile = NULL;
	OPT_Login = NULL;
	OPT_ModifyPass = NULL;
	OPT_OptMan = NULL;
	OPT_LoginNoWin = NULL;

	OPT_CheckLicense = NULL;
	OPT_GetLicenseInfo = NULL;
	OPT_GetMachineInfo = NULL;
	OPT_ShowLicDlg = NULL;
	OPT_GetLicProperty = NULL;
	instance = NULL;
}

CAuthorityMgr::~CAuthorityMgr()
{
	if(	m_handle != 0)
		OPT_Destroy(m_handle);
	if(m_hInstance)
		::FreeLibrary(m_hInstance);
	if(instance)
	{
		delete instance;
		instance = NULL;
	}
};

CAuthorityMgr* CAuthorityMgr::GetInstance()
{
	if(!instance)
		instance = new CAuthorityMgr;
	return instance;
}
BOOL CAuthorityMgr::LoadDrv(LPCTSTR lpszDllFile,CString &szErr)//读入驱动
{
	if(m_hInstance != NULL)
		return TRUE;
	m_hInstance = LoadLibrary(lpszDllFile); 
	if(m_hInstance == NULL)
	{
		szErr.Format("Err:can't find driver<%s>",lpszDllFile);
		TRACE("%s\n",szErr);
		return FALSE;//没找到驱动程序
	}

	OPT_Create			= (iOPT_Create)::GetProcAddress(m_hInstance,"OPT_Create");
	OPT_Destroy			= (iOPT_Destroy)::GetProcAddress(m_hInstance,"OPT_Destroy");
	OPT_MkDefault		= (iOPT_MkDefault)::GetProcAddress(m_hInstance,"OPT_MkDefault");
	OPT_SetOptFile		= (iOPT_SetOptFile)::GetProcAddress(m_hInstance,"OPT_SetOptFile");
	OPT_LoadOptFile		= (iOPT_LoadOptFile)::GetProcAddress(m_hInstance,"OPT_LoadOptFile");
	OPT_Login			= (iOPT_Login)::GetProcAddress(m_hInstance,"OPT_Login");
	OPT_ModifyPass		= (iOPT_ModifyPass)::GetProcAddress(m_hInstance,"OPT_ModifyPass");
	OPT_OptMan			= (iOPT_OptMan)::GetProcAddress(m_hInstance,"OPT_OptMan");
	OPT_LoginNoWin		= (iOPT_LoginNoWin)::GetProcAddress(m_hInstance,"OPT_LoginNoWin");

	OPT_CheckLicense	= (iOPT_CheckLicense)::GetProcAddress(m_hInstance,"OPT_CheckLicense");
	OPT_GetLicenseInfo	= (iOPT_GetLicenseInfo)::GetProcAddress(m_hInstance,"OPT_GetLicenseInfo");
	OPT_GetMachineInfo	= (iOPT_GetMachineInfo)::GetProcAddress(m_hInstance,"OPT_GetMachineInfo");
	OPT_ShowLicDlg		= (iOPT_ShowLicDlg)::GetProcAddress(m_hInstance,"OPT_ShowLicDlg");
	OPT_GetLicProperty	= (iOPT_GetLicProperty)::GetProcAddress(m_hInstance,"OPT_GetLicProperty");

	//应用相关的
	if(OPT_Create)
		m_handle = OPT_Create();
	return TRUE;
}

DWORD CAuthorityMgr::MakeDefault()
{
	if(!m_hInstance)
		return OPTDLL_ERRHANDLE;

	return OPT_MkDefault(m_handle);
}

DWORD CAuthorityMgr::SetOptFile(LPCTSTR lpszFile)
{
	if(!m_hInstance)
		return OPTDLL_ERRHANDLE;
	return OPT_SetOptFile(m_handle,lpszFile);
}

DWORD CAuthorityMgr::LoadOptFile(LPCTSTR lpszFile)
{
	if(!m_hInstance)
		return OPTDLL_ERRHANDLE;
	return OPT_LoadOptFile(m_handle,lpszFile);
}

DWORD CAuthorityMgr::Login() //登录
{
	if(!m_hInstance)
		return OPTDLL_ERRHANDLE;
	DWORD dwPower = 0;
	char sGroup[64];
	char sName[64];
	char sPass[64];
	sGroup[0] = 0;
	DWORD dwret = OPT_Login(m_handle,sName,sGroup,sPass,&dwPower);

	if(dwret == 0)
	{
		m_szGroup = sGroup;
		m_szOpt = sName;
		m_szPass = sPass;
		m_dwPower = dwPower;
	}
	return dwret;
}

DWORD CAuthorityMgr::LoginNoWin(const char* suser,const char* spass,DWORD *pdwPower ) //登录
{
	if(!m_hInstance)
		return OPTDLL_ERRHANDLE;
	return OPT_LoginNoWin(m_handle,suser,spass,pdwPower);
}

DWORD CAuthorityMgr::ModifyPass(LPCTSTR lpszName) //修改密码
{
	if(!m_hInstance)
		return OPTDLL_ERRHANDLE;
	return OPT_ModifyPass(m_handle,lpszName);
}

DWORD CAuthorityMgr::OptMan(LPCTSTR lpszName,LPCTSTR lpszPass) //管理账号
{
	if(!m_hInstance)
		return OPTDLL_ERRHANDLE;
	if(m_szOpt.IsEmpty())
	{
		AfxMessageBox("请登录!");
		return 0;
	}
	return OPT_OptMan(m_handle,lpszName,lpszPass);
}
void	CAuthorityMgr::Logout()
{
	m_szOpt = "";
	m_szGroup = "";
	m_szPass = "";
	m_dwPower = 0;
}
CString CAuthorityMgr::GetLoginMsg()
{
	CString szt = "";
	if(m_szOpt.IsEmpty())
		szt = "登录:无";
	else
	{
		szt = "登录:";
		szt += m_szOpt;
		szt += "(";
		szt += m_szGroup;
		szt += ")";
	}
	return szt;
}

DWORD CAuthorityMgr::CheckLicense() 
{
	__THUNK_CODE_0

		if(!m_hInstance)
			return OPTDLL_ERRHANDLE;

	__THUNK_CODE_1

		DWORD dwret = OPT_CheckLicense(m_handle, m_szProductName);
	__THUNK_CODE_2

		DWORD dwret2 = Random2LicStatus(dwret);

	__THUNK_CODE_3

		return dwret2;
}

DWORD CAuthorityMgr::GetLicenseInfo(CString &szLicInfo) 
{	
	int nlen = 1024 * 4;
	char *pbuf = new char[nlen];

	szLicInfo.Empty();

	DWORD dwret = OPT_GetLicenseInfo(m_handle, m_szProductName, pbuf, &nlen);
	if (dwret == OPTDLL_OK)
		szLicInfo = pbuf;

	delete[] pbuf;
	return dwret;
}

CString CAuthorityMgr::GetMachineInfo() 
{		
	if(!m_hInstance)
		return _T("");

	char szMacCode[80] = { 0 };
	if (OPT_GetMachineInfo(szMacCode) == OPTDLL_OK)
		return CString(szMacCode);
	else
		return _T("");
}

DWORD CAuthorityMgr::ShowLicDlg() 
{		
	if(!m_hInstance)
		return OPTDLL_ERRHANDLE;

	return OPT_ShowLicDlg(m_szProductName);
}
BOOL CAuthorityMgr::PropertyAsBool(LPCTSTR lpszPropertyName, BOOL bDefault)
{
	__THUNK_CODE_0
		BOOL bval = bDefault;
	__THUNK_CODE_1
		int nsize = sizeof(BOOL);
	__THUNK_CODE_2

		if (GetLicProperty(lpszPropertyName, LIC_DT_BOOL, &bval, &nsize) == OPTDLL_OK)
		{
			__THUNK_CODE_3
				return bval;
		}
		else {
			__THUNK_CODE_4
				return bDefault;
		}
}
long CAuthorityMgr::PropertyAsLong(LPCTSTR lpszPropertyName, long lDefault)
{
	__THUNK_CODE_5
		long lval = lDefault;
	__THUNK_CODE_6
		int nsize = sizeof(long);
	__THUNK_CODE_7

		if (GetLicProperty(lpszPropertyName, LIC_DT_NUMBER, &lval, &nsize) == OPTDLL_OK)
		{
			__THUNK_CODE_8
				return lval;
		}
		else {
			__THUNK_CODE_9
				return lDefault;
		}
}
__int64 CAuthorityMgr::PropertyAsInt64(LPCTSTR lpszPropertyName, __int64 nDefault)
{
	__THUNK_CODE_0
		__int64 nval = nDefault;
	__THUNK_CODE_1
		int nsize = sizeof(__int64);
	__THUNK_CODE_2

		if (GetLicProperty(lpszPropertyName, LIC_DT_NUMBER, &nval, &nsize) == OPTDLL_OK)
		{
			__THUNK_CODE_3
				return nval;
		}
		else {
			__THUNK_CODE_4
				return nDefault;
		}
}
float CAuthorityMgr::PropertyAsFloat(LPCTSTR lpszPropertyName, float fDefault)
{
	__THUNK_CODE_5
		float fval = fDefault;
	__THUNK_CODE_6
		int nsize = sizeof(float);
	__THUNK_CODE_7

		if (GetLicProperty(lpszPropertyName, LIC_DT_FLOAT, &fval, &nsize) == OPTDLL_OK)
		{
			__THUNK_CODE_8
				return fval;
		}
		else {
			__THUNK_CODE_9
				return fDefault;
		}
}
double CAuthorityMgr::PropertyAsDouble(LPCTSTR lpszPropertyName, double dblDefault)
{
	__THUNK_CODE_0
		double dblval = dblDefault;
	__THUNK_CODE_1

		int nsize = sizeof(float);

	__THUNK_CODE_2
		if (GetLicProperty(lpszPropertyName, LIC_DT_FLOAT, &dblval, &nsize) == OPTDLL_OK)
		{
			__THUNK_CODE_3
			return dblval;
		}
		else 
		{
			__THUNK_CODE_4
			return dblDefault;
		}
}
CString CAuthorityMgr::PropertyAsString(LPCTSTR lpszPropertyName, LPCTSTR lpszDefault)
{
	__THUNK_CODE_0
		CString szval = lpszDefault ? lpszDefault : _T("");
	__THUNK_CODE_1
		int nsize = 0;
	__THUNK_CODE_2
		// 先查询需要的内存长度
		if (GetLicProperty(lpszPropertyName, LIC_DT_STRING, NULL, &nsize) == OPTDLL_MOMERY)
		{
			__THUNK_CODE_3
				// 字符串太长
				if (nsize > 1024)
					return szval;

			__THUNK_CODE_4

				LPCTSTR lpszStr = szval.GetBuffer(nsize);

			__THUNK_CODE_5

				if (GetLicProperty(lpszPropertyName, LIC_DT_STRING, (void*)lpszStr, &nsize) != OPTDLL_OK)
				{
					__THUNK_CODE_6
						szval = lpszDefault ? lpszDefault : _T("");
				}

				__THUNK_CODE_7

					szval.ReleaseBuffer();

				__THUNK_CODE_8
		}

		__THUNK_CODE_9

			return szval;
}
DWORD CAuthorityMgr::Random2LicStatus(DWORD dwRandom)
{
	__THUNK_CODE_9

		DWORD dwret = dwRandom % LIC_STATUS_FACTOR;

	__THUNK_CODE_8

		return dwret;
}

DWORD CAuthorityMgr::GetLicProperty(const char *lpszPropertyName, int nDataType, void *pData, int *pnSize)
{
	__THUNK_CODE_5
		if(!m_hInstance)
		{
			__THUNK_CODE_6
				return OPTDLL_ERRHANDLE;
		}

		__THUNK_CODE_7
			DWORD dwret = OPT_GetLicProperty(m_handle, m_szProductName, lpszPropertyName, nDataType, pData, pnSize);
		__THUNK_CODE_8

			return dwret;
}