// CLicense.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "CLicenseAPI.h"

/// 默认授权文件名
#define DEFAULT_LICENSE_FILE	"C:\\Program Files\\License\\License.lic"

/// 句柄转换为CLicenseFile指针
#define HANDLE2Ptr(x)	reinterpret_cast<CLicenseSoftware*>(x)

static void StringLimitLength(std::string& s, int nLimitLength)
{
	int len = s.length();
	if (len <= nLimitLength)
		return;

	char ch;
	while(len > nLimitLength)
	{
		ch = s[len - 1];

		if (ch >= 0 && ch <= 127)
			len--;
		else {
			len--;
			if ((len - 2 >= 0) && (s[len - 2] < 0 || s[len - 2] > 127))
				len--;
		}
	}

	s.resize(len);
}

static KIMILIC_ERRCODE StringCopy(char* pDest, unsigned long* pdwSize, const char* pSource)
{
	size_t len = (size_t)*pdwSize;
	std::string s = pSource;
	KIMILIC_ERRCODE err = (len > s.length()) ? KIMI_LIC_ERR_OK : KIMI_LIC_ERR_BUFFER_TOO_SMALL;

	StringLimitLength(s, len - 1);
	strncpy(pDest, s.c_str(), len);
	pDest[len - 1] = '\0';
	*pdwSize = (DWORD)strlen(pDest);

	return err;
}

KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_load(const char *lpszLicFileName, const char *lpszSoftwareName, const char *lpszAuthPropertyName, KIMILIC_HANDLE* pLicHandle)
{
	KIMILIC_ERRCODE err = KIMI_LIC_ERR_FAIL;
	*pLicHandle = NULL;
	CLicenseFile *pLicenseFile = new CLicenseFile();

	if (pLicenseFile->Load(lpszLicFileName ? lpszLicFileName : DEFAULT_LICENSE_FILE))
	{
		CLicenseSoftware *pSoftware = pLicenseFile->GetLicenseSoftware(lpszSoftwareName);
		if (pSoftware)
		{
			if (lpszAuthPropertyName && *lpszAuthPropertyName)
			{
				CLicenseProperty* pPty = pSoftware->GetProperty(lpszAuthPropertyName);

				bool bAuth = false;
				if (pPty)
					bAuth = pPty->AsBool(false);

				if (!bAuth)
				{
					err = KIMI_LIC_ERR_NOT_AUTHORIZED;
					goto _fail_exit;
				}
			}

			if (!pLicenseFile->CheckMachineCode())
			{
				err = KIMI_LIC_ERR_MCODE_NOT_MATCH;
			}
			else if (pLicenseFile->CheckExpiredTime() <= 0)
			{
				err = KIMI_LIC_ERR_AUTH_EXPIRED;
			}
			else
				err = KIMI_LIC_ERR_OK;

			pSoftware->SetUserData( reinterpret_cast<DWORD_PTR>(pLicenseFile) );
			*pLicHandle = reinterpret_cast<KIMILIC_HANDLE>(pSoftware);

			return err;
		}
		else
			err = KIMI_LIC_ERR_SOFTWARE_NOT_EXIST;
	}
	else
		err = KIMI_LIC_ERR_FILE_NOT_EXIST;

_fail_exit:
	delete pLicenseFile;
	return err;
}

// 带文件掩码支持的授权文件加载，比如lpszLicFileMask = "c:\*.lic"
KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_load_mask(const char *lpszLicFileMask, const char *lpszSoftwareName, const char *lpszAuthPropertyName, KIMILIC_HANDLE* pLicHandle)
{
	*pLicHandle = NULL;
	WIN32_FIND_DATA fd;

	if (lpszLicFileMask == NULL)
		lpszLicFileMask = "*.lic";
	else if (lstrlen(lpszLicFileMask) >= (_countof(fd.cFileName)))
	{
		return KIMI_LIC_ERR_FILE_NOT_EXIST;	
	}

	Checked::tcscpy_s(fd.cFileName, _countof(fd.cFileName), lpszLicFileMask);

	HANDLE hFind = ::FindFirstFile(lpszLicFileMask, &fd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return KIMI_LIC_ERR_FILE_NOT_EXIST;
	}

	char szRootPath[_MAX_PATH];
	char* pstr = _fullpath(szRootPath, lpszLicFileMask, _MAX_PATH);

	// passed name isn't a valid path but was found by the API
	ASSERT(pstr != NULL);
	if (pstr == NULL)
	{
		return KIMI_LIC_ERR_FILE_NOT_EXIST;
	}
	else
	{
		char strDrive[_MAX_DRIVE], strDir[_MAX_DIR];
		Checked::tsplitpath_s(szRootPath, strDrive, _MAX_DRIVE, strDir, _MAX_DIR, NULL, 0, NULL, 0);
		Checked::tmakepath_s(szRootPath, _MAX_PATH, strDrive, strDir, NULL, NULL);
	}

	KIMILIC_ERRCODE err = KIMI_LIC_ERR_FILE_NOT_EXIST;

	while(1) 
	{
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) || (fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM))
		{
			// 忽略隐藏文件和系统文件
		}
		else if (((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) && (fd.cFileName[0] != '.'))
		{
			std::string szLicFilePath = szRootPath;

			char* pchLast;
			const char* pszResult = szLicFilePath.c_str();
			pchLast = _tcsdec( pszResult, pszResult + szLicFilePath.length() );
			if (pchLast == NULL)
				return KIMI_LIC_ERR_FILE_NOT_EXIST;

			if ((*pchLast != '\\') && (*pchLast != '/'))
				szLicFilePath += '\\';
			szLicFilePath += fd.cFileName;

			err = kimi_lic_load(szLicFilePath.c_str(), lpszSoftwareName, lpszAuthPropertyName, pLicHandle);
			if (err == KIMI_LIC_ERR_OK)
				break;
		}

		if (!::FindNextFile(hFind, &fd))
			break;
	}

	::FindClose(hFind);

	return err;
}

KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_memory_load(const unsigned char *lpszLicBuffer, const unsigned long dwLicBufferSize, const char *lpszSoftwareName, const char *lpszAuthPropertyName, 
					 KIMILIC_HANDLE* pLicHandle)
{
	KIMILIC_ERRCODE err = KIMI_LIC_ERR_FAIL;
	*pLicHandle = NULL;
	CLicenseFile *pLicenseFile = new CLicenseFile();

	if (pLicenseFile->Load(lpszLicBuffer, dwLicBufferSize))
	{
		CLicenseSoftware *pSoftware = pLicenseFile->GetLicenseSoftware(lpszSoftwareName);
		if (pSoftware)
		{
			if (lpszAuthPropertyName && *lpszAuthPropertyName)
			{
				CLicenseProperty* pPty = pSoftware->GetProperty(lpszAuthPropertyName);

				bool bAuth = false;
				if (pPty)
					bAuth = pPty->AsBool(false);

				if (!bAuth)
				{
					err = KIMI_LIC_ERR_NOT_AUTHORIZED;
					goto _fail_exit;
				}
			}

			if (!pLicenseFile->CheckMachineCode())
			{
				err = KIMI_LIC_ERR_MCODE_NOT_MATCH;
			}
			else if (pLicenseFile->CheckExpiredTime() <= 0)
			{
				err = KIMI_LIC_ERR_AUTH_EXPIRED;
			}
			else
				err = KIMI_LIC_ERR_OK;

			pSoftware->SetUserData( reinterpret_cast<DWORD_PTR>(pLicenseFile) );
			*pLicHandle = reinterpret_cast<KIMILIC_HANDLE>(pSoftware);

			return err;
		}
		else
			err = KIMI_LIC_ERR_SOFTWARE_NOT_EXIST;
	}
	else
		err = KIMI_LIC_ERR_FILE_NOT_EXIST;

_fail_exit:
	delete pLicenseFile;
	return err;
}

KIMI_LICENSE_API(void)
kimi_lic_free(KIMILIC_HANDLE hLicHandle)
{
	CLicenseSoftware *pSoftware = HANDLE2Ptr(hLicHandle);
	if (pSoftware)
	{
		CLicenseFile *pLicenseFile = reinterpret_cast<CLicenseFile *>( pSoftware->GetUserData() );
		delete pLicenseFile;
	}
}


KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_get_property_count(KIMILIC_HANDLE hLicHandle, unsigned long *pdwPropertyCount)
{
	*pdwPropertyCount = 0;

	CLicenseSoftware *pSoftware = HANDLE2Ptr(hLicHandle);

	if (!pSoftware)
	{
		return KIMI_LIC_ERR_INVALID_HANDLE;
	}

	*pdwPropertyCount = (DWORD)pSoftware->GetProperties().size();

	return KIMI_LIC_ERR_OK;
}

KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_get_property_name(KIMILIC_HANDLE hLicHandle, unsigned long dwIndex, char *lpszPropertyName, unsigned long *pdwPropertyNameSize)
{
	CLicenseSoftware *pSoftware = HANDLE2Ptr(hLicHandle);

	if (!pSoftware)
	{
		*pdwPropertyNameSize = 0;
		return KIMI_LIC_ERR_INVALID_HANDLE;
	}

	_LicPropertiesArray &arrProperties = pSoftware->GetProperties();

	if (dwIndex >= 0 && dwIndex < arrProperties.size())
	{
		if ((!lpszPropertyName) || (*pdwPropertyNameSize == 0)) // 查询字符串长度
		{
			*pdwPropertyNameSize = (DWORD)strlen(arrProperties[dwIndex].GetName()) + 1;
			return KIMI_LIC_ERR_OK;
		}
		else {
			/*
			if (!strcpy_s(lpszPropertyName, *pdwPropertyNameSize, arrProperties[dwIndex].GetName()))
			{
				*pdwPropertyNameSize = (DWORD)strlen(lpszPropertyName);
				return KIMI_LIC_ERR_OK;
			}
			else {
				*pdwPropertyNameSize = 0;
				lpszPropertyName[0] = '\0';
				return KIMI_LIC_ERR_FAIL;
			}
			*/
			return StringCopy(lpszPropertyName, pdwPropertyNameSize, arrProperties[dwIndex].GetName());
		}
	}
	else
		return KIMI_LIC_ERR_FAIL;
}

KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_get_property_info(KIMILIC_HANDLE hLicHandle, const char *lpszPropertyName,
						   char *lpszCategory, unsigned long *pdwCategorySize,
						   char *lpszDisplayName, unsigned long *pdwDisplayNameSize,
						   char *lpszDescription, unsigned long *pdwDescriptionSize,
						   char *lpszOptions, unsigned long *pdwOptionsSize,
						   int *pnMinValue, int *pnMaxValue)
{
	CLicenseSoftware *pSoftware = HANDLE2Ptr(hLicHandle);
	if (!pSoftware)
		return KIMI_LIC_ERR_INVALID_HANDLE;

	CLicenseProperty *pPty = pSoftware->GetProperty(lpszPropertyName);
	if (pPty)
	{
		if ((!lpszCategory) || (*pdwCategorySize == 0)) // 查询字符串长度
		{
			*pdwCategorySize = (DWORD)strlen(pPty->GetCategory()) + 1;
		}
		else {
			/*
			if (!strcpy_s(lpszCategory, *pdwCategorySize, pPty->GetCategory()))
			{
				*pdwCategorySize = (DWORD)strlen(lpszCategory);
			}
			else {
				*pdwCategorySize = 0;
				lpszCategory[0] = '\0';
			}
			*/
			StringCopy(lpszCategory, pdwCategorySize, pPty->GetCategory());
		}

		if ((!lpszDisplayName) || (*pdwDisplayNameSize == 0)) // 查询字符串长度
		{
			*pdwDisplayNameSize = (DWORD)strlen(pPty->GetDisplayName()) + 1;
		}
		else {
			/*
			if (!strcpy_s(lpszDisplayName, *pdwDisplayNameSize, pPty->GetDisplayName()))
			{
				*pdwDisplayNameSize = (DWORD)strlen(lpszDisplayName);
			}
			else {
				*pdwDisplayNameSize = 0;
				lpszDisplayName[0] = '\0';
			}
			*/
			StringCopy(lpszDisplayName, pdwDisplayNameSize, pPty->GetDisplayName());
		}

		if ((!lpszDescription) || (*pdwDescriptionSize == 0)) // 查询字符串长度
		{
			*pdwDescriptionSize = (DWORD)strlen(pPty->GetDescription()) + 1;
		}
		else {
			/*
			if (!strcpy_s(lpszDescription, *pdwDescriptionSize, pPty->GetDescription()))
			{
				*pdwDescriptionSize = (DWORD)strlen(lpszDescription);
			}
			else {
				*pdwDescriptionSize = 0;
				lpszDescription[0] = '\0';
			}
			*/
			StringCopy(lpszDescription, pdwDescriptionSize, pPty->GetDescription());
		}

		if ((!lpszOptions) || (*pdwOptionsSize == 0)) // 查询字符串长度
		{
			*pdwOptionsSize = (DWORD)strlen(pPty->GetOptions()) + 1;
		}
		else {
			/*
			if (!strcpy_s(lpszOptions, *pdwOptionsSize, pPty->GetOptions()))
			{
				*pdwOptionsSize = (DWORD)strlen(lpszOptions);
			}
			else {
				*pdwOptionsSize = 0;
				lpszOptions[0] = '\0';
			}
			*/
			StringCopy(lpszOptions, pdwOptionsSize, pPty->GetOptions());
		}

		*pnMinValue = pPty->GetMinValue();
		*pnMaxValue = pPty->GetMaxValue();

		return KIMI_LIC_ERR_OK;
	}
	else {
		return KIMI_LIC_ERR_PROPERTY_NOT_EXIST;
	}
}

KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_get_auth_info(KIMILIC_HANDLE hLicHandle, unsigned long* pbMachineCode, long* pnExpiredTime)
{
	CLicenseSoftware *pSoftware = HANDLE2Ptr(hLicHandle);
	if (!pSoftware)
	{
		return KIMI_LIC_ERR_INVALID_HANDLE;
	}

	KIMILIC_ERRCODE err = KIMI_LIC_ERR_OK;

	CLicenseFile *pLicenseFile = reinterpret_cast<CLicenseFile *>( pSoftware->GetUserData() );

	const char* szMC = pLicenseFile->GetMachineCode();
	*pbMachineCode = (szMC && *szMC);
	*pnExpiredTime = pLicenseFile->IsExpiredTime() ? pLicenseFile->GetExpiredTime() : 0;

	if (!pLicenseFile->CheckMachineCode())
	{
		err = KIMI_LIC_ERR_MCODE_NOT_MATCH;
	}
	else if (pLicenseFile->CheckExpiredTime() <= 0)
	{
		err = KIMI_LIC_ERR_AUTH_EXPIRED;
	}

	return err;
}

KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_get_uername(KIMILIC_HANDLE hLicHandle, char *lpszUserName, unsigned long *pdwSize)
{
	CLicenseSoftware *pSoftware = HANDLE2Ptr(hLicHandle);
	if (!pSoftware)
	{
		*pdwSize = 0;
		return KIMI_LIC_ERR_INVALID_HANDLE;
	}

	CLicenseFile *pLicenseFile = reinterpret_cast<CLicenseFile *>( pSoftware->GetUserData() );
	if (pLicenseFile)
	{
		if ((!lpszUserName) || (*pdwSize == 0)) // 查询字符串长度
		{
			*pdwSize = (DWORD)strlen(pLicenseFile->GetUserName()) + 1;
			return KIMI_LIC_ERR_OK;
		}
		else {
			/*
			if (!strcpy_s(lpszUserName, pLicenseFile->GetUserName(), *pdwSize))
			{
				*pdwSize = (DWORD)strlen(lpszUserName);
				return KIMI_LIC_ERR_OK;
			}
			else {
				*pdwSize = 0;
				lpszUserName[0] = '\0';
				return KIMI_LIC_ERR_FAIL;
			}
			*/
			return StringCopy(lpszUserName, pdwSize, pLicenseFile->GetUserName());
		}
	}
	else
		return KIMI_LIC_ERR_FILE_NOT_EXIST;
}

KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_get_bool(KIMILIC_HANDLE hLicHandle, const char *lpszPropertyName, bool *pbValue, bool bDefault)
{
	*pbValue = bDefault;

	CLicenseSoftware *pSoftware = HANDLE2Ptr(hLicHandle);
	if (!pSoftware)
		return KIMI_LIC_ERR_INVALID_HANDLE;

	CLicenseProperty *pPty = pSoftware->GetProperty(lpszPropertyName);
	if (pPty)
	{
		*pbValue = pPty->AsBool(bDefault);
		return KIMI_LIC_ERR_OK;
	}
	else {
		return KIMI_LIC_ERR_PROPERTY_NOT_EXIST;
	}
}

KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_get_long(KIMILIC_HANDLE hLicHandle, const char *lpszPropertyName, long *plValue, long lDefault)
{
	*plValue = lDefault;

	CLicenseSoftware *pSoftware = HANDLE2Ptr(hLicHandle);
	if (!pSoftware)
		return KIMI_LIC_ERR_INVALID_HANDLE;
	
	CLicenseProperty *pPty = pSoftware->GetProperty(lpszPropertyName);
	if (pPty)
	{
		*plValue = pPty->AsLong(lDefault);
		return KIMI_LIC_ERR_OK;
	}
	else {
		return KIMI_LIC_ERR_PROPERTY_NOT_EXIST;
	}
}

KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_get_double(KIMILIC_HANDLE hLicHandle, const char *lpszPropertyName, double *pdblValue, double dblDefault)
{
	*pdblValue = dblDefault;

	CLicenseSoftware *pSoftware = HANDLE2Ptr(hLicHandle);

	if (!pSoftware)
		return KIMI_LIC_ERR_INVALID_HANDLE;

	CLicenseProperty *pPty = pSoftware->GetProperty(lpszPropertyName);
	if (pPty)
	{
		*pdblValue = pPty->AsDouble(dblDefault);
		return KIMI_LIC_ERR_OK;
	}
	else {
		return KIMI_LIC_ERR_PROPERTY_NOT_EXIST;
	}
}

KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_get_string(KIMILIC_HANDLE hLicHandle, const char *lpszPropertyName, char *lpszBuffer, unsigned long *pdwBufferSize, const char *lpszDefault)
{
	CLicenseSoftware *pSoftware = HANDLE2Ptr(hLicHandle);

	if (!pSoftware)
	{
		*pdwBufferSize = 0;
		return KIMI_LIC_ERR_INVALID_HANDLE;
	}

	CLicenseProperty *pPty = pSoftware->GetProperty(lpszPropertyName);
	if (pPty)
	{
		if ((!lpszBuffer) || (*pdwBufferSize == 0)) // 查询字符串长度
		{
			*pdwBufferSize = (DWORD)strlen(pPty->AsString()) + 1;
			return KIMI_LIC_ERR_OK;
		}
		else {
			/*
			if (!strcpy_s(lpszBuffer, *pdwBufferSize, pPty->AsString()))
			{
				*pdwBufferSize = (DWORD)strlen(lpszBuffer);
				return KIMI_LIC_ERR_OK;
			}
			else {
				*pdwBufferSize = 0;
				lpszBuffer[0] = '\0';
				return KIMI_LIC_ERR_FAIL;
			}
			*/
			return StringCopy(lpszBuffer, pdwBufferSize, pPty->AsString());
		}
	}
	else {
		return KIMI_LIC_ERR_PROPERTY_NOT_EXIST;
	}
}

KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_get_machine_code(
						  char *lpszMachineCode,
						  unsigned long *pdwSize
						  )
{
	CMachineCodeHelper mch;

	string s = mch.GetMachineBase64Encode();

	if ((!lpszMachineCode) || (*pdwSize == 0)) // 查询字符串长度
	{
		*pdwSize = (DWORD)s.length() + 1;
		return KIMI_LIC_ERR_OK;
	}
	else {
		/*
		if (!strcpy_s(lpszMachineCode, *pdwSize, s.c_str()))
		{
			*pdwSize = (DWORD)strlen(lpszMachineCode);
			return KIMI_LIC_ERR_OK;
		}
		else {
			*pdwSize = 0;
			lpszMachineCode[0] = '\0';
			return KIMI_LIC_ERR_FAIL;
		}
		*/
		return StringCopy(lpszMachineCode, pdwSize, s.c_str());
	}
}