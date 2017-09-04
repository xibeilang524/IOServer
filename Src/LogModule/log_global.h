#pragma once

#ifdef _LOG_EXPORTS

#define _LOG_API(x) __declspec(dllexport) x
#define _LOG_CLASS __declspec(dllexport)

#else

#define _LOG_API(x) __declspec(dllimport) x
#define _LOG_CLASS __declspec(dllimport)

#endif