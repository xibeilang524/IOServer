/*
为了安全的使用TPRICommon库，请的在StdAfx.h的VC_EXTRALEAN宏声明后增加以下的宏定义：
 
// 如果必须将位于下面指定平台之前的平台作为目标，请修改下列定义。
// 有关不同平台对应值的最新信息，请参考 MSDN。
#ifndef WINVER				// 允许使用特定于 Windows XP 或更高版本的功能。
#define WINVER 0x0501		// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif

#ifndef _WIN32_WINNT		// 允许使用特定于 Windows XP 或更高版本的功能。
#define _WIN32_WINNT 0x0501	// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif						

#ifndef _WIN32_WINDOWS		// 允许使用特定于 Windows 98 或更高版本的功能。
#define _WIN32_WINDOWS 0x0410 // 将此值更改为适当的值，以指定将 Windows Me 或更高版本作为目标。
#endif

#ifndef _WIN32_IE			// 允许使用特定于 IE 6.0 或更高版本的功能。
#define _WIN32_IE 0x0600	// 将此值更改为相应的值，以适用于 IE 的其他版本。
#endif

*/

#ifndef __KIMI_COMMON_API_H__
#define __KIMI_COMMON_API_H__

// 是否使用名称空间
#define TPRI_USE_NAMESPACE

#ifdef TPRICOMMON_EXPORTS
#define TPRICOMMON_API(x) __declspec(dllexport) x
#define TPRICOMMON_CLASS __declspec(dllexport)
#else
#define TPRICOMMON_API(x) __declspec(dllimport) x
#define TPRICOMMON_CLASS __declspec(dllimport)

#ifndef _LINK_TO_UG_IN_EXTDLL
#define _LINK_TO_UG_IN_EXTDLL	// for Ultimate Grid
#endif

#endif

// 名称空间宏定义
#ifdef TPRI_USE_NAMESPACE
	#define _KIMI_NAMESPACE__BEGIN namespace KIMI { namespace COMMON {
	#define _KIMI_NAMESPACE__END }}
	#define DEFINE_TPRICOMMON_NAMESPACE using namespace KIMI::COMMON
#else
	#define _KIMI_NAMESPACE__BEGIN
	#define _KIMI_NAMESPACE__END
	#define DEFINE_TPRICOMMON_NAMESPACE
#endif

#pragma warning(disable : 4819) // warning C4819: 该文件包含不能在当前代码页(936)中表示的字符。请将该文件保存为 Unicode 格式以防止数据丢失

#include "CGeneral\Ultimate Grid\UGCtrl.h"

#include "CGeneral\Ultimate Grid\UGCTafnt.h"
#include "CGeneral\Ultimate Grid\UGCTAutoSize.h"
#include "CGeneral\Ultimate Grid\UGCTbutn.h"
#include "CGeneral\Ultimate Grid\UGCTDropGrid.h"
#include "CGeneral\Ultimate Grid\UGCTdtp.h"
#include "CGeneral\Ultimate Grid\ugctelps.h"
#include "CGeneral\Ultimate Grid\UGCTExpand.h"
#include "CGeneral\Ultimate Grid\UGCTLabeled.h"
#include "CGeneral\Ultimate Grid\UGCTMail.h"
#include "CGeneral\Ultimate Grid\UGCTMailSort.h"
#include "CGeneral\Ultimate Grid\UGCTMarquee.h"
#include "CGeneral\Ultimate Grid\UGCTmfnt.h"
#include "CGeneral\Ultimate Grid\UGCTNote.h"
#include "CGeneral\Ultimate Grid\ugctnotewnd.h"
#include "CGeneral\Ultimate Grid\UGCTOutlookHeader.h"
#include "CGeneral\Ultimate Grid\UGCTpie.h"
#include "CGeneral\Ultimate Grid\UGCTpro1.h"
#include "CGeneral\Ultimate Grid\UGCTprog.h"
#include "CGeneral\Ultimate Grid\UGCTRado.h"
#include "CGeneral\Ultimate Grid\UGCTsarw.h"
#include "CGeneral\Ultimate Grid\UGCTsldr.h"
#include "CGeneral\Ultimate Grid\UGCTSpin.h"
#include "CGeneral\Ultimate Grid\UTEdit.h"
#include "CGeneral\Ultimate Grid\UTMaskedEdit.h"

#ifndef TPRICOMMON_EXPORTS
///{{ 这里包含模板类头文件

#ifndef __KIMI_NOCASEMAP_H__
#include "CGeneral\NoCaseMap.h"
#endif

///}}
#endif

///{{ 这里包含一些必要的头文件

#ifndef __KIMI_SYSTEMTRAY_H__
#include "CGeneral\SystemTray.h"
#endif

#ifndef __KIMI_SORTOBARRAY_H__
#include "CGeneral\SortObArray.h"
#endif
#ifndef __KIMI_MEMDC_H__
#include "CGeneral\MemDC.h"
#endif

#ifndef __KIMI_WINLIST_H__
#include "CGeneral\WinList.h"
#endif

#ifndef __KIMI_TPRIUGDATASOURCE_H__
#include "CGeneral\TPRIUGDataSource.h"
#endif

#ifndef __KIMI_TPRIUGDGRID_H__
#include "CGeneral\TPRIUGGrid.h"
#endif

#ifndef __SAFELOCK_H__
#include "CGeneral\safelock.h"
#endif
///}}

///{{ 这里声明一些常用的简单类或inline方法
_KIMI_NAMESPACE__BEGIN

_KIMI_NAMESPACE__END
///}}

// 链接动态库声明，如果是DEBUG模式，则链接TPRICommonD.lib，Release模式则链接TPRICommon.lib
#ifndef TPRICOMMON_EXPORTS
	#ifdef _DEBUG
		#pragma comment( lib, "CGenerald.lib")
	#else
		#pragma comment( lib, "CGeneral.lib")
	#endif
#endif

#endif // end of __KIMI_COMMON_API_H__