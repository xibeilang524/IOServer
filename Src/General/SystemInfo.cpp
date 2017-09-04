// SystemInfo.cpp: implementation of the CSystemInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GeneralAPI.h"
#include "General\SystemInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

TRUNK_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSystemInfo::CSystemInfo()
{
#ifdef NO_SDK_SUPPORT
	// 加载 psapi.dll 库
	GetProcessMemoryInfo = NULL;
	m_hPsapiLibHandle = ::LoadLibrary(_T("psapi.dll"));
	if (NULL != m_hPsapiLibHandle)
	{
		GetProcessMemoryInfo  = (PGetProcessMemoryInfo)::GetProcAddress(m_hPsapiLibHandle,
			_T("GetProcessMemoryInfo"));
	}
#endif

	NtQuerySystemInformation = (PNtQuerySystemInformation)GetProcAddress( GetModuleHandle( "ntdll.dll" ), "NtQuerySystemInformation" );
	
	m_dblCPUUsage = 0;
	memset(&m_processinfo, 0, sizeof(PPROCESSINFO));
	GetHostIpAndName();
	//UpdateMemoryCounter();
}

CSystemInfo::~CSystemInfo()
{
#ifdef NO_SDK_SUPPORT
	if (NULL != m_hPsapiLibHandle)
		::FreeLibrary(m_hPsapiLibHandle);
	m_hPsapiLibHandle = NULL;
#endif
}

void CSystemInfo::UpdateMemoryCounter()
{
	memset(&m_pmc, 0, sizeof(PROCESS_MEMORY_COUNTERS));
	m_pmc.cb = sizeof(PROCESS_MEMORY_COUNTERS); 

#ifdef NO_SDK_SUPPORT
	if (NULL == GetProcessMemoryInfo)
		return;
#endif	
	
	GetProcessMemoryInfo(::GetCurrentProcess(), &m_pmc, m_pmc.cb);
}

BOOL CSystemInfo::UpdateProcessInfo(DWORD dwPID)
{
	if (NULL == NtQuerySystemInformation)
		return FALSE;
	
	BOOL bOK = FALSE;
	PVOID pProcInfo = NULL;
	DWORD dwInfoSize = 0;
	PPROCESSINFO pProcessInfo;
	DWORD dwWorkingSet;
	
	static __int64 LastTotalProcessCPUUsage = 0;
	static __int64 LastCurrentProcessCPUUsage = 0;
	__int64 CurrentDelta;
	__int64 TotalDelta;
	__int64 TotalProcessCPUUsage = 0;
	__int64 CurrentProcessCPUUsage = 0;

	try
	{
		long lStatus = NtQuerySystemInformation(SystemProcessInformation, NULL, 0, &dwInfoSize); // 求需要的内存空间
		if (STATUS_INFO_LENGTH_MISMATCH != lStatus)
			return FALSE;
		if (dwInfoSize < sizeof(PROCESSINFO))
			return FALSE;
		pProcInfo = LocalAlloc(LPTR, dwInfoSize);
		if (pProcInfo == NULL)
			return FALSE;
		lStatus = NtQuerySystemInformation(SystemProcessInformation, pProcInfo, dwInfoSize, 0);
		if (!NT_SUCCESS(lStatus))
			return FALSE;

		pProcessInfo = (PPROCESSINFO)pProcInfo;
		PPROCESSINFO pHead = pProcessInfo;

		do
		{
			// 现场运行中pProcessInfo有可能为空
			if (pProcessInfo == NULL)
				break;

			// 内存溢出检测
			if (pProcessInfo - pHead + sizeof(PROCESSINFO) > dwInfoSize)
				break;

			TotalProcessCPUUsage += (__int64)pProcessInfo->KernelTime.QuadPart;
			TotalProcessCPUUsage += (__int64)pProcessInfo->UserTime.QuadPart;

			if(pProcessInfo->dwProcessID == dwPID)
			{
				dwWorkingSet = pProcessInfo->dwWorkingSet;
				CurrentProcessCPUUsage += (__int64)pProcessInfo->KernelTime.QuadPart + (__int64)pProcessInfo->UserTime.QuadPart;

				memcpy(&m_processinfo, pProcessInfo, sizeof(PROCESSINFO)); // 保留指定进程信息
				bOK = TRUE;
			}
			/////////
			if(pProcessInfo->dwOffset == 0)
			{
				break;
			}
			pProcessInfo = (PPROCESSINFO)((byte*)pProcessInfo + pProcessInfo->dwOffset);
		}
		while(true);

		TotalDelta = TotalProcessCPUUsage - LastTotalProcessCPUUsage;
		CurrentDelta = CurrentProcessCPUUsage - LastCurrentProcessCPUUsage;
		if(TotalDelta != 0)
			m_dblCPUUsage = (double)100.0 * CurrentDelta / TotalDelta;
		LastTotalProcessCPUUsage = TotalProcessCPUUsage;
		LastCurrentProcessCPUUsage = CurrentProcessCPUUsage;
		LocalFree(pProcInfo);

		m_dblCPUUsage = (m_dblCPUUsage < 0) ? 0 : m_dblCPUUsage;

		return bOK;
	}
	catch (...)
	{
		return FALSE;
	}
}

// nDecimal==-1表示不限制小数点
CString CSystemInfo::CommaFormat(double dblVal, int nDecimal/* = -1*/)
{
	CString szt;
	if (nDecimal == -1)
		szt.Format(_T("%f"), dblVal);
	else {
		CString szFormat;
		szFormat.Format(_T("0%d"), nDecimal);
		szt.Format(szFormat, dblVal);
	}

	return CommaFormat(szt);
}

CString CSystemInfo::CommaFormat(long lVal)
{
	CString szt;
	szt.Format(_T("%d"), lVal);
	return CommaFormat(szt);
}

CString CSystemInfo::CommaFormat(DWORD dwVal)
{
	CString szt;
	szt.Format(_T("%u"), dwVal);
	return CommaFormat(szt);
}

CString CSystemInfo::CommaFormat(LPCTSTR lpszStr)
{
	CString szt = lpszStr;
	int nPos = szt.Find(_T('.')); // 找小数点
	if (nPos == -1)
		nPos = szt.GetLength() - 1;
	else
		nPos--;
	while(!isdigit(*(lpszStr + nPos))) nPos--; // 跳过后面非数字字符
	while(nPos > 2) {
		szt.Insert(nPos - 2, _T(','));
		nPos -= 3;
	}
	
	return szt;
}

CString CSystemInfo::FormatMemory(DWORD dwMemory)
{
	CString szt;
	double dblMemory = (double)dwMemory;
	
	if (dblMemory < 1024.0)
	{
		szt.Format("%u Bytes", dwMemory);
		return CommaFormat(szt);
	}
	
	dblMemory /= 1024; // KB
	if (dblMemory < 1024.0)
	{
		szt.Format("%.2f KB", dblMemory);
		return CommaFormat(szt);
	}
	
	dblMemory /= 1024; // MB
	if (dblMemory < 1024.0)
	{
		szt.Format("%.2f MB", dblMemory);
		return CommaFormat(szt);
	}
	
	dblMemory /= 1024; // GB
	if (dblMemory < 1024.0)
	{
		szt.Format("%.2f GB", dblMemory);
		return CommaFormat(szt);
	}
	
	return "";
}

CString CSystemInfo::FormatMemoryKB(DWORD dwMemory)
{
	CString szt;
	double dblMemory = (double)dwMemory;
		
	szt.Format("%.2f KB", dblMemory / 1024);
	return CommaFormat(szt);
}

void CSystemInfo::GetHostIpAndName()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    char szHostName[255];
    PHOSTENT hostinfo;
	CString szIp;
	int j;

	m_arrHostIpAddress.RemoveAll();
	
    wVersionRequested = MAKEWORD( 2, 0 );
    if ( WSAStartup( wVersionRequested, &wsaData ) == 0 )
    {   
		if ( gethostname( szHostName, sizeof(szHostName)) == 0)
		{
			m_szHostName = szHostName;
			
			if ((hostinfo = gethostbyname(szHostName)) != NULL)
			{
				for (int i = 0; (hostinfo != NULL) && (hostinfo->h_addr_list[i] != NULL); i++)
				{
					for( j = 0; j < hostinfo->h_length; j++ )
					{
						CString addr;
						if( j > 0 )
							szIp += _T(".");
						addr.Format("%u", (unsigned int)((unsigned char*)hostinfo->h_addr_list[i])[j]);
						szIp += addr;
					}

					m_arrHostIpAddress.Add( szIp );
					szIp.Empty();
				}    
			}
		}
		WSACleanup( );
	} 
}

TRUNK_NAMESPACE_END