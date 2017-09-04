#pragma once
#include "TagVal.h"
#include "DrvItem.h"
#include "Dec2TagVal.h"

class CDecVal
{
public:
	CDecVal();

	CDecVal(long lid,int ntags);
	~CDecVal();

public:
	BOOL	m_bvardataok; //可变数据类型更新
	CTagVal* GetTagVal(LPCTSTR lpszName);//取标签值指针	
	int		GetSaveRecs(CTagVal* lprecs,int nsize);//取存盘数据(基本类型)
	BOOL	GetSaveVarVal(CTagVal* lprecs);//取存盘的可变数据，string blob
	CString GetDecName(){return m_szDecName;};
protected:
	void	Init(CDrvItem* pdec);//初始化
	void	OnDecDataOK(T_DRREC recs[],int nsize); //添加设备值，用于在回调函数中使用，加锁
	DWORD	OnWriteHistoryCB(T_DRREC recs[],int nsize);

protected:
	CString m_szDecName;
	long	m_id;
	int		m_nposval;//取存盘数据的位置
	int		m_nposvar;//可变类型位置
	//CMap<CString,LPCTSTR,CTagVal*,CTagVal*> m_map;	//CTagVal* map,以标签名为索引
	CNoCaseMap<CTagVal*,CTagVal*> m_map;	//CTagVal* map,以标签名为索引
	CArray<CTagVal*,CTagVal*> m_array;				//CTagVal*数组，用于循环取数存到实时库用
	CMap<CString,LPCTSTR,CDec2TagVal*,CDec2TagVal*> m_mapd2t;//映射表,以驱动标签名为索引。
protected:
	friend class CDecValMan;
};

