//SimdrvExobj.h
#pragma once

#include <afxmt.h>
#include <afxtempl.h> //模版支持
#include "..\sdk\drvsdk.h"
#include "..\sdk\sdktool.h"
 
#define SIMTYPE_LINE	0 //线性模拟 line
#define SIMTYPE_SIN		1 //正玄模拟 sin
#define SIMTYPE_RAND    2 //随机模拟 rand wzq

#define FIELD_NAME		0
#define FIELD_DES		1
#define FIELD_UNIT		2
#define FIELD_TYPE		3
#define FIELD_SIMTYPE	4
#define FIELD_BASEVAL	5
#define FIELD_PERCENT	6

class CTagItem : public CSortItem //标签定义项
{
public:
	CTagItem()
	{	
		m_nType = DRT_FLOAT32;
		m_fPercent = 20;
		m_bSel = FALSE;
	};
	virtual ~CTagItem(){};
public:
	CString m_szName; 
	CString m_szDes;
	CString m_szUnit;

	int 	m_nType;		//数据类型
	int 	m_nSimtype;		//模拟方式0锯齿波，1正旋波
	CString m_szBaseVal;	//基准值
	float	m_fPercent;//变化百分比幅度(相对于基准值)

	BOOL	m_bSel;			//选择标志
public:
	CString GetText(int nField);//取数据字符串
	void	CopyTo(CTagItem *pt);//复制
	void	CopyToDrTag(LPDRVTAG lpdrtag);//复制到接口格式标签定义
	void    Serialize(CArchive& ar);

	//排序比较重载
	virtual		int Compare(CSortItem *pOb);
	
public: //static
	static  CString GetTypeStr(int ntype);
	static int		GetTypeVal(LPCTSTR lpszType);
	static CString	GetSimTypeStr(int ntype);
	static int		GetSimTypeVal(LPCTSTR lpszType);
};
class CWinTagInfo;
class CRunTags;
class CTagDefTab		//标签定义表
{
public:
	CTagDefTab()
	{
		m_mapTab.InitHashTable(11111);	//初始化MAP空间
		m_oaTag.SetGrown(8192);			//按8k增长
	};
	~CTagDefTab()
	{
		m_mapTab.RemoveAll();//m_oaTag析构时会删除对象,所以不必逐个删除m_mapTab的对象
	};
protected:
	//m_mapTab和m_oaTag里存储相同的对象指针
	CMap<CString ,LPCTSTR ,CTagItem*,CTagItem*> m_mapTab;	//标签MAP
	CSortObArray m_oaTag;									//标签数组
	friend class CRunTags;
	friend class CWinTagInfo;
public:
	int		GetSize(){return m_oaTag.GetSize();};
	void	Serialize(CArchive& ar);
	BOOL	IsSelected(int nIndex);
	void	SetSel(int nIndex,BOOL bSel);
	BOOL	DelSelect();//删除选择的标签
	
	CString GetText(int nIndex,int nField);
	int	 GetNextSelected(int nstart)//返回选择
	{
		CTagItem *pi;
		int i,ns = nstart,n = m_oaTag.GetSize();
		if(ns < 0) ns = 0;
		for(i=ns;i<n;i++)
		{
			pi = (CTagItem	*)m_oaTag.GetAt(i);
			if(pi->m_bSel)
				return i;
		}
		return -1;
	}

	CTagItem*	GetTag(LPCTSTR lpszName); //返回标签对象
	BOOL		GetAt(int nIndex,CTagItem *ptag); //复制方式取
	BOOL		AddTag(CTagItem *pTag); //添加标签定义，复制方式
	BOOL		UpdateTag(CTagItem *pTag); //更新标签，如果未存在，则添加
	BOOL		IsExist(LPCTSTR lpszName);//是否存在
};

class CTagVal : public CSortItem //标签值
{
public:
	CTagVal();
	CTagVal(CTagItem *pTag);
	~CTagVal();
public:
	void Simulate(long ltime,CRdRec* lprv);//模拟，结果复制到lprv中
public:
	CRdRec m_val;	//实时值 
	//{{以下用于运行时模拟
	union U_BASEVAL
	{
		long  lval;
		float fval;
	}bv; //基准值
	int 	m_nSimtype;		//模拟方式0锯齿波，1正旋波
	float   m_fPercent;		//
	//}}
public:
	CVarVal *GetVarVal(){return (CVarVal *)&m_val.val;};
};	

class CRunTags //标签运行管理
{
public:
	CRunTags(){
		m_oaItems.SetGrown(8192);			//按8k增长
	};
	~CRunTags(){};

public:
	void	Init(CTagDefTab *pTagDefTag); //初始化
	void	Simulate(long ltime,CRdRec rec[],int nsize,DRCB_OnDataOk FunOnDataOk,DRHANDLE h); //模拟
	void	Clear()
	{
		m_oaItems.DeleteObjects();
	};
	
protected:
	CSortObArray	m_oaItems;	//标签值数组

};
class CSimCfg // 配置参数
{
public:
	CSimCfg()
	{
		m_bAllowAcce = FALSE;
		m_span = 1000;
		m_timeincr = 1;
		m_starttime = COleDateTime::GetCurrentTime();
		COleDateTime et(m_starttime.GetYear() , 
			m_starttime.GetMonth(),
			m_starttime.GetDay(), 
			m_starttime.GetHour(), 
			m_starttime.GetMinute(), 
			m_starttime.GetSecond() -1);
		m_endtime   = et;

	};
	~CSimCfg(){};

	void SetModify(BOOL bModify){m_bModify = bModify;};
	BOOL IsModify(){return m_bModify;};

	void	Serialize(CArchive& ar, long lVersion)
	{
		if(ar.IsLoading())
		{

			ar >> m_span;
			ar >> m_timeincr;
			//ar >> m_starttime;
			//ar >> m_endtime;

			if (lVersion == 71022802)
				ar >> m_bAllowAcce;
			else
				m_bAllowAcce = FALSE;
		}
		else
		{
			ar << m_span;
			ar << m_timeincr;
			//ar << m_starttime;
			//ar << m_endtime;

			ar << m_bAllowAcce;
		}
	};

public:
	BOOL m_bModify;
	BOOL m_bAllowAcce;
	int m_span;//加速周期
	int m_timeincr;//时标增量
	COleDateTime m_starttime;//开始时间
	COleDateTime m_endtime; //结束时间
};

#define UPDATEVALTAGS		1000 //每次更新的标签值个数
class CSimDrvObj : public CDrvObj
{
public:
	CSimDrvObj();
	virtual ~CSimDrvObj();

public:
	CTagDefTab		m_tagtab;	//标签表
	CSimCfg         m_simcfg;//配置参数
protected:
	CRunTags		m_runtags;	//标签运行
	CRdRec			m_recs[UPDATEVALTAGS];
//以下重载基类的函数
public: //虚函数
	virtual DRVRET  GetErrMsg(CString& szMsg,DRVRET dwcode);			//获取错误信息
	virtual DRVRET  WriteVar(LPDRREC lpval);							//写变量
	virtual DRVRET  GetTagDef(int nindex,LPDRVTAG  lptag);
					
protected: //纯虚函数
	virtual BOOL	OnConfig();					//配置和配置界面,返回TRUE表示修改了配置
	virtual void	Serialize(CArchive& ar);	//串行化保存，读取配置
	virtual DRVRET	OnStart();					//准备运行
	virtual void	OnStop();					//停止运行后的清理工作
	virtual UINT	DoRun();					//正真的运行过程，被RunThread中调用
};

class CWinTagInfo : public CWinList //标签信息窗口
{
public:
	CWinTagInfo();
	~CWinTagInfo();
public:
	CSimDrvObj* m_pdrv;
public:
	int		GetNextSelected(int nstart);//返回选择
	void	ReDraw();
	void	RedrawList();
	
protected:
	virtual CString GetCellText(int nrow,int ncol);//取显示字符串
	virtual CString GetHeadText(int ncol);//取显示字符串
	virtual void	GetRowColorS(int nrow,int ncol,COLORREF &clrTxt,COLORREF &clrBk);//取行颜色
	virtual void	OnClickHeadCol(int ncol);//鼠标点击表头

	virtual void	OnCurSelChange(int nrow);
	virtual void	OnSelChange(int nrowstart,int nrowend);
	virtual void	OnCtrlSelChange(int nrow);//CTRL按住时的选择

};