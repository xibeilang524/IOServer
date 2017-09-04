#ifndef __TPRI_CRASH_HANDLER_H__
#define __TPRI_CRASH_HANDLER_H__

#include <signal.h>
#include <exception>

#include "CCrashWatch.h"

/* This structure contains pointer to the exception handlers for a thread.*/
struct ThreadExceptionHandlers
{
	ThreadExceptionHandlers()
	{
		m_prevTerm = NULL;
		m_prevUnexp = NULL;
		m_prevSigFPE = NULL;
		m_prevSigILL = NULL;
		m_prevSigSEGV = NULL;
	}

	terminate_handler m_prevTerm;        // Previous terminate handler   
	unexpected_handler m_prevUnexp;      // Previous unexpected handler
	void (__cdecl *m_prevSigFPE)(int);   // Previous FPE handler
	void (__cdecl *m_prevSigILL)(int);   // Previous SIGILL handler
	void (__cdecl *m_prevSigSEGV)(int);  // Previous illegal storage access handler
};

const int NumCodeBytes = 16;	// Number of code bytes to record.
const int MaxStackDump = 3072;	// Maximum number of DWORDS in stack dumps.
const int StackColumns = 4;		// Number of columns in stack dump.

#define CW_CRASHDUMP_FILENAME	_T("crashdump.dmp")
#define CW_ERRORLOG_FILENAME	_T("errorlog.txt")

#define	ONEK			1024
#define	SIXTYFOURK		(64*ONEK)
#define	ONEM			(ONEK*ONEK)
#define	ONEG			(ONEK*ONEK*ONEK)

class CCrashHandler
{
public:
	CCrashHandler();
	virtual ~CCrashHandler();

	BOOL IsInitialized() {
		return m_bInitialized;
	}
	void CrashLock(BOOL bLock)
	{
		if(bLock)
			m_csCrashLock.Lock();
		else
			m_csCrashLock.Unlock();
	}
	LPCTSTR GetAppName() {
		return m_szAppName;
	}
	LPCTSTR GetAppVersion() {
		return m_szAppVersion;
	}
	LPCTSTR GetModuleName() {
		return m_szModuleName;
	}
	LPCTSTR GetModulePath() {
		return m_szModulePath;
	}
	LPCTSTR GetDebugHelpDll() {
		return m_szDebugHelpDll;
	}
	BOOL IsAppRestart() {
		return m_bAppRestart;
	}
	void SetRestart(BOOL bRestart, LPCTSTR lpszCmdLine);

	BOOL Init(LPCRASHWATCHCONFIG lpCrashConfig);
public:
	static CCrashHandler *GetCurrentProcessCrashHandler();
	static void ReleaseCurrentProcessCrashHandler();

	BOOL Destroy();

	// Generates error report
	int GenerateErrorReport(__in_opt LPCWEXCEPTIONINFO pExceptionInfo = NULL);

	// Sets/unsets exception handlers for the current process
	int SetProcessExceptionHandlers(DWORD dwFlags);
	int UnSetProcessExceptionHandlers();

	// Sets/unsets exception handlers for the caller thread
	int SetThreadExceptionHandlers(DWORD dwFlags);   
	int UnSetThreadExceptionHandlers();

protected:
	static BOOL CALLBACK OnMiniDumpCallback(PVOID CallbackParam, PMINIDUMP_CALLBACK_INPUT CallbackInput,
		PMINIDUMP_CALLBACK_OUTPUT CallbackOutput); 
	BOOL OnMinidumpProgress(const PMINIDUMP_CALLBACK_INPUT CallbackInput,
		PMINIDUMP_CALLBACK_OUTPUT CallbackOutput);
	BOOL CreateMiniDump(LPCTSTR lpszPath, HANDLE hProcess, DWORD dwProcessId, DWORD dwThreadId, PEXCEPTION_POINTERS pExceptionPointers);

	bool DumpModuleInfo(HANDLE LogFile, HINSTANCE ModuleHandle, int nModuleNo);
	void DumpModuleList(HANDLE LogFile);
	void DumpSystemInformation(HANDLE LogFile);
	void DumpStack(HANDLE LogFile, DWORD *pStack);
	void DumpRegisters(HANDLE LogFile, PCONTEXT Context);
	int DumpExceptionInfo(LPCTSTR lpszPath, PEXCEPTION_POINTERS pExceptPtrs);
	int TakeDesktopScreenshot(LPCTSTR lpszPath, DWORD dwProcessId);

	int ZipFiles(LPCTSTR lpszPath, int nScreenshotCount);

	int RestartApp(LPCTSTR lpszCmdLineParams, BOOL bWait, HANDLE *phProcess);

	void InitPrevExceptionHandlerPointers();

	void GetExceptionPointers(DWORD dwExceptionCode, EXCEPTION_POINTERS** ppExceptionPointers);

	/* Exception handler functions. */

	static LONG WINAPI SehHandler(__in PEXCEPTION_POINTERS pExceptionPtrs);
	static void __cdecl TerminateHandler();
	static void __cdecl UnexpectedHandler();

#if _MSC_VER>=1300
	static void __cdecl PureCallHandler();
#endif 

#if _MSC_VER>=1300 && _MSC_VER<1400
	static void __cdecl SecurityHandler(int code, void *x);
#endif

#if _MSC_VER>=1400
	static void __cdecl InvalidParameterHandler(const wchar_t* expression, 
		const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t pReserved);
#endif

#if _MSC_VER>=1300
	static int __cdecl NewHandler(size_t);
#endif

	static void SigabrtHandler(int);
	static void SigfpeHandler(int /*code*/, int subcode);
	static void SigintHandler(int);
	static void SigillHandler(int);
	static void SigsegvHandler(int);
	static void SigtermHandler(int);

private:
	/// 是否已经初始化了？
	BOOL m_bInitialized;
	HANDLE m_hEvent;
	MINIDUMP_TYPE m_MinidumpType;
	CString m_szAppName;
	CString m_szAppVersion;
	CString m_szModulePath;
	CString m_szModuleName;
	CString m_szRestartCmdLine;
	CString m_szDebugHelpDll;
	CString m_szCrashGUID;
	CString m_szCrashSavePath;
	int		m_nJpegQuality;
	BOOL	m_bAppRestart;
	CCriticalSection m_csCrashLock;        // Critical section used to synchronize thread access to this object. 

	LPTOP_LEVEL_EXCEPTION_FILTER  m_oldSehHandler;  // previous SEH exception filter.

#if _MSC_VER>=1300
	_purecall_handler m_prevPurec;   // Previous pure virtual call exception filter.
	_PNH m_prevNewHandler; // Previous new operator exception filter.
#endif

#if _MSC_VER>=1400
	_invalid_parameter_handler m_prevInvpar; // Previous invalid parameter exception filter.
#endif

#if _MSC_VER>=1300 && _MSC_VER<1400
	_secerr_handler_func m_prevSec; // Previous security exception filter.
#endif

	void (__cdecl *m_prevSigABRT)(int); // Previous SIGABRT handler.  
	void (__cdecl *m_prevSigINT)(int);  // Previous SIGINT handler.
	void (__cdecl *m_prevSigTERM)(int); // Previous SIGTERM handler.

	// List of exception handlers installed for worker threads of current process.
	std::map<DWORD, ThreadExceptionHandlers> m_ThreadExceptionHandlers;
	CCriticalSection m_csThreadExceptionHandlers; // Synchronization lock for m_ThreadExceptionHandlers.


	static CCrashHandler* m_pProcessCrashHandler; // CCrashHandler的单实例对象指针
};

#endif