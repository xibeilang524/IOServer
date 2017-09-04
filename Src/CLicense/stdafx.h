// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

//#define _CRT_SECURE_NO_WARNINGS

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>

#include <atlbase.h>
#include <comutil.h>
#if _MSC_VER > 1200
#include <comdef.h>
#endif

#include <string>

#define _KIMI_CORE_STATICLINK
#include "kimiCore.h"
DEFINE_KIMI_CORE_NAMESPACE;

// TODO: 在此处引用程序需要的其他头文件
