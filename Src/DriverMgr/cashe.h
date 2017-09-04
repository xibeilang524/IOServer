//cashe.h
#pragma once
#include <afxtempl.h>
#include "../sdk/sdktool.h"
#include "pagefile/pagefile.h"

//压缩
#define  TA_COMPNO		0	//不压缩
#define  TA_COMPPER		1	//百分比精度压缩
#define  TA_COMPVAL		2	//绝对值精度压缩


///////////////////////////////////////////////////////////////////////////////
class CBaseVal //样本值
{
public:
	CBaseVal(){
		m_szName = "";
	};
	~CBaseVal(){};
public:
	CString m_szName; //标签名
	CVarVal m_val;	//值
public:
	void Clear()
	{
		m_szName = "";
		m_val.SetVoid();
	};
};
//标签值使用的基类
class CTvObj
{
public:
	CTvObj();
	virtual ~CTvObj();
public:
	CString m_szname;//标签名
	int		m_ntype; //数据类型
	int     m_ncomptype;//压缩类型
	double  m_fcompval;//压缩精度

	CVarVal m_valreal;      //当前实时值
	long	m_llastsave;		//上次存储
public:
	virtual BOOL AddReal(LPVVAL pval,CBaseVal* pbaseval) = 0;//添加实时值,该压缩的要压缩,返回TRUE表示pbaseval被填写了样本值
	virtual BOOL GetLastHisVal(CBaseVal* pbaseval){return FALSE;};//取最后的历史值
};

class CTv_digital : public CTvObj //Digital
{
public:
	CTv_digital();
	virtual ~CTv_digital();
	
	BOOL m_bdataok; //数据有效
public:
	virtual BOOL AddReal(LPVVAL pval,CBaseVal* pbaseval);//添加实时值,该压缩的要压缩,返回TRUE表示pbaseval被填写了样本值
};

class CTv_int32 : public CTvObj //int32
{
public:
	CTv_int32();
	virtual ~CTv_int32();
	BOOL m_bdataok; //数据有效
public:
	virtual BOOL AddReal(LPVVAL pval,CBaseVal* pbaseval);//添加实时值,该压缩的要压缩,返回TRUE表示pbaseval被填写了样本值
};

class CTv_int64 : public CTvObj //int64
{
public:
	CTv_int64();
	virtual ~CTv_int64();
	BOOL m_bdataok; //数据有效
public:
	virtual BOOL AddReal(LPVVAL pval,CBaseVal* pbaseval);//添加实时值,该压缩的要压缩,返回TRUE表示pbaseval被填写了样本值
};

class CTv_string : public CTvObj //string
{
public:
	CTv_string();
	virtual ~CTv_string();
public:
	virtual BOOL AddReal(LPVVAL pval,CBaseVal* pbaseval);//添加实时值,该压缩的要压缩,返回TRUE表示pbaseval被填写了样本值
};

class CTv_blob : public CTvObj //blob
{
public:
	CTv_blob();
	virtual ~CTv_blob();
public:
	virtual BOOL AddReal(LPVVAL pval,CBaseVal* pbaseval);//添加实时值,该压缩的要压缩,返回TRUE表示pbaseval被填写了样本值
};

class CTv_float32 : public CTvObj //float32
{
public:
	CTv_float32();
	virtual ~CTv_float32();
	int m_nrecs;//但前压缩缓冲区数据个数
	CVarVal m_valtmp[6]; //6个压缩缓冲区
public:
	virtual BOOL AddReal(LPVVAL pval,CBaseVal* pbaseval);//添加实时值,该压缩的要压缩,返回TRUE表示pbaseval被填写了样本值
	virtual BOOL GetLastHisVal(CBaseVal* pval);//取最后的历史值
};

class CTv_float64 : public CTvObj //float64
{
public:
	CTv_float64();
	virtual ~CTv_float64();
	int m_nrecs;//但前压缩缓冲区数据个数
	CVarVal m_valtmp[6]; //6个压缩缓冲区
public:
	virtual BOOL AddReal(LPVVAL pval,CBaseVal* pbaseval);//添加实时值,该压缩的要压缩,返回TRUE表示pbaseval被填写了样本值
	virtual BOOL GetLastHisVal(CBaseVal* pval);//取最后的历史值
};
class CMemCashe;
class CCompCashe //缓存压缩,用于基本数据类型的实时值压缩为缓存样本值
{
public:
	CCompCashe();
	~CCompCashe();
protected:
	//CMap<CString,LPCTSTR,CTvObj*,CTvObj*> m_map;
	CNoCaseMap<CTvObj*,CTvObj*> m_map;
	
public:
	void Clear();//清空
	BOOL AddReal(LPCTSTR sname,int ncomptype,double dblcompval,LPVVAL pval,CBaseVal* pbaseval);//添加实时值
	void SaveLastVal(CMemCashe* pmc);//存储最后一个值，退出时使用，
};

/*
	本地基本数据类型的缓存数据磁盘存储格式:
	流文件方式，分3部分
	1.文件头
	2.name - ID对照表
	3.数据记录
  */
typedef struct t_cashefilehead //基本数据类型本地缓存文件头部格式
{
	DWORD dwflag;//标志 0xDF23AD79
	long  lnameids; //名称-ID对照个数
	long  lvals;	//数据记录个数
	long  ldovals;	//已处理记录个数
}TCAFHEAD,*LPCAFHEAD;

#define MEM_CASHE_SIZE	1000000  //100万条记录

/*
基本数据类型的本地缓存压缩存储设计：
1）能独立使用，不依赖于DDC或DDS环境。
2）压缩后的样本值满100万条记录存储为一个文件。数据占24M大小，头部用于存储转换表,按5万标签计算，转换表约占空间4.4M
  */
class CTagVal;
class CMemCashe //内存缓存，用于基本数据类型的样本值存储，存满后写入文件。
{
protected:
	struct t_caval
	{
		long	ltime;	//时标
		short	snqa;	//质量
		WORD	wtype;	//值
		union 
		{
			long	lval;
			float	fval;
			__int64 llval;
			double	dblval;
		};
		DWORD dwid;		//ID
		DWORD dwres;	//保留
	};//sizeof() = 24;
	typedef struct t_nameid
	{
		char sname[80];
		DWORD dwid;
		DWORD dwres;
	}TNAMEID,*LPNAMEID;//sizeof() = 88;
protected:
	//{{样本值存储区
	int m_nGetPos;//取发送缓存的位置；
	int				m_nsize;	//记录个数
	struct t_caval*	m_pvals;	//存储空间 
	//}}

	//{{ 名称ID转换表
	DWORD m_dwNextID;	//下一个ID;
	//CMap <CString,LPCTSTR,LPNAMEID,LPNAMEID> m_mapname;	//标签名索引
	CNoCaseMap <LPNAMEID,LPNAMEID> m_mapname;	//标签名索引
	CMap <DWORD,DWORD,LPNAMEID,LPNAMEID> m_mapid;		//ID索引
	
	DWORD MakeID(LPCTSTR lpszName);
	//}}
	//{{从磁盘读缓存用

	CString m_szDiskCasheFile;//磁盘缓存文件
protected:
	void ClearMap();//清空MAP
public:
	CMemCashe();
	~CMemCashe();

	DWORD GetID(LPCTSTR lpszName);
	CString  GetName(DWORD dwid);
	BOOL SaveToDisk(LPCTSTR lpszPath); //保存到磁盘
	void Reset();		//复位，重新初始化
	BOOL IsFull();
	BOOL IsEmpty(){return (m_nsize == 0);};
	BOOL AddTagVal(CBaseVal* pbaseval);//添加样本值

	int  GetLastRecs(CTagVal* lprecs,int nsize);//取最新的cashe数据用于cashe发送,返回个数
	void GetPosOk()//确认去发送缓存
	{
		if(m_nGetPos <= m_nsize)
			m_nsize = m_nGetPos;
	};

	BOOL LoadFromDisk(LPCTSTR lpszPath);//从磁盘读取最新的
};

/*
	始终打开当前最新的页面文件，当发送完成后关闭删除，并打开次新的页面文件，如果没有次新文件，则建立新文件。
	满后按时间创建新的页面文件。
	页面文件命名:年月日时分秒.pgf

  页面文件大小估算，如果每个记录均<=1024字节，那么存满目录(65528条)约需最小空间16M+64M = 80M
  最大空间2G存满时，每条记录平均长度可达到31K。
  */
class CPfCashe //页面文件cashe 缓存string,blob,soe类型记录
{
public:
	CPfCashe();
	~CPfCashe();
protected:
	CString m_szPath;	//目录
	CPageFile m_pf;		//页面文件
protected:
	CString GetLastFile(int &n);//获取最新的页面文件名，磁盘文件检索
	
public:
	BOOL Init(LPCTSTR lpszPath);//初始化，打开最新文件，没有则创建
	BOOL SaveRec(CPfRecVal* pvar);//写入
	int			GetLastRec(PF_HANDLE &h,CPfRecVal* prec){
		return m_pf.GetLastRec(h,prec);
	}; //h用于读后发送成功后删除记录用
	int			GetFirstRec(PF_HANDLE &h,CPfRecVal* prec){
		return m_pf.GetFirstRec(h,prec);
	};
	void DeleteRec(PF_HANDLE h){
		m_pf.DeletePf(h);//删除记录
	};
	void OpenNew();
};

class CLocalCashe //本地缓存总装类
{
public:
	CLocalCashe();
	~CLocalCashe();
protected:
	//基本数据类型缓存
	CCompCashe m_compcashe;	//缓存压缩
	CMemCashe  m_memcashe;	//样本值缓存

	//string blob soe缓存
	CPfCashe   m_pfcashe;	//可变类型缓存

	//
	CString m_szPath;			//目录
	CCriticalSection m_lock;    //同步锁
public:
	void SetPath(LPCTSTR lpszPath);
	void AddRealVal(LPCTSTR sname,int ncomptype,double dblcompval,LPVVAL pval);//添加基本类型实时值
	void AddRealVar(LPCTSTR sname,CVarVal *pval);//写String,Blob 到cashe
	void AddSoe(CSoe *psoe);//写SOE到cashe
	//取最新的cashe数据用于cashe发送,返回个数
	int  GetLastRecs(CTagVal* lprecs,int nsize);
	//确认去发送缓存
	void GetPosOk();
	//h用于读后发送成功后删除记录用
	int	GetLastPfRec(PF_HANDLE &h,CPfRecVal* prec);
	int	GetFirstPfRec(PF_HANDLE &h,CPfRecVal* prec);
	void DeletePfRec(PF_HANDLE h);
	void OpenNewPgeFile();
	//从磁盘读取最新的
	BOOL LoadFromDisk();
};