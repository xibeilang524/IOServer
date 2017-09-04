#ifndef __KIMI_NOCASEMAP_H__
#define __KIMI_NOCASEMAP_H__

#include <afxtempl.h> //模版

_KIMI_NAMESPACE__BEGIN

#define MAP_KEY_SIZE	256
#define TO_UPPER_CHAR(a) (((a) >= 'a' && (a) <= 'z') ? (a)-32 : (a))	// ('a'-'A') = 32

// 不区分大小写的Map表
template<class VALUE, class ARG_VALUE>
class CNoCaseMap: public CMap<CString, LPCTSTR, VALUE, ARG_VALUE>
{
public:
	~CNoCaseMap() { RemoveAll(); }

	BOOL Lookup(LPCTSTR key, VALUE& rValue) const;

	VALUE& operator[](LPCTSTR key);
	
	void SetAt(LPCTSTR key, ARG_VALUE newValue);

	BOOL RemoveKey(LPCTSTR key);

	/// 如果当前Hash表中的槽比放置的元素加上nGrownSize增量值要小，该函数会重新分配一个更大的槽
	BOOL AdjustHashTable(int nGrownSize = 0);

	//BOOL UpCase(const char *lpsztag ,CString &szTag) const;//wzq
	void UpCase(LPCTSTR lpszIn, LPTSTR lpszOut) const;
};

template<class VALUE, class ARG_VALUE>
AFX_INLINE BOOL CNoCaseMap<VALUE, ARG_VALUE>::Lookup(LPCTSTR key, VALUE& rValue) const
{
//	CString szKey;
//	UpCase(key , szKey);
	TCHAR szKey[MAP_KEY_SIZE];
	UpCase(key, szKey);

	return CMap<CString, LPCTSTR, VALUE, ARG_VALUE>::Lookup(szKey, rValue);
}

template<class VALUE, class ARG_VALUE>
AFX_INLINE VALUE& CNoCaseMap<VALUE, ARG_VALUE>::operator[](LPCTSTR key)
{
	//	CString szKey;
	//	UpCase(key , szKey);
	TCHAR szKey[MAP_KEY_SIZE];
	UpCase(key, szKey);

	return CMap<CString, LPCTSTR, VALUE, ARG_VALUE>::operator[](szKey);
}

template<class VALUE, class ARG_VALUE>
AFX_INLINE void CNoCaseMap<VALUE, ARG_VALUE>::SetAt(LPCTSTR key, ARG_VALUE newValue)
{
	//	CString szKey;
	//	UpCase(key , szKey);
	TCHAR szKey[MAP_KEY_SIZE];
	UpCase(key, szKey);

	CMap<CString, LPCTSTR, VALUE, ARG_VALUE>::SetAt(szKey, newValue);
}

template<class VALUE, class ARG_VALUE>
AFX_INLINE BOOL CNoCaseMap<VALUE, ARG_VALUE>::RemoveKey(LPCTSTR key)
{
	//	CString szKey;
	//	UpCase(key , szKey);
	TCHAR szKey[MAP_KEY_SIZE];
	UpCase(key, szKey);

	return CMap<CString, LPCTSTR, VALUE, ARG_VALUE>::RemoveKey(szKey);
}

template<class VALUE, class ARG_VALUE>
BOOL CNoCaseMap<VALUE, ARG_VALUE>::AdjustHashTable(int nGrownSize/* = 0*/)
{
	return FALSE;
}

/*
template<class VALUE, class ARG_VALUE>
BOOL CNoCaseMap<VALUE, ARG_VALUE>::UpCase(const char *lpsztag , CString &szTag) const
{
	unsigned int i =0;
	while(lpsztag[i] != 0x00)
	{
		szTag += TO_UPPER_CHAR(lpsztag[i]);
		i++;
	}
	return TRUE;
}
*/
template<class VALUE, class ARG_VALUE>
AFX_INLINE void CNoCaseMap<VALUE, ARG_VALUE>::UpCase(LPCTSTR lpszIn, LPTSTR lpszOut) const
{
	LPCTSTR pin = lpszIn;
	LPTSTR pout = lpszOut;
	TCHAR ch;

	while(ch = *pin++)
		*pout++ = TO_UPPER_CHAR(ch);
	*pout = '\0';
}

_KIMI_NAMESPACE__END

#endif // end of __KIMI_NOCASEMAP_H__