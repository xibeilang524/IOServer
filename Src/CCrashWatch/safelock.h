#ifndef __SAFELOCK_H__
#define __SAFELOCK_H__

#include <afxmt.h>


class CSafeLock  //ÁÙ½çÇø
{
public:
	CSafeLock(CCriticalSection *pLock){
		m_pLock = pLock;
		m_pLock->Lock();
	};
	CSafeLock(CSyncObject *pLock){
		m_pLock = pLock;
		m_pLock->Lock();
	};
	virtual ~CSafeLock(){
		if(m_pLock)
			m_pLock->Unlock();
	};
private:
	//CCriticalSection *m_pLock;
	CSyncObject		*m_pLock;
};


#endif // end of __SAFELOCK_H__