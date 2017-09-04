//drvdef.h
#pragma once
#include <vector>

#define DRT_VOID				0	//空类型
#define DRT_DIGITAL				1	//开关量	,也是4字节整数		
#define DRT_INT32				2	//4字节整数
#define DRT_FLOAT32				3	//4字节浮点数
#define DRT_INT64				4	//8字节整数
#define DRT_FLOAT64				5	//8字节浮点数
#define DRT_STRING				6	//字符串
#define DRT_BLOB				7	//块变量

// 目标服务器质量码
#define DBP_QUALITY_GOOD					0x00	// GOOD
#define DBP_QUALITY_BAD						0x01	// 坏值，但没有特定原因
#define DBP_QUALITY_BAD_CONFIG_ERROR		0x02	// 服务器配置错误。例如，标签从运行的服务器配置中删除了
#define DBP_QUALITY_BAD_NOT_CONNECTED		0x03	// 输入要求连接到其他目标，但无法成功连接。该质量说明在该时间点上的值不可信，因为有一个数据源未提供
#define DBP_QUALITY_BAD_DEVICE_FAIL			0x04	// 检测到设备故障
#define DBP_QUALITY_BAD_SENSOR_FAIL			0x05	// 检测到传感器故障
#define DBP_QUALITY_BAD_LAST				0x06	// 设备与服务器的通讯失败，但是，最后有效值是可用的。请注意，时标标记的是最后一个有效值的时标，而不是质量变更的时标
#define DBP_QUALITY_BAD_COMM_FAIL			0x07	// 设备与服务器的通讯失败，并且没有最后可用的有效值
#define DBP_QUALITY_BAD_OUT_OF_SERVICE		0x08	// 该标签或组被设置为非活动，表示该标签由于某种原因不被服务器更新

#define DBP_QUALITY_UNCERTAIN				0xE0	// 值有效性不确定，但没有特定原因
#define DBP_QUALITY_UNCERTAIN_LAST			0xE1	// 已经停止写入最新值，该值应被视为“过去值”。请注意，该状态表示为在规定时间内标签未被更新。而Bad/Last Usable Value表示为检测到通讯错误发生
#define DBP_QUALITY_UNCERTAIN_SENSOR		0xE2	// 值超出传感器量程范围或者传感器内部诊断为测量不准确
#define DBP_QUALITY_UNCERTAIN_EU_EXCEEDED	0xE3	// 返回值超出了该值定义的范围。请注意，该状态并不意味着该值是超限的
#define DBP_QUALITY_UNCERTAIN_SUB_NORMAL	0xE4	// 该值来自多个源，但其中状态为GOOD的数据源个数少于规定值

#define DBP_QUALITY_LOLO_LIMIT				0xF9	// 低低报
#define DBP_QUALITY_LO_LIMIT				0xFA	// 低报
#define DBP_QUALITY_HI_LIMIT				0xFB	// 高报
#define DBP_QUALITY_HIHI_LIMIT				0xFC	// 高高报
#define DBP_QUALITY_NOT_INIT				0xFD	// 才被创建，还未被初始化
#define DBP_QUALITY_SHUTDOWN				0xFE	// 服务器关闭
#define DBP_QUALITY_UNKOWN					0xFF	// 未知错误

// 为了保证以前老的项目可以正常编译
#define QA_OK								DBP_QUALITY_GOOD
#define QA_ERR								DBP_QUALITY_BAD

//运行状态
#define DRSTATUS_STOP			0	//驱动运行状态 停止
#define DRSTATUS_RUN			1	//驱动运行状态 运行

typedef long DRVRET;	//返回值定义

//错误代码，<0 不能用,0-100以下为驱动规范固定错误代码，>=100为驱动自定义错误代码
#define DRERR_OK			0		//成功
#define DRERR_HANDLE		1		//错误的句柄
#define DRERR_CONFIGDATA	2		//配置数据错误
#define DRERR_CONFIGBUF		3		//配置数据缓冲区不够
#define DRERR_CREATE		4		//创建驱动对象失败
#define DRERR_NOMORETAG		5		//无更多标签
#define DRERR_NOERRCODE		6		//无此错误代码
#define DRERR_NOTSUPPORT	7		//不支持
#define DBERR_NOMOREREC		8		//没有记录
#define DBERR_NOTAG			9		//无此标签
#define DBERR_TYPE			10		//类型无法转换或错误
#define DBERR_TIME			11		//时标错误
#define DBERR_NOVAL			12		//没有数据

#define DBERR_MEMORY		13		// 内存不足
#define DBERR_PACKET_TO_LONG 14		// 报文超长
#define DBERR_QUEUE_FULL	15		// 队列已满

typedef struct DR_TAG  //用于枚举使用
{
	char sname[80];	//名称
	char sdes[80];	//描述
	char sunit[16];	//单位
	WORD wtype;		//数据类型
	WORD waccess;	//存取方式，0只读，1读写
	char cr[4];		//保留
}T_DRVTAG,*LPDRVTAG; //sizeof = 168

typedef struct _DR_TAG  //用于枚举使用
{
	CString szName;	//名称
	CString szDes;	//描述
	CString szUnit;	//单位
	WORD	wType;		//数据类型
	WORD	wAccess;	//存取方式，0只读，1读写
	char cr[4];		//保留
}T_DRVTAG1,*LPDRVTAG1; //sizeof = 168

typedef  struct  DR_HANDLE  //驱动句柄
{
	void *pObject;		//驱动对象，由驱动填写
	long lid;			//宿主提供的驱动ID,驱动不要更改。
	long lParam1;		//宿主使用参数，驱动不要更改
	long lParam2;		//宿主使用参数，驱动不要更改
}T_DRHANDLE,*DRHANDLE; 	//sizeof() = 16

typedef struct DR_INFO //驱动信息
{
	char sname[32]; 	//驱动名字
	char sdes[128];		//描述
	char sver[16];		//驱动程序版本
	DWORD dwver;		//驱动规范版本，遵循的驱动规范版本
	DWORD dwres;		//保留
	char sres[72];		//保留
}T_DRVINFO,*LPDRVINFO;	//sizeof() == 256

#ifndef DEFINE_STRUCT_TVVAL
typedef struct  t_var		//变体结构
{
	DWORD	vt;		//类型
	long	lvlen;	//可变部分长度,对pstr,pblob,psoe有效,其他已知长度的基本数据类型填0
					//字符串长度不包含结尾的0字符
	union 	
	{
		long  	lval;		//存储didital、int32
		float  	fval;		//存储float32
		__int64 llval;		//存储int64
		double 	dblval;		//存储float64
		char*	pstr;		//字符串首指针
		void*	pblob;		//存储pblob的存储区首地址
	};
	long  ltime;			//时标
	short snqa;				//质量
	short snerr;			//错误代码
}TVVAL,*LPVVAL; //sizeof() = 24 

#define DEFINE_STRUCT_TVVAL
#endif

typedef struct DR_REC		//数据记录
{
	char	sname[80];		//标签名
	TVVAL	val;			//值
}T_DRREC,*LPDRREC;			//sizeof() = 104

#ifndef DEFINE_STRUCT_TSOE
typedef struct  t_soe		//事件结构
{
	long	m_ltime;		//时标
	long	m_lmsec;		//毫秒
	char	m_ssrc[64];		//来源
	char	m_sdec[64];		//工程设备
	char	m_sdes[256];	//事件描述
	char	m_stxtargs[256];//文本参数
	long	m_lclsid;		//事件分类
	char	m_sres[4];		//保留
	long	m_lbinarglen;	//参数长度
	void*	m_pbinargs;		//事件参数
}TSOE,*LPSOE; //sizof() = 664
#define DEFINE_STRUCT_TSOE
#endif

//回调函数：
typedef DWORD (WINAPI * DRCB_OnDataOk)(	//数据回调函数
					DRHANDLE handle,	//句柄
					T_DRREC  recs[],		//记录集
					int nrecs			//记录个数
);

typedef DWORD  (WINAPI * DRCB_OnLogOk)( //日志回调函数
					DRHANDLE handle,	//句柄
					const char *plog,	//日志字符串，0结束的字符串
					DWORD dwLogLevel,	//日志级别
					DWORD dwLogOptType	//日志操作类型
);

typedef DWORD  (WINAPI * DRCB_OnSoeOk)( //SOE事件回调函数,每次回调一个事件
					DRHANDLE handle,	//句柄
					LPSOE psoe			//事件
);

//////////////////////////////////////////////////////////////////////////
// 2015-09-22新增数据回调接口
//////////////////////////////////////////////////////////////////////////

// 历史数据回调
typedef DWORD (WINAPI * DRCB_OnWriteHistoryCB)(
	DRHANDLE handle,		// 句柄
	T_DRREC  recs[],		// 记录集
	int nrecs				// 记录个数
	);

// 通用数据报文回调
typedef DWORD (WINAPI * DRCB_OnStreamMessageCB)(
	DRHANDLE handle,				// 句柄
	const char* lpszTagName,		// 标签名称
	DWORD	dwCommandID,			// 报文指令ID
	BYTE*	pBuffer,				// 报文数据缓存
	int		nSize					// 报文长度
	);