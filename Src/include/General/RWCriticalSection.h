#ifndef __RWCRITICALSECTION_H__
#define __RWCRITICALSECTION_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

TRUNK_NAMESPACE_BEGIN

class _GENERAL_CLASS CRWCriticalSection  
{
public:
	class _GENERAL_CLASS AutoReader
	{
	public:
		explicit AutoReader(CRWCriticalSection &crit);
		~AutoReader();

	private:
		CRWCriticalSection &m_crit;

		// No copies do not implement
		AutoReader(const AutoReader &rhs);
		AutoReader &operator=(const AutoReader &rhs);
	};

	class _GENERAL_CLASS AutoWriter
	{
	public:
		explicit AutoWriter(CRWCriticalSection &crit);
		~AutoWriter();

	private:
		CRWCriticalSection &m_crit;

		// No copies do not implement
		AutoWriter(const AutoWriter &rhs);
		AutoWriter &operator=(const AutoWriter &rhs);
	};

	CRWCriticalSection();
	virtual ~CRWCriticalSection();

public:
	bool LockReader(DWORD dwTimeOut = INFINITE);
	void UnlockReader();
	bool LockWriter(DWORD dwTimeOut = INFINITE);
	void UnlockWriter();

protected:
	HANDLE m_hNobodyIsReading;
	HANDLE m_hWritingMutex;
	CRITICAL_SECTION m_csReading;
	int m_nReaders;
};

TRUNK_NAMESPACE_END

#endif // end of __RWCRITICALSECTION_H__
