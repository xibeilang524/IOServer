// TagValue.h: interface for the CTagValue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TAGVALUE_H__317DE317_86D4_4F17_BD6D_D3B00B7A993F__INCLUDED_)
#define AFX_TAGVALUE_H__317DE317_86D4_4F17_BD6D_D3B00B7A993F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include <afxmt.h>
#include "../sdk/dbsdk.h"

typedef struct t_odbctagval	//标签值
{
	TVVAL	m_val;		//值
	long    m_lid;		//标签ID
	long	m_lres;		//保留
}TODBCTAGVAL,*LPODBCTAGVAL; //sizeof() = 32

class CSqlServerDrv;

class COdbcTagVal : public TODBCTAGVAL
{
public:
	COdbcTagVal()
	{
		Init(DT_VOID);
	};
	COdbcTagVal(int ntype)
	{
		Init(ntype);
	};
	~COdbcTagVal()
	{
		Clear();
	};
public:
	void Init(int ntype)
	{
		GetVval().Init(ntype);
		m_lid = 0;
		m_lres = 0;
	};
	void Clear()
	{
		GetVval().Clear();
		m_lres = 0;
	};
	COdbcTagVal& operator=( COdbcTagVal& ov)
	{
		GetVval() = ov.GetVval();	//值
		m_lid = ov.m_lid;
		m_lres = ov.m_lres;
		return *this;
	}; //重载=符号
	COdbcTagVal& operator=( CTagVal& tv)
	{
		GetVval() = tv.GetVval();//值
		m_lres = 0;
		return *this;
	}; //重载=符号
	COdbcTagVal& operator=( CVarVal& vv)
	{
		GetVval() = vv;//值
		m_lres = 0;
		return *this;
	}; //重载=符号
	void SetToTagVal(CTagVal *ptv)
	{
		ptv->GetVval() = GetVval();
	};
	
	CVarVal& GetVval()
	{
		return *((CVarVal*)&m_val);
	};
};

//typedef CArray<COdbcTagVal*,COdbcTagVal*> aodbcval;
//标签值使用的基类
class CTvObj
{
public:
	CTvObj();
	virtual ~CTvObj();
public:
	CString m_szname;//标签名
	long	m_lid;	//标签ID
	BOOL	m_bsave; //是否存历史
	int		m_ntype; //数据类型
	int     m_ncomptype;//压缩类型
	double  m_fcompval;//压缩精度

	CVarVal m_valreal;      //当前实时值
	CVarVal m_valhis;       //处理后的历史值
	BOOL    m_bRealUpdate;  //实时更新标志
	BOOL	m_bHisUpdate;	//历史更新标志

	long	m_lLastDoTime; // 最后处理时间
public:
	BOOL	GetRealVal(COdbcTagVal* pval);//取存盘实时值
	BOOL	GetHisVal(COdbcTagVal* pval);//取存盘历史值
	virtual void AddReal(LPVVAL pval) = 0;//添加实时值,该压缩的要压缩

	virtual BOOL GetLastHisVal(COdbcTagVal* pval){return FALSE;};//取最后的历史值
};

class CTv_digital : public CTvObj //Digital
{
public:
	CTv_digital();
	virtual ~CTv_digital();
public:
	virtual void AddReal(LPVVAL pval);//添加实时值,该压缩的要压缩
};

class CTv_int32 : public CTvObj //int32
{
public:
	CTv_int32();
	virtual ~CTv_int32();
public:
	virtual void AddReal(LPVVAL pval);//添加实时值,该压缩的要压缩
};

class CTv_int64 : public CTvObj //int64
{
public:
	CTv_int64();
	virtual ~CTv_int64();
public:
	virtual void AddReal(LPVVAL pval);//添加实时值,该压缩的要压缩
};

class CTv_string : public CTvObj //string
{
public:
	CTv_string();
	virtual ~CTv_string();
public:
	virtual void AddReal(LPVVAL pval);//添加实时值,该压缩的要压缩
};

class CTv_blob : public CTvObj //blob
{
public:
	CTv_blob();
	virtual ~CTv_blob();
public:
	virtual void AddReal(LPVVAL pval);//添加实时值,该压缩的要压缩
};

class CTv_float32 : public CTvObj //float32
{
public:
	CTv_float32();
	virtual ~CTv_float32();
	int m_nrecs;//但前压缩缓冲区数据个数
	CVarVal m_valtmp[6]; //6个压缩缓冲区
public:
	virtual void AddReal(LPVVAL pval);//添加实时值,该压缩的要压缩
	virtual BOOL GetLastHisVal(COdbcTagVal* pval);//取最后的历史值
};

class CTv_float64 : public CTvObj //float64
{
public:
	CTv_float64();
	virtual ~CTv_float64();
	int m_nrecs;//但前压缩缓冲区数据个数
	CVarVal m_valtmp[6]; //6个压缩缓冲区
public:
	virtual void AddReal(LPVVAL pval);//添加实时值,该压缩的要压缩
	virtual BOOL GetLastHisVal(COdbcTagVal* pval);//取最后的历史值
};
typedef CArray<COdbcTagVal*,COdbcTagVal*> CArrayOdbcVal;

class CMemRdb //内存实时库，不含历史
{
public:
	CMemRdb();
	~CMemRdb();
	
	CMap<CString,LPCTSTR,CTvObj*,CTvObj*> m_map;
protected:
	CCriticalSection m_Lock; //置锁状态m_bConnected访问
public:
	int GetCount() { return m_map.GetCount(); }
	void InitHashSize(int nHashSize) { m_map.InitHashTable(nHashSize); }
	CTvObj *GetTag(LPCTSTR lpszTagName) {
		CTvObj *pt;
		if (m_map.Lookup(lpszTagName, pt))
			return pt;
		else
			return NULL;
	}
	void AddTag(LPDRVTAGATTR ptd); //添加标签
	void AddRealVal(LPCTSTR sname,LPVVAL pval); //添加标签值

	void GetSaveVals(CArrayOdbcVal* par,CArrayOdbcVal* pah); //取需要存盘的实时和历史,调用之前par和pah要设置增量SetSize(0,1024);

	void RemoveTag(LPCTSTR lpszTagName);
	void Clear();//清空

};

#endif // !defined(AFX_TAGVALUE_H__317DE317_86D4_4F17_BD6D_D3B00B7A993F__INCLUDED_)
