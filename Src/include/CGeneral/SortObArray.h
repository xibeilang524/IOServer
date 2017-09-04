//SortObArray.h
#ifndef __KIMI_SORTOBARRAY_H__
#define __KIMI_SORTOBARRAY_H__

_KIMI_NAMESPACE__BEGIN

///////////////////////////////////////////////////////////////////////////////
class TPRICOMMON_CLASS CSortItem	//排序对象基类
{
public:
	CSortItem();
	virtual		~CSortItem();
	void		SetSortType(int	nSortWho,BOOL	bAsc);//设置排序方式
	virtual		int Compare(CSortItem *pOb);//排序比较
protected:
	int			m_nSortWho;	//对哪一个排序
	BOOL		m_bAsc;		//是否升序
};

///////////////////////////////////////////////////////////////////////////////
typedef CSortItem *LPSORTOB;//类型定义
class TPRICOMMON_CLASS CSortObArray //排序对象动态数组
{
public:
	CSortObArray();
	~CSortObArray();//删除对象,删除空间

public:
	BOOL		Add(LPSORTOB pOb);		//添加
	int			GetSize();				//取当前对象个数
	LPSORTOB	GetAt(int nIndex);		//取一个对象

	void		RemoveAll();			//清除空间并重新初始化 ，不删除对象
	LPSORTOB	RemoveAt(int nIndex);	//移除对象，不删除初对象，返回被移除的对象
	void		DeleteObjects();		//删除所有对象
	void		SetGrown(int ngrown);	//设置增长空间，默认256
	void		SetSortType(int	nSortWho,BOOL	bAsc);//设置排序方式
	BOOL		IsSortAsc(){return m_bAsc;};

	void		Qsort();		//快速排序
	int			BinarySearch(CSortItem *ps);//2分查找，假设已按升序排序,返回位置index, -1标示没有
protected:
	LPSORTOB	*m_ob;		//对象数组
	int			m_nMaxSize;	//空间大小
	int			m_nSize;	//当前对象个数
	int			m_nGrown;	//每次空间增量

	//排序用
	int			m_nSortWho;	//对哪一个排序
	BOOL		m_bAsc;		//是否升序
private:
	static int  Comp(const void *p1,const void *p2);
	BOOL		Grown();//增长
};

_KIMI_NAMESPACE__END

#endif // end of __KIMI_SORTOBARRAY_H__