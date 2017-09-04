#ifndef __KIMI_KERNEL_RANDOM_H__
#define __KIMI_KERNEL_RANDOM_H__

_KIMI_NAMESPACE_BEGIN

#ifndef __RTL_GENRANDOM
#define __RTL_GENRANDOM 1
typedef BOOLEAN (_stdcall* RtlGenRandomFunc)(void* pRandomBuffer, ULONG ulRandomBufferLength);
#endif

class KIMI_CORE_CLASS Random
{
public:
	static bool GetBuffer(void* pRandomBuffer, ULONG ulRandomBufferLength);

	static int GetInt32();

	static int GetInt32(int nMin, int nMax);

	static __int64 GetInt64();

	static __int64 GetInt64(__int64 nMin, __int64 nMax);

	static float GetFloat32();

	static float GetFloat32(float fltMin, float fltMax);

	static double GetFloat64();

	static double GetFloat64(double dblMin, double dblMax);
};

_KIMI_NAMESPACE_END

#endif // end of __KIMI_KERNEL_RANDOM_H__