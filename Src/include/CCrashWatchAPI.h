#ifndef __KIMI_CRASH_WATCH_API_H__
#define __KIMI_CRASH_WATCH_API_H__
#pragma once
#include <windows.h>

#ifndef _DBGHELP_

typedef enum _MINIDUMP_TYPE {
MiniDumpNormal                         = 0x00000000,
MiniDumpWithDataSegs                   = 0x00000001,
MiniDumpWithFullMemory                 = 0x00000002,
MiniDumpWithHandleData                 = 0x00000004,
MiniDumpFilterMemory                   = 0x00000008,
MiniDumpScanMemory                     = 0x00000010,
MiniDumpWithUnloadedModules            = 0x00000020,
MiniDumpWithIndirectlyReferencedMemory = 0x00000040,
MiniDumpFilterModulePaths              = 0x00000080,
MiniDumpWithProcessThreadData          = 0x00000100,
MiniDumpWithPrivateReadWriteMemory     = 0x00000200,
MiniDumpWithoutOptionalData            = 0x00000400,
MiniDumpWithFullMemoryInfo             = 0x00000800,
MiniDumpWithThreadInfo                 = 0x00001000,
MiniDumpWithCodeSegs                   = 0x00002000,
MiniDumpWithoutAuxiliaryState          = 0x00004000,
MiniDumpWithFullAuxiliaryState         = 0x00008000,
MiniDumpWithPrivateWriteCopyMemory     = 0x00010000,
MiniDumpIgnoreInaccessibleMemory       = 0x00020000,
MiniDumpWithTokenInformation           = 0x00040000,
MiniDumpValidTypeFlags                 = 0x0007ffff,
} MINIDUMP_TYPE;

#endif

// Define SAL macros to be empty if some old Visual Studio used
#ifndef __reserved
#define __reserved
#endif
#ifndef __in
#define __in
#endif
#ifndef __in_opt
#define __in_opt
#endif
#ifndef __out_ecount_z
#define __out_ecount_z(x)
#endif

#ifdef __cplusplus
#define CRASHWATCH_EXTERNC extern "C"
#else
#define CRASHWATCH_EXTERNC
#endif

#define CCRASHWATCH_API(rettype) CRASHWATCH_EXTERNC rettype WINAPI

// Flags for CRASHWATCHCONFIG::dwFlags
#define CW_STRUCTURED_EXCEPTION_HANDLER   0x1    //!< Install SEH handler (deprecated name, use \ref CR_INST_SEH_EXCEPTION_HANDLER instead).
#define CW_SEH_EXCEPTION_HANDLER          0x1    //!< Install SEH handler.
#define CW_TERMINATE_HANDLER              0x2    //!< Install terminate handler.
#define CW_UNEXPECTED_HANDLER             0x4    //!< Install unexpected handler.
#define CW_PURE_CALL_HANDLER              0x8    //!< Install pure call handler (VS .NET and later).
#define CW_NEW_OPERATOR_ERROR_HANDLER     0x10   //!< Install new operator error handler (VS .NET and later).
#define CW_SECURITY_ERROR_HANDLER         0x20   //!< Install security errror handler (VS .NET and later).
#define CW_INVALID_PARAMETER_HANDLER      0x40   //!< Install invalid parameter handler (VS 2005 and later).
#define CW_SIGABRT_HANDLER                0x80   //!< Install SIGABRT signal handler.
#define CW_SIGFPE_HANDLER                 0x100  //!< Install SIGFPE signal handler.   
#define CW_SIGILL_HANDLER                 0x200  //!< Install SIGILL signal handler.  
#define CW_SIGINT_HANDLER                 0x400  //!< Install SIGINT signal handler.  
#define CW_SIGSEGV_HANDLER                0x800  //!< Install SIGSEGV signal handler.
#define CW_SIGTERM_HANDLER                0x1000 //!< Install SIGTERM signal handler.  

#define CW_ALL_EXCEPTION_HANDLERS         0       //!< Install all possible exception handlers.
#define CW_CRT_EXCEPTION_HANDLERS         0x1FFE  //!< Install exception handlers for the linked CRT module.

// 发生Crash时，通过回调函数向主程序报告异常发生日志
typedef void (WINAPI *PCrashReportCallback)(LPCTSTR lpszMessage);

typedef struct tagCRASHWATCHCONFIG
{
	WORD cb;                        // 该结构体的字节大小，必须在使用前初始化
	LPCTSTR pszAppName;             // 应用程序名称
	LPCTSTR pszAppVersion;          // 应用程序版本号
	LPCTSTR pszCrashSavePath;		// 异常报告存储路径
	LPCTSTR pszDebugHelpDLL;        // dbghelp.dll文件全路径，如果为NULL，则默认为当前路径
	MINIDUMP_TYPE uMiniDumpType;    // Minidump type.
	BOOL	bAppRestart;			// 是否允许重启
	LPCTSTR pszRestartCmdLine;      // 重启命令行
	DWORD	dwFlags;				// 异常标志
	DWORD	dwDelaySeconds;			// 窗口显示延时时间（单位为秒），默认为5秒
	PCrashReportCallback pCrashReportFunc;
}CRASHWATCHCONFIG, *LPCRASHWATCHCONFIG;

// Exception types
#define CW_WIN32_STRUCTURED_EXCEPTION   0    //!< SEH exception (deprecated name, use \ref CR_SEH_EXCEPTION instead).
#define CW_SEH_EXCEPTION                0    //!< SEH exception.
#define CW_CPP_TERMINATE_CALL           1    //!< C++ terminate() call.
#define CW_CPP_UNEXPECTED_CALL          2    //!< C++ unexpected() call.
#define CW_CPP_PURE_CALL                3    //!< C++ pure virtual function call (VS .NET and later).
#define CW_CPP_NEW_OPERATOR_ERROR       4    //!< C++ new operator fault (VS .NET and later).
#define CW_CPP_SECURITY_ERROR           5    //!< Buffer overrun error (VS .NET only).
#define CW_CPP_INVALID_PARAMETER        6    //!< Invalid parameter exception (VS 2005 and later).
#define CW_CPP_SIGABRT                  7    //!< C++ SIGABRT signal (abort).
#define CW_CPP_SIGFPE                   8    //!< C++ SIGFPE signal (flotating point exception).
#define CW_CPP_SIGILL                   9    //!< C++ SIGILL signal (illegal instruction).
#define CW_CPP_SIGINT                   10   //!< C++ SIGINT signal (CTRL+C).
#define CW_CPP_SIGSEGV                  11   //!< C++ SIGSEGV signal (invalid storage access).
#define CW_CPP_SIGTERM                  12   //!< C++ SIGTERM signal (termination request).

typedef struct tagCWEXCEPTIONINFO
{
	WORD cb;                   //!< Size of this structure in bytes; should be initialized before using.
	PEXCEPTION_POINTERS pexcptrs; //!< Exception pointers.
	int exctype;               //!< Exception type.
	DWORD code;                //!< Code of SEH exception.
	unsigned int fpe_subcode;  //!< Floating point exception subcode.
	const wchar_t* expression; //!< Assertion expression.
	const wchar_t* function;   //!< Function in which assertion happened.
	const wchar_t* file;       //!< File in which assertion happened.
	unsigned int line;         //!< Line number.
	BOOL bManual;              //!< Flag telling if the error report is generated manually or not.
	HANDLE hSenderProcess;     //!< Handle to the CrashSender.exe process.
}CWEXCEPTIONINFO, *LPCWEXCEPTIONINFO;


CCRASHWATCH_API(int) cwInstall(LPCRASHWATCHCONFIG lpCrashConfig);

CCRASHWATCH_API(int) cwUninstall();

CCRASHWATCH_API(int) cwInstallThread(DWORD dwFlags);

CCRASHWATCH_API(int) cwUninstallThread();

CCRASHWATCH_API(int) cwSetErrorMsg(LPCTSTR lpszErrorMsg);

CCRASHWATCH_API(int) cwGetLastErrorMsg(LPTSTR lpszBuffer, UINT uBufferSize);

CCRASHWATCH_API(int) cwGenerateErrorReport(LPCWEXCEPTIONINFO pExceptionInfo);

CCRASHWATCH_API(int) cwExceptionFilter(unsigned int code, __in_opt struct _EXCEPTION_POINTERS* ep);

#define CW_NONCONTINUABLE_EXCEPTION  32  //!< Non continuable sofware exception. 
#define CW_THROW                     33  //!< Throw C++ typed exception.

CCRASHWATCH_API(int) cwEmulateCrash(unsigned int ExceptionType) throw (...);

CCRASHWATCH_API(int) cwSetRestart(BOOL bRestart, LPCTSTR lpszCmdLine);

/// 异常监控器助手
class CCrashWatchHelper
{
public:
	// 安装异常监控器
	CCrashWatchHelper(__in LPCRASHWATCHCONFIG pInfo = NULL)
	{
		m_nInstallStatus = -1;
		if (pInfo)
			m_nInstallStatus = cwInstall(pInfo);

#ifdef _USE_CONSOLE_MODE
		CCrashWatchHelper::m_pCrashWatchHelper = this;
		// 注册控制台事件处理器
		::SetConsoleCtrlHandler( (PHANDLER_ROUTINE)OnConsoleHandler, TRUE );
#endif
	}

	// 卸载异常监控器
	~CCrashWatchHelper()
	{
		Uninit();
	}

	int Init(__in LPCRASHWATCHCONFIG pInfo)
	{
		m_nInstallStatus = cwInstall(pInfo);
		return m_nInstallStatus;
	}

	void Uninit() 
	{
		if (m_nInstallStatus == 0)
		{
			cwUninstall();
			m_nInstallStatus = -1;
		}
	}

	int GetInstallStatus() {
		return m_nInstallStatus;
	}

#ifdef _USE_CONSOLE_MODE
	static BOOL OnConsoleHandler( DWORD fdwCtrlType ) 
	{ 
		switch( fdwCtrlType ) 
		{ 
		case CTRL_C_EVENT:			// 用户按下[Ctrl][C]
		case CTRL_CLOSE_EVENT:		// 用户试图关闭控制台窗口
			ConsoleCrashWatchExit();
			return TRUE; 

		case CTRL_BREAK_EVENT:		// 用户按下[Ctrl][Break]
		case CTRL_LOGOFF_EVENT:		// 用户试图从系统注销
		case CTRL_SHUTDOWN_EVENT:	// 用户试图关闭计算机
			return FALSE; 

		default: 
			return FALSE; 
		} 
	}
	static void ConsoleCrashWatchExit()
	{
		if (CCrashWatchHelper::m_pCrashWatchHelper)
			CCrashWatchHelper::m_pCrashWatchHelper->Uninit();
	}
#endif
private:
	int m_nInstallStatus;

#ifdef _USE_CONSOLE_MODE
	static CCrashWatchHelper *m_pCrashWatchHelper;
#endif
};

#ifdef _USE_CONSOLE_MODE
CCrashWatchHelper* CCrashWatchHelper::m_pCrashWatchHelper = NULL;
#endif

/// 线程异常监控器助手
class CCrashWatchThreadHelper
{
public:
	//! Installs exception handlers to the caller thread
	CCrashWatchThreadHelper(DWORD dwFlags = 0)
	{
		m_nInstallStatus = cwInstallThread(dwFlags);
	}

	//! Uninstalls exception handlers from the caller thread
	~CCrashWatchThreadHelper()
	{
		Uninit();
	}

	void Uninit() 
	{
		if (m_nInstallStatus == 0)
		{
			cwUninstall();
			m_nInstallStatus = -1;
		}
	}

	int GetInstallStatus() {
		return m_nInstallStatus;
	}

private:
	int m_nInstallStatus;
};

// 链接动态库声明，如果是DEBUG模式，则链接TPRICrashWatchD.lib，Release模式则链接TPRICrashWatch.lib
#ifndef CRASHWATCH_EXPORTS
#	ifdef _UNICODE
#		ifdef CRASHWATCH_USE_MFC
#			ifdef _DEBUG
#				pragma comment( lib, "CrashWatchMFCUD.lib")
#			else
#				pragma comment( lib, "CrashWatchMFCU.lib")
#			endif
#		else
#			ifdef _DEBUG
#				pragma comment( lib, "CrashWatchUD.lib")
#			else
#				pragma comment( lib, "CrashWatchU.lib")
#			endif
#		endif
#	else
#		ifdef CRASHWATCH_USE_MFC
#			ifdef _DEBUG
#				pragma comment( lib, "CrashWatchMFCD.lib")
#			else
#				pragma comment( lib, "CrashWatchMFC.lib")
#			endif
#		else
#			ifdef _DEBUG
#				pragma comment( lib, "CrashWatchD.lib")
#			else
#				pragma comment( lib, "CrashWatch.lib")
#			endif
#		endif
#	endif
#endif

#endif // end of __KIMI_CRASH_WATCH_API_H__