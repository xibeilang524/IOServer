//pagefile.h 页面文件
#pragma once
/*
	页面文件,string blob soe 缓存专用版
  */
//返回值定义
#define PFR_OK				0	//成功	
#define PFR_ERR				-1	//失败
#define PFR_NOOPEN			-2	//页面文件没有打开
#define PFR_NOPAGE			-3	//页面满
#define PFR_NOINDEX			-4	//没有ID
#define PFR_INDEXFULL		-5	//索引满
#define PFR_MEMERR			-6	//内存错误
#define	PFR_NOTPF			-7	//不是tonma页面文件
#define PFR_INDEXERR		-8	//索引错误
#define PFR_PAGEMARKERR		-9	//页面标记错误
#define PFR_FILEEXIST		-10	//文件存在
#define PFR_CREATEINDEXERR  -11	//创建索引错误
#define PFR_MARKERR			-12	//页面标记错误

//固定参数定义 页面尺寸1K版本
// 目录区空间8M-1024，页面标记空间8M
// 数据空间，2000M
// 最大页面文件空间 2000M + 16M  = 2016M  < 2048M,不会超出文件系统大小
#define PF_PAGESIZE			1024	//页面大小
#define PF_MAXINDEXS		65528	//最大索引个数,目录数,占用空间8M - 1024
#define PF_MAXPAGES			2048000	//最大块标记,每页面1K,共可标记2000M,占用标记空间8192000字节
#define PF_INDEXPOS			1024	//索引位置
#define PF_PAGEMARKPOS		8388608	//页标记位置   8M(8 * 1024 * 1024)
#define PF_DATAPOS			16777216//文件数据位置 16M(16 * 1024 * 1024) + 

#define PF_PAGEEND			(-1)	//文件块结束标志


//注：页面标记每个标记占4字节，共

//读写参数
typedef struct			//总文件头
{
	DWORD	dwfalg;		//文件标识  0x81018202
	long	lpages;		//当前页面数
	long	lindexs;	//当前文件个数
	long	lgrownsize;	//文件每次增加大小，默认16M
	char	cres[1024 - 16];
}PFHEAD,*LPPFHEAD;      //sizeof() = 1024

#define PGFDESLEN	104  //页面文件目录描述空间大小
typedef struct		//索引结构,也叫目录结构
{
	long	ltime;		//创建时间,=0表示该目录未用，因此写记录时该值一定不能为0
	long	lpageno;	//开始页面号
	long	lsize;		//文件大小
	long	ltype;		//类型 0 string ,1 blob,2 soe 
	short	snqa;		//质量
	WORD	wres;		//保留
	DWORD	dwres;		//保留
	char	sdes[104];	//文件名或描述,可用格式串方式

}PFINDEX,*LPPFINDEX; //sizeof() = 128

#define PFILE_VOID		0
#define PFILE_STRING	1
#define PFILE_BLOB		2
#define PFILE_SOE		3 
class CPageFile;
class CPfRecVal //记录值
{
public:
	CPfRecVal();
	~CPfRecVal();
	CString m_szdes;//描述
	long	m_ltime;//更新时间
	long	m_ntype; //类型,PFILE_STRING等
	short	m_snqa;  //质量
	WORD	m_wres;		//保留
	DWORD	m_dwres;		//保留
protected:
	long	m_nszie; //数据字节数
	long	m_lbufsize;
	void*	m_pdata;
	friend class CPageFile;
public:
	long	GetDataSize(){return m_nszie;};
	long	GetBufSize(){return m_lbufsize;};
	void	ResetBufSize(long lsize); //重置大小
	void*	GetBuf(){
		return m_pdata;
	};

	BOOL	SetData(void* pdata,long lsize);//设置数据
};

typedef int		PF_HANDLE;		//页面文件句柄

#define PF_ERR_HANDLE	(-1);	//无效句柄

class CPageFile //页面文件类
{
public:
	CPageFile();
	~CPageFile();
protected:
	PFHEAD		m_head;		//文件头
	LPPFINDEX	m_indexs;	//索引
	long*		m_pages;	//页面标记

	CFile		m_file;		//总文件
	CString		m_szFile;	//文件名

	BOOL		m_bOpen;	//是否打开
	long		m_lgrownsize;	//增长尺寸

	BOOL		m_bNeedFlush;
protected:
	void		Clear();	//清空索引和块标记
	long		GetEmptyPages();//取空闲的页面数
	long		GetOneEmptyIndex();//取一个空闲索引位置
	
	int			GetNextEmptyIndex(int nstart);//取下一个可用的索引号
	int			GetNextEmptyPage(int nstartpos);

	//{{对页面文件操作函数
	int			PF_GetPages(LPPFINDEX pf);//取一个页面文件所占页面数
	int			PF_AddPage(PF_HANDLE h);//添加一个页面,返回新加的页面号
	
	int			PF_GetPageNo(LPPFINDEX pf,long offset);//取据页面文件数据区位置为offset的页面号,返回-1表示错误
	int			PF_Write(PF_HANDLE h,long offset,const void *pbuf,long nsize,BOOL bFlush = TRUE);//写
	int			PF_Read(PF_HANDLE h,long offset,void *pbuf,long nsize,long &nreadsize);//读
	int			PF_Delete(PF_HANDLE h);//删除
	//}}

	//{{debug
	void		TraceIndex();
	void		TracePageMark();
	//}}

	//创建记录的文件
	PF_HANDLE	PF_CreateRecDir(CPfRecVal* prec);//创建页面文件，返回索引号，-1表示错误
public:
	BOOL		IsOpen(){return m_bOpen;};
	void		SetGrownSize(long lmsize) //设置文件增长大小，单位M
	{
		long lt = lmsize;
		if(lt < 1)
			lt = 1;
		if(lt > 500 )
			lt = 500;
		m_lgrownsize = lt * 1024 * 1024;

	};//文件增长大小
	int			Open(LPCTSTR lpszFile);		//打开,没有则创建
	int			Close();					//关闭
	int			Create(LPCTSTR lpszFile);	//创建
	void		Flush();

public: //缓存用
	int			WriteRec(CPfRecVal* prec);
	int			GetLastRec(PF_HANDLE &h,CPfRecVal* prec); //h用于读后发送成功后删除记录用
	int			GetFirstRec(PF_HANDLE &h,CPfRecVal* prec); //h用于读后发送成功后删除记录用
	int			DeletePf(PF_HANDLE h){return PF_Delete(h);};//删除记录
	void		EmptyFile();//清空，截取多余的文件尾
	void		CloseAndDelFile();//关闭和删除文件
};