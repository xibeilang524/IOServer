#include "stdafx.h"
#include "Utility.h"

CString Utility::getAppName()
{
	TCHAR szFileName[_MAX_PATH];
	GetModuleFileName(NULL, szFileName, _MAX_FNAME);

	CString sAppName; // Extract from last '\' to '.'
   sAppName = szFileName;
   sAppName = sAppName.Mid(sAppName.ReverseFind(_T('\\')) + 1)
                      .SpanExcluding(_T("."));

   return sAppName;
}

CString Utility::GetModuleName(HMODULE hModule)
{
	CString string;
	LPTSTR buf = string.GetBuffer(_MAX_PATH);
	GetModuleFileName(hModule, buf, _MAX_PATH);
	string.ReleaseBuffer();
	return string;
}

CString Utility::GetModulePath(HMODULE hModule)
{
	CString string;
	LPTSTR buf = string.GetBuffer(_MAX_PATH);
	GetModuleFileName(hModule, buf, _MAX_PATH);
	TCHAR* ptr = _tcsrchr(buf,'\\');
	if(ptr!=NULL)
		*(ptr)=0; // remove executable name
	string.ReleaseBuffer();
	
	return string;
}

int Utility::getTempDirectory(CString& strTemp)
{
	TCHAR* pszTempVar = NULL;
  
#if _MSC_VER<1400
	pszTempVar = _tgetenv(_T("TEMP"));
	strTemp = CString(pszTempVar);
#else
	size_t len = 0;
	errno_t err = _tdupenv_s(&pszTempVar, &len, _T("TEMP"));
	if(err!=0)
	{
		// Couldn't get environment variable TEMP
		return 1;
	}
	strTemp = CString(pszTempVar);
	free(pszTempVar);
#endif
	
	return 0;
}

CString Utility::getTempFileName()
{
	TCHAR szTempDir[MAX_PATH - 14]   = _T("");
	TCHAR szTempFile[MAX_PATH]       = _T("");

	if (GetTempPath(MAX_PATH - 14, szTempDir))
		GetTempFileName(szTempDir, getAppName(), 0, szTempFile);

	return szTempFile;
}

int Utility::GetSystemTimeUTC(CString& sTime)
{
	sTime.Empty();

	// Get system time in UTC format

	time_t cur_time;
	time(&cur_time);
	char szDateTime[64];
  
#if _MSC_VER<1400
	struct tm* timeinfo = gmtime(&cur_time);
	strftime(szDateTime, 64,  "%Y-%m-%dT%H:%M:%SZ", timeinfo);
#else
	struct tm timeinfo;
	gmtime_s(&timeinfo, &cur_time);
	strftime(szDateTime, 64,  "%Y-%m-%dT%H:%M:%SZ", &timeinfo);
#endif

	sTime = szDateTime;

	return 0;
}

void Utility::UTC2SystemTime(CString sUTC, SYSTEMTIME& st)
{
	CString sYear = sUTC.Mid(0, 4);
	CString sMonth = sUTC.Mid(5, 2);
	CString sDay = sUTC.Mid(8, 2);
	CString sHour = sUTC.Mid(11, 2);
	CString sMin = sUTC.Mid(14, 2);
	CString sSec = sUTC.Mid(17, 2);

	SYSTEMTIME UtcTime;
	memset(&UtcTime, 0, sizeof(SYSTEMTIME));
	UtcTime.wYear = (WORD)_ttoi(sYear);
	UtcTime.wMonth = (WORD)_ttoi(sMonth);
	UtcTime.wDay = (WORD)_ttoi(sDay);
	UtcTime.wHour = (WORD)_ttoi(sHour);
	UtcTime.wMinute = (WORD)_ttoi(sMin);
	UtcTime.wSecond = (WORD)_ttoi(sSec);

	// Convert to local time
	SystemTimeToTzSpecificLocalTime(NULL, &UtcTime, &st);
}

int Utility::GenerateGUID(CString& sGUID)
{
	int status = 1;
	sGUID.Empty();

	// Create GUID

	UCHAR *pszUuid = 0; 
	GUID *pguid = NULL;
	pguid = new GUID;
	if(pguid!=NULL)
	{
		HRESULT hr = CoCreateGuid(pguid);
		if(SUCCEEDED(hr))
		{
			// Convert the GUID to a string
			hr = UuidToStringA(pguid, &pszUuid);
			if(SUCCEEDED(hr) && pszUuid!=NULL)
			{ 
				status = 0;
				sGUID = (char*)pszUuid;
				RpcStringFreeA(&pszUuid);
			}
		}
		delete pguid; 
	}

	return status;
}

int Utility::GetOSFriendlyName(CString& sOSName)
{
	sOSName.Empty();
	CRegKey regKey;
	LONG lResult = regKey.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"), KEY_READ);
	if(lResult==ERROR_SUCCESS)
	{    
		TCHAR buf[1024];
		ULONG buf_size = 0;

		TCHAR* PRODUCT_NAME = _T("ProductName");
		TCHAR* CURRENT_BUILD_NUMBER = _T("CurrentBuildNumber");
		TCHAR* CSD_VERSION = _T("CSDVersion");

#pragma warning(disable:4996)

		buf_size = 1023;
		if(ERROR_SUCCESS == regKey.QueryValue(buf, PRODUCT_NAME, &buf_size))
		{
			sOSName += buf;
		}
    
		buf_size = 1023;
		if(ERROR_SUCCESS == regKey.QueryValue(buf, CURRENT_BUILD_NUMBER, &buf_size))
		{
			sOSName += _T(" Build ");
			sOSName += buf;
		}

		buf_size = 1023;
		if(ERROR_SUCCESS == regKey.QueryValue(buf, CSD_VERSION, &buf_size))
		{
			sOSName += _T(" ");
			sOSName += buf;
		}

#pragma warning(default:4996)

		regKey.Close(); 

		return 0;
	}

	return 1;
}

BOOL Utility::IsOS64Bit()
{
	BOOL b64Bit = FALSE;

#ifdef _WIN64
	// 64-bit applications always run under 64-bit Windows
	return TRUE;
#endif

	// Check for 32-bit applications
	typedef BOOL (WINAPI *PFNISWOW64PROCESS)(HANDLE, PBOOL);

	HMODULE hKernel32 = LoadLibrary(_T("kernel32.dll"));
	if(hKernel32!=NULL)
	{
		PFNISWOW64PROCESS pfnIsWow64Process = 
			(PFNISWOW64PROCESS)GetProcAddress(hKernel32, "IsWow64Process");
		if(pfnIsWow64Process==NULL)
		{
			// If there is no IsWow64Process() API, than Windows is 32-bit for sure
			FreeLibrary(hKernel32);
			return FALSE;
		}
	
		pfnIsWow64Process(GetCurrentProcess(), &b64Bit);
		FreeLibrary(hKernel32);
	}

	return b64Bit;
}

int Utility::GetSpecialFolder(int csidl, CString& sFolderPath)
{
	sFolderPath.Empty();

	TCHAR szPath[_MAX_PATH];
	BOOL bResult = SHGetSpecialFolderPath(NULL, szPath, csidl, TRUE);
	if(!bResult)
		return 1;

	sFolderPath = CString(szPath);

	return 0;
}

CString Utility::ReplaceInvalidCharsInFileName(CString sFileName)
{
	sFileName.Replace(_T("*"),_T("_"));
	sFileName.Replace(_T("|"),_T("_"));
	sFileName.Replace(_T("/"),_T("_"));
	sFileName.Replace(_T("?"),_T("_"));
	sFileName.Replace(_T("<"),_T("_"));
	sFileName.Replace(_T(">"),_T("_"));
	return sFileName;
}

int Utility::RecycleFile(CString sFilePath, bool bPermanentDelete)
{
	SHFILEOPSTRUCT fop;
	memset(&fop, 0, sizeof(SHFILEOPSTRUCT));

	TCHAR szFrom[_MAX_PATH];  
	memset(szFrom, 0, sizeof(TCHAR)*(_MAX_PATH));
	_TCSCPY_S(szFrom, _MAX_PATH, sFilePath.GetBuffer(0));
	szFrom[sFilePath.GetLength()+1] = 0;

	fop.fFlags |= FOF_SILENT;                // don't report progress
	fop.fFlags |= FOF_NOERRORUI;           // don't report errors
	fop.fFlags |= FOF_NOCONFIRMATION;        // don't confirm delete
	fop.wFunc = FO_DELETE;                   // REQUIRED: delete operation
	fop.pFrom = szFrom;                      // REQUIRED: which file(s)
	fop.pTo = NULL;                          // MUST be NULL
	if (bPermanentDelete) 
	{ 
		// if delete requested..
		fop.fFlags &= ~FOF_ALLOWUNDO;   // ..don't use Recycle Bin
	} 
	else 
	{                                 // otherwise..
		fop.fFlags |= FOF_ALLOWUNDO;    // ..send to Recycle Bin
	}

	return SHFileOperation(&fop); // do it!  
}

CString Utility::GetINIString(LPCTSTR pszFile, LPCTSTR pszSection, LPCTSTR pszName)
{  
	TCHAR szBuffer[1024] = _T("");  
	GetPrivateProfileString(pszSection, pszName, _T(""), szBuffer, 1024, pszFile);

	CString sResult = szBuffer;
	sResult.Replace(_T("\\n"), _T("\n"));

	return sResult;
}

void Utility::SetINIString(LPCTSTR pszFile, LPCTSTR pszSection, LPCTSTR pszName, LPCTSTR pszValue)
{
	WritePrivateProfileString(pszSection, pszName, pszValue, pszFile);
}


void Utility::SetLayoutRTL(HWND hWnd)
{
	DWORD dwExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
	dwExStyle |= WS_EX_LAYOUTRTL;
	SetWindowLong(hWnd, GWL_EXSTYLE, dwExStyle);

	SetLayout(GetDC(hWnd), LAYOUT_RTL);

	CRect rcWnd;
	::GetClientRect(hWnd, &rcWnd);

	HWND hWndChild = GetWindow(hWnd, GW_CHILD);
	while(hWndChild!=NULL)
	{    
		SetLayoutRTL(hWndChild);

		CRect rc;
		::GetWindowRect(hWndChild, &rc);    
		::MapWindowPoints(0, hWnd, (LPPOINT)&rc, 2);
		::MoveWindow(hWndChild, rcWnd.Width()-rc.right, rc.top, rc.Width(), rc.Height(), TRUE);

		SetLayout(GetDC(hWndChild), LAYOUT_RTL);

		hWndChild = GetWindow(hWndChild, GW_HWNDNEXT);
	}  
}

CString Utility::FormatErrorMsg(DWORD dwErrorCode)
{
	LPTSTR msg = 0;
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL, dwErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&msg, 0, NULL);
	CString str = msg;
	str.Replace(_T("\r\n"), _T(""));
	GlobalFree(msg);
	return str;
}

// GetBaseFileName
// This helper function returns file name without extension
CString Utility::GetFileName(LPCTSTR sPath)
{
	CString sBase = sPath;
	int pos1 = sBase.ReverseFind('\\');
	if(pos1>=0)
		sBase = sBase.Mid(pos1+1);

	return sBase;
}

// GetBaseFileName
// This helper function returns file name without extension
CString Utility::GetBaseFileName(LPCTSTR sFileName)
{
	CString sBase = sFileName;
	int pos1 = sBase.ReverseFind('\\');
	if(pos1>=0)
		sBase = sBase.Mid(pos1+1);

	int pos2 = sBase.ReverseFind('.');
	if(pos2>=0)
	{
		sBase = sBase.Mid(0, pos2);
	}
	return sBase;
}

// GetFileExtension
// This helper function returns file extension by file name
CString Utility::GetFileExtension(CString sFileName)
{
	CString sExt;
	int pos = sFileName.ReverseFind('.');
	if(pos>=0)
	{
		sExt = sFileName.Mid(pos+1);
	}
	return sExt;
}

CString Utility::GetProductVersion(CString sModuleName)
{
	CString sProductVer; 

	DWORD dwBuffSize = GetFileVersionInfoSize(sModuleName, 0);
	LPBYTE pBuff = (LPBYTE)GlobalAlloc(GPTR, dwBuffSize);  

	if(NULL!=pBuff && 0!=GetFileVersionInfo(sModuleName, 0, dwBuffSize, pBuff))
	{
		VS_FIXEDFILEINFO* fi = NULL;
		UINT uLen = 0;
		VerQueryValue(pBuff, _T("\\"), (LPVOID*)&fi, &uLen);

		WORD dwVerMajor = HIWORD(fi->dwProductVersionMS);
		WORD dwVerMinor = LOWORD(fi->dwProductVersionMS);
		WORD dwPatchLevel = HIWORD(fi->dwProductVersionLS);
		WORD dwVerBuild = LOWORD(fi->dwProductVersionLS);

		sProductVer.Format(_T("%u.%u.%u.%u"), 
		dwVerMajor, dwVerMinor, dwPatchLevel, dwVerBuild);    
	} 

	GlobalFree((HGLOBAL)pBuff);

	return sProductVer;
}

// Creates a folder. If some intermediate folders in the path do not exist,
// it creates them.
BOOL Utility::CreateFolder(CString sFolderName)
{  
	CString sIntermediateFolder;

	// Skip disc drive name "X:\" if presents
	int start = sFolderName.Find(':', 0);
	if(start>=0)
	start+=2; 

	int pos = start;  
	for(;;)
	{
		pos = sFolderName.Find('\\', pos);
		if(pos<0)
		{
			sIntermediateFolder = sFolderName;
		}
		else
		{
			sIntermediateFolder = sFolderName.Left(pos);
		}

		BOOL bCreate = CreateDirectory(sIntermediateFolder, NULL);
		if(!bCreate && GetLastError()!=ERROR_ALREADY_EXISTS)
		return FALSE;

		DWORD dwAttrs = GetFileAttributes(sIntermediateFolder);
		if((dwAttrs&FILE_ATTRIBUTE_DIRECTORY)==0)
		return FALSE;

		if(pos==-1)
		break;

		pos++;
	}

	return TRUE;
}

ULONG64 Utility::SystemTimeToULONG64( const SYSTEMTIME& st )
{
	FILETIME ft ;
	SystemTimeToFileTime( &st, &ft ) ;
	ULARGE_INTEGER integer ;
	integer.LowPart = ft.dwLowDateTime ;
	integer.HighPart = ft.dwHighDateTime ;
	return integer.QuadPart ;
}

CString Utility::FileSizeToStr(ULONG64 uFileSize)
{
	CString sFileSize;

	if(uFileSize==0)
	{
		sFileSize = _T("0 KB");
	}
	else if(uFileSize<1024)
	{
		float fSizeKbytes = (float)uFileSize/(float)1024;
		TCHAR szStr[64];
#if _MSC_VER<1400
	_stprintf(szStr, _T("%0.1f KB"), fSizeKbytes);    
#else
	_stprintf_s(szStr, 64, _T("%0.1f KB"), fSizeKbytes);    
#endif
		sFileSize = szStr;
	}
	else if(uFileSize<1024*1024)
	{
		sFileSize.Format(_T("%I64u KB"), uFileSize/1024);
	}
	else
	{
		float fSizeMbytes = (float)uFileSize/(float)(1024*1024);
		TCHAR szStr[64];
#if _MSC_VER<1400
		_stprintf(szStr, _T("%0.1f MB"), fSizeMbytes);    
#else
		_stprintf_s(szStr, 64, _T("%0.1f MB"), fSizeMbytes);    
#endif
		sFileSize = szStr;
	}

	return sFileSize;
}

#define HPRINTF_BUFFER_SIZE (8*1024)				// must be at least 2048
static TCHAR hprintf_buffer[HPRINTF_BUFFER_SIZE];	// wvsprintf never prints more than one K.
static int  hprintf_index = 0;

void Utility::FileFlush(HANDLE LogFile)
{
	if (hprintf_index > 0)
	{
		DWORD NumBytes;
		WriteFile(LogFile, hprintf_buffer, lstrlen(hprintf_buffer), &NumBytes, 0);
		hprintf_index = 0;
	}
}

void Utility::PrintFile(HANDLE LogFile, LPCTSTR Format, ...)
{
	if (hprintf_index > (HPRINTF_BUFFER_SIZE-1024))
	{
		DWORD NumBytes;
		WriteFile(LogFile, hprintf_buffer, lstrlen(hprintf_buffer), &NumBytes, 0);
		hprintf_index = 0;
	}

	va_list arglist;
	va_start( arglist, Format);
	hprintf_index += wvsprintf(&hprintf_buffer[hprintf_index], Format, arglist);
	va_end( arglist);
}

void Utility::FormatTime(LPTSTR output, FILETIME TimeToPrint)
{
	output[0] = _T('\0');
	WORD Date, Time;
	if (FileTimeToLocalFileTime(&TimeToPrint, &TimeToPrint) &&
		FileTimeToDosDateTime(&TimeToPrint, &Date, &Time))
	{
		wsprintf(output, _T("%d-%d-%d %02d:%02d:%02d"),
			Date & 31, (Date / 512) + 1980, (Date / 32) & 15, 
			(Time >> 11), (Time >> 5) & 0x3F, (Time & 0x1F) * 2);
	}
}


const TCHAR *Utility::GetExceptionDescription(DWORD ExceptionCode)
{
	struct ExceptionNames
	{
		DWORD	ExceptionCode;
		TCHAR *	ExceptionName;
	};

#if 0  // from winnt.h
#define STATUS_WAIT_0                    ((DWORD   )0x00000000L)    
#define STATUS_ABANDONED_WAIT_0          ((DWORD   )0x00000080L)    
#define STATUS_USER_APC                  ((DWORD   )0x000000C0L)    
#define STATUS_TIMEOUT                   ((DWORD   )0x00000102L)    
#define STATUS_PENDING                   ((DWORD   )0x00000103L)    
#define STATUS_SEGMENT_NOTIFICATION      ((DWORD   )0x40000005L)    
#define STATUS_GUARD_PAGE_VIOLATION      ((DWORD   )0x80000001L)    
#define STATUS_DATATYPE_MISALIGNMENT     ((DWORD   )0x80000002L)    
#define STATUS_BREAKPOINT                ((DWORD   )0x80000003L)    
#define STATUS_SINGLE_STEP               ((DWORD   )0x80000004L)    
#define STATUS_ACCESS_VIOLATION          ((DWORD   )0xC0000005L)    
#define STATUS_IN_PAGE_ERROR             ((DWORD   )0xC0000006L)    
#define STATUS_INVALID_HANDLE            ((DWORD   )0xC0000008L)    
#define STATUS_NO_MEMORY                 ((DWORD   )0xC0000017L)    
#define STATUS_ILLEGAL_INSTRUCTION       ((DWORD   )0xC000001DL)    
#define STATUS_NONCONTINUABLE_EXCEPTION  ((DWORD   )0xC0000025L)    
#define STATUS_INVALID_DISPOSITION       ((DWORD   )0xC0000026L)    
#define STATUS_ARRAY_BOUNDS_EXCEEDED     ((DWORD   )0xC000008CL)    
#define STATUS_FLOAT_DENORMAL_OPERAND    ((DWORD   )0xC000008DL)    
#define STATUS_FLOAT_DIVIDE_BY_ZERO      ((DWORD   )0xC000008EL)    
#define STATUS_FLOAT_INEXACT_RESULT      ((DWORD   )0xC000008FL)    
#define STATUS_FLOAT_INVALID_OPERATION   ((DWORD   )0xC0000090L)    
#define STATUS_FLOAT_OVERFLOW            ((DWORD   )0xC0000091L)    
#define STATUS_FLOAT_STACK_CHECK         ((DWORD   )0xC0000092L)    
#define STATUS_FLOAT_UNDERFLOW           ((DWORD   )0xC0000093L)    
#define STATUS_INTEGER_DIVIDE_BY_ZERO    ((DWORD   )0xC0000094L)    
#define STATUS_INTEGER_OVERFLOW          ((DWORD   )0xC0000095L)    
#define STATUS_PRIVILEGED_INSTRUCTION    ((DWORD   )0xC0000096L)    
#define STATUS_STACK_OVERFLOW            ((DWORD   )0xC00000FDL)    
#define STATUS_CONTROL_C_EXIT            ((DWORD   )0xC000013AL)    
#define STATUS_FLOAT_MULTIPLE_FAULTS     ((DWORD   )0xC00002B4L)    
#define STATUS_FLOAT_MULTIPLE_TRAPS      ((DWORD   )0xC00002B5L)    
#define STATUS_ILLEGAL_VLM_REFERENCE     ((DWORD   )0xC00002C0L)     
#endif

	ExceptionNames ExceptionMap[] =
	{
		{0x40010005, _T("a Control-C")},
		{0x40010008, _T("a Control-Break")},
		{0x80000002, _T("a Datatype Misalignment")},
		{0x80000003, _T("a Breakpoint")},
		{0xc0000005, _T("an Access Violation")},
		{0xc0000006, _T("an In Page Error")},
		{0xc0000017, _T("a No Memory")},
		{0xc000001d, _T("an Illegal Instruction")},
		{0xc0000025, _T("a Noncontinuable Exception")},
		{0xc0000026, _T("an Invalid Disposition")},
		{0xc000008c, _T("a Array Bounds Exceeded")},
		{0xc000008d, _T("a Float Denormal Operand")},
		{0xc000008e, _T("a Float Divide by Zero")},
		{0xc000008f, _T("a Float Inexact Result")},
		{0xc0000090, _T("a Float Invalid Operation")},
		{0xc0000091, _T("a Float Overflow")},
		{0xc0000092, _T("a Float Stack Check")},
		{0xc0000093, _T("a Float Underflow")},
		{0xc0000094, _T("an Integer Divide by Zero")},
		{0xc0000095, _T("an Integer Overflow")},
		{0xc0000096, _T("a Privileged Instruction")},
		{0xc00000fD, _T("a Stack Overflow")},
		{0xc0000142, _T("a DLL Initialization Failed")},
		{0xe06d7363, _T("a Microsoft C++ Exception")},
	};

	for (int i = 0; i < sizeof(ExceptionMap) / sizeof(ExceptionMap[0]); i++)
		if (ExceptionCode == ExceptionMap[i].ExceptionCode)
			return ExceptionMap[i].ExceptionName;

	return _T("an Unknown exception type");
}