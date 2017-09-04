/*
 * DrvVersion.h 驱动配置版本控制宏定义
 *
 *  所有驱动增加驱动类型标志和版本号标志，
 */
#pragma once

// 驱动类型标志 － DWORD类型，不使用CString作为驱动类型标志，是因为如果提供不合法的
//                 串行化数据时，加载CString类型可能会出现异常，而DWORD类型一般不容易出现异常
#define DRVCFG_FLAG_OPCCLIENT			0x00000001		// OPC驱动
#define DRVCFG_FLAG_DBPROXY				0x00000002		// 目标服务器驱动
#define DRVCFG_FLAG_ODBC				0x00000003		// ODBC驱动
#define DRVCFG_FLAG_MODBUS				0x00000004		// Modbus驱动

#define DRVCFG_FLAG_USER				0x0000FFFF		// 使用开发驱动从该值起定义，并且尽量不要重复
														// 用户开发驱动时，可以先定义：
														// #define DRVCFG_FLAG_MYDRV	(DRVCFG_FLAG_USER + 100)
														// 然后就可以使用该驱动类型标志了	

// 版本号标志 － WORD类型，可以从0开始，每升级一次版本，版本号值加一，也可以自己定义，
//               比如WORD高字节为主版本号，低字符为副版本号
#define DRVCFG_VERSION_USER				0x0000			// 版本号基值