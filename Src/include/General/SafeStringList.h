#ifndef __SAFESTRINGLIST_H__
#define __SAFESTRINGLIST_H__

TRUNK_NAMESPACE_BEGIN

class _GENERAL_CLASS CSafeStringList
{
public:
	CSafeStringList();
	CSafeStringList(int nMaxItems);
	~CSafeStringList();

public:
	BOOL GetMaxLimit() { return m_bMaxLimit; }
	void SetMaxLimit(BOOL bLimit) { m_bMaxLimit = bLimit; }
	void SetMaxItems(int nMaxItems){m_nMaxItems = nMaxItems;};

	BOOL Add(LPCTSTR lpszTxt);
	BOOL Get(CString &szTxt);

protected:
	CCriticalSection m_csLock;
	CStringList		m_arrLogList;

	BOOL	m_bMaxLimit;
	int		m_nMaxItems;
};

TRUNK_NAMESPACE_END

#endif // end of __SAFESTRINGLIST_H__