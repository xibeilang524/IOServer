#ifndef __KIMI_KERNEL_UTILITY_H__
#define __KIMI_KERNEL_UTILITY_H__

#include <io.h>
#include <string>
#include <tchar.h>

_KIMI_NAMESPACE_BEGIN

class KIMI_CORE_CLASS Utility
{
public:
	//////////////////////////////////////////////////////////////////////////
	/// 系统相关
	//////////////////////////////////////////////////////////////////////////

	/// 当前系统是否为NT平台
	static BOOL IsWindowNT(void);

	/// 当前系统版本号
	static string GetWindowVersion(void);

	static string FormatErrorMessage(DWORD dwErrorCode);


	//////////////////////////////////////////////////////////////////////////
	/// 字符串相关
	//////////////////////////////////////////////////////////////////////////
	/// 格式化文件大小，自动选择KB/GB/TB单位
	static string FormatFileSize(__int64 nFileSize);

	/// 以KB为单位格式化文件大小为
	static string FormatFileSizeKB(__int64 nFileSize);

	//////////////////////////////////////////////////////////////////////////
	/// 磁盘与目录相关
	//////////////////////////////////////////////////////////////////////////

	/// 获取模块的绝对路径，对于MFC应用，hModule = AfxGetInstanceHandle()时可获取当前运行程序的路径
	static string GetModulePath(HMODULE hModule/* = NULL*/);

	/// 指定目录是否存在
	static BOOL DirectoryExists(const char *lpszPath);
	static BOOL DirectoryExists(const wchar_t *lpszPath);

	/// 创建目录，可逐级建立
	static BOOL CreateDirectory(const char *lpszPath);
	static BOOL CreateDirectory(const wchar_t *lpszPath);

	/// 删除目录，递归方式
	static BOOL DeleteDirectory(const char *lpszPath);
	static BOOL DeleteDirectory(const wchar_t *lpszPath);

	/// 获取磁盘可用空间
	static ULONGLONG GetDiskFreeSpace(const char *lpszDisk);
	static ULONGLONG GetDiskFreeSpace(const wchar_t *lpszDisk);

	//////////////////////////////////////////////////////////////////////////
	/// 文件版本相关
	//////////////////////////////////////////////////////////////////////////

	/// 获取指定文件的版本号
	static BOOL GetFileVersion(const char *lpszFileName, WORD &wProductVersionMS1, WORD &wProductVersionMS2, WORD &wProductVersionLS1, WORD &wProductVersionLS2);
	static BOOL GetFileVersion(const wchar_t *lpszFileName, WORD &wProductVersionMS1, WORD &wProductVersionMS2, WORD &wProductVersionLS1, WORD &wProductVersionLS2);
	
	/// 获取文件的版本字符串
	static BOOL GetFileVersion(const char *lpszFileName, std::string &strVersion, BOOL bGetBuildInfo = TRUE);
	static BOOL GetFileVersion(const wchar_t *lpszFileName, std::wstring &strVersion, BOOL bGetBuildInfo = TRUE);

	//////////////////////////////////////////////////////////////////////////
	/// 数据校验及HASH值相关
	//////////////////////////////////////////////////////////////////////////
	/// 计算CRC32校验值
	static DWORD GetCRC32(const BYTE *szData, DWORD dwSize);
	/// 计算CRC64校验值
	static ULONGLONG GetCRC64(ULONGLONG ullCRC64, const unsigned char *pBuffer, ULONGLONG ullBufferSize);

	/// 计算数据流的MD5值，ANSI版本
	static string GetMD5(const unsigned char* lpBuffer, unsigned int nTextLen);
	/// 计算数据流的MD5值，Unicode版本
	static wstring GetMD5W(const unsigned char* lpBuffer, unsigned int nTextLen);
	/// 计算指定字符串的MD5值，ANSI版本
	static string GetMD5(const char* lpszText);
	/// 计算指定字符串的MD5值，Unicode版本
	static wstring GetMD5W(const wchar_t* lpszText);

	/// 计算数据流的SHA1值，ANSI版本
	static string GetSHA1(const unsigned char* lpBuffer, unsigned int nTextLen);
	/// 计算数据流的SHA1值，Unicode版本
	static wstring GetSHA1W(const unsigned char* lpBuffer, unsigned int nTextLen);
	/// 计算字符串的SHA1值，ANSI版本
	static string GetSHA1(const char* lpszText);
	/// 计算字符串的SHA1值，Unicode版本
	static wstring GetSHA1W(const wchar_t* lpszText);

protected:
	static BOOL GetVersionStringInfo(LPVOID lpvMem, const char *lpszKey, char *lpszReturnValue, DWORD *pdwReturnValueSize);
	static BOOL GetVersionStringInfo(LPVOID lpvMem, const wchar_t *lpszKey, wchar_t *lpszReturnValue, DWORD *pdwReturnValueSize);
};

_KIMI_NAMESPACE_END

#endif // end of __KIMI_KERNEL_UTILITY_H__