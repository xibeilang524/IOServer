#pragma once
#include "tool/SpinLock.h"

class CExpressionEngine;
class CTagVal
{
public:
	CTagVal();
	CTagVal(LPCTSTR lpszName,LPCTSTR lpszNamedb,LPCTSTR lpszExpcp,int ntype,int ncomptype,LPCTSTR scompval);
	CTagVal(int ntype);
	virtual ~CTagVal();

	CTagVal& operator=(const CTagVal& tv); //重载=符号
public:
	CString m_szName;		//驱动标签名
	CString m_szExpcp;		//转换公式
	CString m_szNamedb;		//目标服务器标签名
	int		m_nType;		//数据类型
	int		m_nCompType;	//压缩方式
	CString m_szCompVal;	//压缩率
	int		m_nUpdate;		//更新
	BOOL	m_bresend;		//5分钟重发
	long	m_lsendtime;	//发送时间
	double m_dblcompval;

	CSpinLock m_lock;		//用于读写值线程安全的自旋锁
	CVarVal m_val;			//值

	CExpressionEngine *m_pEngine;
};

