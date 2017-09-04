#if !defined _DPAPI2_H_I
#define _DPAPI2_H_I

//数据类型
#ifndef DT_VOID
#define DT_VOID		0	//空类型
#define DT_DIGITAL	1	//开关量	,也是4字节整数		
#define DT_INT32	2	//4字节整数
#define DT_FLOAT32	3	//4字节浮点数
#define DT_INT64	4	//8字节整数
#define DT_FLOAT64	5	//8字节浮点数
#define DT_STRING	6	//字符串
#define DT_BLOB		7	//二进制数据块
#endif

// 代理服务器质量码
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

// 读历史str,历史blob,SOE是时标的意义
#define  RST_CUR			1		//当前时标处
#define  RST_AFTER			2		//当前之后一条
#define  RST_DEFAULT		(RST_CUR | RST_AFTER) //当前或之后一条

//权限
#define DS_POWERMAN		0x00000001 //账号管理
#define DS_SYSMAN		0x00000002 //系统管理,数据库管理，标签管理
#define DS_WRITE		0x00000004 //写权限
#define DS_READ			0x00000008 //读权限

typedef void* DBPH; //句柄
typedef struct t_dbparg  //代理服务器连接参数
{
	char	sip[16];    	//服务器IP
	char	suser[16];		//登录账号
	char	spass[16];  	//登录密码
	WORD	wport;	//端口
	short	snres;	//保留
	char	sres[4];		//保留
}TDBPARG,*LPDBPARG;	//sizeof() = 56

//标签类型
#define TAG_FLAG_DDC	0 //现场数据标签(实时库标签) 导入名"ddctag"
#define TAG_FLAG_DEF	1 //定义标签(手工标签)       导入名"deftag"
#define TAG_FLAG_PRE	2 //预处理标签(预处理标签)   导入名"pretag"
#define TAG_FLAG_CUR	3 //曲线标签                 导入名"curtag"	
#define TAG_FLAG_CALC	4 //计算标签				 导入名"calctag"

typedef struct t_tagitem		//代理服务器标签定义
{
	char	m_sname[80];		//标签名
	char	m_sdbname[16];		//数据库实例名
	char	m_sdbtagname[80];	//数据库标签名
	char	m_sdes[80];			//描述
	char	m_sunit[16];		//单位
	DWORD   m_dwid;				//标签ID
	WORD	m_wtype;			//数据类型
	short   m_errcode;			//错误代码
	long	m_ltagflag;			//标签标志,
	DWORD   m_dwres;			//保留，填写0
}TTAGITEM,*LPTAGITEM; //sizeof() = 288

#ifndef DEFINE_STRUCT_TVVAL
typedef struct  t_vval		//变体结构
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

#ifndef DEFINE_STRUCT_TSOE
typedef struct  t_soe		//事件结构
{
	long	m_ltime;			//时标
	long	m_lmsec;			//毫秒
	char	m_ssrc[64];			//来源
	char	m_sdec[64];			//工程设备
	char	m_sdes[256];		//事件描述
	char	m_stxtargs[256];	//文本参数
	long	m_lclsid;			//事件分类
	char	m_sres[4];			//保留
	long	m_lbinarglen;		//参数长度
	void*	m_pbinargs;			//事件参数
}TSOE,*LPSOE; //sizof() = 664
#define DEFINE_STRUCT_TSOE
#endif


typedef struct t_ServerInfo
{
	DWORD dwVersion;			// 代理服务器版本号，版本号V2.5.4.0表示为0x02050400
	long lNowTime;				// 当前时间
	long lStartupTime;			// 启动时间
	DWORD dwConnectCount;		// 当前连接数
	DWORD dwTagCount;			// 所有标签数
	float fCPUUsage;			// CPU占用
	DWORD dwHandleCount;		// 句柄数
	DWORD dwThreadsCount;		// 线程数
	DWORD dwMemoryBytes;		// 内存使用
	DWORD dwMemoryBytesPeak;	// 高峰内存使用
	DWORD dwVirtualBytes;		// 虚拟内存大小
	DWORD dwVirtualBytesPeak;	// 高峰虚拟内存大小
	DWORD dwReserves[52];
}SERVERINFO, *LPSERVERINFO; // sizeof()=256

//枚举函数一次处理一批标签，原来是处理一个标签，这里多加了一个参数ntags
typedef void (WINAPI *Enum_TagAttr)(LPTAGITEM ptag, int ntags, long lparam); //枚举标签函数

extern "C"{
	DWORD  __stdcall DBP_GetVersion();

	DBPH  __stdcall DBP_Open( 
		TDBPARG  conargs[],
		int nsize
		);

	DBPH  __stdcall DBP_Open2( 
		const char *szIP, 
		WORD wPort,
		const char *szUserName,
		const char *szPassword,
		BOOL bUseGroup
		);

	DWORD __stdcall DBP_Close( 
		DBPH h
		);

	DWORD __stdcall DBP_Connect( 
		DBPH h
		);

	DWORD __stdcall DBP_DisConnect( 
		DBPH h
		);

	DWORD __stdcall DBP_IsConnect( 
		DBPH h
		);

	DWORD __stdcall DBP_GetTagAttr(
		DBPH  h, 
		TTAGITEM tags[],
		int nsize
		);

	DWORD __stdcall DBP_GetSnapshot(
		DBPH  h, 
		char sTagNames[][80],
		TVVAL tvs[],
		int nsize
		);

	DWORD __stdcall DBP_GetHisVal(
		DBPH  h, 
		const char* lpszTagName,
		long lts,
		long lte,
		long linsertsec,
		long lflag,
		TVVAL pvals[],
		int nsize,
		int* pnrecsize
		);

	DWORD __stdcall DBP_GetHisVal2(
		DBPH  h,
		long ltime,
		char sTagNames[][80],
		TVVAL tvs[],
		int nsize
		);//读多点历史数据

	DWORD __stdcall DBP_WriteReal(
		DBPH  h, 
		char sTagNames[][80],
		TVVAL tvs[],
		int nsize
		);

	DWORD __stdcall DBP_WriteHis(
		DBPH  h, 
		char sTagNames[][80],
		TVVAL tvs[],
		int nsize
		);

	DWORD __stdcall DBP_WriteRealStr(
		DBPH  h, 
		const char* sName,
		TVVAL* ptv
		);

	DWORD __stdcall DBP_WriteRealBlob(
		DBPH  h, 
		const char* sName,
		TVVAL* ptv
		);

	DWORD __stdcall DBP_WriteHisStr(
		DBPH  h, 
		const char* sName,
		TVVAL* ptv
		);

	DWORD __stdcall DBP_WriteHisBlob(
		DBPH  h, 
		const char* sName,
		TVVAL* ptv
		);

	DWORD __stdcall DBP_WriteSoe(
		DBPH  h, 
		TSOE* psoe
		);

	DWORD __stdcall DBP_DelHis(
		DBPH  h, 
		const char* sName,
		long lts,
		long lte
		);

	DWORD __stdcall DBP_DelHis2(
		DBPH  h, 
		const char sNames[][80],
		long lts[],
		long lte[],
		short snerr[],
		int nsize
		);

	DWORD __stdcall DBP_DelSoe(
		DBPH  h, 
		long ltimes, 
		long lmsecs, 
		long ltimee,  
		long lmsece, 
		const char* ssrc, 
		const char* sdec, 
		long lclsid       
		);

	DWORD __stdcall DBP_GetSumVal(
		DBPH  h, 
		const char*  sName,
		long lts,
		long lte,
		long ltype,
		long lds,
		TVVAL pvals[],
		int nsize,
		int* pnrecsize
		);

	DWORD __stdcall DBP_GetRealStr(
		DBPH  h, 
		const char*  sName,
		TVVAL** ppval
		);

	DWORD __stdcall DBP_GetRealBlob(
		DBPH  h, 
		const char*  sName,
		TVVAL** ppval
		);

	DWORD __stdcall DBP_GetHisStr(
		DBPH  h, 
		const char*  sName,
		long ltime,
		unsigned long dwstyle,
		TVVAL** ppval
		);

	DWORD __stdcall DBP_GetHisBlob(
		DBPH  h, 
		const char*  sName,
		long ltime,
		unsigned long dwstyle,
		TVVAL** ppval
		);

	DWORD __stdcall DBP_GetSoe(
		DBPH  h, 
		long lts, 
		long lms, 
		const char* ssrc, 
		const char* sdec, 
		long lclsid,       
		DWORD dwStyle,		
		TSOE** ppsoe
		);

	DWORD __stdcall DBP_QueryTag(
		DBPH  h,
		LPCTSTR sfltname,
		LPCTSTR sfltdes,
		int ntype,
		Enum_TagAttr pfEnum,
		long lparam
		); //查询标签

	DWORD __stdcall DBP_QueryTag2(
		DBPH  h,
		LPCTSTR sfltname,
		LPCTSTR sfltdes,
		int ntype,
		int ntagtype,
		Enum_TagAttr pfEnum,
		long lparam); //查询标签

	DWORD __stdcall DBP_GetSvrConInfo(
		DBPH  h,
		LPDBPARG pconarg,
		int* pnNum);//取服务器客户个数

	DWORD __stdcall DBP_SendIdel(
		DBPH  h,
		long* plsrvoder);//发送空闲，看服务器是否有命令

	DWORD __stdcall DBP_InportTags(
		DBPH  h,
		const char* lpsztags,
		int nsize,
		BOOL bEnd); //导入标签到dbp服务器

	DWORD __stdcall DBP_GetGroups(
		DBPH  h,
		TDBPARG  conargs[],
		int *pnsize); //获取DBP群组配置

	DWORD __stdcall DBP_SetGroups(
		DBPH  h,
		TDBPARG  conargs[],
		int nsize); //设置DBP群组配置

	DWORD __stdcall DBP_GetServerInfo(
		DBPH h, 
		LPSERVERINFO lpServerInfo
		);
}
#endif //_DPAPI2_H_I