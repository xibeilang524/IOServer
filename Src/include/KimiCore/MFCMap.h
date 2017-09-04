//////////////////////////////////////////////////////////////////////////
/// \file: CMFCMap.h
/// Copyright (TPRI) 2013 - All Rights Reserved
/// Author:  ZCQ help by HQ.
/// Date:   [2013/07/23]
/// Description:
/// 从MFC代码中拷贝出来的
//////////////////////////////////////////////////////////////////////////
#ifndef __KIMI_KERNEL_MFCMAP_H__
#define __KIMI_KERNEL_MFCMAP_H__

#ifndef ASSERT
#	define ASSERT(exp) ((VOID) 0)
#endif

_KIMI_NAMESPACE_BEGIN

struct __MAP_POSITION
{
};
typedef __MAP_POSITION* MAP_POSITION;

#define BEFORE_START_MAP_POSITION ((MAP_POSITION)-1L)

struct CMapPlex     // warning variable length structure
{
    CMapPlex* pNext;

	void* data() {
		return this + 1;
	}

	static CMapPlex* PASCAL Create(CMapPlex*& pHead, UINT_PTR nMax, UINT_PTR cbElement) {
		ASSERT(nMax > 0 && cbElement > 0);
		if (nMax == 0 || cbElement == 0)
		{
			ASSERT(FALSE);
		}

		CMapPlex* p = (CMapPlex*) new BYTE[sizeof(CMapPlex) + nMax * cbElement];
		// may throw exception
		p->pNext = pHead;
		pHead = p;  // change head (adds in reverse order for simplicity)
		return p;
	}
    // like 'calloc' but no zero fill
    // may throw memory exceptions

	void CMapPlex::FreeDataChain() {    // free this one and links
	
		CMapPlex* p = this;
		while (p != NULL)
		{
			BYTE* bytes = (BYTE*) p;
			CMapPlex* pTemp = p->pNext;
			delete[] bytes;
			p = pTemp;
		}
	}
};

/////////////////////////////////////////////////////////////////////////////
// CMFCMap 通用HASH类型的键值计算
template<class ARG_KEY>
inline UINT __stdcall MapHashKey(ARG_KEY key)
{
 	// default identity hash - works for most primitive values
 	return (DWORD)(((DWORD_PTR)key)>>4);
}

// CMFCMap 特定HASH类型的键值计算---字符串
template<class ARG_KEY>
UINT MapHashKey(const string &key)
{
	const char *k = key.c_str();
	UINT nHash = 0;
	while (*k)
		nHash = (nHash<<5) + nHash + *k++;
	return nHash;
}

// CMFCMap  元素类型比较
template<class TYPE, class ARG_TYPE>
BOOL MapCompareElements(const TYPE* pElement1, const ARG_TYPE* pElement2)
{
	return *pElement1 == *pElement2;
}


//////////////////////////////////CMap1 类型定义/////////////////////////////////////////////

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
class CMFCMap 
{
public:
	// CMapPair
	struct CMapPair
	{
		const KEY key;
		VALUE value;
	protected:
		CMapPair( ARG_KEY keyval ) : key( keyval )	{}
	};

protected:
	// Association
	class CMapAssoc : public CMapPair
	{
		friend class CMFCMap<KEY,ARG_KEY,VALUE,ARG_VALUE>;
		CMapAssoc* pNext;
		UINT nHashValue;  // needed for efficient iteration
	public:
		CMapAssoc( ARG_KEY key ) : CMapPair( key ) {}
	};

public:
	// Construction
	/* explicit */ CMFCMap(INT_PTR nBlockSize = 10);

	// Attributes
	// number of elements
	INT_PTR GetCount() const;
	INT_PTR GetSize() const;
	BOOL IsEmpty() const;

	// Lookup
	BOOL Lookup(ARG_KEY key, VALUE& rValue) const;
	const CMapPair *PLookup(ARG_KEY key) const;
	CMapPair *PLookup(ARG_KEY key);

	// Operations
	// Lookup and add if not there
	VALUE& operator[](ARG_KEY key);

	// add a new (key, value) pair
	void SetAt(ARG_KEY key, ARG_VALUE newValue);

	// removing existing (key, ?) pair
	BOOL RemoveKey(ARG_KEY key);
	void RemoveAll();

	// iterating all (key, value) pairs
	MAP_POSITION GetStartPosition() const;

	const CMapPair *PGetFirstAssoc() const;
	CMapPair *PGetFirstAssoc();

	void GetNextAssoc(MAP_POSITION& rNextPosition, KEY& rKey, VALUE& rValue) const;

	const CMapPair *PGetNextAssoc(const CMapPair *pAssocRet) const;
	CMapPair *PGetNextAssoc(const CMapPair *pAssocRet);

	// advanced features for derived classes
	UINT GetHashTableSize() const;
	void InitHashTable(UINT hashSize, BOOL bAllocNow = TRUE);

	// Implementation
protected:
	CMapAssoc** m_pHashTable;
	UINT m_nHashTableSize;
	INT_PTR m_nCount;
	CMapAssoc* m_pFreeList;
	struct CMapPlex* m_pBlocks;
	INT_PTR m_nBlockSize;

	CMapAssoc* NewAssoc(ARG_KEY key);
	void FreeAssoc(CMapAssoc*);
	CMapAssoc* GetAssocAt(ARG_KEY, UINT&, UINT&) const;

public:
	~CMFCMap();
};


//////////////////////////////////CMFCMap 方法实现/////////////////////////////////////////////

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
inline INT_PTR CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetCount() const
{ return m_nCount; }

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
inline  INT_PTR CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetSize() const
{ return m_nCount; }

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
inline BOOL CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::IsEmpty() const
{ return m_nCount == 0; }

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
inline void CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::SetAt(ARG_KEY key, ARG_VALUE newValue)
{ (*this)[key] = newValue; }

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
inline MAP_POSITION CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetStartPosition() const
{ return (m_nCount == 0) ? NULL : BEFORE_START_MAP_POSITION; }

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
const typename CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CMapPair* CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::PGetFirstAssoc() const
{ 
	ASSERT(this);
	if(m_nCount == 0) return NULL;

	ASSERT(m_pHashTable != NULL);  // never call on empty map

	CMapAssoc* pAssocRet = (CMapAssoc*)BEFORE_START_MAP_POSITION;

	// find the first association
	for (UINT nBucket = 0; nBucket < m_nHashTableSize; nBucket++)
		if ((pAssocRet = m_pHashTable[nBucket]) != NULL)
			break;
	ASSERT(pAssocRet != NULL);  // must find something

	return pAssocRet;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
typename CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CMapPair* CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::PGetFirstAssoc()
{ 
	ASSERT(this);
	if(m_nCount == 0) return NULL;

	ASSERT(m_pHashTable != NULL);  // never call on empty map

	CMapAssoc* pAssocRet = (CMapAssoc*)BEFORE_START_MAP_POSITION;

	// find the first association
	for (UINT nBucket = 0; nBucket < m_nHashTableSize; nBucket++)
		if ((pAssocRet = m_pHashTable[nBucket]) != NULL)
			break;
	ASSERT(pAssocRet != NULL);  // must find something

	return pAssocRet;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
inline UINT CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetHashTableSize() const
{ return m_nHashTableSize; }

/////////////////////////////////////////////////////////////////////////////
// CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE> out-of-line functions

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CMFCMap(INT_PTR nBlockSize)
{
	ASSERT(nBlockSize > 0);

	m_pHashTable = NULL;
	m_nHashTableSize = 17;  // default size
	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks = NULL;
	m_nBlockSize = nBlockSize;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::InitHashTable(
	UINT nHashSize, BOOL bAllocNow)
	//
	// Used to force allocation of a hash table or to override the default
	//   hash table size of (which is fairly small)
{
	ASSERT(this);
	ASSERT(m_nCount == 0);
	ASSERT(nHashSize > 0);

	if (m_pHashTable != NULL)
	{
		// free hash table
		delete[] m_pHashTable;
		m_pHashTable = NULL;
	}

	if (bAllocNow)
	{
		m_pHashTable = new CMapAssoc* [nHashSize];
		// ENSURE(m_pHashTable != NULL);
		memset(m_pHashTable, 0, sizeof(CMapAssoc*) * nHashSize);
	}
	m_nHashTableSize = nHashSize;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::RemoveAll()
{
	ASSERT(this);

	if (m_pHashTable != NULL)
	{
		// destroy elements (values and keys)
		for (UINT nHash = 0; nHash < m_nHashTableSize; nHash++)
		{
			CMapAssoc* pAssoc;
			for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL;
				pAssoc = pAssoc->pNext)
			{
				pAssoc->CMapAssoc::~CMapAssoc();
// 				DestructElements<VALUE>(&pAssoc->value, 1);
// 				DestructElements<KEY>((KEY*)&pAssoc->key, 1);
			}
		}
	}

	// free hash table
	delete[] m_pHashTable;
	m_pHashTable = NULL;

	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks->FreeDataChain();
	m_pBlocks = NULL;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::~CMFCMap()
{
	RemoveAll();
	ASSERT(m_nCount == 0);
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
typename CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CMapAssoc*
CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::NewAssoc(ARG_KEY key)
{
	if (m_pFreeList == NULL)
	{
		// add another block
		CMapPlex* newBlock = CMapPlex::Create(m_pBlocks, m_nBlockSize, sizeof(CMFCMap::CMapAssoc));
		// chain them into free list
		CMFCMap::CMapAssoc* pAssoc = (CMFCMap::CMapAssoc*) newBlock->data();
		// free in reverse order to make it easier to debug
		pAssoc += m_nBlockSize - 1;
		for (INT_PTR i = m_nBlockSize-1; i >= 0; i--, pAssoc--)
		{
			pAssoc->pNext = m_pFreeList;
			m_pFreeList = pAssoc;
		}
	}

    //ENSURE(m_pFreeList != NULL);  // we must have something

	CMFCMap::CMapAssoc* pAssoc = m_pFreeList;

	// zero the memory
	CMFCMap::CMapAssoc* pTemp = pAssoc->pNext;
	memset( pAssoc, 0, sizeof(CMFCMap::CMapAssoc) );
	pAssoc->pNext = pTemp;

	m_pFreeList = m_pFreeList->pNext;
	m_nCount++;
	ASSERT(m_nCount > 0);  // make sure we don't overflow
#pragma push_macro("new")
#undef new
	::new(pAssoc) CMFCMap::CMapAssoc(key);
#pragma pop_macro("new")
	//	ConstructElements<KEY>(&pAssoc->key, 1);
	//	ConstructElements<VALUE>(&pAssoc->value, 1);   // special construct values
	return pAssoc;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::FreeAssoc(CMapAssoc* pAssoc)
{
	pAssoc->CMapAssoc::~CMapAssoc();
	//	DestructElements<VALUE>(&pAssoc->value, 1);
	//	DestructElements<KEY>(&pAssoc->key, 1);
	pAssoc->pNext = m_pFreeList;
	m_pFreeList = pAssoc;
	m_nCount--;
	ASSERT(m_nCount >= 0);  // make sure we don't underflow

	// if no more elements, cleanup completely
	if (m_nCount == 0)
		RemoveAll();
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
typename CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CMapAssoc*
CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetAssocAt(ARG_KEY key, UINT& nHashBucket, UINT& nHashValue) const
// find association (or return NULL)
{
	nHashValue = MapHashKey<ARG_KEY>(key);
	nHashBucket = nHashValue % m_nHashTableSize;

	if (m_pHashTable == NULL)
		return NULL;

	// see if it exists
	CMapAssoc* pAssoc;
	for (pAssoc = m_pHashTable[nHashBucket]; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if (pAssoc->nHashValue == nHashValue && MapCompareElements<KEY, ARG_KEY>(&pAssoc->key, &key))
			return pAssoc;
	}
	return NULL;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
BOOL CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::Lookup(ARG_KEY key, VALUE& rValue) const
{
	ASSERT(this);

	UINT nHashBucket, nHashValue;
	CMapAssoc* pAssoc = GetAssocAt(key, nHashBucket, nHashValue);
	if (pAssoc == NULL)
		return FALSE;  // not in map

	rValue = pAssoc->value;
	return TRUE;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
const typename CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CMapPair* CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::PLookup(ARG_KEY key) const
{
	ASSERT(this);

	UINT nHashBucket, nHashValue;
	CMapAssoc* pAssoc = GetAssocAt(key, nHashBucket, nHashValue);
	return pAssoc;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
typename CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CMapPair* CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::PLookup(ARG_KEY key)
{
	ASSERT(this);

	UINT nHashBucket, nHashValue;
	CMapAssoc* pAssoc = GetAssocAt(key, nHashBucket, nHashValue);
	return pAssoc;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
VALUE& CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::operator[](ARG_KEY key)
{
	ASSERT(this);

	UINT nHashBucket, nHashValue;
	CMapAssoc* pAssoc;
	if ((pAssoc = GetAssocAt(key, nHashBucket, nHashValue)) == NULL)
	{
		if (m_pHashTable == NULL)
			InitHashTable(m_nHashTableSize);

		// ENSURE(m_pHashTable);
		// it doesn't exist, add a new Association
		pAssoc = NewAssoc(key);
		pAssoc->nHashValue = nHashValue;
		//'pAssoc->value' is a constructed object, nothing more

		// put into hash table
		pAssoc->pNext = m_pHashTable[nHashBucket];
		m_pHashTable[nHashBucket] = pAssoc;
	}
	return pAssoc->value;  // return new reference
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
BOOL CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::RemoveKey(ARG_KEY key)
// remove key - return TRUE if removed
{
	ASSERT(this);

	if (m_pHashTable == NULL)
		return FALSE;  // nothing in the table

	UINT nHashValue;
	CMapAssoc** ppAssocPrev;
	nHashValue = MapHashKey<ARG_KEY>(key);
	ppAssocPrev = &m_pHashTable[nHashValue%m_nHashTableSize];

	CMapAssoc* pAssoc;
	for (pAssoc = *ppAssocPrev; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if ((pAssoc->nHashValue == nHashValue) && MapCompareElements<KEY, ARG_KEY>(&pAssoc->key, &key))
		{
			// remove it
			*ppAssocPrev = pAssoc->pNext;  // remove from list
			FreeAssoc(pAssoc);
			return TRUE;
		}
		ppAssocPrev = &pAssoc->pNext;
	}
	return FALSE;  // not found
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetNextAssoc(MAP_POSITION& rNextPosition,
														 KEY& rKey, VALUE& rValue) const
{
	ASSERT(this);
	// ENSURE(m_pHashTable != NULL);  // never call on empty map

	CMapAssoc* pAssocRet = (CMapAssoc*)rNextPosition;
	// ENSURE(pAssocRet != NULL);

	if (pAssocRet == (CMapAssoc*) BEFORE_START_MAP_POSITION)
	{
		// find the first association
		for (UINT nBucket = 0; nBucket < m_nHashTableSize; nBucket++)
		{
			if ((pAssocRet = m_pHashTable[nBucket]) != NULL)
			{
				break;
			}
		}
		// ENSURE(pAssocRet != NULL);  // must find something
	}

	// find next association
	ASSERT(AfxIsValidAddress(pAssocRet, sizeof(CMapAssoc)));
	CMapAssoc* pAssocNext;
	if ((pAssocNext = pAssocRet->pNext) == NULL)
	{
		// go to next bucket
		for (UINT nBucket = (pAssocRet->nHashValue % m_nHashTableSize) + 1;
			nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocNext = m_pHashTable[nBucket]) != NULL)
				break;
	}

	rNextPosition = (MAP_POSITION) pAssocNext;

	// fill in return data
	rKey = pAssocRet->key;
	rValue = pAssocRet->value;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
const typename CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CMapPair*
CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::PGetNextAssoc(const typename CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CMapPair* pPairRet) const
{
	ASSERT(this);

	CMapAssoc* pAssocRet = (CMapAssoc*)pPairRet;

	ASSERT(m_pHashTable != NULL);  // never call on empty map
	ASSERT(pAssocRet != NULL);

	if(m_pHashTable == NULL || pAssocRet == NULL)
		return NULL;

	ASSERT(pAssocRet != (CMapAssoc*)BEFORE_START_MAP_POSITION);

	// find next association
	ASSERT(AfxIsValidAddress(pAssocRet, sizeof(CMapAssoc)));
	CMapAssoc* pAssocNext;
	if ((pAssocNext = pAssocRet->pNext) == NULL)
	{
		// go to next bucket
		for (UINT nBucket = (pAssocRet->nHashValue % m_nHashTableSize) + 1;
			nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocNext = m_pHashTable[nBucket]) != NULL)
				break;
	}

	return pAssocNext;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
typename CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CMapPair*
CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::PGetNextAssoc(const typename CMFCMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CMapPair* pPairRet)
{
	ASSERT(this);

	CMapAssoc* pAssocRet = (CMapAssoc*)pPairRet;

	ASSERT(m_pHashTable != NULL);  // never call on empty map
	ASSERT(pAssocRet != NULL);

	if(m_pHashTable == NULL || pAssocRet == NULL)
		return NULL;

	ASSERT(pAssocRet != (CMapAssoc*)BEFORE_START_MAP_POSITION);

	// find next association
	ASSERT(AfxIsValidAddress(pAssocRet, sizeof(CMapAssoc)));
	CMapAssoc* pAssocNext;
	if ((pAssocNext = pAssocRet->pNext) == NULL)
	{
		// go to next bucket
		for (UINT nBucket = (pAssocRet->nHashValue % m_nHashTableSize) + 1;
			nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocNext = m_pHashTable[nBucket]) != NULL)
				break;
	}

	return pAssocNext;
}

//////////////////////////////////////////////////////////////////////////
/// CStringNoCaseMap - 不区分大小写的字符串Hash表
//////////////////////////////////////////////////////////////////////////


#define MAP_KEY_SIZE	256
#define TO_UPPER_CHAR(a) (((a) >= 'a' && (a) <= 'z') ? (a)-32 : (a))	// ('a'-'A') = 32

// 不区分大小写的Map表
template<class VALUE, class ARG_VALUE>
class CStringNoCaseMap: public CMFCMap<string, const string&, VALUE, ARG_VALUE>
{
public:
	~CStringNoCaseMap() { RemoveAll(); }

	BOOL Lookup(const char* key, VALUE& rValue) const;

	VALUE& operator[](const char* key);
	
	void SetAt(const char*key, ARG_VALUE newValue);

	BOOL RemoveKey(const char*key);

	/// 如果当前Hash表中的槽比放置的元素加上nGrownSize增量值要小，该函数会重新分配一个更大的槽
	BOOL AdjustHashTable(int nGrownSize = 0);

	void UpCase(const char*lpszIn, char* lpszOut) const;
};

template<class VALUE, class ARG_VALUE>
inline BOOL CStringNoCaseMap<VALUE, ARG_VALUE>::Lookup(const char* key, VALUE& rValue) const
{
	char szKey[MAP_KEY_SIZE];
	UpCase(key, szKey);

	return CMFCMap<string, const string, VALUE, ARG_VALUE>::Lookup(szKey, rValue);
}

template<class VALUE, class ARG_VALUE>
inline VALUE& CStringNoCaseMap<VALUE, ARG_VALUE>::operator[](const char* key)
{
	char szKey[MAP_KEY_SIZE];
	UpCase(key, szKey);

	return CMFCMap<string, const string, VALUE, ARG_VALUE>::operator[](szKey);
}

template<class VALUE, class ARG_VALUE>
inline void CStringNoCaseMap<VALUE, ARG_VALUE>::SetAt(const char* key, ARG_VALUE newValue)
{
	char szKey[MAP_KEY_SIZE];
	UpCase(key, szKey);

	CMFCMap<string, const string, VALUE, ARG_VALUE>::SetAt(szKey, newValue);
}

template<class VALUE, class ARG_VALUE>
inline BOOL CStringNoCaseMap<VALUE, ARG_VALUE>::RemoveKey(const char* key)
{
	char szKey[MAP_KEY_SIZE];
	UpCase(key, szKey);

	return CMFCMap<string, const string, VALUE, ARG_VALUE>::RemoveKey(szKey);
}

template<class VALUE, class ARG_VALUE>
BOOL CStringNoCaseMap<VALUE, ARG_VALUE>::AdjustHashTable(int nGrownSize/* = 0*/)
{
	return FALSE;
}

template<class VALUE, class ARG_VALUE>
inline void CStringNoCaseMap<VALUE, ARG_VALUE>::UpCase(const char* lpszIn, char* lpszOut) const
{
	const char* pin = lpszIn;
	char* pout = lpszOut;
	char ch;

	while(ch = *pin++)
		*pout++ = TO_UPPER_CHAR(ch);
	*pout = '\0';
}

_KIMI_NAMESPACE_END

#endif // end of __KIMI_KERNEL_MFCMAP_H__