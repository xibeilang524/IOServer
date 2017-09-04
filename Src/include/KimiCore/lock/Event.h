#ifndef __KIMI_KERNEL_EVENT_H__
#define __KIMI_KERNEL_EVENT_H__

_KIMI_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
// CEvent
///////////////////////////////////////////////////////////////////////////////
class KIMI_CORE_CLASS CEvent  
{
public :
	CEvent(
		LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		BOOL bManualReset,
		BOOL bInitialState);	
	CEvent(
		LPSECURITY_ATTRIBUTES lpSecurityAttributes,
        BOOL bManualReset,
		BOOL bInitialState,
		LPCTSTR lpszName);
	virtual ~CEvent();

	HANDLE GetEvent() const;
	BOOL Wait() const;
	BOOL Wait(DWORD dwTimeoutMillis) const;
	BOOL Reset();
	BOOL Set();
	BOOL Pulse();

private :
	HANDLE m_hEvent;

	// No copies do not implement
	CEvent(const CEvent &rhs);
	CEvent &operator=(const CEvent &rhs);
};

///////////////////////////////////////////////////////////////////////////////
// CWSAutoResetEvent
///////////////////////////////////////////////////////////////////////////////

class KIMI_CORE_CLASS CAutoResetEvent : public CEvent
{
public:
	explicit CAutoResetEvent(BOOL bInitialState = FALSE);
	explicit CAutoResetEvent(LPCTSTR lpszName, BOOL bInitialState = FALSE);

private:
	// No copies do not implement
	CAutoResetEvent(const CAutoResetEvent &rhs);
	CAutoResetEvent &operator=(const CAutoResetEvent &rhs);
};

///////////////////////////////////////////////////////////////////////////////
// CManualResetEvent
///////////////////////////////////////////////////////////////////////////////

class KIMI_CORE_CLASS CManualResetEvent : public CEvent
{
public:
	explicit CManualResetEvent(BOOL bInitialState = FALSE);     
	explicit CManualResetEvent(LPCTSTR lpszName, BOOL bInitialState = FALSE);

private:
	// No copies do not implement
	CManualResetEvent(const CManualResetEvent &rhs);
	CManualResetEvent &operator=(const CManualResetEvent &rhs);
};

_KIMI_NAMESPACE_END

#endif // end of __KIMI_KERNEL_EVENT_H__
