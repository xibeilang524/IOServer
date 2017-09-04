// TPRICrashWatch.cpp : 定义 DLL 应用程序的入口点。
//

#include "stdafx.h"
#include "CCrashWatch.h"
#include "Utility.h"
#include "CrashHandler.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCriticalSection			g_cs;					// 临界区对象，用于线程安全的访问错误信息列表
std::map<DWORD, CString>	g_mapErrorMsg;			// 第个调用线程的最后错误信息

// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// 初始化 MFC 并在失败时显示错误
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: 更改错误代码以符合您的需要
		_tprintf(_T("错误: MFC 初始化失败\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: 在此处为应用程序的行为编写代码。
	}

	return nRetCode;
}

//////////////////////////////////////////////////////////////////////////
// 导出API
//////////////////////////////////////////////////////////////////////////
CCRASHWATCH_API(int) cwInstall(LPCRASHWATCHCONFIG lpCrashConfig)
{
	int nStatus = -1;
	cwSetErrorMsg(_T("Success."));

	CCrashHandler *pCrashHandler = NULL;

	// Validate input parameters.
	if (lpCrashConfig==NULL || 
		lpCrashConfig->cb != sizeof(CRASHWATCHCONFIG))     
	{     
		cwSetErrorMsg(_T("pInfo is NULL or pInfo->cb member is not valid."));
		nStatus = 1;
		goto cleanup;
	}

	// Check if crInstall() already was called for current process.
	pCrashHandler = CCrashHandler::GetCurrentProcessCrashHandler();

	if(pCrashHandler!=NULL &&
		pCrashHandler->IsInitialized())
	{    
		cwSetErrorMsg(_T("Can't install crash handler to the same process twice."));
		nStatus = 2; 
		goto cleanup;
	}

	if(pCrashHandler==NULL)
	{
		pCrashHandler = new CCrashHandler();
		if(pCrashHandler==NULL)
		{    
			cwSetErrorMsg(_T("Error allocating memory for crash handler."));
			nStatus = 3; 
			goto cleanup;
		}
	}

	if (!pCrashHandler->Init(lpCrashConfig))
	{    
		nStatus = 4;
		goto cleanup;
	}

	// OK.
	nStatus = 0;

cleanup:

	if(nStatus!=0) // If failed
	{
		if(pCrashHandler!=NULL && 
			!pCrashHandler->IsInitialized())
		{
			// Release crash handler object
			CCrashHandler::ReleaseCurrentProcessCrashHandler();
		}
	}

	return nStatus;
}

CCRASHWATCH_API(int) cwUninstall()
{
	CCrashHandler *pCrashHandler = CCrashHandler::GetCurrentProcessCrashHandler();

	if(pCrashHandler==NULL ||
		!pCrashHandler->IsInitialized())
	{     
		cwSetErrorMsg(_T("Crash handler wasn't preiviously installed for this process."));
		return 1; 
	}

	// Uninstall main thread's C++ exception handlers
	int nUnset = pCrashHandler->UnSetThreadExceptionHandlers();
	if(nUnset!=0)
		return 2;

	int nDestroy = pCrashHandler->Destroy();
	if(nDestroy!=0)
		return 3;

	delete pCrashHandler;

	return 0;
}


// Sets C++ exception handlers for the calling thread
CCRASHWATCH_API(int) cwInstallThread(DWORD dwFlags)
{
	cwSetErrorMsg(_T("Success."));

	CCrashHandler *pCrashHandler = 
		CCrashHandler::GetCurrentProcessCrashHandler();

	if(pCrashHandler==NULL)
	{    
		cwSetErrorMsg(_T("Crash handler was already installed for current thread."));
		return 1; 
	}

	int nResult = pCrashHandler->SetThreadExceptionHandlers(dwFlags);
	if(nResult!=0)
		return 2; // Error?

	// Ok.
	return 0;
}

// Unsets C++ exception handlers from the calling thread
CCRASHWATCH_API(int) cwUninstallThread()
{
	cwSetErrorMsg(_T("Success."));

	CCrashHandler *pCrashHandler = CCrashHandler::GetCurrentProcessCrashHandler();

	if (pCrashHandler == NULL)
	{
		ATLASSERT(pCrashHandler != NULL);
		cwSetErrorMsg(_T("Crash handler wasn't previously installed for current thread."));
		return 1; // Invalid parameter?
	}

	int nResult = pCrashHandler->UnSetThreadExceptionHandlers();
	if(nResult!=0)
		return 2; // Error?

	// OK.
	return 0;
}


CCRASHWATCH_API(int) cwSetErrorMsg(LPCTSTR lpszErrorMsg)
{
	CSafeLock lock(&g_cs);

	DWORD dwThreadID = ::GetCurrentThreadId();
	g_mapErrorMsg[dwThreadID] = lpszErrorMsg;

	return 0;
}

CCRASHWATCH_API(int) cwGetLastErrorMsg(LPTSTR lpszBuffer, UINT uBufferSize)
{
	if (lpszBuffer == NULL || uBufferSize == 0)
		return -1;

	CSafeLock lock(&g_cs);

	DWORD dwThreadID = ::GetCurrentThreadId();
	std::map<DWORD, CString>::iterator it = g_mapErrorMsg.find(dwThreadID);
	if (it == g_mapErrorMsg.end())
	{
		lpszBuffer[0] = 0;
		return 0;
	}

	int size = min(it->second.GetLength(), (int)uBufferSize -1);
	_tcsncpy(lpszBuffer, it->second, size);
	lpszBuffer[uBufferSize - 1] = 0;

	return size;
}

CCRASHWATCH_API(int) cwGenerateErrorReport(LPCWEXCEPTIONINFO pExceptionInfo)
{
	cwSetErrorMsg(_T("Unspecified error."));

	if(pExceptionInfo==NULL || 
		pExceptionInfo->cb != sizeof(CWEXCEPTIONINFO))
	{
		cwSetErrorMsg(_T("Exception info is NULL or invalid."));    
		return 1;
	}

	CCrashHandler *pCrashHandler = CCrashHandler::GetCurrentProcessCrashHandler();

	if (pCrashHandler == NULL)
	{    
		// Handler is not installed for current process 
		cwSetErrorMsg(_T("Crash handler wasn't previously installed for current process."));
		ATLASSERT(pCrashHandler!=NULL);
		return 2;
	} 

	return pCrashHandler->GenerateErrorReport(pExceptionInfo);  
}

CCRASHWATCH_API(int) cwExceptionFilter(unsigned int code, __in_opt struct _EXCEPTION_POINTERS* ep)
{
	cwSetErrorMsg(_T("Unspecified error."));

	CCrashHandler *pCrashHandler = CCrashHandler::GetCurrentProcessCrashHandler();

	if (pCrashHandler==NULL)
	{    
		cwSetErrorMsg(_T("Crash handler wasn't previously installed for current process."));
		return EXCEPTION_CONTINUE_SEARCH; 
	}

	CWEXCEPTIONINFO ei;
	memset(&ei, 0, sizeof(CWEXCEPTIONINFO));
	ei.cb = sizeof(CWEXCEPTIONINFO);  
	ei.exctype = CW_SEH_EXCEPTION;
	ei.pexcptrs = ep;
	ei.code = code;

	int res = pCrashHandler->GenerateErrorReport(&ei);
	if(res!=0)
	{
		// If goes here than GenerateErrorReport() failed  
		return EXCEPTION_CONTINUE_SEARCH;  
	}  

	cwSetErrorMsg(_T("Success."));

	return EXCEPTION_EXECUTE_HANDLER; 
}

CCRASHWATCH_API(int) cwSetRestart(BOOL bRestart, LPCTSTR lpszCmdLine)
{
	cwSetErrorMsg(_T("Unspecified error."));

	CCrashHandler *pCrashHandler = CCrashHandler::GetCurrentProcessCrashHandler();

	if (pCrashHandler==NULL)
	{    
		cwSetErrorMsg(_T("Crash handler wasn't previously installed for current process."));
		return 1; 
	}

	pCrashHandler->SetRestart(bRestart, lpszCmdLine);

	cwSetErrorMsg(_T("Success."));

	return 0; 
}

//-----------------------------------------------------------------------------------------------
// 以下代码与 cwEmulateCrash() 有关

class CDerived;
class CBase
{
public:
	CBase(CDerived *derived): m_pDerived(derived) {};
	~CBase();
	virtual void function(void) = 0;

	CDerived * m_pDerived;
};

class CDerived : public CBase
{
public:
	CDerived() : CBase(this) {};   // C4355
	virtual void function(void) {};
};

CBase::~CBase()
{
	m_pDerived -> function();
}

#include <float.h>
void sigfpe_test()
{ 
	// Code taken from http://www.devx.com/cplus/Article/34993/1954

	//Set the x86 floating-point control word according to what
	//exceptions you want to trap. 
	_clearfp(); //Always call _clearfp before setting the control
	//word
	//Because the second parameter in the following call is 0, it
	//only returns the floating-point control word
	unsigned int cw; 
#if _MSC_VER<1400
	cw = _controlfp(0, 0); //Get the default control
#else
	_controlfp_s(&cw, 0, 0); //Get the default control
#endif 
	//word
	//Set the exception masks off for exceptions that you want to
	//trap.  When a mask bit is set, the corresponding floating-point
	//exception is //blocked from being generating.
	cw &=~(EM_OVERFLOW|EM_UNDERFLOW|EM_ZERODIVIDE|
		EM_DENORMAL|EM_INVALID);
	//For any bit in the second parameter (mask) that is 1, the 
	//corresponding bit in the first parameter is used to update
	//the control word.  
	unsigned int cwOriginal;
#if _MSC_VER<1400
	cwOriginal = _controlfp(cw, MCW_EM); //Set it.
#else
	_controlfp_s(&cwOriginal, cw, MCW_EM); //Set it.
#endif
	//MCW_EM is defined in float.h.
	//Restore the original value when done:
	//_controlfp(cwOriginal, MCW_EM);

	// Divide by zero

	float a = 1;
	float b = 0;
	float c = a/b;
	c; 
}

#define BIG_NUMBER 0x1fffffff
#pragma warning(disable: 4717) // avoid C4717 warning
int RecurseAlloc() 
{
	int *pi = new int[BIG_NUMBER];
	pi;
	RecurseAlloc();
	return 0;
}

// Vulnerable function
#pragma warning(disable : 4996)   // for strcpy use
#pragma warning(disable : 6255)   // warning C6255: _alloca indicates failure by raising a stack overflow exception. Consider using _malloca instead
#pragma warning(disable : 6204)   // warning C6204: Possible buffer overrun in call to 'strcpy': use of unchecked parameter 'str'
void test_buffer_overrun(const char *str) 
{
	char* buffer = (char*)_alloca(10);
	strcpy(buffer, str); // overrun buffer !!!

	// use a secure CRT function to help prevent buffer overruns
	// truncate string to fit a 10 byte buffer
	// strncpy_s(buffer, _countof(buffer), str, _TRUNCATE);
}
#pragma warning(default : 4996)  
#pragma warning(default : 6255)   
#pragma warning(default : 6204)   

CCRASHWATCH_API(int) cwEmulateCrash(unsigned int ExceptionType) throw (...)
{
	cwSetErrorMsg(_T("Unspecified error."));

	switch(ExceptionType)
	{
	case CW_SEH_EXCEPTION:
		{
			// Access violation
			int *p = 0;
#pragma warning(disable : 6011)   // warning C6011: Dereferencing NULL pointer 'p'
			*p = 0;
#pragma warning(default : 6011)   
		}
		break;
	case CW_CPP_TERMINATE_CALL:
		{
			// Call terminate
			terminate();
		}
		break;
	case CW_CPP_UNEXPECTED_CALL:
		{
			// Call unexpected
			unexpected();
		}
		break;
	case CW_CPP_PURE_CALL:
		{
			// pure virtual method call
			CDerived derived;
		}
		break;
	case CW_CPP_SECURITY_ERROR:
		{
			// Cause buffer overrun (/GS compiler option)

			// declare buffer that is bigger than expected
			char large_buffer[] = "This string is longer than 10 characters!!";
			test_buffer_overrun(large_buffer);
		}
		break;
	case CW_CPP_INVALID_PARAMETER:
		{      
			char* formatString;
			// Call printf_s with invalid parameters.
			formatString = NULL;
#pragma warning(disable : 6387)   // warning C6387: 'argument 1' might be '0': this does not adhere to the specification for the function 'printf'
			printf(formatString);
#pragma warning(default : 6387)   

		}
		break;
	case CW_CPP_NEW_OPERATOR_ERROR:
		{
			// Cause memory allocation error
			RecurseAlloc();
		}
		break;
	case CW_CPP_SIGABRT: 
		{
			// Call abort
			abort();
		}
		break;
	case CW_CPP_SIGFPE:
		{
			// floating point exception ( /fp:except compiler option)
			sigfpe_test();
			return 1;
		}    
	case CW_CPP_SIGILL: 
		{
			int result = raise(SIGILL);  
			ATLASSERT(result==0);
			cwSetErrorMsg(_T("Error raising SIGILL."));
			return result;
		}    
	case CW_CPP_SIGINT: 
		{
			int result = raise(SIGINT);  
			ATLASSERT(result==0);
			cwSetErrorMsg(_T("Error raising SIGINT."));
			return result;
		}    
	case CW_CPP_SIGSEGV: 
		{
			int result = raise(SIGSEGV);  
			ATLASSERT(result==0);
			cwSetErrorMsg(_T("Error raising SIGSEGV."));
			return result;
		}    
	case CW_CPP_SIGTERM: 
		{
			int result = raise(SIGTERM);  
			cwSetErrorMsg(_T("Error raising SIGTERM."));
			ATLASSERT(result==0);     
			return result;
		}
	case CW_NONCONTINUABLE_EXCEPTION: 
		{
			// Raise noncontinuable software exception
			RaiseException(123, EXCEPTION_NONCONTINUABLE, 0, NULL);        
		}
		break;
	case CW_THROW: 
		{
			// Throw typed C++ exception.
			throw 13;
		}
		break;
	default:
		{
			cwSetErrorMsg(_T("Unknown exception type specified."));          
		}
		break;
	}

	return 1;
}