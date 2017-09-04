#include "StdAfx.h"
#include "KimiCore.h"
#include "KimiCore\lock\CondVarLock.h"
#include "KimiCore\Utility\Win32API.h"

_KIMI_NAMESPACE_BEGIN

#define HAVE_CONDVAR_API (Win32API::pInitializeConditionVariable != NULL)

///////////////////////////////////////////////////////////////////////////////
// CCondVarLock
///////////////////////////////////////////////////////////////////////////////

CCondVarLock::CCondVarLock()
{
	if (HAVE_CONDVAR_API)
	{
		Win32API::pInitializeConditionVariable(&cond_var_.cond_var_);
	}
	else {
		cond_var_.fallback_.waiters_count_ = 0;

		::InitializeCriticalSection(&cond_var_.fallback_.waiters_count_lock_);

		/* Create an auto-reset event. */
		cond_var_.fallback_.signal_event_ = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		// TODO: 错误处理...

		/* Create a manual-reset event. */
		cond_var_.fallback_.broadcast_event_ = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		// TODO: 错误处理...
	}
}

CCondVarLock::~CCondVarLock()
{
	if (HAVE_CONDVAR_API)
	{
		;
	}
	else {
		::CloseHandle(cond_var_.fallback_.broadcast_event_);
		::CloseHandle(cond_var_.fallback_.signal_event_);
		::DeleteCriticalSection(&cond_var_.fallback_.waiters_count_lock_);
	}
}

void CCondVarLock::Signal()
{
	if (HAVE_CONDVAR_API)
	{
		Win32API::pWakeConditionVariable(&cond_var_.cond_var_);
	}
	else {
		int have_waiters;

		/* Avoid race conditions. */
		::EnterCriticalSection(&cond_var_.fallback_.waiters_count_lock_);
		have_waiters = cond_var_.fallback_.waiters_count_ > 0;
		::LeaveCriticalSection(&cond_var_.fallback_.waiters_count_lock_);

		if (have_waiters)
			::SetEvent(cond_var_.fallback_.signal_event_);
	}
}

void CCondVarLock::Broadcast()
{
	if (HAVE_CONDVAR_API)
	{
		Win32API::pWakeAllConditionVariable(&cond_var_.cond_var_);
	}
	else {
		int have_waiters;

		/* Avoid race conditions. */
		::EnterCriticalSection(&cond_var_.fallback_.waiters_count_lock_);
		have_waiters = cond_var_.fallback_.waiters_count_ > 0;
		::LeaveCriticalSection(&cond_var_.fallback_.waiters_count_lock_);

		if (have_waiters)
			::SetEvent(cond_var_.fallback_.broadcast_event_);
	}
}

BOOL CCondVarLock::Wait(PCRITICAL_SECTION pCSLock, DWORD dwMilliseconds/* = INFINITE*/)
{
	if (HAVE_CONDVAR_API)
	{
		return Win32API::pSleepConditionVariableCS(&cond_var_.cond_var_, pCSLock, INFINITE);
	}
	else {
		DWORD result;
		int last_waiter;
		HANDLE handles[2] = {
			cond_var_.fallback_.signal_event_,
			cond_var_.fallback_.broadcast_event_
		};

		/* Avoid race conditions. */
		::EnterCriticalSection(&cond_var_.fallback_.waiters_count_lock_);
		cond_var_.fallback_.waiters_count_++;
		::LeaveCriticalSection(&cond_var_.fallback_.waiters_count_lock_);

		/* It's ok to release the <mutex> here since Win32 manual-reset events */
		/* maintain state when used with <SetEvent>. This avoids the "lost wakeup" */
		/* bug. */
		::LeaveCriticalSection(pCSLock);

		/* Wait for either event to become signaled due to <uv_cond_signal> being */
		/* called or <uv_cond_broadcast> being called. */
		result = ::WaitForMultipleObjects(2, handles, FALSE, dwMilliseconds);

		EnterCriticalSection(&cond_var_.fallback_.waiters_count_lock_);
		cond_var_.fallback_.waiters_count_--;
		last_waiter = result == WAIT_OBJECT_0 + 1 && cond_var_.fallback_.waiters_count_ == 0;
		LeaveCriticalSection(&cond_var_.fallback_.waiters_count_lock_);

		/* Some thread called <pthread_cond_broadcast>. */
		if (last_waiter) {
			/* We're the last waiter to be notified or to stop waiting, so reset the */
			/* the manual-reset event. */
			::ResetEvent(cond_var_.fallback_.broadcast_event_);
		}

		/* Reacquire the <mutex>. */
		::EnterCriticalSection(pCSLock);

		if (result == WAIT_OBJECT_0 || result == WAIT_OBJECT_0 + 1)
			return TRUE;
		else
			return FALSE;
	}
}

BOOL CCondVarLock::Wait(CFastLock* pLock, DWORD dwMilliseconds/* = INFINITE*/)
{
	return Wait(pLock->GetHandle(), dwMilliseconds);
}

_KIMI_NAMESPACE_END