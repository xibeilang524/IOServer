// HashTableUtils.cpp : 辅助函数
//

#include "stdafx.h"
#include "GeneralAPI.h"
#include "General\HashTableUtils.h"

TRUNK_NAMESPACE_BEGIN

// 素数表，基于素数初始化Hashtable可以优化搜索性能
static int hashtable_primes[] = {
	3, 7, 11, 17, 23, 29, 37, 47, 59, 71, 89, // 不需要太小的Hash表大小值，意义不大
	107, 131, 163, 197, 239, 293, 353, 431, 521, 631, 761, 919, 
	1103, 1327, 1597, 1931, 2333, 2801, 3371, 4049, 4861, 5839, 7013, 8419, 10103, 12143, 14591, 		
	17519, 21023, 25229, 30293, 36353, 43627, 52361, 62851, 75431, 90523, 108631, 130363, 156437,
	187751, 225307, 270371, 324449, 389357, 467237,	560689, 672827, 807403, 968897, 1162687, 1395263,	
	1674319, 2009191, 2411033, 2893249, 3471899, 4166287, 4999559, 5999471, 7199369
};
	

_GENERAL_API(int) GetHashTablePrime(int nHashTableSize)
{
	if (nHashTableSize < 0)
		return 107;

	for (int i = 0; i < sizeof(hashtable_primes)/sizeof(int); i++) 		
	{		
		if (hashtable_primes[i] >= nHashTableSize)
			return hashtable_primes[i];
	}

	return 156437;
}

_GENERAL_API(int) CalcHashTableSize(int nDataSize, double dblFactor/* = 1.2*/)
{
	if (dblFactor < 1.0)
		dblFactor = 1.0;

	int nHashSize = (int)(nDataSize * dblFactor);
	nHashSize = GetHashTablePrime(nHashSize);
	return nHashSize;
}

TRUNK_NAMESPACE_END