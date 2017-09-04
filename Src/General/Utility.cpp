// Utility.cpp : 辅助函数
//

#include "stdafx.h"
#include "GeneralAPI.h"
#include "General\Utility.h"

TRUNK_NAMESPACE_BEGIN


CString Utility::GetModulePath(HMODULE hModule/* = NULL*/)
{
	TCHAR sFilename[_MAX_PATH];
	TCHAR sDrive[_MAX_DRIVE];
	TCHAR sDir[_MAX_DIR];
	TCHAR sFname[_MAX_FNAME];
	TCHAR sExt[_MAX_EXT];

	if (hModule == NULL)
		hModule = AfxGetInstanceHandle();

	::GetModuleFileName(hModule, sFilename, _MAX_PATH);
	_tsplitpath(sFilename, sDrive, sDir, sFname, sExt);

	CString rVal;
	rVal.Append(sDrive);
	rVal.Append(sDir);

	if (rVal.GetAt(rVal.GetLength() - 1) != _T('\\'))
		rVal += _T("\\");

	return rVal;
}

DWORD Utility::GetModuleVersion(HMODULE hModule/* = NULL*/)
{
	TCHAR szFullPath[MAX_PATH];
	DWORD dwVerInfoSize = 0;
	DWORD dwVerHnd;
	VS_FIXEDFILEINFO * pFileInfo;

	DWORD dwVersion = 0;
	BYTE *pbVersion = (BYTE *)&dwVersion;

	::GetModuleFileName(hModule, szFullPath, sizeof(szFullPath));
	dwVerInfoSize = ::GetFileVersionInfoSize(szFullPath, &dwVerHnd);
	if (dwVerInfoSize)
	{
		// If we were able to get the information, process it:
		HANDLE  hMem;
		LPVOID  lpvMem;
		unsigned int uInfoSize = 0;

		hMem = ::GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
		lpvMem = ::GlobalLock(hMem);

		::GetFileVersionInfo(szFullPath, dwVerHnd, dwVerInfoSize, lpvMem);
		::VerQueryValue(lpvMem, (LPTSTR)_T("\\"), (void**)&pFileInfo, &uInfoSize);

		pbVersion[3] = (BYTE)HIWORD(pFileInfo->dwProductVersionMS);
		pbVersion[2] = (BYTE)LOWORD(pFileInfo->dwProductVersionMS);
		pbVersion[1] = (BYTE)HIWORD(pFileInfo->dwProductVersionLS);
		pbVersion[0] = (BYTE)LOWORD(pFileInfo->dwProductVersionLS);

		::GlobalUnlock(hMem);
		::GlobalFree(hMem);
	}

	return dwVersion;
}

BOOL Utility::GetVersionStringInfo(LPVOID  lpvMem, LPCTSTR lpszKey, LPTSTR lpszReturnValue)
{
	BOOL rc;
	DWORD *pdwTranslation;
	UINT nLength;
	LPTSTR lpszValue;

	if (lpvMem == NULL)
		return FALSE;

	if (!lpszReturnValue)
		return FALSE;

	if (!lpszKey)
		return FALSE;

	*lpszReturnValue = 0;

	rc = ::VerQueryValue(lpvMem, _T("\\VarFileInfo\\Translation"), 
		(void**) &pdwTranslation, &nLength);
	if (!rc)
		return FALSE;

	TCHAR szKey[2000];
	wsprintf(szKey, _T("\\StringFileInfo\\%04x%04x\\%s"),
		LOWORD (*pdwTranslation), HIWORD (*pdwTranslation),
		lpszKey);

	rc = ::VerQueryValue(lpvMem, szKey, (void**) &lpszValue, &nLength);

	if (!rc)
		return FALSE;

	lstrcpy(lpszReturnValue, lpszValue);

	return TRUE;
}

BOOL Utility::GetModuleVersion(CString &szVersion, BOOL bGetBuildInfo/* = TRUE*/, HMODULE hModule/* = NULL*/)
{
	TCHAR szFullPath[MAX_PATH];
	DWORD dwVerInfoSize = 0;
	DWORD dwVerHnd;
	VS_FIXEDFILEINFO * pFileInfo;

	DWORD dwVersion = 0;
	BYTE *pbVersion = (BYTE *)&dwVersion;

	szVersion.Empty();

	::GetModuleFileName(hModule, szFullPath, sizeof(szFullPath));
	dwVerInfoSize = ::GetFileVersionInfoSize(szFullPath, &dwVerHnd);
	if (dwVerInfoSize)
	{
		// If we were able to get the information, process it:
		HANDLE  hMem;
		LPVOID  lpvMem;
		unsigned int uInfoSize = 0;

		hMem = ::GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
		lpvMem = ::GlobalLock(hMem);

		::GetFileVersionInfo(szFullPath, dwVerHnd, dwVerInfoSize, lpvMem);
		::VerQueryValue(lpvMem, (LPTSTR)_T("\\"), (void**)&pFileInfo, &uInfoSize);

		szVersion.Format(_T("V%d.%d.%d.%d"), 
			HIWORD(pFileInfo->dwProductVersionMS),
			LOWORD(pFileInfo->dwProductVersionMS),
			HIWORD(pFileInfo->dwProductVersionLS),
			LOWORD(pFileInfo->dwProductVersionLS)
			);

		// 取创建日期
		if (bGetBuildInfo)
		{
			TCHAR szBuildVersion[MAX_PATH * 2];
			szBuildVersion[0] = _T('\0');
			BOOL bret = GetVersionStringInfo(lpvMem, _T("SpecialBuild"), szBuildVersion);
			if (!bret)
			{
				szBuildVersion[0] = _T('\0');
				bret = GetVersionStringInfo(lpvMem, _T("PrivateBuild"), szBuildVersion);
			}

			if (bret && szBuildVersion[0] != _T('\0'))
			{
				szVersion.Append(_T("."));
				szVersion.Append(szBuildVersion);
			}
		}

		::GlobalUnlock(hMem);
		::GlobalFree(hMem);

		return TRUE;
	}
	else
		return FALSE;
}

BOOL Utility::MatchStr(LPCTSTR ssrc, LPCTSTR sfliter)
{
	TCHAR ssub[256],cp=0;
	TCHAR *ps=ssub, *ss = (TCHAR *)ssrc, *sf=(TCHAR *)sfliter;
	if(*sf == 0)//过滤串为空
		return TRUE;
	if((*sf == _T('*')) && (*(sf+1) ==  0))
		return TRUE;
	while((*sf) && (*ss))//扫描过滤字串
	{
		if(*sf == _T('*')){
			if(ps != ssub){
				*ps = 0;
				ss = _tcsstr(ss,ssub);
				if(!ss)
					return FALSE;
				ss += (ps - ssub);	
				ps=ssub;
			}
			cp = _T('*');	sf++;	//ss++;
		}
		else if(*sf == '?'){
			if(ps != ssub){
				*ps = 0;
				ss = _tcsstr(ss,ssub);
				if(!ss)
					return FALSE;
				ss += (ps - ssub);	
				ps=ssub;
			}
			ps = ssub;	
			cp = _T('?');
			ss++;
			sf++;
		}
		else //字符,需要逐个匹配
		{
			if(cp == _T('*'))
				*ps++ = *sf++;
			else{
				if(*sf != *ss)
					return FALSE;
				sf++;		ss++;
			}
		}
	}//while
	if(cp != _T('*'))
	{
		if( *ss == *sf)
			return TRUE;
		if(*sf == _T('*')){
			sf++;
			if(*sf == 0)
				return TRUE;
		}
		return FALSE;
	}
	if(ps != ssub){
		*ps = 0;
		ss = _tcsstr(ss,ssub);
		if(!ss)
			return FALSE;
		ss += (ps - ssub);	
		if(!*ss)
			return TRUE;
		return FALSE; //已非*非?结束的最后不匹配，即去掉原来的默认最后是*规则
	}
	return TRUE;
}

BOOL Utility::CreateDir(LPCTSTR lpszPath) //创建目录,可逐级建立
{
	CString szpath = lpszPath;
	CString szt = _T(""), szMsg, szLastDir = _T("");
	char c;
	int i,n=szpath.GetLength();
	for(i=0;i<n;i++)
	{
		c = szpath[i];
		if((c!='\\') && (c!='/'))
		{
			szt+=c;
			continue;
		}
		if(i==0)
		{
			szt += c;
			continue;
		}
		if(szt[i-1] ==':')
		{
			szt+=c;
			continue;
		}
		TRACE("%s\t",szt);
		if(-1 == _access( szt, 0 ))//目录不存在
		{
			if(!CreateDirectory(szt,NULL))
				return FALSE;//创建目录失败!
		}
		szt+='\\';
		szLastDir = szt;
	}
	if(szLastDir.IsEmpty())
		return FALSE;
	if(szt == szLastDir)
		return TRUE;
	if(-1 == _access( szt, 0 ))//目录不存在
	{
		if(!CreateDirectory(szt,NULL))
			return FALSE;//创建目录失败!
	}
	return TRUE;
}

// 判断后面是否有斜杠,无则加上
void Utility::MakePath(CString &szPath)
{
	if(szPath.IsEmpty())
		return;
	int n=szPath.GetLength();
	if((szPath[n-1] != '/') &&(szPath[n-1] != '\\'))
		szPath += '\\';
}

// 删除目录,递归方式
void Utility::DeleteDir(CString strDelDir)
{
	CFileFind ff;
	CString strDir, strFile;

	strDir = strDelDir;
	Utility::MakePath(strDir);
	strDir += "*.*";

	BOOL bFind = ff.FindFile(strDir);
	while ( bFind )
	{
		bFind = ff.FindNextFile();
		if ( ff.IsDots() )
			continue;
		strFile = ff.GetFilePath();
		if ( ff.IsDirectory() )
			Utility::DeleteDir(strFile);
		else
		{
			if ( ff.IsReadOnly() )
				SetFileAttributes(strFile, GetFileAttributes(strFile) & (~FILE_ATTRIBUTE_READONLY) );
			DeleteFile(strFile);
		}
	}
	ff.Close();
	RemoveDirectory(strDelDir);

}

long Utility::GetDiskSpace(LPCTSTR lpszDisk)
{
	CString szdisk = lpszDisk; //取根目录
	ULARGE_INTEGER ullFreeBytesAvailable,ullTotalNumberOfBytes,ullTotalNumberOfFreeBytes;
	ullFreeBytesAvailable.QuadPart = 0;
	ullTotalNumberOfBytes.QuadPart = 0;
	ullTotalNumberOfFreeBytes.QuadPart = 0;
	BOOL bret = GetDiskFreeSpaceEx(
		szdisk,      // directory name
		&ullFreeBytesAvailable,    // bytes available to caller
		&ullTotalNumberOfBytes,    // bytes on disk
		&ullTotalNumberOfFreeBytes // free bytes on disk
		);
	if(bret)
	{
		ULONGLONG lspace = ullFreeBytesAvailable.QuadPart / 1048576;
		return (long)lspace;
	}
	return 0;
}

TRUNK_NAMESPACE_END