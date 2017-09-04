#include "stdafx.h"
#include "GeneralAPI.h"
#include "General\RWCriticalSection.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

TRUNK_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRWCriticalSection::CRWCriticalSection()
{
	// Create the flag that indicates if somebody is reading the object
	//     initially, nobody is...
	m_hNobodyIsReading = CreateEvent(NULL, TRUE, true, NULL);

	// Create a mutex indicating who is writing (only one)
	m_hWritingMutex = CreateMutex(NULL, false, NULL);

	ASSERT( m_hNobodyIsReading && m_hWritingMutex);

	// Create the critical section for maintaining the number of readers
	//   it should not delay much (so no need to analyse for deadlocks)
	InitializeCriticalSection(&m_csReading);
	
	m_nReaders = 0;
}

CRWCriticalSection::~CRWCriticalSection()
{
	CloseHandle(m_hNobodyIsReading);
	CloseHandle(m_hWritingMutex);
	DeleteCriticalSection(&m_csReading);

	ASSERT(m_nReaders == 0);
}

bool CRWCriticalSection::LockReader(DWORD dwTimeOut)
{
	// We make sure that nobody is writing
	if ( WaitForSingleObject(m_hWritingMutex,dwTimeOut) != WAIT_OBJECT_0 )
		return false;

	// We need a critical section to update the number of readers
	::EnterCriticalSection(&m_csReading);
	{
		m_nReaders++;
		if ( m_nReaders == 1 )
		{
			// We must indicate that there are some readers
			ResetEvent(m_hNobodyIsReading);
		}
	}
	::LeaveCriticalSection(&m_csReading);

	// I release the mutex for writing that I do not need
	ReleaseMutex(m_hWritingMutex);
	
	return true;
}


void CRWCriticalSection::UnlockReader()
{
	// We need a critical section to update the number of readers
	::EnterCriticalSection(&m_csReading);
	{
		m_nReaders--;
		ASSERT(m_nReaders >= 0);
		if ( m_nReaders == 0 )
		{
			// We indicate that there are no more readers
			SetEvent(m_hNobodyIsReading);
		}
	}
	::LeaveCriticalSection(&m_csReading);
}

bool CRWCriticalSection::LockWriter(DWORD dwTimeOut)
{
	// Only one writer at a time
	if ( WaitForSingleObject(m_hWritingMutex,dwTimeOut) != WAIT_OBJECT_0 )
		return false;

	// Wait for all readers to leave
	if ( WaitForSingleObject(m_hNobodyIsReading,dwTimeOut) != WAIT_OBJECT_0 )
	{
		// We have waited too long, so we have to set the "Writing" mutex
		ReleaseMutex(m_hWritingMutex);
		return false;
	}
	
	return true;
}


void CRWCriticalSection::UnlockWriter()
{
	// Let other readers and writers in
	ReleaseMutex(m_hWritingMutex);
}

///////////////////////////////////////////////////////////////////////////////
// CRWCriticalSection::AutoReader
///////////////////////////////////////////////////////////////////////////////

CRWCriticalSection::AutoReader::AutoReader(CRWCriticalSection &crit)
: m_crit(crit)
{
	m_crit.LockReader();
}

CRWCriticalSection::AutoReader::~AutoReader()
{
	m_crit.UnlockReader();
}

///////////////////////////////////////////////////////////////////////////////
// CRWCriticalSection::AutoWriter
///////////////////////////////////////////////////////////////////////////////

CRWCriticalSection::AutoWriter::AutoWriter(CRWCriticalSection &crit)
: m_crit(crit)
{
	m_crit.LockWriter();
}

CRWCriticalSection::AutoWriter::~AutoWriter()
{
	m_crit.UnlockWriter();
}

TRUNK_NAMESPACE_END