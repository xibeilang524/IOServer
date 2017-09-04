#pragma once

#ifdef _DRIVERMGR_EXPORTS

#define _DRIVERMGR_API(x) __declspec(dllexport) x
#define _DRIVERMGR_CLASS __declspec(dllexport)

#else

#define _DRIVERMGR_API(x) __declspec(dllimport) x
#define _DRIVERMGR_CLASS __declspec(dllimport)

#endif