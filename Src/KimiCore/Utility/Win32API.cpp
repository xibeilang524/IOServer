// Utility.cpp : 辅助函数
//

#include "stdafx.h"
#include "KimiCore.h"
#include "hash\CRC32.h"
#include "hash\CRC64.h"
#include "KimiCore\MD5.h"
#include "KimiCore\Utility\Win32API.h"

_KIMI_NAMESPACE_BEGIN

// 优先初始化该类
#pragma warning(disable: 4073)
#pragma init_seg(lib)
sInitializeSRWLock Win32API::pInitializeSRWLock = NULL;
sAcquireSRWLockShared Win32API::pAcquireSRWLockShared = NULL;
sAcquireSRWLockExclusive Win32API::pAcquireSRWLockExclusive = NULL;
sTryAcquireSRWLockShared Win32API::pTryAcquireSRWLockShared = NULL;
sTryAcquireSRWLockExclusive Win32API::pTryAcquireSRWLockExclusive = NULL;
sReleaseSRWLockShared Win32API::pReleaseSRWLockShared = NULL;
sReleaseSRWLockExclusive Win32API::pReleaseSRWLockExclusive = NULL;

sInitializeConditionVariable Win32API::pInitializeConditionVariable = NULL;
sSleepConditionVariableCS Win32API::pSleepConditionVariableCS = NULL;
sSleepConditionVariableSRW Win32API::pSleepConditionVariableSRW = NULL;
sWakeAllConditionVariable Win32API::pWakeAllConditionVariable = NULL;
sWakeConditionVariable Win32API::pWakeConditionVariable = NULL;

static Win32API g_Win32API;

//////////////////////////////////////////////////////////////////////////
// 用于自动加载Win32 API的辅助类
Win32API::Win32API()
{
	kernel32_module = GetModuleHandleA("kernel32.dll");

	pInitializeSRWLock = (sInitializeSRWLock)GetProcAddress(kernel32_module, "InitializeSRWLock");

	pAcquireSRWLockShared = (sAcquireSRWLockShared)GetProcAddress(kernel32_module, "AcquireSRWLockShared");

	pAcquireSRWLockExclusive = (sAcquireSRWLockExclusive)GetProcAddress(kernel32_module, "AcquireSRWLockExclusive");

	pTryAcquireSRWLockShared = (sTryAcquireSRWLockShared)GetProcAddress(kernel32_module, "TryAcquireSRWLockShared");

	pTryAcquireSRWLockExclusive = (sTryAcquireSRWLockExclusive)GetProcAddress(kernel32_module, "TryAcquireSRWLockExclusive");

	pReleaseSRWLockShared = (sReleaseSRWLockShared)GetProcAddress(kernel32_module, "ReleaseSRWLockShared");

	pReleaseSRWLockExclusive = (sReleaseSRWLockExclusive)GetProcAddress(kernel32_module, "ReleaseSRWLockExclusive");

	pInitializeConditionVariable = (sInitializeConditionVariable)GetProcAddress(kernel32_module, "InitializeConditionVariable");

	pSleepConditionVariableCS = (sSleepConditionVariableCS)GetProcAddress(kernel32_module, "SleepConditionVariableCS");

	pSleepConditionVariableSRW = (sSleepConditionVariableSRW)GetProcAddress(kernel32_module, "SleepConditionVariableSRW");

	pWakeAllConditionVariable = (sWakeAllConditionVariable)GetProcAddress(kernel32_module, "WakeAllConditionVariable");

	pWakeConditionVariable = (sWakeConditionVariable)GetProcAddress(kernel32_module, "WakeConditionVariable");
}

Win32API::~Win32API()
{
	// kernel32_module不需要释放，进程退出后会自动释放
}

_KIMI_NAMESPACE_END