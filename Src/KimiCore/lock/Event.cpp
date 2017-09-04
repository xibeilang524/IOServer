#include "StdAfx.h"
#include "KimiCore.h"
#include "KimiCore\lock\Event.h"

_KIMI_NAMESPACE_BEGIN
	
///////////////////////////////////////////////////////////////////////////////
// CWSEvent
///////////////////////////////////////////////////////////////////////////////

CEvent::CEvent(
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	BOOL bManualReset,
	BOOL bInitialState)
   :  m_hEvent(CreateEvent(lpSecurityAttributes, bManualReset, bInitialState, 0))
{

}

CEvent::CEvent(
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	BOOL bManualReset,
	BOOL bInitialState,
	LPCTSTR lpszName)
   :  m_hEvent(CreateEvent(lpSecurityAttributes, bManualReset, bInitialState, lpszName))
{

}

CEvent::~CEvent()
{
	::CloseHandle(m_hEvent);
}

HANDLE CEvent::GetEvent() const
{
	return m_hEvent;
}

BOOL CEvent::Wait() const
{
	return Wait(INFINITE);
}

BOOL CEvent::Wait(DWORD dwTimeoutMillis) const
{
	BOOL bOK;

	DWORD dwResult = ::WaitForSingleObject(m_hEvent, dwTimeoutMillis);

	if (dwResult == WAIT_TIMEOUT)
	{
		bOK = FALSE;
	}
	else if (dwResult == WAIT_OBJECT_0)
	{
		bOK = TRUE;
	}
	else
	{
		//throw CException(_T("CEvent::Wait() - WaitForSingleObject"), ::GetLastError());
		bOK = FALSE;
	}
    
	return bOK;
}

BOOL CEvent::Reset()
{
	return ::ResetEvent(m_hEvent);
}

BOOL CEvent::Set()
{
	return ::SetEvent(m_hEvent);
}

BOOL CEvent::Pulse()
{
	return ::PulseEvent(m_hEvent);
}

///////////////////////////////////////////////////////////////////////////////
// CAutoResetEvent
///////////////////////////////////////////////////////////////////////////////

CAutoResetEvent::CAutoResetEvent(BOOL bInitialState /* = FALSE */)
   :  CEvent(0, FALSE, bInitialState)
{

}

CAutoResetEvent::CAutoResetEvent(LPCTSTR lpszName, BOOL bInitialState /* = FALSE */)
   :  CEvent(0, FALSE, bInitialState, lpszName)
{
   
}

///////////////////////////////////////////////////////////////////////////////
// CManualResetEvent
///////////////////////////////////////////////////////////////////////////////

CManualResetEvent::CManualResetEvent(BOOL bInitialState /* = FALSE */)
   :  CEvent(0, TRUE, bInitialState)
{

}

CManualResetEvent::CManualResetEvent(LPCTSTR lpszName, BOOL bInitialState /* = FALSE */)
   :  CEvent(0, TRUE, bInitialState, lpszName)
{
   
}

_KIMI_NAMESPACE_END