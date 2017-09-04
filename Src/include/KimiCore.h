/*
 * 可用宏定义：
 *		_KIMI_CORE_STATICLINK		- 以静态库方便链接
 *		_KIMI_CORE_NOT_VC_RUNTIME	- 增加/M参数链接，输出文件不需要VC2008运行库
 */
#ifndef __KIMI_CORE_H__
#define __KIMI_CORE_H__

#ifndef AFX_STATIC
#define AFX_STATIC static
#endif

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <objbase.h>

using namespace std;

// 是否使用名称空间
#define KIMI_CORE_USE_NAMESPACE

// 名称空间宏定义

#ifdef _KIMI_CORE_EXPORTS
#	ifdef _KIMI_CORE_STATICLINK
#		define KIMI_CORE_API(x) x
#		define KIMI_CORE_CLASS
#	else
#		define KIMI_CORE_API(x) __declspec(dllexport) x
#		define KIMI_CORE_CLASS __declspec(dllexport)
#	endif
#else
#	ifdef _KIMI_CORE_STATICLINK
#		define KIMI_CORE_API(x) x
#		define KIMI_CORE_CLASS
#	else
#		define KIMI_CORE_API(x) __declspec(dllimport) x
#		define KIMI_CORE_CLASS __declspec(dllimport)
#	endif
#endif


#ifdef KIMI_CORE_USE_NAMESPACE
#	define _KIMI_NAMESPACE_BEGIN namespace KIMI { namespace KimiCore {
#	define _KIMI_NAMESPACE_END }}
#	define DEFINE_KIMI_CORE_NAMESPACE using namespace KIMI::KimiCore
#else
#	define _KIMI_NAMESPACE_BEGIN
#	define _KIMI_NAMESPACE_END
#	define DEFINE_KIMI_CORE_NAMESPACE
#endif

#ifndef _KIMI_CORE_EXPORTS
///{{ 这里包含模板类头文件

#ifndef __KIMI_CORE_MFCMAP_H__
#include "KimiCore\MFCMap.h"
#endif

#ifndef __KIMI_CORE_DYNAMIC_ARRAY_H__
#include "KimiCore\stream\DynamicArray.h"
#endif

#ifndef __KIMI_CORE_BUFFERPTR_H__
#include "KimiCore\SmartPtr\BufferPtr.h"
#endif

#ifndef __KIMI_CORE_PRIVATEHEAP_H__
#include "KimiCore\memory\PrivateHeap.h"
#endif

#ifndef __KIMI_CORE_LOCK_GUARD_H__
#include "KimiCore\lock\LockGuard.h"
#endif

///}}
#endif

#ifndef __KIMI_CORE_TIME_COUNTER_H__
#include "KimiCore\Utility\TimeCounter.h"
#endif

#ifndef __KIMI_CORE_DEBUG_UTILS_H__
#include "KimiCore\Utility\DebugUtils.h"
#endif

#ifndef __KIMI_CORE_RANDOM_H__
#include "KimiCore\Utility\Random.h"
#endif

#ifndef __KIMI_CORE_UTILITY_H__
#include "KimiCore\Utility\Utility.h"
#endif

#ifndef __KIMI_CORE_MD5_H__
#include "KimiCore\MD5.h"
#endif

#ifndef TINYXML_INCLUDED
#define TIXML_USE_STL
#include "KimiCore\tinyxml.h"
#endif

#ifndef __KIMI_CORE_STL_STRING_H__
#include "KimiCore\stl_string.h"
#endif

#ifndef __KIMI_CORE_BASE64_H__
#include "KimiCore\Crypto\Base64.h"
#endif

#ifndef __KIMI_CORE_RIJNDAEL_H__
#include "KimiCore\Crypto\Rijndael.h"
#endif

#ifndef __KIMI_CORE_MEMORY_STREAM_H__
#include "KimiCore\stream\MemoryStream.h"
#endif

#ifndef __KIMI_CORE_DATA_COMPRESSOR_H__
#include "KimiCore\DataCompressor.h"
#endif

#ifndef __KIMI_CORE_FAST_LOCK_H__
#include "KimiCore\lock\FastLock.h"
#endif

#ifndef __KIMI_CORE_SPIN_LOCK_H__
#include "KimiCore\lock\SpinLock.h"
#endif

#ifndef __KIMI_CORE_SPIN_LOCK_H__
#include "KimiCore\lock\SpinLock.h"
#endif

#ifndef __KIMI_CORE_MUTEX_LOCK_H__
#include "KimiCore\lock\MutexLock.h"
#endif

#ifndef __KIMI_CORE_RW_LOCK_H__
#include "KimiCore\lock\RWLock.h"
#endif

#ifndef __KIMI_CORE_CONDVAR_LOCK_H__
#include "KimiCore\lock\CondVarLock.h"
#endif

#ifndef __KIMI_CORE_THREAD_H__
#include "KimiCore\Thread\Thread.h"
#endif

#ifndef __KIMI_CORE_IOCP_THREAD_POOL_H__
#include "KimiCore\Thread\IOCPThreadPool.h"
#endif

#ifdef _KIMI_CORE_STATICLINK

#ifndef __KIMI_CORE_HARDWARE_SERIAL_NUMBER_H__
#include "KimiCore\License\HardwareSerialNumber.h"
#endif

#ifndef __KIMI_CORE_MACHINE_CODE_HELPER_H__
#include "KimiCore\License\MachineCodeHelper.h"
#endif

#ifndef __KIMI_CORE_LICENSE_FILE_H__
#include "KimiCore\License\LicenseFile.h"
#endif

#endif // end of _KIMI_CORE_STATICLINK

///{{ 这里声明一些常用的简单类或inline方法
_KIMI_NAMESPACE_BEGIN

/// COM自动初始化对象
class CoInitializeHelper
{
public:
	CoInitializeHelper(DWORD dwCoInit = COINIT_APARTMENTTHREADED) {
		//::CoInitialize(NULL);
		::CoInitializeEx(NULL, dwCoInit);
	}
	~CoInitializeHelper() {
		::CoUninitialize();
	}
};

_KIMI_NAMESPACE_END
///}}

#ifndef _KIMI_CORE_EXPORTS
#	ifdef _KIMI_CORE_STATICLINK
#		ifdef _DEBUG
#			pragma comment( lib, "KimiCore_staticD.lib")
#		else
#			ifdef _KIMI_CORE_NOT_VC_RUNTIME
#				pragma comment( lib, "KimiCore_staticS.lib")
#			else
#				pragma comment( lib, "KimiCore_static.lib")
#			endif
#		endif
#	else
#		ifdef _DEBUG
#			pragma comment( lib, "KimiCoreD.lib")
#		else
#			pragma comment( lib, "KimiCore.lib")
#		endif
#	endif
#endif

#endif // end of __KIMI_CORE_H__