// Utility.cpp : 辅助函数
//

#include "stdafx.h"
#include "KimiCore.h"
#include "hash\CRC32.h"
#include "hash\CRC64.h"
#include "KimiCore\MD5.h"
#include "KimiCore\Utility\Utility.h"
#include "hash\SHA1.h"
#include "stl_string.h"
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "version.lib")

_KIMI_NAMESPACE_BEGIN

static OSVERSIONINFO g_osviOSVersionInfo = { sizeof(OSVERSIONINFO) };
static BOOL g_bReadOSVersionInfo = FALSE;

//////////////////////////////////////////////////////////////////////////
/// 系统相关
//////////////////////////////////////////////////////////////////////////
BOOL Utility::IsWindowNT(void)
{
	if (!g_bReadOSVersionInfo)
		g_bReadOSVersionInfo = (::GetVersionEx(&g_osviOSVersionInfo) == TRUE);

	return (g_osviOSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT);
}

string Utility::GetWindowVersion(void)
{
	if (!g_bReadOSVersionInfo)
		g_bReadOSVersionInfo = (::GetVersionEx(&g_osviOSVersionInfo) == TRUE);

	string s;
	stl_string::format(s, "%u.%u.%u", g_osviOSVersionInfo.dwMajorVersion, g_osviOSVersionInfo.dwMinorVersion, g_osviOSVersionInfo.dwBuildNumber);

	return s;
}

string Utility::FormatErrorMessage(DWORD dwErrorCode)
{
	char* buf = NULL;

	::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwErrorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buf, 0, NULL);

	string s;
	if (buf)
	{
		s = buf;
		::LocalFree(buf);
	}
	else
		stl_string::format(s, "Unknown error(%02X)", dwErrorCode);

	return s;
}

//////////////////////////////////////////////////////////////////////////
/// 字符串相关
//////////////////////////////////////////////////////////////////////////
string Utility::FormatFileSize(__int64 nFileSize)
{
	char pBuffer[32];
	UINT nSize = 32;

	if (::StrFormatByteSize64A(nFileSize, pBuffer, nSize))
		return string(pBuffer);
	else
		return "";
}

string Utility::FormatFileSizeKB(__int64 nFileSize)
{
	char pBuffer[32];
	UINT nSize = 32;

	if (::StrFormatKBSize(nFileSize, pBuffer, nSize))
		return string(pBuffer);
	else
		return "";
}

//////////////////////////////////////////////////////////////////////////
/// 磁盘与目录相关
//////////////////////////////////////////////////////////////////////////

string Utility::GetModulePath(HMODULE hModule/* = NULL*/)
{
	char sFilename[_MAX_PATH];
	char sDrive[_MAX_DRIVE];
	char sDir[_MAX_DIR];
	char sFname[_MAX_FNAME];
	char sExt[_MAX_EXT];

// 	if (hModule == NULL)
// 		hModule = AfxGetInstanceHandle();

	::GetModuleFileNameA(hModule, sFilename, _MAX_PATH);
	
	_splitpath_s(sFilename, sDrive, _MAX_DRIVE, sDir, _MAX_DIR, sFname, _MAX_FNAME, sExt, _MAX_EXT);

	string s;
	s = sDrive;
	s += sDir;

	if (s[s.length() - 1] != '\\')
		s += "\\";

	return s;
}

BOOL Utility::DirectoryExists(const char *lpszPath)
{
	if (!lpszPath || !*lpszPath)
		return FALSE;

	//if (-1 == _access_s( lpszPath, 0 )) // 目录不存在
	if (ENOENT == _access_s( lpszPath, 0 ))
		return FALSE;
	else
		return TRUE;
}

BOOL Utility::DirectoryExists(const wchar_t *lpszPath)
{
	USES_CONVERSION;
	return DirectoryExists( W2A(lpszPath) );
}

BOOL Utility::CreateDirectory(const char *lpszPath)
{
	std::string szpath = lpszPath;
	std::string szt = "", szMsg, szLastDir = "";
	char c;
	size_t i, n = szpath.length();
	for (i = 0; i < n; i++)
	{
		c = szpath[i];
		if ( (c != '\\') && (c != '/'))
		{
			szt += c;
			continue;
		}
		if (i == 0)
		{
			szt += c;
			continue;
		}
		if (szt[i-1] == ':')
		{
			szt += c;
			continue;
		}
		if (!DirectoryExists(szt.c_str())) // 目录不存在
		{
			if (!::CreateDirectoryA(szt.c_str(), NULL))
				return false; // 创建目录失败!
		}
		szt += '\\';
		szLastDir = szt;
	}

	if (szLastDir.empty())
		return FALSE;

	if (szt == szLastDir)
		return TRUE;

	if (!DirectoryExists(szt.c_str())) // 目录不存在
	{
		if (!::CreateDirectoryA(szt.c_str(), NULL))
			return FALSE; // 创建目录失败!
	}

	return TRUE;
}

BOOL Utility::CreateDirectory(const wchar_t *lpszPath)
{
	std::wstring szpath = lpszPath;
	std::wstring szt = L"", szMsg, szLastDir = L"";
	wchar_t c;
	size_t i, n = szpath.length();
	for (i = 0; i < n; i++)
	{
		c = szpath[i];
		if ( (c != '\\') && (c != '/'))
		{
			szt += c;
			continue;
		}
		if (i == 0)
		{
			szt += c;
			continue;
		}
		if (szt[i-1] == ':')
		{
			szt += c;
			continue;
		}
		if (!DirectoryExists(szt.c_str())) // 目录不存在
		{
			if (!::CreateDirectoryW(szt.c_str(), NULL))
				return FALSE; // 创建目录失败!
		}
		szt += '\\';
		szLastDir = szt;
	}

	if (szLastDir.empty())
		return FALSE;

	if (szt == szLastDir)
		return TRUE;

	if (!DirectoryExists(szt.c_str())) // 目录不存在
	{
		if (!::CreateDirectoryW(szt.c_str(), NULL))
			return FALSE; // 创建目录失败!
	}

	return TRUE;
}

BOOL Utility::DeleteDirectory(const char *lpszPath)
{
	std::string strRoot, strDir, strFile;

	strDir = lpszPath;
	if (strDir[strDir.length() - 1] != '\\')
		strDir += "\\";

	strRoot = strDir;

	strDir += "*.*";

	WIN32_FIND_DATAA findFileData;

	HANDLE hFind = ::FindFirstFileA(strDir.c_str(), &findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return FALSE;

	BOOL bFind = TRUE;

	while( bFind )
	{
		if (findFileData.cFileName[0] == '.')
		{
			if (findFileData.cFileName[1] == '\0' ||
				(findFileData.cFileName[1] == '.' &&
				findFileData.cFileName[2] == '\0'))
			{
				continue;
			}
		}

		strFile = strRoot;
		strFile += findFileData.cFileName;

		if ( findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			Utility::DeleteDirectory(strFile.c_str());
		else
		{
			if ( findFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY )
				::SetFileAttributesA(strFile.c_str(), ::GetFileAttributesA(strFile.c_str()) & (~FILE_ATTRIBUTE_READONLY) );

			::DeleteFileA(strFile.c_str());
		}

		bFind = ::FindNextFileA(hFind, &findFileData);
	}
	::FindClose(hFind);

	::RemoveDirectoryA(lpszPath);

	return TRUE;
}

BOOL Utility::DeleteDirectory(const wchar_t *lpszPath)
{
	std::wstring strRoot, strDir, strFile;

	strDir = lpszPath;
	if (strDir[strDir.length() - 1] != '\\')
		strDir += L"\\";

	strRoot = strDir;

	strDir += L"*.*";

	WIN32_FIND_DATAW findFileData;

	HANDLE hFind = ::FindFirstFileW(strDir.c_str(), &findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return FALSE;

	BOOL bFind = TRUE;

	while( bFind )
	{
		if (findFileData.cFileName[0] == '.')
		{
			if (findFileData.cFileName[1] == '\0' ||
				(findFileData.cFileName[1] == '.' &&
				findFileData.cFileName[2] == '\0'))
			{
				continue;
			}
		}

		strFile = strRoot;
		strFile += findFileData.cFileName;

		if ( findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			Utility::DeleteDirectory(strFile.c_str());
		else
		{
			if ( findFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY )
				::SetFileAttributesW(strFile.c_str(), ::GetFileAttributesW(strFile.c_str()) & (~FILE_ATTRIBUTE_READONLY) );

			::DeleteFileW(strFile.c_str());
		}

		bFind = ::FindNextFileW(hFind, &findFileData);
	}
	::FindClose(hFind);

	::RemoveDirectoryW(lpszPath);

	return TRUE;
}

ULONGLONG Utility::GetDiskFreeSpace(const char *lpszDisk)
{
	ULARGE_INTEGER ullFreeBytesAvailable, ullTotalNumberOfBytes, ullTotalNumberOfFreeBytes;

	ullFreeBytesAvailable.QuadPart = 0;
	ullTotalNumberOfBytes.QuadPart = 0;
	ullTotalNumberOfFreeBytes.QuadPart = 0;

	BOOL bret = ::GetDiskFreeSpaceExA(
		lpszDisk,      // directory name
		&ullFreeBytesAvailable,    // bytes available to caller
		&ullTotalNumberOfBytes,    // bytes on disk
		&ullTotalNumberOfFreeBytes // free bytes on disk
		);

	if (bret)
		return ullFreeBytesAvailable.QuadPart;
	else
		return 0;
}

ULONGLONG Utility::GetDiskFreeSpace(const wchar_t *lpszDisk)
{
	ULARGE_INTEGER ullFreeBytesAvailable, ullTotalNumberOfBytes, ullTotalNumberOfFreeBytes;

	ullFreeBytesAvailable.QuadPart = 0;
	ullTotalNumberOfBytes.QuadPart = 0;
	ullTotalNumberOfFreeBytes.QuadPart = 0;

	BOOL bret = ::GetDiskFreeSpaceExW(
		lpszDisk,      // directory name
		&ullFreeBytesAvailable,    // bytes available to caller
		&ullTotalNumberOfBytes,    // bytes on disk
		&ullTotalNumberOfFreeBytes // free bytes on disk
		);

	if (bret)
		return ullFreeBytesAvailable.QuadPart;
	else
		return 0;
}

//////////////////////////////////////////////////////////////////////////
/// 文件版本相关
//////////////////////////////////////////////////////////////////////////
BOOL Utility::GetFileVersion(const char *lpszFileName, WORD &wProductVersionMS1, WORD &wProductVersionMS2, WORD &wProductVersionLS1, WORD &wProductVersionLS2)
{
	DWORD dwVerInfoSize = 0;
	DWORD dwVerHnd;
	VS_FIXEDFILEINFO * pFileInfo;

	DWORD dwVersion = 0;
	BYTE *pbVersion = (BYTE *)&dwVersion;

	if (!Utility::DirectoryExists(lpszFileName))
		return FALSE;

	dwVerInfoSize = ::GetFileVersionInfoSizeA(lpszFileName, &dwVerHnd);
	if (dwVerInfoSize)
	{
		// If we were able to get the information, process it:
		HANDLE  hMem;
		LPVOID  lpvMem;
		unsigned int uInfoSize = 0;

		hMem = ::GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
		lpvMem = ::GlobalLock(hMem);

		::GetFileVersionInfoA(lpszFileName, dwVerHnd, dwVerInfoSize, lpvMem);
		::VerQueryValueA(lpvMem, "\\", (void**)&pFileInfo, &uInfoSize);

		wProductVersionMS1 = (BYTE)HIWORD(pFileInfo->dwProductVersionMS);
		wProductVersionMS2 = (BYTE)LOWORD(pFileInfo->dwProductVersionMS);
		wProductVersionLS1 = (BYTE)HIWORD(pFileInfo->dwProductVersionLS);
		wProductVersionLS2 = (BYTE)LOWORD(pFileInfo->dwProductVersionLS);

		::GlobalUnlock(hMem);
		::GlobalFree(hMem);

		return TRUE;
	}
	else
		return FALSE;
}

BOOL Utility::GetFileVersion(const wchar_t *lpszFileName, WORD &wProductVersionMS1, WORD &wProductVersionMS2, WORD &wProductVersionLS1, WORD &wProductVersionLS2)
{
	DWORD dwVerInfoSize = 0;
	DWORD dwVerHnd;
	VS_FIXEDFILEINFO * pFileInfo;

	DWORD dwVersion = 0;
	BYTE *pbVersion = (BYTE *)&dwVersion;

	if (!Utility::DirectoryExists(lpszFileName))
		return FALSE;

	dwVerInfoSize = ::GetFileVersionInfoSizeW(lpszFileName, &dwVerHnd);
	if (dwVerInfoSize)
	{
		// If we were able to get the information, process it:
		HANDLE  hMem;
		LPVOID  lpvMem;
		unsigned int uInfoSize = 0;

		hMem = ::GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
		lpvMem = ::GlobalLock(hMem);

		::GetFileVersionInfoW(lpszFileName, dwVerHnd, dwVerInfoSize, lpvMem);
		::VerQueryValueW(lpvMem, L"\\", (void**)&pFileInfo, &uInfoSize);

		wProductVersionMS1 = (BYTE)HIWORD(pFileInfo->dwProductVersionMS);
		wProductVersionMS2 = (BYTE)LOWORD(pFileInfo->dwProductVersionMS);
		wProductVersionLS1 = (BYTE)HIWORD(pFileInfo->dwProductVersionLS);
		wProductVersionLS2 = (BYTE)LOWORD(pFileInfo->dwProductVersionLS);

		::GlobalUnlock(hMem);
		::GlobalFree(hMem);

		return TRUE;
	}
	else
		return FALSE;
}

BOOL Utility::GetVersionStringInfo(LPVOID lpvMem, const char *lpszKey, char *lpszReturnValue, DWORD *pdwReturnValueSize)
{
	BOOL rc;
	DWORD *pdwTranslation;
	UINT nLength;
	char *lpszValue;

	if (lpvMem == NULL)
		return FALSE;

	if (!lpszReturnValue)
		return FALSE;

	if (!lpszKey)
		return FALSE;

	*lpszReturnValue = 0;

	rc = ::VerQueryValueA(lpvMem, "\\VarFileInfo\\Translation", 
		(void**) &pdwTranslation, &nLength);
	if (!rc)
		return FALSE;

	char szKey[2000];
	wsprintfA(szKey, "\\StringFileInfo\\%04x%04x\\%s",
		LOWORD (*pdwTranslation), HIWORD (*pdwTranslation),
		lpszKey);

	rc = ::VerQueryValueA(lpvMem, szKey, (void**) &lpszValue, &nLength);

	if (!rc)
		return FALSE;

	if ((!lpszReturnValue) || (*pdwReturnValueSize == 0)) // 查询字符串长度
	{
		*pdwReturnValueSize = nLength + 1;
		return TRUE;
	}
	else {
		if (!strcpy_s(lpszReturnValue, *pdwReturnValueSize, lpszValue))
		{
			*pdwReturnValueSize = (DWORD)strlen(lpszReturnValue);
			return TRUE;
		}
		else {
			*pdwReturnValueSize = 0;
			lpszReturnValue[0] = '\0';
			return FALSE;
		}
	}
}

BOOL Utility::GetVersionStringInfo(LPVOID lpvMem, const wchar_t *lpszKey, wchar_t *lpszReturnValue, DWORD *pdwReturnValueSize)
{
	BOOL rc;
	DWORD *pdwTranslation;
	UINT nLength;
	wchar_t *lpszValue;

	if (lpvMem == NULL)
		return FALSE;

	if (!lpszReturnValue)
		return FALSE;

	if (!lpszKey)
		return FALSE;

	*lpszReturnValue = 0;

	rc = ::VerQueryValueW(lpvMem, L"\\VarFileInfo\\Translation", 
		(void**) &pdwTranslation, &nLength);
	if (!rc)
		return FALSE;

	wchar_t szKey[2000];
	wsprintfW(szKey, L"\\StringFileInfo\\%04x%04x\\%s",
		LOWORD (*pdwTranslation), HIWORD (*pdwTranslation),
		lpszKey);

	rc = ::VerQueryValueW(lpvMem, szKey, (void**) &lpszValue, &nLength);

	if (!rc)
		return FALSE;

	if ((!lpszReturnValue) || (*pdwReturnValueSize == 0)) // 查询字符串长度
	{
		*pdwReturnValueSize = nLength + 1;
		return TRUE;
	}
	else {
		if (!wcscpy_s(lpszReturnValue, *pdwReturnValueSize, lpszValue))
		{
			*pdwReturnValueSize = (DWORD)wcslen(lpszReturnValue);
			return TRUE;
		}
		else {
			*pdwReturnValueSize = 0;
			lpszReturnValue[0] = '\0';
			return FALSE;
		}
	}
}

BOOL Utility::GetFileVersion(const char *lpszFileName, std::string &strVersion, BOOL bGetBuildInfo/* = TRUE*/)
{
	DWORD dwVerInfoSize = 0;
	DWORD dwVerHnd;
	VS_FIXEDFILEINFO * pFileInfo;

	DWORD dwVersion = 0;
	BYTE *pbVersion = (BYTE *)&dwVersion;

	if (!Utility::DirectoryExists(lpszFileName))
		return FALSE;

	char szVersion[128];

	dwVerInfoSize = ::GetFileVersionInfoSizeA(lpszFileName, &dwVerHnd);
	if (dwVerInfoSize)
	{
		// If we were able to get the information, process it:
		HANDLE  hMem;
		LPVOID  lpvMem;
		unsigned int uInfoSize = 0;

		hMem = ::GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
		lpvMem = ::GlobalLock(hMem);

		::GetFileVersionInfoA(lpszFileName, dwVerHnd, dwVerInfoSize, lpvMem);
		::VerQueryValueA(lpvMem, "\\", (void**)&pFileInfo, &uInfoSize);


		wsprintfA(szVersion, "V%d.%d.%d.%d",
			HIWORD(pFileInfo->dwProductVersionMS),
			LOWORD(pFileInfo->dwProductVersionMS),
			HIWORD(pFileInfo->dwProductVersionLS),
			LOWORD(pFileInfo->dwProductVersionLS)
			);

		strVersion = szVersion;

		// 取创建日期
		if (bGetBuildInfo)
		{
			char szBuildVersion[MAX_PATH];
			DWORD dwSize = MAX_PATH;
			szBuildVersion[0] = '\0';
			BOOL bret = Utility::GetVersionStringInfo(lpvMem, "SpecialBuild", szBuildVersion, &dwSize);
			if (!bret)
			{
				szBuildVersion[0] = '\0';
				dwSize = MAX_PATH;
				bret = Utility::GetVersionStringInfo(lpvMem, "PrivateBuild", szBuildVersion, &dwSize);
			}

			if (bret && szBuildVersion[0] != '\0')
			{
				strVersion += ".";
				strVersion += szBuildVersion;
			}
		}

		::GlobalUnlock(hMem);
		::GlobalFree(hMem);

		return TRUE;
	}
	else
		return FALSE;
}

BOOL Utility::GetFileVersion(const wchar_t *lpszFileName, std::wstring &strVersion, BOOL bGetBuildInfo/* = TRUE*/)
{
	DWORD dwVerInfoSize = 0;
	DWORD dwVerHnd;
	VS_FIXEDFILEINFO * pFileInfo;

	DWORD dwVersion = 0;
	BYTE *pbVersion = (BYTE *)&dwVersion;

	if (!Utility::DirectoryExists(lpszFileName))
		return FALSE;

	wchar_t szVersion[128];

	dwVerInfoSize = ::GetFileVersionInfoSizeW(lpszFileName, &dwVerHnd);
	if (dwVerInfoSize)
	{
		// If we were able to get the information, process it:
		HANDLE  hMem;
		LPVOID  lpvMem;
		unsigned int uInfoSize = 0;

		hMem = ::GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
		lpvMem = ::GlobalLock(hMem);

		::GetFileVersionInfoW(lpszFileName, dwVerHnd, dwVerInfoSize, lpvMem);
		::VerQueryValueW(lpvMem, L"\\", (void**)&pFileInfo, &uInfoSize);


		wsprintfW(szVersion, L"V%d.%d.%d.%d",
			HIWORD(pFileInfo->dwProductVersionMS),
			LOWORD(pFileInfo->dwProductVersionMS),
			HIWORD(pFileInfo->dwProductVersionLS),
			LOWORD(pFileInfo->dwProductVersionLS)
			);

		strVersion = szVersion;

		// 取创建日期
		if (bGetBuildInfo)
		{
			wchar_t szBuildVersion[MAX_PATH];
			DWORD dwSize = MAX_PATH;
			szBuildVersion[0] = '\0';
			BOOL bret = Utility::GetVersionStringInfo(lpvMem, L"SpecialBuild", szBuildVersion, &dwSize);
			if (!bret)
			{
				szBuildVersion[0] = '\0';
				dwSize = MAX_PATH;
				bret = Utility::GetVersionStringInfo(lpvMem, L"PrivateBuild", szBuildVersion, &dwSize);
			}

			if (bret && szBuildVersion[0] != '\0')
			{
				strVersion += L".";
				strVersion += szBuildVersion;
			}
		}

		::GlobalUnlock(hMem);
		::GlobalFree(hMem);

		return TRUE;
	}
	else
		return FALSE;
}

//////////////////////////////////////////////////////////////////////////
/// 数据校验及HASH值相关
//////////////////////////////////////////////////////////////////////////

DWORD Utility::GetCRC32(const BYTE *szData, DWORD dwSize)
{
	return get_crc32(szData, dwSize);
}

ULONGLONG Utility::GetCRC64(ULONGLONG ullCRC64, const unsigned char *pBuffer, ULONGLONG ullBufferSize)
{
	return get_crc64(ullCRC64, pBuffer, ullBufferSize);
}

string Utility::GetMD5(const unsigned char* lpBuffer, unsigned int nTextLen)
{
	if (!lpBuffer || !nTextLen)
		return string("");

	unsigned char outBuf[16];
	char buf2[33];

	MD5_Encode((unsigned char*)lpBuffer, nTextLen, outBuf);
	MD5_ToString(outBuf, buf2);

	buf2[32] = 0;
	return string(buf2);
}

wstring Utility::GetMD5W(const unsigned char* lpBuffer, unsigned int nTextLen)
{
	if (!lpBuffer || !nTextLen)
		return wstring(L"");

	unsigned char outBuf[16];
	char buf2[33];

	MD5_Encode((unsigned char*)lpBuffer, nTextLen, outBuf);
	MD5_ToString(outBuf, buf2);

	buf2[32] = 0;

	USES_CONVERSION;
	return wstring( A2W(buf2) );
}

string Utility::GetMD5(const char* lpszText)
{
	if (!lpszText)
		return string("");

	unsigned char outBuf[16];
	char buf2[33];

	MD5_Encode((unsigned char*)lpszText, (unsigned int)strlen(lpszText), outBuf);
	MD5_ToString(outBuf, buf2);

	buf2[32] = 0;
	return string(buf2);
}

wstring Utility::GetMD5W(const wchar_t* lpszText)
{
	if (!lpszText)
		return wstring(L"");

	unsigned char outBuf[16];
	char buf2[33];

	USES_CONVERSION;
	string s = CW2A(lpszText);

	MD5_Encode((unsigned char*)s.c_str(), (unsigned int)s.size(), outBuf);
	MD5_ToString(outBuf, buf2);

	buf2[32] = 0;

	return wstring( A2W(buf2) );
}

string Utility::GetSHA1(const unsigned char* lpBuffer, unsigned int nTextLen)
{
	char szHash[41] ={0};

	SHA1(lpBuffer, nTextLen, szHash, 41);

	return szHash;
}

wstring Utility::GetSHA1W(const unsigned char* lpBuffer, unsigned int nTextLen)
{
	char szHash[41] ={0};

	SHA1(lpBuffer, nTextLen, szHash, 41);

	USES_CONVERSION;
	return wstring( A2W(szHash) );
}

string Utility::GetSHA1(const char* lpszText)
{
	return GetSHA1((const unsigned char*)lpszText, (unsigned int)strlen(lpszText));
}

wstring Utility::GetSHA1W(const wchar_t* lpszText)
{
	USES_CONVERSION;
	string s = CW2A(lpszText);

	return GetSHA1W((const unsigned char*)s.c_str(), (unsigned int)s.size());
}

_KIMI_NAMESPACE_END