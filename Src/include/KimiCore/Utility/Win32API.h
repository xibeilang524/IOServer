#ifndef __KIMI_KERNEL_WIN32_API_H__
#define __KIMI_KERNEL_WIN32_API_H__

_KIMI_NAMESPACE_BEGIN

typedef VOID (WINAPI* sInitializeSRWLock)(PSRWLOCK SRWLock);

typedef VOID (WINAPI* sAcquireSRWLockShared)(PSRWLOCK SRWLock);

typedef VOID (WINAPI* sAcquireSRWLockExclusive)(PSRWLOCK SRWLock);

typedef BOOL (WINAPI* sTryAcquireSRWLockShared)(PSRWLOCK SRWLock);

typedef BOOL (WINAPI* sTryAcquireSRWLockExclusive)(PSRWLOCK SRWLock);

typedef VOID (WINAPI* sReleaseSRWLockShared)(PSRWLOCK SRWLock);

typedef VOID (WINAPI* sReleaseSRWLockExclusive)(PSRWLOCK SRWLock);

typedef VOID (WINAPI* sInitializeConditionVariable)(PCONDITION_VARIABLE ConditionVariable);

typedef BOOL (WINAPI* sSleepConditionVariableCS)(PCONDITION_VARIABLE ConditionVariable, PCRITICAL_SECTION CriticalSection, DWORD dwMilliseconds);

typedef BOOL (WINAPI* sSleepConditionVariableSRW)(PCONDITION_VARIABLE ConditionVariable, PSRWLOCK SRWLock, DWORD dwMilliseconds, ULONG Flags);

typedef VOID (WINAPI* sWakeAllConditionVariable)(PCONDITION_VARIABLE ConditionVariable);

typedef VOID (WINAPI* sWakeConditionVariable)(PCONDITION_VARIABLE ConditionVariable);

class Win32API
{
public:
	Win32API();
	~Win32API();

public:
	static sInitializeSRWLock pInitializeSRWLock;
	static sAcquireSRWLockShared pAcquireSRWLockShared;
	static sAcquireSRWLockExclusive pAcquireSRWLockExclusive;
	static sTryAcquireSRWLockShared pTryAcquireSRWLockShared;
	static sTryAcquireSRWLockExclusive pTryAcquireSRWLockExclusive;
	static sReleaseSRWLockShared pReleaseSRWLockShared;
	static sReleaseSRWLockExclusive pReleaseSRWLockExclusive;

	static sInitializeConditionVariable pInitializeConditionVariable;
	static sSleepConditionVariableCS pSleepConditionVariableCS;
	static sSleepConditionVariableSRW pSleepConditionVariableSRW;
	static sWakeAllConditionVariable pWakeAllConditionVariable;
	static sWakeConditionVariable pWakeConditionVariable;
private:
	HMODULE kernel32_module;
};


_KIMI_NAMESPACE_END

#endif // end of __KIMI_KERNEL_WIN32_API_H__