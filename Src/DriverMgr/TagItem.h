#pragma once
#include "DriverMgr_global.h"


class _DRIVERMGR_CLASS CTagItem :
	public CSortItem
{
public:
	CTagItem();
	virtual ~CTagItem();
	virtual		int Compare(CSortItem *pOb);	//排序比较
	void			Serialize(CArchive& ar);	//存盘
	void			CopyTo(CTagItem *pt);
	static  CString GetTypeStr(int ntype);
	static int		GetTypeVal(LPCTSTR lpszType);
	static  CString GetCompTypeStr(int ntype);
	static int		GetCompTypeVal(LPCTSTR lpszType);
protected:
	void			parsecomp(LPCTSTR lpsztxt);//分解压缩属性
public:
	int				m_ntype;	//数据类型
	CString			m_szName;	//标签名
	CString 		m_szdes;	//描述
	CString 		m_szunit;	//单位
	CString 		m_sznamedb;	//在目标服务器中对应的标签名
	CString 		m_sznamedrv;//对应驱动中标签
	CString 		m_szexpcp;	//采集值转换到工程值转换公式
	CString 		m_szexppc;	//工程值转换为采集值公式,暂不用

	//	CString m_szres;	//保留，本版保存了压缩定义，用\n分开，第一行压缩方式，第二行压缩率 needcode

	//{{不存盘区
	int				m_nCompType;//压缩方式
	CString 		m_szCompVal;//压缩率
	BOOL			m_bSel;		//选择
	int				m_nErrStatus;
	//}}

	long	m_ldecid;//设备ID modify811
};

