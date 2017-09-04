#include "stdafx.h"
#include "CrashHandler.h"
#include "Utility.h"

#include <sys/stat.h>
#include <psapi.h>
#include <rtcapi.h>
#include <Shellapi.h>
#include "GetWinVer.h"
#include "MiniVersion.h"
#include "DlgCrashReport.h"
#include "ScreenCap.h"
#include "XZip.h"

#ifndef _AddressOfReturnAddress

// Taken from: http://msdn.microsoft.com/en-us/library/s975zw7k(VS.71).aspx
#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

// _ReturnAddress and _AddressOfReturnAddress should be prototyped before use 
EXTERNC void * _AddressOfReturnAddress(void);
EXTERNC void * _ReturnAddress(void);

#endif 

CCrashHandler *CCrashHandler::m_pProcessCrashHandler = NULL;

CCrashHandler::CCrashHandler()
{
	m_bInitialized = FALSE;  
	m_MinidumpType = MiniDumpNormal;
	m_bAppRestart = FALSE;
	m_hEvent = NULL;
	m_nJpegQuality = 95;

	InitPrevExceptionHandlerPointers();
}

CCrashHandler::~CCrashHandler()
{
	Destroy();
}

int CCrashHandler::RestartApp(LPCTSTR lpszCmdLineParams, BOOL bWait, HANDLE *phProcess)
{
	cwSetErrorMsg(_T("Success."));

	/* Create CrashSender process */

	STARTUPINFO si;
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	PROCESS_INFORMATION pi;
	memset(&pi, 0, sizeof(PROCESS_INFORMATION));    

	CString sCmdLine;
	sCmdLine.Format(_T("\"%s\" \"%s\""), m_szModuleName, lpszCmdLineParams);
	BOOL bCreateProcess = CreateProcess(m_szModuleName, sCmdLine.GetBuffer(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	if(pi.hThread)
	{
		CloseHandle(pi.hThread);
		pi.hThread = NULL;
	}
	if(!bCreateProcess)
	{
		ATLASSERT(bCreateProcess);
		cwSetErrorMsg(_T("Error restart process."));
		return 1;
	}
  
	if (bWait)
	{
		/* Wait until CrashSender finishes with making screenshot, copying files, creating minidump. */  

		WaitForSingleObject(m_hEvent, INFINITE);
	}

	// Return handle to the CrashSender.exe process.
	if(phProcess!=NULL)
	{
		*phProcess = pi.hProcess;
	}

	return 0;
}

BOOL CCrashHandler::Init(LPCRASHWATCHCONFIG lpCrashConfig)
{
	cwSetErrorMsg(_T("Unspecified error."));
	if (lpCrashConfig == NULL)
		return FALSE;

	m_MinidumpType = lpCrashConfig->uMiniDumpType;
	m_szAppName = lpCrashConfig->pszAppName;

	if(m_szAppName.IsEmpty())
	{
		m_szAppName = Utility::getAppName();
	}

	m_bAppRestart = lpCrashConfig->bAppRestart;

	// Get process image name
	m_szModuleName = Utility::GetModuleName(NULL);

	m_szAppVersion = lpCrashConfig->pszAppVersion;

	// If no app version provided, use the default (EXE product version)
	if(m_szAppVersion.IsEmpty())
	{
		m_szAppVersion = Utility::GetProductVersion(m_szModuleName);
		if(m_szAppVersion.IsEmpty())
		{
			// If product version missing, return error.      
			cwSetErrorMsg(_T("Application version is not specified."));
			return FALSE;
		}
	}

	m_szRestartCmdLine = (lpCrashConfig->pszRestartCmdLine) ? lpCrashConfig->pszRestartCmdLine : _T("");

	if (lpCrashConfig->pszDebugHelpDLL == NULL)
	{
		m_szDebugHelpDll = _T("");       
	}
	else
	{
		m_szDebugHelpDll = lpCrashConfig->pszDebugHelpDLL;
		if (m_szDebugHelpDll.Right(1) != _T('\\'))
			m_szDebugHelpDll += _T("\\");
	}  

	m_szDebugHelpDll.Append(_T("dbghelp.dll"));


	HANDLE hDbgHelpDll = LoadLibrary(m_szDebugHelpDll);    

	if (!hDbgHelpDll)
	{
		cwSetErrorMsg(_T("Couldn't load dbghelp.dll."));      
		return FALSE;
	}

	FreeLibrary((HMODULE)hDbgHelpDll);
	hDbgHelpDll = NULL;

	// Generate unique GUID for this crash report.
	if(0!=Utility::GenerateGUID(m_szCrashGUID))
	{
		ATLASSERT(0);
		cwSetErrorMsg(_T("Couldn't generate crash GUID."));
		return FALSE; 
	}

	m_szModulePath = Utility::GetModulePath(NULL);
	if (m_szModulePath.Right(1) != _T('\\'))
		m_szModulePath += _T("\\");

	// Create event that will be used to synchronize with CrashSender.exe process
	CString sEventName;
	sEventName.Format(_T("Local\\CrashWatchEvent_%s"), m_szCrashGUID);
	m_hEvent = CreateEvent(NULL, FALSE, FALSE, sEventName);
	if(m_hEvent==NULL)
	{
		ATLASSERT(m_hEvent!=NULL);
		cwSetErrorMsg(_T("Couldn't create synchronization event."));
		return FALSE; 
	}

	if (lpCrashConfig->pszCrashSavePath == NULL)
	{
		m_szCrashSavePath = m_szModulePath + _T("Crash\\");
	}
	else
	{    
		m_szCrashSavePath = lpCrashConfig->pszCrashSavePath;
	}
	if (m_szCrashSavePath.Right(1) != _T('\\'))
		m_szCrashSavePath += _T("\\");

	if (!Utility::CreateFolder(m_szCrashSavePath))
	{
		ATLASSERT(0);
		cwSetErrorMsg(_T("Couldn't create crash report directory."));
		return FALSE; 
	}

	// Set exception handlers with initial values (NULLs)
	InitPrevExceptionHandlerPointers();

	// Set exception handlers that work on per-process basis
	int nSetProcessHandlers = SetProcessExceptionHandlers(lpCrashConfig->dwFlags);   
	if (nSetProcessHandlers != 0)
	{
		ATLASSERT(nSetProcessHandlers == 0);
		cwSetErrorMsg(_T("Couldn't set C++ exception handlers for current process."));
		return 1;
	}

	// Set exception handlers that work on per-thread basis
	int nSetThreadHandlers = SetThreadExceptionHandlers(lpCrashConfig->dwFlags);
	if(nSetThreadHandlers != 0)
	{
		ATLASSERT(nSetThreadHandlers==0);
		cwSetErrorMsg(_T("Couldn't set C++ exception handlers for main execution thread."));
		return 1;
	}

	// Associate this handler with the caller process
	m_pProcessCrashHandler =  this;

	// Initialization OK.
	m_bInitialized = TRUE;
	cwSetErrorMsg(_T("Success."));

	return TRUE;
}

void CCrashHandler::SetRestart(BOOL bRestart, LPCTSTR lpszCmdLine)
{
	m_bAppRestart = bRestart;
	if (bRestart)
	{
		m_szRestartCmdLine = (lpszCmdLine) ? lpszCmdLine : _T("");
	}
}

CCrashHandler *CCrashHandler::GetCurrentProcessCrashHandler()
{
	return m_pProcessCrashHandler;
}

void CCrashHandler::ReleaseCurrentProcessCrashHandler()
{
	if (m_pProcessCrashHandler != NULL)
	{
		delete m_pProcessCrashHandler;
		m_pProcessCrashHandler = NULL;
	}
}

void CCrashHandler::GetExceptionPointers(DWORD dwExceptionCode, 
										 EXCEPTION_POINTERS** ppExceptionPointers)
{
	// The following code was taken from VC++ 8.0 CRT (invarg.c: line 104)

	EXCEPTION_RECORD ExceptionRecord;
	CONTEXT ContextRecord;
	memset(&ContextRecord, 0, sizeof(CONTEXT));

#ifdef _X86_

	__asm {
		mov dword ptr [ContextRecord.Eax], eax
			mov dword ptr [ContextRecord.Ecx], ecx
			mov dword ptr [ContextRecord.Edx], edx
			mov dword ptr [ContextRecord.Ebx], ebx
			mov dword ptr [ContextRecord.Esi], esi
			mov dword ptr [ContextRecord.Edi], edi
			mov word ptr [ContextRecord.SegSs], ss
			mov word ptr [ContextRecord.SegCs], cs
			mov word ptr [ContextRecord.SegDs], ds
			mov word ptr [ContextRecord.SegEs], es
			mov word ptr [ContextRecord.SegFs], fs
			mov word ptr [ContextRecord.SegGs], gs
			pushfd
			pop [ContextRecord.EFlags]
	}

	ContextRecord.ContextFlags = CONTEXT_CONTROL;
#pragma warning(push)
#pragma warning(disable:4311)
	ContextRecord.Eip = (ULONG)_ReturnAddress();
	ContextRecord.Esp = (ULONG)_AddressOfReturnAddress();
#pragma warning(pop)
	ContextRecord.Ebp = *((ULONG *)_AddressOfReturnAddress()-1);


#elif defined (_IA64_) || defined (_AMD64_)

	/* Need to fill up the Context in IA64 and AMD64. */
	RtlCaptureContext(&ContextRecord);

#else  /* defined (_IA64_) || defined (_AMD64_) */

	ZeroMemory(&ContextRecord, sizeof(ContextRecord));

#endif  /* defined (_IA64_) || defined (_AMD64_) */

	ZeroMemory(&ExceptionRecord, sizeof(EXCEPTION_RECORD));

	ExceptionRecord.ExceptionCode = dwExceptionCode;
	ExceptionRecord.ExceptionAddress = _ReturnAddress();

	///

	EXCEPTION_RECORD* pExceptionRecord = new EXCEPTION_RECORD;
	memcpy(pExceptionRecord, &ExceptionRecord, sizeof(EXCEPTION_RECORD));
	CONTEXT* pContextRecord = new CONTEXT;
	memcpy(pContextRecord, &ContextRecord, sizeof(CONTEXT));

	*ppExceptionPointers = new EXCEPTION_POINTERS;
	(*ppExceptionPointers)->ExceptionRecord = pExceptionRecord;
	(*ppExceptionPointers)->ContextRecord = pContextRecord;  
}

int CCrashHandler::SetProcessExceptionHandlers(DWORD dwFlags)
{
	cwSetErrorMsg(_T("Unspecified error."));

	// If 0 is specified as dwFlags, assume all handlers should be
	// installed
	if ((dwFlags & 0x1FF) == 0)
		dwFlags |= 0x1FFF;

	if (dwFlags & CW_STRUCTURED_EXCEPTION_HANDLER)
	{
		// Install top-level SEH handler
		m_oldSehHandler = SetUnhandledExceptionFilter(SehHandler);    
	}

	_set_error_mode(_OUT_TO_STDERR);

#if _MSC_VER>=1300
	if (dwFlags & CW_PURE_CALL_HANDLER)
	{
		// Catch pure virtual function calls.
		// Because there is one _purecall_handler for the whole process, 
		// calling this function immediately impacts all threads. The last 
		// caller on any thread sets the handler. 
		// http://msdn.microsoft.com/en-us/library/t296ys27.aspx
		m_prevPurec = _set_purecall_handler(PureCallHandler);    
	}

	if (dwFlags & CW_NEW_OPERATOR_ERROR_HANDLER)
	{
		// Catch new operator memory allocation exceptions
		_set_new_mode(1); // Force malloc() to call new handler too
		m_prevNewHandler = _set_new_handler(NewHandler);
	}
#endif

#if _MSC_VER>=1400
	if (dwFlags & CW_INVALID_PARAMETER_HANDLER)
	{
		// Catch invalid parameter exceptions.
		m_prevInvpar = _set_invalid_parameter_handler(InvalidParameterHandler); 
	}
#endif


#if _MSC_VER>=1300 && _MSC_VER<1400    
	if (dwFlags & CW_SECURITY_ERROR_HANDLER)
	{
		// Catch buffer overrun exceptions
		// The _set_security_error_handler is deprecated in VC8 C++ run time library
		m_prevSec = _set_security_error_handler(SecurityHandler);
	}
#endif

	// Set up C++ signal handlers


	if (dwFlags & CW_SIGABRT_HANDLER)
	{
#if _MSC_VER>=1400  
		_set_abort_behavior(_CALL_REPORTFAULT, _CALL_REPORTFAULT);
#endif
		// Catch an abnormal program termination
		m_prevSigABRT = signal(SIGABRT, SigabrtHandler);  
	}

	if (dwFlags & CW_SIGILL_HANDLER)
	{
		// Catch illegal instruction handler
		m_prevSigINT = signal(SIGINT, SigintHandler);     
	}

	if (dwFlags & CW_TERMINATE_HANDLER)
	{
		// Catch a termination request
		m_prevSigTERM = signal(SIGTERM, SigtermHandler);          
	}

	cwSetErrorMsg(_T("Success."));

	return 0;
}

int CCrashHandler::UnSetProcessExceptionHandlers()
{
	cwSetErrorMsg(_T("Unspecified error."));

	// Unset all previously set handlers

#if _MSC_VER>=1300
	if(m_prevPurec!=NULL)
		_set_purecall_handler(m_prevPurec);

	if(m_prevNewHandler!=NULL)
		_set_new_handler(m_prevNewHandler);
#endif

#if _MSC_VER>=1400
	if(m_prevInvpar!=NULL)
		_set_invalid_parameter_handler(m_prevInvpar);
#endif //_MSC_VER>=1400  

#if _MSC_VER>=1300 && _MSC_VER<1400    
	if(m_prevSec!=NULL)
		_set_security_error_handler(m_prevSec);
#endif //_MSC_VER<1400

	if(m_prevSigABRT!=NULL)
		signal(SIGABRT, m_prevSigABRT);  

	if(m_prevSigINT!=NULL)
		signal(SIGINT, m_prevSigINT);     

	if(m_prevSigTERM!=NULL)
		signal(SIGTERM, m_prevSigTERM);    

	cwSetErrorMsg(_T("Success."));

	return 0;
}

// Installs C++ exception handlers that function on per-thread basis
int CCrashHandler::SetThreadExceptionHandlers(DWORD dwFlags)
{
	cwSetErrorMsg(_T("Unspecified error."));

	// If 0 is specified as dwFlags, assume all available exception handlers should be
	// installed  
	if((dwFlags&0x1FFF)==0)
		dwFlags |= 0x1FFF;

	DWORD dwThreadId = GetCurrentThreadId();

	CSafeLock lock(&m_csThreadExceptionHandlers);

	std::map<DWORD, ThreadExceptionHandlers>::iterator it = 
		m_ThreadExceptionHandlers.find(dwThreadId);

	if(it!=m_ThreadExceptionHandlers.end())
	{
		// handlers are already set for the thread    
		cwSetErrorMsg(_T("Can't install handlers for current thread twice."));
		return 1; // failed
	}

	ThreadExceptionHandlers handlers;

	if (dwFlags & CW_TERMINATE_HANDLER)
	{
		// Catch terminate() calls. 
		// In a multithreaded environment, terminate functions are maintained 
		// separately for each thread. Each new thread needs to install its own 
		// terminate function. Thus, each thread is in charge of its own termination handling.
		// http://msdn.microsoft.com/en-us/library/t6fk7h29.aspx
		handlers.m_prevTerm = set_terminate(TerminateHandler);       
	}

	if (dwFlags & CW_UNEXPECTED_HANDLER)
	{
		// Catch unexpected() calls.
		// In a multithreaded environment, unexpected functions are maintained 
		// separately for each thread. Each new thread needs to install its own 
		// unexpected function. Thus, each thread is in charge of its own unexpected handling.
		// http://msdn.microsoft.com/en-us/library/h46t5b69.aspx  
		handlers.m_prevUnexp = set_unexpected(UnexpectedHandler);    
	}

	if (dwFlags & CW_SIGFPE_HANDLER)
	{
		// Catch a floating point error
		typedef void (*sigh)(int);
		handlers.m_prevSigFPE = signal(SIGFPE, (sigh)SigfpeHandler);     
	}


	if (dwFlags & CW_SIGILL_HANDLER)
	{
		// Catch an illegal instruction
		handlers.m_prevSigILL = signal(SIGILL, SigillHandler);     
	}

	if (dwFlags & CW_SIGSEGV_HANDLER)
	{
		// Catch illegal storage access errors
		handlers.m_prevSigSEGV = signal(SIGSEGV, SigsegvHandler);   
	}

	// Insert the structure to the list of handlers  
	m_ThreadExceptionHandlers[dwThreadId] = handlers;

	// OK.
	cwSetErrorMsg(_T("Success."));

	return 0;
}

int CCrashHandler::UnSetThreadExceptionHandlers()
{
	cwSetErrorMsg(_T("Unspecified error."));

	DWORD dwThreadId = ::GetCurrentThreadId();

	CSafeLock lock(&m_csThreadExceptionHandlers);

	std::map<DWORD, ThreadExceptionHandlers>::iterator it = 
		m_ThreadExceptionHandlers.find(dwThreadId);

	if (it == m_ThreadExceptionHandlers.end())
	{
		// No exception handlers were installed for the caller thread?    
		cwSetErrorMsg(_T("Crash handler wasn't previously installed for current thread."));
		return 1;
	}

	ThreadExceptionHandlers* handlers = &(it->second);

	if(handlers->m_prevTerm!=NULL)
		set_terminate(handlers->m_prevTerm);

	if(handlers->m_prevUnexp!=NULL)
		set_unexpected(handlers->m_prevUnexp);

	if(handlers->m_prevSigFPE!=NULL)
		signal(SIGFPE, handlers->m_prevSigFPE);     

	if(handlers->m_prevSigILL!=NULL)
		signal(SIGINT, handlers->m_prevSigILL);     

	if(handlers->m_prevSigSEGV!=NULL)
		signal(SIGSEGV, handlers->m_prevSigSEGV); 

	// Remove from the list
	m_ThreadExceptionHandlers.erase(it);

	// OK.
	cwSetErrorMsg(_T("Success."));
	return 0;
}

BOOL CCrashHandler::Destroy()
{
	cwSetErrorMsg(_T("Unspecified error."));

	if (!m_bInitialized)
	{
		cwSetErrorMsg(_T("Can't destroy not initialized crash handler."));
		return FALSE;
	}  

	// Reset exception callback
	if (m_oldSehHandler)
		SetUnhandledExceptionFilter(m_oldSehHandler);

	m_oldSehHandler = NULL;

	// All installed per-thread C++ exception handlers should be uninstalled 
	// using crUninstallFromCurrentThread() before calling Destroy()
	{
		CSafeLock lock(&m_csThreadExceptionHandlers);
		ATLASSERT(m_ThreadExceptionHandlers.size()==0);
	}
	m_pProcessCrashHandler = NULL;

	if (m_hEvent)
		::CloseHandle(m_hEvent);
	
	// OK.
	m_bInitialized = FALSE;
	cwSetErrorMsg(_T("Success."));
	return 0;
}

// Sets internal pointers to previously used exception handlers to NULL
void CCrashHandler::InitPrevExceptionHandlerPointers()
{
	m_oldSehHandler = NULL;

#if _MSC_VER>=1300
	m_prevPurec = NULL;
	m_prevNewHandler = NULL;
#endif

#if _MSC_VER>=1300 && _MSC_VER<1400    
	m_prevSec = NULL;
#endif

#if _MSC_VER>=1400
	m_prevInvpar = NULL;
#endif  

	m_prevSigABRT = NULL;
	m_prevSigINT = NULL;
	m_prevSigTERM = NULL;
}

// Structured exception handler
LONG WINAPI CCrashHandler::SehHandler(PEXCEPTION_POINTERS pExceptionPtrs)
{ 
	CCrashHandler* pCrashHandler = CCrashHandler::GetCurrentProcessCrashHandler();
	ATLASSERT(pCrashHandler!=NULL);  

	if(pCrashHandler!=NULL)
	{
		// Acquire lock to avoid other threads (if exist) to crash while we are 
		// inside. 
		pCrashHandler->CrashLock(TRUE);

		CWEXCEPTIONINFO ei;
		memset(&ei, 0, sizeof(CWEXCEPTIONINFO));
		ei.cb = sizeof(CWEXCEPTIONINFO);
		ei.exctype = CW_SEH_EXCEPTION;
		ei.pexcptrs = pExceptionPtrs;

		pCrashHandler->GenerateErrorReport(&ei);

		// Terminate process
		TerminateProcess(GetCurrentProcess(), 1);    
	}   

	// Unreacheable code  
	return EXCEPTION_EXECUTE_HANDLER;
}

// CRT terminate() call handler
void __cdecl CCrashHandler::TerminateHandler()
{
	// Abnormal program termination (terminate() function was called)

	CCrashHandler* pCrashHandler = CCrashHandler::GetCurrentProcessCrashHandler();
	ATLASSERT(pCrashHandler!=NULL);

	if(pCrashHandler!=NULL)
	{
		// Acquire lock to avoid other threads (if exist) to crash while we are 
		// inside. We do not unlock, because process is to be terminated.
		pCrashHandler->CrashLock(TRUE);

		// Fill in the exception info
		CWEXCEPTIONINFO ei;
		memset(&ei, 0, sizeof(CWEXCEPTIONINFO));
		ei.cb = sizeof(CWEXCEPTIONINFO);
		ei.exctype = CW_CPP_TERMINATE_CALL;

		pCrashHandler->GenerateErrorReport(&ei);

		// Terminate process
		TerminateProcess(GetCurrentProcess(), 1);    
	}    
}

// CRT unexpected() call handler
void __cdecl CCrashHandler::UnexpectedHandler()
{
	// Unexpected error (unexpected() function was called)

	CCrashHandler* pCrashHandler = CCrashHandler::GetCurrentProcessCrashHandler();
	ATLASSERT(pCrashHandler!=NULL);

	if(pCrashHandler!=NULL)
	{
		// Acquire lock to avoid other threads (if exist) to crash while we are 
		// inside. We do not unlock, because process is to be terminated.
		pCrashHandler->CrashLock(TRUE);

		// Fill in the exception info
		CWEXCEPTIONINFO ei;
		memset(&ei, 0, sizeof(CWEXCEPTIONINFO));
		ei.cb = sizeof(CWEXCEPTIONINFO);
		ei.exctype = CW_CPP_UNEXPECTED_CALL;

		pCrashHandler->GenerateErrorReport(&ei);

		// Terminate process
		TerminateProcess(GetCurrentProcess(), 1);    
	}    
}

// CRT Pure virtual method call handler
#if _MSC_VER>=1300
void __cdecl CCrashHandler::PureCallHandler()
{
	// Pure virtual function call

	CCrashHandler* pCrashHandler = CCrashHandler::GetCurrentProcessCrashHandler();
	ATLASSERT(pCrashHandler!=NULL);

	if(pCrashHandler!=NULL)
	{
		// Acquire lock to avoid other threads (if exist) to crash while we are 
		// inside. We do not unlock, because process is to be terminated.
		pCrashHandler->CrashLock(TRUE);

		// Fill in the exception info
		CWEXCEPTIONINFO ei;
		memset(&ei, 0, sizeof(CWEXCEPTIONINFO));
		ei.cb = sizeof(CWEXCEPTIONINFO);
		ei.exctype = CW_CPP_PURE_CALL;

		pCrashHandler->GenerateErrorReport(&ei);

		// Terminate process
		TerminateProcess(GetCurrentProcess(), 1);    
	}  
}
#endif

// CRT buffer overrun handler. Available in CRT 7.1 only
#if _MSC_VER>=1300 && _MSC_VER<1400
void __cdecl CCrashHandler::SecurityHandler(int code, void *x)
{
	// Security error (buffer overrun).

	code;
	x;

	CCrashHandler* pCrashHandler = CCrashHandler::GetCurrentProcessCrashHandler();
	ATLASSERT(pCrashHandler!=NULL);

	if(pCrashHandler!=NULL)
	{    
		// Acquire lock to avoid other threads (if exist) to crash while we are 
		// inside. We do not unlock, because process is to be terminated.
		pCrashHandler->CrashLock(TRUE);

		// Fill in the exception info
		CWEXCEPTIONINFO ei;
		memset(&ei, 0, sizeof(CWEXCEPTIONINFO));
		ei.cb = sizeof(CWEXCEPTIONINFO);
		ei.exctype = CW_CPP_SECURITY_ERROR;

		pCrashHandler->GenerateErrorReport(&ei);

		// Terminate process
		TerminateProcess(GetCurrentProcess(), 1);
	}
}
#endif 

// CRT invalid parameter handler
#if _MSC_VER>=1400
void __cdecl CCrashHandler::InvalidParameterHandler(
	const wchar_t* expression, 
	const wchar_t* function, 
	const wchar_t* file, 
	unsigned int line, 
	uintptr_t pReserved)
{
	pReserved;

	// Invalid parameter exception

	CCrashHandler* pCrashHandler = CCrashHandler::GetCurrentProcessCrashHandler();
	ATLASSERT(pCrashHandler!=NULL);

	if(pCrashHandler!=NULL)
	{
		// Acquire lock to avoid other threads (if exist) to crash while we are 
		// inside. We do not unlock, because process is to be terminated.
		pCrashHandler->CrashLock(TRUE);

		// Fill in the exception info
		CWEXCEPTIONINFO ei;
		memset(&ei, 0, sizeof(CWEXCEPTIONINFO));
		ei.cb = sizeof(CWEXCEPTIONINFO);
		ei.exctype = CW_CPP_INVALID_PARAMETER;
		ei.expression = expression;
		ei.function = function;
		ei.file = file;
		ei.line = line;    

		pCrashHandler->GenerateErrorReport(&ei);

		pCrashHandler->CrashLock(FALSE);

		// Terminate process
		TerminateProcess(GetCurrentProcess(), 1);    
	}   
}
#endif

// CRT new operator fault handler
#if _MSC_VER>=1300
int __cdecl CCrashHandler::NewHandler(size_t)
{
	// 'new' operator memory allocation exception

	CCrashHandler* pCrashHandler = CCrashHandler::GetCurrentProcessCrashHandler();
	ATLASSERT(pCrashHandler!=NULL);

	if(pCrashHandler!=NULL)
	{     
		// Acquire lock to avoid other threads (if exist) to crash while we are 
		// inside. We do not unlock, because process is to be terminated.
		pCrashHandler->CrashLock(TRUE);

		// Fill in the exception info
		CWEXCEPTIONINFO ei;
		memset(&ei, 0, sizeof(CWEXCEPTIONINFO));
		ei.cb = sizeof(CWEXCEPTIONINFO);
		ei.exctype = CW_CPP_NEW_OPERATOR_ERROR;
		ei.pexcptrs = NULL;    

		pCrashHandler->GenerateErrorReport(&ei);

		pCrashHandler->CrashLock(FALSE);

		// Terminate process
		TerminateProcess(GetCurrentProcess(), 1);
	}

	// Unreacheable code
	return 0;
}
#endif //_MSC_VER>=1300

// CRT SIGABRT signal handler
void CCrashHandler::SigabrtHandler(int)
{
	// Caught SIGABRT C++ signal

	CCrashHandler* pCrashHandler = CCrashHandler::GetCurrentProcessCrashHandler();
	ATLASSERT(pCrashHandler!=NULL);

	if(pCrashHandler!=NULL)
	{     
		// Acquire lock to avoid other threads (if exist) to crash while we are 
		// inside. We do not unlock, because process is to be terminated.
		pCrashHandler->CrashLock(TRUE);

		// Fill in the exception info
		CWEXCEPTIONINFO ei;
		memset(&ei, 0, sizeof(CWEXCEPTIONINFO));
		ei.cb = sizeof(CWEXCEPTIONINFO);
		ei.exctype = CW_CPP_SIGABRT;    

		pCrashHandler->GenerateErrorReport(&ei);

		// Terminate process
		TerminateProcess(GetCurrentProcess(), 1);   
	}
}

// CRT SIGFPE signal handler
void CCrashHandler::SigfpeHandler(int /*code*/, int subcode)
{
	// Floating point exception (SIGFPE)

	CCrashHandler* pCrashHandler = CCrashHandler::GetCurrentProcessCrashHandler();
	ATLASSERT(pCrashHandler!=NULL);

	if(pCrashHandler!=NULL)
	{     
		// Acquire lock to avoid other threads (if exist) to crash while we are 
		// inside. We do not unlock, because process is to be terminated.
		pCrashHandler->CrashLock(TRUE);

		// Fill in the exception info
		CWEXCEPTIONINFO ei;
		memset(&ei, 0, sizeof(CWEXCEPTIONINFO));
		ei.cb = sizeof(CWEXCEPTIONINFO);
		ei.exctype = CW_CPP_SIGFPE;
		ei.pexcptrs = (PEXCEPTION_POINTERS)_pxcptinfoptrs;
		ei.fpe_subcode = subcode;

		pCrashHandler->GenerateErrorReport(&ei);

		// Terminate process
		TerminateProcess(GetCurrentProcess(), 1);    
	}
}

// CRT sigill signal handler
void CCrashHandler::SigillHandler(int)
{
	// Illegal instruction (SIGILL)

	CCrashHandler* pCrashHandler = CCrashHandler::GetCurrentProcessCrashHandler();
	ATLASSERT(pCrashHandler!=NULL);

	if(pCrashHandler!=NULL)
	{    
		// Acquire lock to avoid other threads (if exist) to crash while we are 
		// inside. We do not unlock, because process is to be terminated.
		pCrashHandler->CrashLock(TRUE);

		// Fill in the exception info
		CWEXCEPTIONINFO ei;
		memset(&ei, 0, sizeof(CWEXCEPTIONINFO));
		ei.cb = sizeof(CWEXCEPTIONINFO);
		ei.exctype = CW_CPP_SIGILL;

		pCrashHandler->GenerateErrorReport(&ei);

		// Terminate process
		TerminateProcess(GetCurrentProcess(), 1);    
	}
}

// CRT sigint signal handler
void CCrashHandler::SigintHandler(int)
{
	// Interruption (SIGINT)

	CCrashHandler* pCrashHandler = CCrashHandler::GetCurrentProcessCrashHandler();
	ATLASSERT(pCrashHandler!=NULL);

	if(pCrashHandler!=NULL)
	{ 
		// Acquire lock to avoid other threads (if exist) to crash while we are 
		// inside. We do not unlock, because process is to be terminated.
		pCrashHandler->CrashLock(TRUE);

		// Fill in the exception info
		CWEXCEPTIONINFO ei;
		memset(&ei, 0, sizeof(CWEXCEPTIONINFO));
		ei.cb = sizeof(CWEXCEPTIONINFO);
		ei.exctype = CW_CPP_SIGINT;

		pCrashHandler->GenerateErrorReport(&ei);

		// Terminate process
		TerminateProcess(GetCurrentProcess(), 1);    
	}
}

// CRT SIGSEGV signal handler
void CCrashHandler::SigsegvHandler(int)
{
	// Invalid storage access (SIGSEGV)

	CCrashHandler* pCrashHandler = CCrashHandler::GetCurrentProcessCrashHandler();

	if(pCrashHandler!=NULL)
	{     
		// Acquire lock to avoid other threads (if exist) to crash while we are 
		// inside. We do not unlock, because process is to be terminated.
		pCrashHandler->CrashLock(TRUE);

		// Fill in exception info
		CWEXCEPTIONINFO ei;
		memset(&ei, 0, sizeof(CWEXCEPTIONINFO));
		ei.cb = sizeof(CWEXCEPTIONINFO);    
		ei.exctype = CW_CPP_SIGSEGV;
		ei.pexcptrs = (PEXCEPTION_POINTERS)_pxcptinfoptrs;

		pCrashHandler->GenerateErrorReport(&ei);

		// Terminate process
		TerminateProcess(GetCurrentProcess(), 1);    
	}
}

// CRT SIGTERM signal handler
void CCrashHandler::SigtermHandler(int)
{
	// Termination request (SIGTERM)

	CCrashHandler* pCrashHandler = CCrashHandler::GetCurrentProcessCrashHandler();
	ATLASSERT(pCrashHandler!=NULL);

	if(pCrashHandler!=NULL)
	{    
		// Acquire lock to avoid other threads (if exist) to crash while we are 
		// inside. We do not unlock, because process is to be terminated.
		pCrashHandler->CrashLock(TRUE);

		// Fill in the exception info
		CWEXCEPTIONINFO ei;
		memset(&ei, 0, sizeof(CWEXCEPTIONINFO));
		ei.cb = sizeof(CWEXCEPTIONINFO);
		ei.exctype = CW_CPP_SIGTERM;

		pCrashHandler->GenerateErrorReport(&ei);

		// Terminate process
		TerminateProcess(GetCurrentProcess(), 1);    
	}
}

BOOL CALLBACK CCrashHandler::OnMiniDumpCallback(
	PVOID CallbackParam,
	PMINIDUMP_CALLBACK_INPUT CallbackInput,
	PMINIDUMP_CALLBACK_OUTPUT CallbackOutput )
{
	CCrashHandler* pErrorReportSender = (CCrashHandler*)CallbackParam;  

	return pErrorReportSender->OnMinidumpProgress(CallbackInput, CallbackOutput);  
}

BOOL CCrashHandler::OnMinidumpProgress(const PMINIDUMP_CALLBACK_INPUT CallbackInput,
						PMINIDUMP_CALLBACK_OUTPUT CallbackOutput)
{
	switch(CallbackInput->CallbackType)
	{
	case CancelCallback: 
		{
			// This callback allows to cancel minidump generation
			/*
			if(m_Assync.IsCancelled())
			{
				CallbackOutput->Cancel = TRUE;      
				m_Assync.SetProgress(_T("Dump generation cancelled by user"), 0, true);
			}
			*/
		}
		break;

	case ModuleCallback:
		{
			// We are currently dumping some module
			/*
			CString sMsg;
			sMsg.Format(_T("Dumping info for module %s"), CallbackInput->Module.FullPath);
			m_Assync.SetProgress(sMsg, 0, true);
			*/
		}
		break;
	case ThreadCallback:
		{      
			// We are currently dumping some thread 
			/*
			CString sMsg;
			sMsg.Format(_T("Dumping info for thread 0x%X"), 
				CallbackInput->Thread.ThreadId);
			m_Assync.SetProgress(sMsg, 0, true);
			*/
		}
		break;

	}

	return TRUE;
}

BOOL CCrashHandler::CreateMiniDump(LPCTSTR lpszPath, HANDLE hProcess, DWORD dwProcessId, DWORD dwThreadId, PEXCEPTION_POINTERS pExceptionPointers)
{   
	BOOL bStatus = FALSE;
	HMODULE hDbgHelp = NULL;
	HANDLE hFile = NULL;
	MINIDUMP_EXCEPTION_INFORMATION mei;
	MINIDUMP_CALLBACK_INFORMATION mci;
	CString sMinidumpFile = lpszPath;
	sMinidumpFile.Append(CW_CRASHDUMP_FILENAME);
	
	// Load dbghelp.dll
	hDbgHelp = ::LoadLibrary(m_szDebugHelpDll);

	if (hDbgHelp == NULL)
	{
		cwSetErrorMsg(_T("dbghelp.dll couldn't be loaded."));
		goto cleanup;
	}

	// Create the minidump file
	hFile = CreateFile(
		sMinidumpFile,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		DWORD dwError = GetLastError();
		CString sMsg;    
		sMsg.Format(_T("Couldn't create minidump file: %s"), 
			Utility::FormatErrorMsg(dwError));
		cwSetErrorMsg(sMsg);

		return FALSE;
	}

	// Set valid dbghelp API version  
	typedef LPAPI_VERSION (WINAPI* LPIMAGEHLPAPIVERSIONEX)(LPAPI_VERSION AppVersion);  
	LPIMAGEHLPAPIVERSIONEX lpImagehlpApiVersionEx = 
		(LPIMAGEHLPAPIVERSIONEX)GetProcAddress(hDbgHelp, "ImagehlpApiVersionEx");
	ATLASSERT(lpImagehlpApiVersionEx!=NULL);
	if(lpImagehlpApiVersionEx!=NULL)
	{    
		API_VERSION CompiledApiVer;
		CompiledApiVer.MajorVersion = 6;
		CompiledApiVer.MinorVersion = 1;
		CompiledApiVer.Revision = 11;    
		CompiledApiVer.Reserved = 0;
		LPAPI_VERSION pActualApiVer = lpImagehlpApiVersionEx(&CompiledApiVer);    
		pActualApiVer;
		ATLASSERT(CompiledApiVer.MajorVersion==pActualApiVer->MajorVersion);
		ATLASSERT(CompiledApiVer.MinorVersion==pActualApiVer->MinorVersion);
		ATLASSERT(CompiledApiVer.Revision==pActualApiVer->Revision);    
	}

	// Write minidump to the file
	mei.ThreadId = dwThreadId;
	mei.ExceptionPointers = pExceptionPointers;
	mei.ClientPointers = TRUE;

	mci.CallbackRoutine = OnMiniDumpCallback;
	mci.CallbackParam = this;

	typedef BOOL (WINAPI *LPMINIDUMPWRITEDUMP)(
		HANDLE, 
		DWORD, 
		HANDLE, 
		MINIDUMP_TYPE, 
		CONST PMINIDUMP_EXCEPTION_INFORMATION, 
		CONST PMINIDUMP_USER_STREAM_INFORMATION, 
		CONST PMINIDUMP_CALLBACK_INFORMATION);

	LPMINIDUMPWRITEDUMP pfnMiniDumpWriteDump = 
		(LPMINIDUMPWRITEDUMP)GetProcAddress(hDbgHelp, "MiniDumpWriteDump");
	if(!pfnMiniDumpWriteDump)
	{    
		cwSetErrorMsg(_T("Bad MiniDumpWriteDump function"));
		return FALSE;
	}

	BOOL bWriteDump = pfnMiniDumpWriteDump(
		hProcess,
		dwProcessId,
		hFile,
		m_MinidumpType,
		&mei,
		NULL,
		&mci);

	if(!bWriteDump)
	{    
		CString sMsg = Utility::FormatErrorMsg(GetLastError());
		cwSetErrorMsg(sMsg);
		goto cleanup;
	}

	bStatus = TRUE;

cleanup:

	// Close file
	if(hFile)
		CloseHandle(hFile);

	// Unload dbghelp.dll
	if(hDbgHelp)
		FreeLibrary(hDbgHelp);

	return bStatus;
}

bool CCrashHandler::DumpModuleInfo(HANDLE LogFile, HINSTANCE ModuleHandle, int nModuleNo)
{
	bool rc = false;
	TCHAR szModName[MAX_PATH*2];
	ZeroMemory(szModName, sizeof(szModName));

	__try
	{
		if (GetModuleFileName(ModuleHandle, szModName, sizeof(szModName)-2) > 0)
		{
			// If GetModuleFileName returns greater than zero then this must
			// be a valid code module address. Therefore we can try to walk
			// our way through its structures to find the link time stamp.
			IMAGE_DOS_HEADER *DosHeader = (IMAGE_DOS_HEADER*)ModuleHandle;
			if (IMAGE_DOS_SIGNATURE != DosHeader->e_magic)
				return false;

			IMAGE_NT_HEADERS *NTHeader = (IMAGE_NT_HEADERS*)((TCHAR *)DosHeader
				+ DosHeader->e_lfanew);
			if (IMAGE_NT_SIGNATURE != NTHeader->Signature)
				return false;

			// open the code module file so that we can get its file date and size
			HANDLE ModuleFile = CreateFile(szModName, GENERIC_READ,
				FILE_SHARE_READ, 0, OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL, 0);

			TCHAR TimeBuffer[100];
			TimeBuffer[0] = _T('\0');

			DWORD FileSize = 0;
			if (ModuleFile != INVALID_HANDLE_VALUE)
			{
				FileSize = GetFileSize(ModuleFile, 0);
				FILETIME LastWriteTime;
				if (GetFileTime(ModuleFile, 0, 0, &LastWriteTime))
				{
					Utility::FormatTime(TimeBuffer, LastWriteTime);
				}
				CloseHandle(ModuleFile);
			}
			Utility::PrintFile(LogFile, _T("Module %d\r\n"), nModuleNo);
			Utility::PrintFile(LogFile, _T("%s\r\n"), szModName);
			Utility::PrintFile(LogFile, _T("Image Base: 0x%08x  Image Size: 0x%08x\r\n"), 
				NTHeader->OptionalHeader.ImageBase, 
				NTHeader->OptionalHeader.SizeOfImage), 

				Utility::PrintFile(LogFile, _T("Checksum:   0x%08x  Time Stamp: 0x%08x\r\n"), 
				NTHeader->OptionalHeader.CheckSum,
				NTHeader->FileHeader.TimeDateStamp);

			Utility::PrintFile(LogFile, _T("File Size:  %-10d  File Time:  %s\r\n"),
				FileSize, TimeBuffer);

			Utility::PrintFile(LogFile, _T("Version Information:\r\n"));

			CMiniVersion ver(szModName);
			TCHAR szBuf[200];
			WORD dwBuf[4];

			ver.GetCompanyName(szBuf, sizeof(szBuf)-1);
			Utility::PrintFile(LogFile, _T("     Company:    %s\r\n"), szBuf);

			ver.GetProductName(szBuf, sizeof(szBuf)-1);
			Utility::PrintFile(LogFile, _T("    Product:    %s\r\n"), szBuf);

			ver.GetFileDescription(szBuf, sizeof(szBuf)-1);
			Utility::PrintFile(LogFile, _T("    FileDesc:   %s\r\n"), szBuf);

			ver.GetFileVersion(dwBuf);
			Utility::PrintFile(LogFile, _T("    FileVer:    %d.%d.%d.%d\r\n"), 
				dwBuf[0], dwBuf[1], dwBuf[2], dwBuf[3]);

			ver.GetProductVersion(dwBuf);
			Utility::PrintFile(LogFile, _T("    ProdVer:    %d.%d.%d.%d\r\n"), 
				dwBuf[0], dwBuf[1], dwBuf[2], dwBuf[3]);

			ver.GetPrivateBuild(szBuf, sizeof(szBuf)-1);
			Utility::PrintFile(LogFile, _T("    PrivateBuild:   %s\r\n"), szBuf);

			ver.GetSpecialBuild(szBuf, sizeof(szBuf)-1);
			Utility::PrintFile(LogFile, _T("    SpecialBuild:   %s\r\n"), szBuf);

			ver.Release();

			Utility::PrintFile(LogFile, _T("\r\n"));

			rc = true;
		}
	}
	// Handle any exceptions by continuing from this point.
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}
	return rc;
}

void CCrashHandler::DumpModuleList(HANDLE LogFile)
{
	SYSTEM_INFO	SystemInfo;
	GetSystemInfo(&SystemInfo);

	const size_t PageSize = SystemInfo.dwPageSize;

	// Set NumPages to the number of pages in the 4GByte address space,
	// while being careful to avoid overflowing ints
	const size_t NumPages = 4 * size_t(ONEG / PageSize);
	size_t pageNum = 0;
	void *LastAllocationBase = 0;

	int nModuleNo = 1;

	while (pageNum < NumPages)
	{
		MEMORY_BASIC_INFORMATION MemInfo;
		if (VirtualQuery((void *)(pageNum * PageSize), &MemInfo,
			sizeof(MemInfo)))
		{
			if (MemInfo.RegionSize > 0)
			{
				// Adjust the page number to skip over this block of memory
				pageNum += MemInfo.RegionSize / PageSize;
				if (MemInfo.State == MEM_COMMIT && MemInfo.AllocationBase >
					LastAllocationBase)
				{
					// Look for new blocks of committed memory, and try
					// recording their module names - this will fail
					// gracefully if they aren't code modules
					LastAllocationBase = MemInfo.AllocationBase;
					if (DumpModuleInfo(LogFile, 
						(HINSTANCE)LastAllocationBase, 
						nModuleNo))
					{
						nModuleNo++;
					}
				}
			}
			else
				pageNum += SIXTYFOURK / PageSize;
		}
		else
			pageNum += SIXTYFOURK / PageSize;

		// If VirtualQuery fails we advance by 64K because that is the
		// granularity of address space doled out by VirtualAlloc()
	}
}

void CCrashHandler::DumpSystemInformation(HANDLE LogFile)
{
	FILETIME CurrentTime;
	GetSystemTimeAsFileTime(&CurrentTime);
	TCHAR szTimeBuffer[100];
	Utility::FormatTime(szTimeBuffer, CurrentTime);

	Utility::PrintFile(LogFile, _T("Error occurred at %s.\r\n"), szTimeBuffer);

	TCHAR szModuleName[MAX_PATH*2];
	ZeroMemory(szModuleName, sizeof(szModuleName));
	if (GetModuleFileName(0, szModuleName, _countof(szModuleName)-2) <= 0)
		lstrcpy(szModuleName, _T("Unknown"));

	TCHAR szUserName[200];
	ZeroMemory(szUserName, sizeof(szUserName));
	DWORD UserNameSize = _countof(szUserName)-2;
	if (!GetUserName(szUserName, &UserNameSize))
		lstrcpy(szUserName, _T("Unknown"));

	Utility::PrintFile(LogFile, _T("%s, run by %s.\r\n"), szModuleName, szUserName);

	// print out operating system
	TCHAR szWinVer[50], szMajorMinorBuild[50];
	int nWinVer;
	GetWinVer(szWinVer, &nWinVer, szMajorMinorBuild);
	Utility::PrintFile(LogFile, _T("Operating system:  %s (%s).\r\n"), 
		szWinVer, szMajorMinorBuild);

	SYSTEM_INFO	SystemInfo;
	GetSystemInfo(&SystemInfo);
	Utility::PrintFile(LogFile, _T("%d processor(s), type %d.\r\n"),
		SystemInfo.dwNumberOfProcessors, SystemInfo.dwProcessorType);

	MEMORYSTATUS MemInfo;
	MemInfo.dwLength = sizeof(MemInfo);
	GlobalMemoryStatus(&MemInfo);

	// Print out info on memory, rounded up.
	Utility::PrintFile(LogFile, _T("%d%% memory in use.\r\n"), MemInfo.dwMemoryLoad);
	Utility::PrintFile(LogFile, _T("%d MBytes physical memory.\r\n"), (MemInfo.dwTotalPhys +
		ONEM - 1) / ONEM);
	Utility::PrintFile(LogFile, _T("%d MBytes physical memory free.\r\n"), 
		(MemInfo.dwAvailPhys + ONEM - 1) / ONEM);
	Utility::PrintFile(LogFile, _T("%d MBytes paging file.\r\n"), (MemInfo.dwTotalPageFile +
		ONEM - 1) / ONEM);
	Utility::PrintFile(LogFile, _T("%d MBytes paging file free.\r\n"), 
		(MemInfo.dwAvailPageFile + ONEM - 1) / ONEM);
	Utility::PrintFile(LogFile, _T("%d MBytes user address space.\r\n"), 
		(MemInfo.dwTotalVirtual + ONEM - 1) / ONEM);
	Utility::PrintFile(LogFile, _T("%d MBytes user address space free.\r\n"), 
		(MemInfo.dwAvailVirtual + ONEM - 1) / ONEM);
}

///////////////////////////////////////////////////////////////////////////////
// DumpStack
void CCrashHandler::DumpStack(HANDLE LogFile, DWORD *pStack)
{
	Utility::PrintFile(LogFile, _T("\r\n\r\nStack:\r\n"));

	__try
	{
		// Esp contains the bottom of the stack, or at least the bottom of
		// the currently used area.
		DWORD* pStackTop;

		__asm
		{
			// Load the top (highest address) of the stack from the
			// thread information block. It will be found there in
			// Win9x and Windows NT.
			mov	eax, fs:[4]
			mov pStackTop, eax
		}

		if (pStackTop > pStack + MaxStackDump)
			pStackTop = pStack + MaxStackDump;

		int Count = 0;

		DWORD* pStackStart = pStack;

		int nDwordsPrinted = 0;

		while (pStack + 1 <= pStackTop)
		{
			if ((Count % StackColumns) == 0)
			{
				pStackStart = pStack;
				nDwordsPrinted = 0;
				Utility::PrintFile(LogFile, _T("0x%08x: "), pStack);
			}

			if ((++Count % StackColumns) == 0 || pStack + 2 > pStackTop)
			{
				Utility::PrintFile(LogFile, _T("%08x "), *pStack);
				nDwordsPrinted++;

				int n = nDwordsPrinted;
				while (n < 4)
				{
					Utility::PrintFile(LogFile, _T("         "));
					n++;
				}

				for (int i = 0; i < nDwordsPrinted; i++)
				{
					DWORD dwStack = *pStackStart;
					for (int j = 0; j < 4; j++)
					{
						char c = (char)(dwStack & 0xFF);
						if (c < 0x20 || c > 0x7E)
							c = '.';
#ifdef _UNICODE
						WCHAR w = (WCHAR)c;
						Utility::PrintFile(LogFile, _T("%c"), w);
#else
						Utility::PrintFile(LogFile, _T("%c"), c);
#endif
						dwStack = dwStack >> 8;
					}
					pStackStart++;
				}

				Utility::PrintFile(LogFile, _T("\r\n"));
			}
			else
			{
				Utility::PrintFile(LogFile, _T("%08x "), *pStack);
				nDwordsPrinted++;
			}
			pStack++;
		}
		Utility::PrintFile(LogFile, _T("\r\n"));
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		Utility::PrintFile(LogFile, _T("Exception encountered during stack dump.\r\n"));
	}
}

///////////////////////////////////////////////////////////////////////////////
// DumpRegisters
void CCrashHandler::DumpRegisters(HANDLE LogFile, PCONTEXT Context)
{
	// Print out the register values in an XP error window compatible format.
	Utility::PrintFile(LogFile, _T("\r\n"));
	Utility::PrintFile(LogFile, _T("Context:\r\n"));
	Utility::PrintFile(LogFile, _T("EDI:    0x%08x  ESI: 0x%08x  EAX:   0x%08x\r\n"),
		Context->Edi, Context->Esi, Context->Eax);
	Utility::PrintFile(LogFile, _T("EBX:    0x%08x  ECX: 0x%08x  EDX:   0x%08x\r\n"),
		Context->Ebx, Context->Ecx, Context->Edx);
	Utility::PrintFile(LogFile, _T("EIP:    0x%08x  EBP: 0x%08x  SegCs: 0x%08x\r\n"),
		Context->Eip, Context->Ebp, Context->SegCs);
	Utility::PrintFile(LogFile, _T("EFlags: 0x%08x  ESP: 0x%08x  SegSs: 0x%08x\r\n"),
		Context->EFlags, Context->Esp, Context->SegSs);
}

int CCrashHandler::DumpExceptionInfo(LPCTSTR lpszPath, PEXCEPTION_POINTERS pExceptPtrs)
{
	TCHAR pszFileName[MAX_PATH*2];
	lstrcpy(pszFileName, lpszPath);
	lstrcat(pszFileName, CW_ERRORLOG_FILENAME);

	HANDLE hLogFile = CreateFile(pszFileName, GENERIC_WRITE, 0, 0,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, 0);

	if (hLogFile == INVALID_HANDLE_VALUE)
	{
		cwSetErrorMsg(_T("Error creating exception report"));
		return EXCEPTION_CONTINUE_SEARCH;
	}

	// Append to the error log
	::SetFilePointer(hLogFile, 0, 0, FILE_END);


	PEXCEPTION_RECORD Exception = pExceptPtrs->ExceptionRecord;
	PCONTEXT          Context   = pExceptPtrs->ContextRecord;

	TCHAR szCrashModulePathName[MAX_PATH*2];
	ZeroMemory(szCrashModulePathName, sizeof(szCrashModulePathName));

	MEMORY_BASIC_INFORMATION MemInfo;

	// VirtualQuery can be used to get the allocation base associated with a
	// code address, which is the same as the ModuleHandle. This can be used
	// to get the filename of the module that the crash happened in.
	::VirtualQuery((void*)Context->Eip, &MemInfo, sizeof(MemInfo));

	// Print out the beginning of the error log in a Win95 error window
	// compatible format.
	Utility::PrintFile(hLogFile, _T("%s caused %s (0x%08x) \r\nin module %s at %04x:%08x.\r\n\r\n"),
		CW_ERRORLOG_FILENAME, Utility::GetExceptionDescription(Exception->ExceptionCode),
		Exception->ExceptionCode,
		m_szModuleName, Context->SegCs, Context->Eip);

	Utility::PrintFile(hLogFile, _T("Exception handler called in %s V%s.\r\n"), m_szAppName, m_szAppVersion);

	DumpSystemInformation(hLogFile);

	// If the exception was an access violation, print out some additional
	// information, to the error log and the debugger.
	if (Exception->ExceptionCode == STATUS_ACCESS_VIOLATION &&
		Exception->NumberParameters >= 2)
	{
		TCHAR szDebugMessage[1000];
		const TCHAR* readwrite = _T("Read from");
		if (Exception->ExceptionInformation[0])
			readwrite = _T("Write to");
		wsprintf(szDebugMessage, _T("%s location %08x caused an access violation.\r\n"),
			readwrite, Exception->ExceptionInformation[1]);

#ifdef	_DEBUG
		// The Visual C++ debugger doesn't actually tell you whether a read
		// or a write caused the access violation, nor does it tell what
		// address was being read or written. So I fixed that.
		OutputDebugString(_T("Exception handler: "));
		OutputDebugString(szDebugMessage);
#endif

		Utility::PrintFile(hLogFile, _T("%s"), szDebugMessage);
	}

	DumpRegisters(hLogFile, Context);

	// Print out the bytes of code at the instruction pointer. Since the
	// crash may have been caused by an instruction pointer that was bad,
	// this code needs to be wrapped in an exception handler, in case there
	// is no memory to read. If the dereferencing of code[] fails, the
	// exception handler will print '??'.
	Utility::PrintFile(hLogFile, _T("\r\nBytes at CS:EIP:\r\n"));
	BYTE * code = (BYTE *)Context->Eip;
	for (int codebyte = 0; codebyte < NumCodeBytes; codebyte++)
	{
		Utility::PrintFile(hLogFile, _T("%02x "), code[codebyte]);
	}

	// Time to print part or all of the stack to the error log. This allows
	// us to figure out the call stack, parameters, local variables, etc.

	// Esp contains the bottom of the stack, or at least the bottom of
	// the currently used area
	DWORD* pStack = (DWORD *)Context->Esp;

	DumpStack(hLogFile, pStack);

	DumpModuleList(hLogFile);

	Utility::PrintFile(hLogFile, _T("\r\n===== [end of %s] =====\r\n"), CW_ERRORLOG_FILENAME);
	Utility::FileFlush(hLogFile);
	CloseHandle(hLogFile);

	return EXCEPTION_CONTINUE_SEARCH;
}

int CCrashHandler::ZipFiles(LPCTSTR lpszPath, int nScreenshotCount)
{
	int nFiles = 0;

	CStringArray arrFileDetails;
	arrFileDetails.Add(CW_CRASHDUMP_FILENAME);
	arrFileDetails.Add(CW_ERRORLOG_FILENAME);

	int i = 0;
	for (i = 0; i < nScreenshotCount; i++)
	{
		CString jpeg;
		jpeg.Format(_T("screenshot%d.jpg"), i);
		arrFileDetails.Add(jpeg);
	}

	CString szZipFileName = lpszPath;
	CTime ft = CTime::GetCurrentTime();
	szZipFileName.Append(_T("crash_"));
	szZipFileName.Append( ft.Format(_T("%y%m%d%H%M%S")) );
	szZipFileName.Append(_T(".zip"));

	HZIP hZip = NULL;

	for (i = 0; i < arrFileDetails.GetCount(); i++)
	{
		// if this is first file, create zip now
		if (!hZip)
		{
			hZip = CreateZip((LPVOID)(LPCTSTR)szZipFileName, 0, ZIP_FILENAME);

			if (!hZip)
			{
				TRACE(_T("ERROR - failed to create %s\n"), szZipFileName);
				break;
			}
		}

		// zip is open, add file

		CString strFileName = lpszPath;
		strFileName.Append(arrFileDetails[i]);

		ZRESULT zr = ZipAdd(hZip, 
			arrFileDetails[i], 
			(LPVOID)(LPCTSTR)strFileName, 
			0, 
			ZIP_FILENAME);
		if (zr == ZR_OK)
		{
			nFiles++;
		}
		else
		{
			TRACE(_T("ERROR - failed to add '%s' to zip\n"), strFileName);
		}
	}

	if (hZip)
	{
		CloseZip(hZip);

		// 删除生成的报告文件
		for (i = 0; i < arrFileDetails.GetCount(); i++)
		{
			CString strFileName = lpszPath;
			strFileName.Append(arrFileDetails[i]);
			::DeleteFile(strFileName);
		}
	}

	return nFiles;
}

int CCrashHandler::TakeDesktopScreenshot(LPCTSTR lpszPath, DWORD dwProcessId)
{
	CScreenCapture sc;
	ScreenshotInfo ssi; 
	std::vector<CString> screenshot_names;

	int ncount = 0;

	std::vector<CRect> wnd_list;

	{
		// Take screenshot of the main window
		std::vector<WindowInfo> aWindows; 
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
		if(hProcess!=NULL)
		{
			sc.FindWindows(hProcess, FALSE, &aWindows);
			CloseHandle(hProcess);
		}
		if(aWindows.size()>0)
		{
			wnd_list.push_back(aWindows[0].m_rcWnd);
			ssi.m_aWindows.push_back(aWindows[0]);
			ncount++;
		}
	}
/*
	{
		// Take screenshot of the main window    
		std::vector<WindowInfo> aWindows; 
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
		if(hProcess!=NULL)
		{
			sc.FindWindows(hProcess, TRUE, &aWindows);
			CloseHandle(hProcess);
		}

		int i;
		for(i=0; i<(int)aWindows.size(); i++)
			wnd_list.push_back(aWindows[i].m_rcWnd);
		ssi.m_aWindows = aWindows;
	}
*/
	{
		// Take screenshot of the entire desktop
		CRect rcScreen;
		sc.GetScreenRect(&rcScreen);    
		wnd_list.push_back(rcScreen);
		ncount++;
	}

	ssi.m_bValid = TRUE;
	sc.GetScreenRect(&ssi.m_rcVirtualScreen);
	ncount++;

	BOOL bGrayscale = FALSE;

	BOOL bTakeScreenshot = sc.CaptureScreenRect(wnd_list, 
		lpszPath, 
		0, 
		m_nJpegQuality, bGrayscale, 
		ssi.m_aMonitors, screenshot_names);

	// Done
	if (bTakeScreenshot)
		return ncount;
	else
		return 0;
}

int CCrashHandler::GenerateErrorReport(LPCWEXCEPTIONINFO pExceptionInfo)
{
	cwSetErrorMsg(_T("Unspecified error."));

	// Validate input parameters 
	if(pExceptionInfo==NULL)
	{
		cwSetErrorMsg(_T("Exception info is NULL."));
		return 1;
	}
	// Get exception pointers if not provided. 
	if(pExceptionInfo->pexcptrs==NULL)
	{
		GetExceptionPointers(pExceptionInfo->code, &pExceptionInfo->pexcptrs);
	}

	CString szPath = m_szCrashSavePath;
	Utility::CreateFolder(szPath);

	HANDLE hProcess = ::GetCurrentProcess();
	DWORD dwProcessId = GetCurrentProcessId();
	DWORD dwThreadId = ::GetCurrentThreadId();
	
	int ncount = TakeDesktopScreenshot(szPath, dwProcessId);

	CreateMiniDump(szPath, hProcess, dwProcessId, dwThreadId, pExceptionInfo->pexcptrs);
	DumpExceptionInfo(szPath, pExceptionInfo->pexcptrs);
	ZipFiles(szPath, ncount);

	AFX_MANAGE_STATE(AfxGetStaticModuleState());//资源切换

	//AfxMessageBox(_T("程序发生不可处理异常，请重新启动！"), MB_OK | MB_ICONERROR);
	CDlgCrashReport dlg;
	dlg.m_pCrashHandler = this;
	dlg.DoModal();

	if (dlg.m_bRestart)
	{
		HANDLE hRestartProcess;
		int nret = RestartApp(m_szRestartCmdLine, FALSE, &hRestartProcess);
		if (nret != 0)
		{
			ASSERT(nret == 0);
			cwSetErrorMsg(_T("Error restart process"));
			return 3;
		}
	}

	// OK
	cwSetErrorMsg(_T("Success."));

	return 0; 
}
