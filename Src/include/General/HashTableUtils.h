#ifndef __HASHTABLE_UTILS_H__
#define __HASHTABLE_UTILS_H__

TRUNK_NAMESPACE_BEGIN

_GENERAL_API(int) GetHashTablePrime(int nHashTableSize);
_GENERAL_API(int) CalcHashTableSize(int nDataSize, double dblFactor = 1.2);

TRUNK_NAMESPACE_END

#endif // end of __HASHTABLE_UTILS_H__