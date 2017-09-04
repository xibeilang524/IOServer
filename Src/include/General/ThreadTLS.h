#ifndef __THREAD_TLS_H__
#define __THREAD_TLS_H__

TRUNK_NAMESPACE_BEGIN

/// 初始化线程TLS存储槽
_GENERAL_API(void) TlsInitThreadSlodData();

/// 释放线程TLS存储槽
_GENERAL_API(void) TlsFreeThreadSlodData();

/// 设置数据到线程TLS存储槽
_GENERAL_API(void) TlsSetThreadSlodData(LPVOID lpszData);

/// 从线程TLS存储槽中读取数据
_GENERAL_API(LPVOID) TlsGetThreadSlodData();

TRUNK_NAMESPACE_END

#endif // end of __THREAD_TLS_H__