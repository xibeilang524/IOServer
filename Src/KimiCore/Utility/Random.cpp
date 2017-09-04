// Utility.cpp : ¸¨Öúº¯Êı
//

#include "stdafx.h"
#include "KimiCore.h"
#include "Utility\Random.h"


_KIMI_NAMESPACE_BEGIN

static RtlGenRandomFunc RtlGenRandom = NULL;

#pragma warning(push)
#pragma warning(disable : 4800)
bool Random::GetBuffer(void* pRandomBuffer, ULONG ulRandomBufferLength)
{
	if (RtlGenRandom == NULL) {
		// load proc if not loaded
		HMODULE lib = LoadLibraryA("advapi32.dll");
		RtlGenRandom = (RtlGenRandomFunc)GetProcAddress(lib, "SystemFunction036");
		if (RtlGenRandom == NULL) return 1;
	}

	return RtlGenRandom(pRandomBuffer, ulRandomBufferLength);
}
#pragma warning(pop)

int Random::GetInt32()
{
	unsigned int x = 0;
	GetBuffer(&x, sizeof(unsigned int));

	return (int)(x >> 1);
}

int Random::GetInt32(int nMin, int nMax)
{
	if (nMin >= nMax)
		return   nMin;  

	return nMin + GetInt32() % ( nMax - nMin + 1);  
}

__int64 Random::GetInt64()
{
	ULONGLONG x = 0;
	GetBuffer(&x, sizeof(ULONGLONG));

	return (__int64)(x >> 1);
}

__int64 Random::GetInt64(__int64 nMin, __int64 nMax)
{
	if (nMin >= nMax)
		return   nMin;  

	return nMin + GetInt64() % ( nMax - nMin + 1);  
}


float Random::GetFloat32()
{
	float x = 0;

	GetBuffer(&x, sizeof(float));

	return (x < 0) ? -x : x;
}

float Random::GetFloat32(float fltMin, float fltMax)
{ 
	int minInteger = (int)(fltMin * 100); 
	int maxInteger = (int)(fltMax * 100); 

	int randInteger = GetInt32();

	int diffInteger = maxInteger - minInteger; 

	int resultInteger = randInteger % diffInteger + minInteger; 

	return (float)(resultInteger / 100.0);
}

double Random::GetFloat64()
{
	double x = 0;

	GetBuffer(&x, sizeof(double));

	return (x < 0) ? -x : x;
}

double Random::GetFloat64(double dblMin, double dblMax)
{
	__int64 minInteger = (__int64)(dblMin * 10000); 
	__int64 maxInteger = (__int64)(dblMax * 10000); 

	__int64 randInteger = GetInt64();
	
	__int64 diffInteger = maxInteger - minInteger; 

	__int64 resultInteger = randInteger % diffInteger + minInteger; 

	return resultInteger / 10000.0;
}

_KIMI_NAMESPACE_END