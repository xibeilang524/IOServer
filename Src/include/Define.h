#pragma once

#define TGF_NAME	0 //标签名
#define TGF_NAMEDB	1 //实时库标签名
#define TGF_DES		2 //描述
#define TGF_UNIT	3 //单位
#define TGF_TYPE	4 //数据类型

#define TGF_NAMEDRV 5 //驱动中标签名
#define TGF_CATYPE  6 //缓存压缩方式
#define TGF_CAVAL   7//缓存压缩精度

#define TGF_TIME	8 //时标
#define TGF_VAL		9 //当前值
#define TGF_QA		10 //质量
#define TGF_EXPCP	11 //转换


#define CASHE_COMP_NONE		0 //不压缩
#define CASHE_COMP_PER		1 //百分比压缩
#define CASHE_COMP_VAL		2 //绝对值压缩

#define WDDS_OD_LOGIN		0x10 //登录命令
#define WDDS_OD_REALVAL		0x12 //写基本类型实时
#define WDDS_OD_REALVAR		0x13 //写可变类型实时

#define WDDS_OD_CASHEVAL	0x14 //写基本类型cashe
#define WDDS_OD_CASHEVAR	0x15 //写可变类型cashe
#define WDDS_OD_SOE			0x16  //写SOE

#define WDDS_OD_TAGS		0x17	//发送标签
#define WDDS_OD_EMPTY		0x18   //空命令
#define WDDS_OD_PING		0x19	//用于维持UDP连接的PING指令
#define WDDS_OD_STREAM_MESSAGE 0x1A // 流消息指令

#define WDDS_OD_LOGIN		0x10 //登录命令
#define WDDS_OD_REALVAL		0x12 //写基本类型实时
#define WDDS_OD_REALVAR		0x13 //写可变类型实时

#define WDDS_OD_CASHEVAL	0x14 //写基本类型cashe
#define WDDS_OD_CASHEVAR	0x15 //写可变类型cashe
#define WDDS_OD_SOE			0x16  //写SOE

#define WDDS_OD_TAGS		0x17	//发送标签
#define WDDS_OD_EMPTY		0x18   //空命令
#define WDDS_OD_PING		0x19	//用于维持UDP连接的PING指令
#define WDDS_OD_STREAM_MESSAGE 0x1A // 流消息指令

#define  MAX_DBP_CONARGS 8			//同时最多8个DDS服务器

#define NET_MODE_TCP		0
#define NET_MODE_UDP		1

typedef struct t_dbparg  //目标服务器连接参数
{
	char	sip[16];    //服务器IP
	char	suser[16];	//登录账号
	char	spass[16];  //登录密码
	WORD	wport;		//端口
	short	shNetMode;		//0-TCP，1-UDP
	char	sres[4];	//保留
}TDBPARG,*LPDBPARG;	//sizeof() = 56

// TDBPARG& operator = (const TDBPARG& B)
// {
// 	this->shNetMode = B.shNetMode;
// 	this->wport = B.wport;
// 	memcpy_s(this->sip, sizeof(this->sip), B.sip, sizeof(B.sip));
// 	memcpy_s(this->suser, sizeof(this->suser), B.suser, sizeof(B.suser));
// 	memcpy_s(this->spass, sizeof(this->spass), B.spass, sizeof(B.spass));
// 	memcpy_s(this->sres, sizeof(this->sres), B.sres, sizeof(B.sres));
// }
// 当前登录用户
//#define LOG_LOGIN_USER	(theApp.m_opt.m_szOpt.IsEmpty()?_T("未登录"):theApp.m_opt.m_szOpt)
//#define LOG_LOGIN_USER	_T(" ssss ")
//
//// 日志格式化文本
//#define LOG_ADD_DEVICE_FORMAT	_T("<%s> 添加设备 <%s>")
//#define LOG_INSERT_TAG_FORMAT	_T("<%s> 新增标签项 <%s>")
//#define LOG_UPDATE_TAG_FORMAT	_T("<%s> 更新标签项 <%s>")
//#define LOG_DELETE_TAG_FORMAT	_T("<%s> 删除标签项 <%s>")
//
//#define DEFAULT_LOGFILE_SIZE_MAX	2097152 //最大LOG文件大小2M 
//
//// 日志信息级别
//#define LOG_LEVEL_ERROR		0x00	// 错误级别
//#define LOG_LEVEL_WARNING	0x01	// 警告级别
//#define LOG_LEVEL_INFO		0x02	// 提示级别
//#define LOG_LEVEL_DEBUG		0x03	// Debug级别
//
//// 日志操作类型
//#define LOG_OPT_DISPLAY		0x01	// 在窗口中显示日志
//#define LOG_OPT_STORAGE		0x02	// 写日志到日志文件
//#define LOG_OPT_ALL			(LOG_OPT_DISPLAY | LOG_OPT_STORAGE)
//
//#define MAX_LOG_MESSAGE_LEN	1024
///// 最大允许存留的可用日志项
//#define DEFAULT_MAX_LOG_FREE_COUNT	1024

//#define NODE_DRVROOT	0	// 驱动管理根节点
#define NODE_WAP		1	// 驱动节点
#define NODE_DEC		2	// 设备节点

typedef struct {
	int nType;			//节点类型
	//DWORD dwtData;		//节点数据指针
	char sName[128];	//结点名字
}NODEDATA,*PNODEDATA;

#define NODE_ROOT		0	// 驱动管理根节点
#define NODE_DRVROOT	1	// 驱动管理根节点
#define NODE_RDB		2	// 数据库实例节点
#define NODE_MODULE		3	// dll节点
#define NODE_CLIENT		4   // 客户连接
#define NODE_TAGTAB		5	// 标签表
#define NODE_SAFE_ROOT  6	// 安全根
#define NODE_SAFE_ACTOR 7	// 角色
#define NODE_SAFE_USER	8	// 账号
#define NODE_SAFE_TRUST	9	// 账号
#define NODE_ABOUT		10	// 关于
#define NODE_RUNINFO	11	// 运行信息

//权限
#define DS_POWERMAN		0x00000001 //账号管理
#define DS_SYSMAN		0x00000002 //系统管理,数据库管理，标签管理
#define DS_WRITE		0x00000004 //写权限
#define DS_READ			0x00000008 //读权限

#define ERR_DRV_OK	0	//设备创建错误
#define ERR_DRV_EXIST	-1 //设备存在
#define ERR_DRV_CREATE  -2 //设备创建错误
#define ERR_DRV_LOAD	-3 //设备创建错误
#define ERR_DRV_NO	-4 //设备创建错误

typedef struct _DR_INFO //驱动信息
{
	CString szName; 	//驱动名字
	CString szDes;		//描述
	CString szVer;		//驱动程序版本
	DWORD dwVer;		//驱动规范版本，遵循的驱动规范版本
	DWORD dwRes;		//保留
	CString szRes;		//保留
}T_DRVINFO1,*LPDRVINFO1;	//sizeof() == 256



typedef struct _DRV_DES //驱动信息
{
	CString szName; 	//驱动名字
	CString szModule;	//驱动dll文件名
	long	lID;		//驱动实例ID
}T_DRV_DES,*LPDRV_DES;

typedef vector<T_DRV_DES> vecDrivers;
typedef vector<TDBPARG> CDBPArgs;
typedef vector<T_DRVTAG1> CDrvTags;

#define DEC_STATUS_STOP			0	//驱动运行状态 停止
#define DEC_STATUS_RUN			1	//驱动运行状态 运行

