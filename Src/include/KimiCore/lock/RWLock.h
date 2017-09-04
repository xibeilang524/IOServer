#ifndef __KIMI_KERNEL_RW_LOCK_H__
#define __KIMI_KERNEL_RW_LOCK_H__

_KIMI_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////
// 自适应读写锁 - CRWLock
//////////////////////////////////////////////////////////////////////////
class KIMI_CORE_CLASS CRWLock
{
public:
	CRWLock();
	~CRWLock();

	void RLock();
	BOOL TryRLock();
	void RUnlock();
	void WLock();
	BOOL TryWLock();
	void WUnlock();

private:
	union {
		SRWLOCK srwlock_;	// Win7等系统支持的读写锁
		struct {			// 如果系统不支持读写锁，则自己实现一个
			CRITICAL_SECTION read_mutex_;
			CRITICAL_SECTION write_mutex_;
			unsigned int num_readers_;
		} fallback_;
	} rwlock_;

	// No copies do not implement
	CRWLock(const CRWLock &rhs);
	CRWLock &operator=(const CRWLock &rhs);
};

_KIMI_NAMESPACE_END

#endif // end of __KIMI_KERNEL_RW_LOCK_H__
