#ifndef __KIMI_KERNEL_SPIN_LOCK_H__
#define __KIMI_KERNEL_SPIN_LOCK_H__

#include "LockGuard.h"

_KIMI_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////
// зда§Ыј - CSpinLock
//////////////////////////////////////////////////////////////////////////
class KIMI_CORE_CLASS CSpinLock
{
public:
	CSpinLock();
	~CSpinLock();

	void Lock();
	BOOL TryLock();
	void Unlock();

private:
	volatile long m_lock;

	// No copies do not implement
	CSpinLock(const CSpinLock &rhs);
	CSpinLock &operator=(const CSpinLock &rhs);
};

typedef CLockGuard<CSpinLock>	CSpinLockGuard;

_KIMI_NAMESPACE_END

#endif // end of __KIMI_KERNEL_SPIN_LOCK_H__
