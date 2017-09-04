/*
为了安全的使用GENERAL库，请的在StdAfx.h的VC_EXTRALEAN宏声明后增加以下的宏定义：
 
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

#ifndef __GENERAL_API_H__
#define __GENERAL_API_H__

// 是否使用名称空间
#define TRUNK_USE_NAMESPACE

#ifdef _GENERAL_EXPORTS
#define _GENERAL_API(x) __declspec(dllexport) x
#define _GENERAL_CLASS __declspec(dllexport)
#else
#define _GENERAL_API(x) __declspec(dllimport) x
#define _GENERAL_CLASS __declspec(dllimport)

#ifndef _LINK_TO_UG_IN_EXTDLL
#define _LINK_TO_UG_IN_EXTDLL	// for Ultimate Grid
#endif

#endif

// 名称空间宏定义
#ifdef TRUNK_USE_NAMESPACE
	#define TRUNK_NAMESPACE_BEGIN namespace TRUNK{
	#define TRUNK_NAMESPACE_END }
	#define DEFINE_TRUNK_NAMESPACE using namespace TRUNK
#else
	#define TRUNK_NAMESPACE_BEGIN
	#define TRUNK_NAMESPACE_END
	#define DEFINE_TRUNK_NAMESPACE
#endif

#pragma warning(disable : 4819) // warning C4819: 该文件包含不能在当前代码页(936)中表示的字符。请将该文件保存为 Unicode 格式以防止数据丢失

#include "General\Ultimate Grid\UGCtrl.h"

#include "General\Ultimate Grid\UGCTafnt.h"
#include "General\Ultimate Grid\UGCTAutoSize.h"
#include "General\Ultimate Grid\UGCTbutn.h"
#include "General\Ultimate Grid\UGCTDropGrid.h"
#include "General\Ultimate Grid\UGCTdtp.h"
#include "General\Ultimate Grid\ugctelps.h"
#include "General\Ultimate Grid\UGCTExpand.h"
#include "General\Ultimate Grid\UGCTLabeled.h"
#include "General\Ultimate Grid\UGCTMail.h"
#include "General\Ultimate Grid\UGCTMailSort.h"
#include "General\Ultimate Grid\UGCTMarquee.h"
#include "General\Ultimate Grid\UGCTmfnt.h"
#include "General\Ultimate Grid\UGCTNote.h"
#include "General\Ultimate Grid\ugctnotewnd.h"
#include "General\Ultimate Grid\UGCTOutlookHeader.h"
#include "General\Ultimate Grid\UGCTpie.h"
#include "General\Ultimate Grid\UGCTpro1.h"
#include "General\Ultimate Grid\UGCTprog.h"
#include "General\Ultimate Grid\UGCTRado.h"
#include "General\Ultimate Grid\UGCTsarw.h"
#include "General\Ultimate Grid\UGCTsldr.h"
#include "General\Ultimate Grid\UGCTSpin.h"
#include "General\Ultimate Grid\UTEdit.h"
#include "General\Ultimate Grid\UTMaskedEdit.h"

#ifndef _GENERAL_EXPORTS
///{{ 这里包含模板类头文件

#ifndef _NOCASEMAP_H__
#include "General\NoCaseMap.h"
#endif

#ifndef __SINGLETON_H__
#include "General\DesignPatterns\Singleton.h"
#endif

///}}
#endif

///{{ 这里包含一些必要的头文件

//如果使用了Xtreme ToolkitPro控件，则声明__NOT_USE_CRT_HELPER宏来禁止CRT函数辅助头文件
#ifndef __NOT_USE_CRT_HELPER

#ifndef __CRT_API_HELPER_H__
#include "General\CrtApiHelper.h"
#endif

#endif

//#ifndef __THEME_UTIL_H__
//#include "General\ThemeUtil.h"
//#endif

#ifndef __DEBUG_UTILS_H__
#include "General\DebugUtils.h"
#endif

#ifndef __TIME_UTILS_H__
#include "General\TimeUtils.h"
#endif

#ifndef __HASHTABLE_UTILS_H__
#include "General\HashTableUtils.h"
#endif

#ifndef __UTILITY_H__
#include "General\Utility.h"
#endif

#ifndef __MD5_H__
#include "General\md5.h"
#endif

#ifndef __SAFELOCK_H__
#include "General\safelock.h"
#endif

#ifndef __REGEX_H__
#include "General\regex.h"
#endif

#ifndef __READINI_H__
#include "General\readini.h"
#endif

#ifndef __SYSTEMTRAY_H__
#include "General\SystemTray.h"
#endif

#ifndef __SYSTEMINFO_H__
#include "General\SystemInfo.h"
#endif

#ifndef __CSVFILE_H__
#include "General\CSVFile.h"
#endif

#ifndef __FILEEXPLORER_H__
#include "General\FileExplorer.h"
#endif


#ifndef __SORTOBARRAY_H__
#include "General\SortObArray.h"
#endif

#ifndef __SIZECBAR_H__
#include "General\Sizecbar.h"
#endif

#ifndef __TRUECOLOR_TOOLBAR_H__
#include "General\TrueColorToolbar.h"
#endif

#ifndef __LISTBOXEX_H__
#include "General\ListBoxEx.h"
#endif

#ifndef __MDITABS_H__
#include "General\MDITabs.h"
#endif

#ifndef __TABSHEET_H__
#include "General\TabSheet.h"
#endif

#ifndef __SAFESTRINGLIST_H__
#include "General\SafeStringList.h"
#endif

#ifndef __THREAD_H__
#include "General\Thread.h"
#endif

#ifndef __THREAD_TLS_H__
#include "General\ThreadTLS.h"
#endif

#ifndef __MEMDC_H__
#include "General\MemDC.h"
#endif

#ifndef __WINLIST_H__
#include "General\WinList.h"
#endif

#ifndef __LOG_WINLIST_H__
#include "General\LogWinList.h"
#endif

#ifndef __MYBYTEARRAY_H__
#include "General\MyByteArray.h"
#endif

#ifndef __CCLIPBOARD_H__
#include "General\Clipboard.h"
#endif

#ifndef __COLORLISTBOX_H__
#include "General\ColorListBox.h"
#endif

#ifndef __HYPERLINK_H__
#include "General\HyperLink.h"
#endif

#ifndef __RESIZESHEET_H__
#include "General\ResizeSheet.h"
#endif

#ifndef __TPRIUGDATASOURCE_H__
#include "General\TPRIUGDataSource.h"
#endif

#ifndef __TPRIUGDGRID_H__
#include "General\TPRIUGGrid.h"
#endif

#ifndef __NPIPE_H__
#include "General\NamedPipe.h"
#endif

#ifndef __RWCRITICALSECTION_H__
#include "General\RWCriticalSection.h"
#endif

#ifndef __DRAW_HTML_H__
#include "General\DrawHTML.h"
#endif

#ifndef __HTML_LIST_CTRL_H__
#include "General\HTMLListCtrl.h"
#endif

#ifndef __EDITNUMBER_H__
#include "General\EditNumber.h"
#endif
//
//#ifndef __CUSTOM_TAB_CTRL_H__
//#include "General\CustomTabCtrl.h"
//#endif
//
#ifndef __LISTOPTIONSCTRL_H__
#include "General\ListOptionsCtrl.h"
#endif

#ifndef __PARAMETER_STRING_PARSER_H__
#include "General\ParameterStringParser.h"
#endif
///}}

///{{ 这里声明一些常用的简单类或inline方法
TRUNK_NAMESPACE_BEGIN

TRUNK_NAMESPACE_END
///}}

// 链接动态库声明，如果是DEBUG模式，则链接GeneralD.lib，Release模式则链接GeneralD.lib
#ifndef _GENERAL_EXPORTS
	#ifdef _DEBUG
		#pragma comment( lib, "GeneralD.lib")
	#else
		#pragma comment( lib, "General.lib")
	#endif
#endif

#endif // end of __GENERAL_API_H__