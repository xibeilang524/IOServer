#ifndef __TPRI_KERNEL_SPIN_LOCK_H__
#define __TPRI_KERNEL_SPIN_LOCK_H__

template <class T>
class CLockGuard
{
public:
	explicit CLockGuard(T* pLock) : m_pLock(pLock) { m_pLock->Lock(); }
	~CLockGuard() { m_pLock->Unlock(); }

private:
	T* m_pLock;

	// No copies do not implement
	CLockGuard(const CLockGuard &rhs);
	CLockGuard &operator=(const CLockGuard &rhs);
};

//////////////////////////////////////////////////////////////////////////
// зда§Ыј - CSpinLock
//////////////////////////////////////////////////////////////////////////
class CSpinLock
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

#endif // end of __TPRI_KERNEL_SPIN_LOCK_H__
