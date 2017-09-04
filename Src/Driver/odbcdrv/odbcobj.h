//odbcobj.h
#pragma once

#include <afxmt.h>
#include <afxtempl.h> //模版支持
#include "../sdk/drvsdk.h"
#include "../sdk/sdktool.h"

class CSafeLock  //临界区
{
public:
	CSafeLock(CCriticalSection *pLock){
		if(pLock != NULL){
			m_pLock = pLock;
			m_pLock->Lock();
		}
	};
	virtual ~CSafeLock(){
		if(m_pLock != NULL)
			m_pLock->Unlock();
	};
private:
	CCriticalSection *m_pLock;
};

#define FIELD_NAME		0
#define FIELD_ALIASNAME	1
#define FIELD_DES		2
#define FIELD_UNIT		3
#define FIELD_TYPE		4


class CTagItem : public CSortItem //标签定义项
{
public:
	CTagItem()
	{
		m_szName = _T("");
		m_szAliasName = _T("");
		m_nType = DRT_FLOAT32;
		m_bSel = FALSE;
		m_ldatatime = 0;
	};
	virtual ~CTagItem(){};
public:
	CString m_szName;		// 标签名，或者字段的ID值
	CString m_szAliasName;	// 标签别名，该别名会上传到DDC
	CString m_szDes;
	CString m_szUnit;
	int 	m_nType;		//数据类型

//不存盘部分
public:
	BOOL	m_bSel;			//选择标志
	long	m_ldatatime;	//数据时标，用于相同时间不重发 
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
};

class CWinTagInfo;
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
	void Initdatatime()
	{
		CTagItem *pi;
		int i,n = m_oaTag.GetSize();
	
		for(i=0;i<n;i++)
		{
			pi = (CTagItem	*)m_oaTag.GetAt(i);
			if(pi)
				pi->m_ldatatime = 0;
		}
	};
	CTagItem*	GetTag(LPCTSTR lpszName); //返回标签对象
	BOOL		GetAt(int nIndex,CTagItem *ptag); //复制方式取
	BOOL		AddTag(CTagItem *pTag); //添加标签定义，复制方式
	BOOL		IsExist(LPCTSTR lpszName,long *ptime = NULL);//是否存在
};

#define UPDATEVALTAGS		2000 //每次更新的标签值个数
#define	TAB_REAL			0   //实时表
#define TAB_HIS				1   //历史表
class COdbcDrvObj : public CDrvObj
{
public:
	COdbcDrvObj();
	virtual ~COdbcDrvObj();

public:
	CTagDefTab		m_tagtab;	//标签表
	
	static BOOL IsNeedReConnect(CString &szstatus);//是否需要重新连接
protected:
	//全局配置
	int		m_ntabstyle;
	int		m_ntimestyle; //未用
	CString	m_szconstr; //连接字符串
	CString	m_szfname;	//标签名字段名
	CString	m_szqa;		//质量字段名
	CString	m_szftime;	//时标字段名
	CString	m_szfval;	//值字段名
	CString	m_sztabname;//表名

	CString m_szres;//保留
	//运行参数
	CDatabase	m_db; //关系数据库

	int				m_nrecsize;
	CRdRec			m_recs[UPDATEVALTAGS];
protected:
	CString GetFieldList(LPCTSTR lpszTableName = NULL);
	BOOL			Connectdb(CString &szerr);//连接数据库
	void			OnDataOk(CDBVariant &varname,CDBVariant &vartime,CDBVariant &varqa,CDBVariant &varval);
	void			OnUploadData();

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
	COdbcDrvObj* m_pdrv;
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
