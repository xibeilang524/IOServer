// SystemInfo.h: interface for the CSystemInfo class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __SYSTEMINFO_H__
#define __SYSTEMINFO_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

TRUNK_NAMESPACE_BEGIN

// GetProcessMemoryInfo 函数需要Windows NT 4.0以上系统支持，
// 该函数在 SDK 的 Psapi.h 中声明，编译时需要连接Psapi.lib
// 以下声明是为了在不需要 SDK 情况下编译
#ifndef PROCESS_MEMORY_COUNTERS

#define NO_SDK_SUPPORT	1

typedef struct _PROCESS_MEMORY_COUNTERS {
    DWORD cb;
    DWORD PageFaultCount;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage;
    SIZE_T QuotaPagedPoolUsage;
    SIZE_T QuotaPeakNonPagedPoolUsage;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
} PROCESS_MEMORY_COUNTERS;
typedef PROCESS_MEMORY_COUNTERS *PPROCESS_MEMORY_COUNTERS;

typedef BOOL (WINAPI *PGetProcessMemoryInfo)(
  HANDLE Process,                          // handle to process
  PPROCESS_MEMORY_COUNTERS ppsmemCounters, // buffer
  DWORD cb                                 // size of buffer
);
#else

#include <psapi.h>
#pragma comment (lib,"psapi.lib")

#endif

//#include <winternl.h>
//#include <ntstatus.h>

#define NT_SUCCESS(Status) (((long)(Status)) >= 0)
#define STATUS_INFO_LENGTH_MISMATCH      ((long)0xC0000004L)

typedef struct _THREAD_INFO
{
	LARGE_INTEGER CreateTime;
	DWORD dwUnknown1;
	DWORD dwStartAddress;
	DWORD StartEIP;
	DWORD dwOwnerPID;
	DWORD dwThreadId;
	DWORD dwCurrentPriority;
	DWORD dwBasePriority;
	DWORD dwContextSwitches;
	DWORD Unknown;
	DWORD WaitReason;
}THREADINFO, *PTHREADINFO;
typedef struct _UNICODE_STRING
{
	USHORT Length;
	USHORT MaxLength;
	PWSTR Buffer;
} UNICODE_STRING;
typedef struct _PROCESS_INFO
{
	DWORD dwOffset;
	DWORD dwThreadsCount;
	DWORD dwUnused1[6];
	LARGE_INTEGER CreateTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER KernelTime;
	UNICODE_STRING ProcessName;
	DWORD dwBasePriority;
	DWORD dwProcessID;
	DWORD dwParentProcessId;
	DWORD dwHandleCount;
	DWORD dwUnused3[2];
	DWORD dwVirtualBytesPeak;
	DWORD dwVirtualBytes;
	DWORD dwPageFaults;
	DWORD dwWorkingSetPeak;
	DWORD dwWorkingSet;
	DWORD dwQuotaPeakPagedPoolUsage;
	DWORD dwQuotaPagedPoolUsage;
	DWORD dwQuotaPeakNonPagedPoolUsage;
	DWORD dwQuotaNonPagedPoolUsage;
	DWORD dwPageFileUsage;
	DWORD dwPageFileUsagePeak;
	DWORD dCommitCharge;
	THREADINFO ThreadSysInfo[1];
} PROCESSINFO, *PPROCESSINFO;

typedef struct _SYSTEM_PERFORMANCE_INFORMATION
{
	LARGE_INTEGER	IdleTime;								// CPU空闲时间;
	LARGE_INTEGER	ReadTransferCount;						// I/O读操作数目;
	LARGE_INTEGER	WriteTransferCount;						// I/O写操作数目;
	LARGE_INTEGER	OtherTransferCount;						// I/O其他操作数目;
	DWORD			ReadOperationCount;						// I/O读数据数目;
	DWORD			WriteOperationCount;					// I/O写数据数目;
	DWORD			OtherOperationCount;					// I/O其他操作数据数目;
	DWORD			AvailablePages;							// 可获得的页数目;
	DWORD			TotalCommittedPages;					// 总共提交页数目;
	DWORD			TotalCommitLimit;						// 已提交页数目;
	DWORD			PeakCommitment;							// 页提交峰值;
	DWORD			PageFaults;								// 页故障数目;
	DWORD			WriteCopyFaults;						// Copy-On-Write故障数目;
	DWORD			TransitionFaults;						// 软页故障数目;
	DWORD			Reserved1;
	DWORD			DemandZeroFaults;						// 需求0故障数;
	DWORD			PagesRead;								// 读页数目;
	DWORD			PageReadIos;							// 读页I/O操作数;
	DWORD			Reserved2[2];
	DWORD			PagefilePagesWritten;					// 已写页文件页数;
	DWORD			PagefilePageWriteIos;					// 已写页文件操作数;
	DWORD			MappedFilePagesWritten;					// 已写映射文件页数;
	DWORD			MappedFileWriteIos;						// 已写映射文件操作数;
	DWORD			PagedPoolUsage;							// 分页池使用;
	DWORD			NonPagedPoolUsage;						// 非分页池使用;
	DWORD			PagedPoolAllocs;						// 分页池分配情况;
	DWORD			PagedPoolFrees;							// 分页池释放情况;
	DWORD			NonPagedPoolAllocs;						// 非分页池分配情况;
	DWORD			NonPagedPoolFress;						// 非分页池释放情况;
	DWORD			TotalFreeSystemPtes;					// 系统页表项释放总数;
	DWORD			SystemCodePage;							// 操作系统代码页数;
	DWORD			TotalSystemDriverPages;					// 可分页驱动程序页数;
	DWORD			TotalSystemCodePages;					// 操作系统代码页总数;
	DWORD			SmallNonPagedLookasideListAllocateHits;	// 小非分页侧视列表分配次数;
	DWORD			SmallPagedLookasideListAllocateHits;	// 小分页侧视列表分配次数;
	DWORD			Reserved3;
	DWORD			MmSystemCachePage;						// 系统缓存页数;
	DWORD			PagedPoolPage;							// 分页池页数;
	DWORD			SystemDriverPage;						// 可分页驱动页数;
	DWORD			FastReadNoWait;							// 异步快速读数目;
	DWORD			FastReadWait;							// 同步快速读数目;
	DWORD			FastReadResourceMiss;					// 快速读资源冲突数;
	DWORD			FastReadNotPossible;					// 快速读失败数;
	DWORD			FastMdlReadNoWait;						// 异步MDL快速读数目;
	DWORD			FastMdlReadWait;						// 同步MDL快速读数目;
	DWORD			FastMdlReadResourceMiss;				// MDL读资源冲突数;
	DWORD			FastMdlReadNotPossible;					// MDL读失败数;
	DWORD			MapDataNoWait;							// 异步映射数据次数;
	DWORD			MapDataWait;							// 同步映射数据次数;
	DWORD			MapDataNoWaitMiss;						// 异步映射数据冲突次数;
	DWORD			MapDataWaitMiss;						// 同步映射数据冲突次数;
	DWORD			PinMappedDataCount;						// 牵制映射数据数目;
	DWORD			PinReadNoWait;							// 牵制异步读数目;
	DWORD			PinReadWait;							// 牵制同步读数目;
	DWORD			PinReadNoWaitMiss;						// 牵制异步读冲突数目;
	DWORD			PinReadWaitMiss;						// 牵制同步读冲突数目;
	DWORD			CopyReadNoWait;							// 异步拷贝读次数;
	DWORD			CopyReadWait;							// 同步拷贝读次数;
	DWORD			CopyReadNoWaitMiss;						// 异步拷贝读故障次数;
	DWORD			CopyReadWaitMiss;						// 同步拷贝读故障次数;
	DWORD			MdlReadNoWait;							// 异步MDL读次数;
	DWORD			MdlReadWait;							// 同步MDL读次数;
	DWORD			MdlReadNoWaitMiss;						// 异步MDL读故障次数;
	DWORD			MdlReadWaitMiss;						// 同步MDL读故障次数;
	DWORD			ReadAheadIos;							// 向前读操作数目;
	DWORD			LazyWriteIos;							// LAZY写操作数目;
	DWORD			LazyWritePages;							// LAZY写页文件数目;
	DWORD			DataFlushes;							// 缓存刷新次数;
	DWORD			DataPages;								// 缓存刷新页数;
	DWORD			ContextSwitches;						// 环境切换数目;
	DWORD			FirstLevelTbFills;						// 第一层缓冲区填充次数;
	DWORD			SecondLevelTbFills;						// 第二层缓冲区填充次数;
	DWORD			SystemCall;								// 系统调用次数;
}SYSTEM_PERFORMANCE_INFORMATION,*PSYSTEM_PERFORMANCE_INFORMATION;

typedef enum _SYSTEM_INFORMATION_CLASS {
	SystemBasicInformation = 0,
	SystemPerformanceInformation = 2,
	SystemTimeOfDayInformation = 3,
	SystemProcessInformation = 5,
	SystemProcessorPerformanceInformation = 8,
	SystemInterruptInformation = 23,
	SystemExceptionInformation = 33,
	SystemRegistryQuotaInformation = 37,
	SystemLookasideInformation = 45
} SYSTEM_INFORMATION_CLASS;

typedef long ( __stdcall *PNtQuerySystemInformation )(IN     SYSTEM_INFORMATION_CLASS,
													  IN OUT PVOID,
													  IN     ULONG,
													  OUT    PULONG OPTIONAL);

class _GENERAL_CLASS CSystemInfo  
{
public:
	CSystemInfo();
	virtual ~CSystemInfo();

	void UpdateMemoryCounter();
	BOOL UpdateProcessInfo(DWORD dwPID);

public:
	PROCESS_MEMORY_COUNTERS		m_pmc;
	PROCESSINFO					m_processinfo;

	CString &GetHostName() {
		return m_szHostName;
	}
	CStringArray &GetHostIpAddress() {
		return m_arrHostIpAddress;
	}
	double GetCPUUsage() { return m_dblCPUUsage; }

	static CString CommaFormat(double dblVal, int nDecimal = -1); // nDecimal==-1表示不限制小数点
	static CString CommaFormat(long lVal);
	static CString CommaFormat(DWORD dwVal);
	static CString CommaFormat(LPCTSTR lpszStr);
	static CString FormatMemory(DWORD dwMemory);
	static CString FormatMemoryKB(DWORD dwMemory);

protected:
	double	m_dblCPUUsage;
	void GetHostIpAndName();

private:
#ifdef NO_SDK_SUPPORT
	HINSTANCE	m_hPsapiLibHandle;
	PGetProcessMemoryInfo	GetProcessMemoryInfo;
	PNtQuerySystemInformation NtQuerySystemInformation;
#endif
	CString			m_szHostName;
	CStringArray	m_arrHostIpAddress;
};

TRUNK_NAMESPACE_END

#endif // __SYSTEMINFO_H__
