#include "StdAfx.h"
#include "SpinLock.h"

///////////////////////////////////////////////////////////////////////////////
// CSpinLock
///////////////////////////////////////////////////////////////////////////////

CSpinLock::CSpinLock()
: m_lock(0)
{
}

CSpinLock::~CSpinLock()
{
}


void CSpinLock::Lock()
{
	while (::InterlockedExchange(&m_lock, 1) == 1)
	{
		SwitchToThread();
		//Sleep(0);
	}
}

BOOL CSpinLock::TryLock()
{
	return (::InterlockedExchange(&m_lock, 1) == 0);
}

void CSpinLock::Unlock()
{
	::InterlockedExchange(&m_lock, 0);
}