#ifndef __KIMI_KERNEL_MUTEX_LOCK_H__
#define __KIMI_KERNEL_MUTEX_LOCK_H__

#include "LockGuard.h"

_KIMI_NAMESPACE_BEGIN

class KIMI_CORE_CLASS CMutexLock
{
public:
	CMutexLock(BOOL bInitiallyOwn = FALSE, LPCTSTR lpszName = NULL,
		LPSECURITY_ATTRIBUTES lpsaAttribute = NULL);
	~CMutexLock();

	HANDLE GetHandle() {
		return m_hMutex;
	}

	void Lock();
	BOOL TryLock();
	void Unlock();

private:
	HANDLE m_hMutex;

	// No copies do not implement
	CMutexLock(const CMutexLock &rhs);
	CMutexLock &operator=(const CMutexLock &rhs);
};

typedef CLockGuard<CMutexLock>	CMutexLockGuard;

_KIMI_NAMESPACE_END

#endif // end of __KIMI_KERNEL_MUTEX_LOCK_H__
