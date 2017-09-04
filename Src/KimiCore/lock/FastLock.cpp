#include "StdAfx.h"
#include "KimiCore.h"
#include "KimiCore\lock\FastLock.h"

_KIMI_NAMESPACE_BEGIN
	
///////////////////////////////////////////////////////////////////////////////
// CFastLock
///////////////////////////////////////////////////////////////////////////////

CFastLock::CFastLock()
{
	//::InitializeCriticalSection(&m_crit);
	::InitializeCriticalSectionAndSpinCount(&m_crit, 0x80000400);
}
      
CFastLock::~CFastLock()
{
	::DeleteCriticalSection(&m_crit);
}


void CFastLock::Lock()
{
	::EnterCriticalSection(&m_crit);
}

BOOL CFastLock::TryLock()
{
	return ::TryEnterCriticalSection(&m_crit);
}

void CFastLock::Unlock()
{
	::LeaveCriticalSection(&m_crit);
}

_KIMI_NAMESPACE_END