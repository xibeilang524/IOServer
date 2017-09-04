#include "StdAfx.h"
#include <process.h>
#include "GeneralAPI.h"

TRUNK_NAMESPACE_BEGIN

DWORD g_UserDataTLS = 0xffffffff;

_GENERAL_API(void) TlsInitThreadSlodData()
{
	if (g_UserDataTLS == 0xffffffff)
		g_UserDataTLS = ::TlsAlloc();
}

_GENERAL_API(void) TlsFreeThreadSlodData()
{
	if (g_UserDataTLS != 0xffffffff)
		::TlsFree(g_UserDataTLS);
}

_GENERAL_API(void) TlsSetThreadSlodData(LPVOID lpszData)
{
	::TlsSetValue(g_UserDataTLS, lpszData);
}

_GENERAL_API(LPVOID) TlsGetThreadSlodData()
{
	return ::TlsGetValue(g_UserDataTLS);
}

TRUNK_NAMESPACE_END