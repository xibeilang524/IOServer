#ifndef __UTILITY_H__
#define __UTILITY_H__

#include  <io.h>


TRUNK_NAMESPACE_BEGIN

DWORD TL_GetCRC32(BYTE *szData, DWORD dwSize);

class _GENERAL_CLASS Utility
{
public:
	/// 计算CRC32校验值
	static DWORD GetCRC32(BYTE *szData, DWORD dwSize) {
		return TL_GetCRC32(szData, dwSize);
	}

	/// 获取当前程序的版本号
	static DWORD GetModuleVersion(HMODULE hModule = NULL);

	static BOOL GetModuleVersion(CString &szVersion, BOOL bGetBuildInfo = TRUE, HMODULE hModule = NULL);

	/// 带*/?通配符的出字符串比较函数
	static BOOL MatchStr(LPCTSTR ssrc, LPCTSTR sfliter);

	/// 获取指定模块的路径名称
	static CString GetModulePath(HMODULE hModule = NULL);

	/// 创建目录,可逐级建立
	static BOOL CreateDir(LPCTSTR lpszPath);

	/// 判断后面是否有斜杠,无则加上
	static void	MakePath(CString &szPath);

	/// 删除目录,递归方式
	static void DeleteDir(CString strDelDir);
	
	/// 获取磁盘空间
	static long GetDiskSpace(LPCTSTR lpszDisk);

protected:
	static BOOL GetVersionStringInfo(LPVOID  lpvMem, LPCTSTR lpszKey, LPTSTR lpszReturnValue);
};

TRUNK_NAMESPACE_END

#endif // end of __UTILITY_H__