#include "StdAfx.h"
#include "GeneralAPI.h"
#include "General\SafeStringList.h"
#include "General\safelock.h"

TRUNK_NAMESPACE_BEGIN

CSafeStringList::CSafeStringList(void)
{
	m_nMaxItems = 1024;
	m_bMaxLimit = TRUE;
}

CSafeStringList::CSafeStringList(int nMaxItems)
{
	m_nMaxItems = nMaxItems;
	m_bMaxLimit = TRUE;
};

CSafeStringList::~CSafeStringList(void)
{
	CSafeLock lock(&m_csLock);

	m_arrLogList.RemoveAll();
}

BOOL CSafeStringList::Add(LPCTSTR lpszTxt)
{
	CSafeLock lock(&m_csLock);

	if((m_arrLogList.GetCount() > m_nMaxItems) && m_bMaxLimit)
		return FALSE;

	CString szt;
	m_arrLogList.AddTail(lpszTxt);

	//if(m_List.GetCount() > m_nMaxItems)
	//	szt = m_List.RemoveHead();

	return TRUE;
}

BOOL CSafeStringList::Get(CString &szTxt)
{
	CSafeLock lock(&m_csLock);

	if(m_arrLogList.IsEmpty())
		return FALSE;

	szTxt = m_arrLogList.RemoveHead();

	return TRUE;
}

TRUNK_NAMESPACE_END