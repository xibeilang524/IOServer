///////////////////////////////////////////////////////////////////
//   DbProxy目标服务器驱动
//
//   该单元提供格式化的OutputDebugString输出能力，可以使用DbgView
//   调试工具查看输出调试信息
//
//       为了能够发现内存泄露的报告信息，需要在项目中加入如下代码：
//    1. 在StdAfx.h文件中加入
//        #define _crtdbg_map_alloc
//        #include "stdlib.h"
//        #include "crtdbg.h"
//    2. 在CWinApp中重载ExitInstance()函数，并调用如下函数：
//        _CrtDumpMemoryLeaks()
//
//   作者：何清
//   tech
// 
///////////////////////////////////////////////////////////////////
//   更新历史:
// 
//   @2010-03-24
//      创建文档
///////////////////////////////////////////////////////////////////
#ifndef __DEBUG_UTILS_H__
#define __DEBUG_UTILS_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxmt.h>

TRUNK_NAMESPACE_BEGIN

#ifdef _DEBUG

static CCriticalSection __debug_lock;

inline void __DebugOutput(LPCTSTR lpszFormat, ...)
{
	static TCHAR buf[1024];
	
	__debug_lock.Lock();
	va_list args;
	va_start(args, lpszFormat);
#if _MSC_VER > 1200
	vsprintf_s(buf, 1024, lpszFormat, args);
#else
	_vstprintf(buf, lpszFormat, args);
#endif
	va_end(args);
	// 输入换行符，以方便在VC的Output中显示
	int nlen = (int)_tcslen(buf);
	buf[nlen] = _T('\n');
	buf[nlen + 1] = _T('\0');
	
	OutputDebugString(buf);
	__debug_lock.Unlock();
}
#define OutputDebug										::__DebugOutput						
#define OutputDebug0(sz)								::__DebugOutput(_T("%s"), _T(sz))
#define OutputDebug1(sz, p1)							::__DebugOutput(_T(sz), p1)
#define OutputDebug2(sz, p1, p2)						::__DebugOutput(_T(sz), p1, p2)						
#define OutputDebug3(sz, p1, p2, p3)					::__DebugOutput(_T(sz), p1, p2, p3)
#define OutputDebug4(sz, p1, p2, p3, p4)				::__DebugOutput(_T(sz), p1, p2, p3, p4)
#define OutputDebug5(sz, p1, p2, p3, p4, p5)			::__DebugOutput(_T(sz), p1, p2, p3, p4, p5)
#define OutputDebug6(sz, p1, p2, p3, p4, p5, p6)		::__DebugOutput(_T(sz), p1, p2, p3, p4, p5, p6)

#else // _DEBUG

inline void __DebugOutput(LPCTSTR, ...) { }
#define OutputDebug              1 ? (void)0 : ::__DebugOutput
#define OutputDebug0(sz)
#define OutputDebug1(sz, p1)
#define OutputDebug2(sz, p1, p2)
#define OutputDebug3(sz, p1, p2, p3)
#define OutputDebug4(sz, p1, p2, p3, p4)
#define OutputDebug5(sz, p1, p2, p3, p4, p5)
#define OutputDebug6(sz, p1, p2, p3, p4, p5, p6)

#endif // _DEBUG

TRUNK_NAMESPACE_END

#endif // end of __DEBUG_UTILS_H__