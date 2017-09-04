#include "stdafx.h"
#include "KimiCore.h"
#include "KimiCore/Utility/DebugUtils.h"


_KIMI_NAMESPACE_BEGIN

#define _DEBUG_OUTPUT_BUFFER_MAX	1024

#ifdef _DEBUG

void __DebugOutput(LPCTSTR lpszFormat, ...)
{
	TCHAR buf[_DEBUG_OUTPUT_BUFFER_MAX];
	
	va_list args;
	va_start(args, lpszFormat);
#if _MSC_VER > 1200
	_vstprintf_s(buf, _DEBUG_OUTPUT_BUFFER_MAX, lpszFormat, args);
#else
	_vstprintf(buf, lpszFormat, args);
#endif
	va_end(args);

	// 输入换行符，以方便在VC的Output中显示
	size_t nlen = _tcslen(buf);
	if (nlen < _DEBUG_OUTPUT_BUFFER_MAX)
	{
		buf[nlen] = '\n';
		buf[nlen + 1] = '\0';
	}
	
	OutputDebugString(buf);
}

#endif // _DEBUG

_KIMI_NAMESPACE_END
