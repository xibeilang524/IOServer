#include "StdAfx.h"
#include "KimiCore.h"
#include "KimiCore\lock\RWLock.h"
#include "KimiCore\Utility\Win32API.h"

_KIMI_NAMESPACE_BEGIN

#define HAVE_SRWLOCK_API (Win32API::pTryAcquireSRWLockShared != NULL)

///////////////////////////////////////////////////////////////////////////////
// CRWLock
///////////////////////////////////////////////////////////////////////////////

CRWLock::CRWLock()
{
	if (HAVE_SRWLOCK_API)
	{
		Win32API::pInitializeSRWLock(&rwlock_.srwlock_);
	}
	else {
		::InitializeCriticalSectionAndSpinCount(&rwlock_.fallback_.read_mutex_, 0x80000400);
		::InitializeCriticalSectionAndSpinCount(&rwlock_.fallback_.write_mutex_, 0x80000400);
		rwlock_.fallback_.num_readers_ = 0;
	}
}

CRWLock::~CRWLock()
{
	if (HAVE_SRWLOCK_API)
	{
		;
	}
	else {
		::DeleteCriticalSection(&rwlock_.fallback_.read_mutex_);
		::DeleteCriticalSection(&rwlock_.fallback_.write_mutex_);
	}
}


void CRWLock::RLock()
{
	if (HAVE_SRWLOCK_API)
	{
		Win32API::pAcquireSRWLockShared(&rwlock_.srwlock_);
	}
	else {
		::EnterCriticalSection(&rwlock_.fallback_.read_mutex_);

		if (++rwlock_.fallback_.num_readers_ == 1)
			::EnterCriticalSection(&rwlock_.fallback_.write_mutex_);

		::LeaveCriticalSection(&rwlock_.fallback_.read_mutex_);
	}
}

BOOL CRWLock::TryRLock()
{
	if (HAVE_SRWLOCK_API)
	{
		return Win32API::pTryAcquireSRWLockShared(&rwlock_.srwlock_);
	}
	else {
		BOOL bLock = ::TryEnterCriticalSection(&rwlock_.fallback_.read_mutex_);
		if (!bLock)
			return FALSE;
		
		if (rwlock_.fallback_.num_readers_ == 0)
			bLock = ::TryEnterCriticalSection(&rwlock_.fallback_.write_mutex_);

		if (bLock)
			rwlock_.fallback_.num_readers_++;

		::LeaveCriticalSection(&rwlock_.fallback_.read_mutex_);

		return bLock;
	}
}

void CRWLock::RUnlock()
{
	if (HAVE_SRWLOCK_API)
	{
		Win32API::pReleaseSRWLockShared(&rwlock_.srwlock_);
	}
	else {
		::EnterCriticalSection(&rwlock_.fallback_.read_mutex_);

		if (--rwlock_.fallback_.num_readers_ == 0)
			::LeaveCriticalSection(&rwlock_.fallback_.write_mutex_);

		::LeaveCriticalSection(&rwlock_.fallback_.read_mutex_);
	}
}

void CRWLock::WLock()
{
	if (HAVE_SRWLOCK_API)
	{
		Win32API::pAcquireSRWLockExclusive(&rwlock_.srwlock_);
	}
	else {
		::EnterCriticalSection(&rwlock_.fallback_.write_mutex_);
	}
}

BOOL CRWLock::TryWLock()
{
	if (HAVE_SRWLOCK_API)
	{
		return Win32API::pTryAcquireSRWLockExclusive(&rwlock_.srwlock_);
	}
	else {
		return ::TryEnterCriticalSection(&rwlock_.fallback_.write_mutex_);
	}
}

void CRWLock::WUnlock()
{
	if (HAVE_SRWLOCK_API)
	{
		Win32API::pReleaseSRWLockExclusive(&rwlock_.srwlock_);
	}
	else {
		::LeaveCriticalSection(&rwlock_.fallback_.write_mutex_);
	}
}

_KIMI_NAMESPACE_END