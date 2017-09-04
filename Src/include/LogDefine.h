// LogConstDef.h
#pragma once

// 当前登录用户
#define LOG_LOGIN_USER	(theApp.GetOperator().IsEmpty()? _T("未登录") : theApp.GetOperator())

// 日志格式化文本
#define LOG_ADD_DEVICE_FORMAT	_T("<%s> 添加设备 <%s>")
#define LOG_INSERT_TAG_FORMAT	_T("<%s> 新增标签项 <%s>")
#define LOG_UPDATE_TAG_FORMAT	_T("<%s> 更新标签项 <%s>")
#define LOG_DELETE_TAG_FORMAT	_T("<%s> 删除标签项 <%s>")


#define DEFAULT_LOGFILE_SIZE_MAX	2097152 //最大LOG文件大小2M 

// 日志信息级别
#define LOG_LEVEL_ERROR		0x00	// 错误级别
#define LOG_LEVEL_WARNING	0x01	// 警告级别
#define LOG_LEVEL_INFO		0x02	// 提示级别
#define LOG_LEVEL_DEBUG		0x03	// Debug级别

// 日志操作类型
#define LOG_OPT_DISPLAY		0x01	// 在窗口中显示日志
#define LOG_OPT_STORAGE		0x02	// 写日志到日志文件
#define LOG_OPT_ALL			(LOG_OPT_DISPLAY | LOG_OPT_STORAGE)

#define MAX_LOG_MESSAGE_LEN	1024
/// 最大允许存留的可用日志项
#define DEFAULT_MAX_LOG_FREE_COUNT	1024
