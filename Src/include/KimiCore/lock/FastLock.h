#ifndef __KIMI_KERNEL_FAST_LOCK_H__
#define __KIMI_KERNEL_FAST_LOCK_H__

#include "LockGuard.h"

_KIMI_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////
// ÁÙ½çÇøËø - CFastLock
//////////////////////////////////////////////////////////////////////////
class KIMI_CORE_CLASS CFastLock
{
public:
	CFastLock();
	~CFastLock();

	void Lock();
	BOOL TryLock();
	void Unlock();

	PCRITICAL_SECTION GetHandle() {
		return &m_crit;
	}

private:
	CRITICAL_SECTION m_crit;

	// No copies do not implement
	CFastLock(const CFastLock &rhs);
	CFastLock &operator=(const CFastLock &rhs);
};

typedef CLockGuard<CFastLock>	CFastLockGuard;

_KIMI_NAMESPACE_END

#endif // end of __KIMI_KERNEL_FAST_LOCK_H__
