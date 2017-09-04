#include "StdAfx.h"
#include "KimiCore.h"
#include "KimiCore\lock\MutexLock.h"

_KIMI_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
// CMutexLock
///////////////////////////////////////////////////////////////////////////////

CMutexLock::CMutexLock(BOOL bInitiallyOwn/* = FALSE*/, LPCTSTR lpszName/* = NULL*/,
			   LPSECURITY_ATTRIBUTES lpsaAttribute/* = NULL*/)
{
	m_hMutex = ::CreateMutex(lpsaAttribute, bInitiallyOwn, lpszName);
}

CMutexLock::~CMutexLock()
{
	::CloseHandle(m_hMutex);
}

void CMutexLock::Lock()
{
	::WaitForSingleObject( m_hMutex, INFINITE );
}

BOOL CMutexLock::TryLock()
{
	int ret = ::WaitForSingleObject( m_hMutex, 0 );
	return (WAIT_OBJECT_0 == ret);
}

void CMutexLock::Unlock()
{
	::ReleaseMutex(m_hMutex);
}

_KIMI_NAMESPACE_END