//SortObArray.cpp
//
#include "stdafx.h"
#include "SortObArray.h"

/////////////////////////////////////////////////////////////////////////////
CSortItem::CSortItem()
{
	m_nSortWho = 0;	
	m_bAsc = TRUE;		
}
CSortItem::~CSortItem()
{
}
void CSortItem::SetSortType(int	nSortWho,BOOL	bAsc)//设置排序方式
{
	m_nSortWho = nSortWho;
	m_bAsc = bAsc;
}
int CSortItem::Compare(CSortItem *pOb)//排序比较,bASC是升序
{
	return 0;//基类不做任何判断 
}

/////////////////////////////////////////////////////////////////////////////
CSortObArray::CSortObArray()
{
	m_ob = NULL;
	m_nMaxSize = 0;
	m_nSize = 0;
	m_nGrown = 256;

	m_nSortWho = 0;	
	m_bAsc = TRUE;		
}

CSortObArray::~CSortObArray()
{
	DeleteObjects();//删除所有对象

	if(m_ob)
		free(m_ob); //释放空间
}

void CSortObArray::SetSortType(int	nSortWho,BOOL	bAsc)//设置排序方式
{
	m_nSortWho = nSortWho;
	m_bAsc = bAsc;
}

int CSortObArray::Comp(const void *p1,const void *p2 )
{
	LPSORTOB pi1 ,pi2;
	pi1 = *((LPSORTOB *)p1);
	pi2 = *((LPSORTOB *)p2);

	return pi1->Compare(pi2);
};


void  CSortObArray::Qsort()//排序
{
	if((m_ob == NULL)||(m_nSize == 0))
		return ;
	for(int i=0;i<m_nSize;i++)
		m_ob[i]->SetSortType(m_nSortWho,m_bAsc);
	qsort(m_ob, m_nSize,sizeof(LPSORTOB),Comp);
}

void CSortObArray::SetGrown(int ngrown)	//设置增长空间，默认256
{
	m_nGrown = ngrown;
	if(m_nGrown < 16)
		m_nGrown = 16;
	if(m_nGrown > 32 * 1024)
		m_nGrown = 32 * 1024;
}

BOOL	CSortObArray::Grown()//增长
{
	LPSORTOB *ob = NULL;
	ob = (LPSORTOB *)malloc((m_nMaxSize + m_nGrown) * sizeof(LPSORTOB));//分配初始空间
	
	if(ob == NULL)
		return FALSE;

	m_nMaxSize += m_nGrown;

	if(m_ob == NULL)
	{
		m_ob = ob;
		return TRUE;
	}
	
	int i;
	for(i=0;i<m_nSize;i++)
		ob[i] =  m_ob[i];//复制
	
	while(i < m_nMaxSize)//多余的清零
	{
		ob[i] = NULL;
		i++;
	}
	free(m_ob);//清除旧的
	m_ob = ob;//保存新的
	return TRUE;
}

void CSortObArray::RemoveAll()//移除所有对象，不删除对象
{
	if((m_ob == NULL)||(m_nSize == 0))
		return;
	
	for(int i=0;i< m_nMaxSize;i++)
		m_ob[i] = NULL;

	m_nSize = 0;
}

LPSORTOB	CSortObArray::RemoveAt(int nIndex)//移除对象，不删除初对象，返回被移除的对象
{
	if((m_ob == NULL)||(m_nSize == 0))
		return NULL;

	if((nIndex < 0)||(nIndex >= m_nSize))
		return NULL;

	LPSORTOB pob = m_ob[nIndex];
	
	int i;
	for(i=nIndex;i<m_nSize-1;i++)
		m_ob[i] = m_ob[i+1];

	m_ob[i] = NULL;
	m_nSize--;

	return pob;
}

void	CSortObArray::DeleteObjects()//删除所有对象
{
	if((m_ob == NULL)||(m_nSize == 0))
		return;
	for(int i=0;i< m_nSize;i++)
	{
		delete m_ob[i];
		m_ob[i] = 0;
	}
	m_nSize = 0;
}

int		CSortObArray::GetSize()//取当前对象个数
{
	return m_nSize;
}

LPSORTOB CSortObArray::GetAt(int nIndex)//取一个对象
{
	if((m_ob == NULL)||(m_nSize == 0))
		return NULL;
	if((nIndex >=0) &&(nIndex <m_nSize))
		return m_ob[nIndex];
	return NULL;
};

BOOL CSortObArray::Add(LPSORTOB pOb)//添加
{
	if(m_nSize < m_nMaxSize)
	{
		m_ob[m_nSize] = pOb;
		m_nSize++;
		return TRUE;
	}
	if(!Grown())
		return FALSE;
	m_ob[m_nSize] = pOb;
	m_nSize++;
	return TRUE;
}

/*
二分查找,返回对象指针,查找前假设已按升序,返回位置index
返回值：
	>=0:index
	-1:没找到

  */
int CSortObArray::BinarySearch(CSortItem *ps)
{
	if(m_nSize == 0)
		return -1;

	int nh=0, nt=m_nSize-1;
	int j=-1,ncmp=0;
	while(nh <= nt)
	{
		j = (nh + nt)/2;
		
		ncmp = ps->Compare(m_ob[j]);//->Compare(ps);
		if(ncmp == 0)
			return j;
		if(ncmp < 0)
			nt = j-1;
		else
			nh = j+1;
	}
	if(nh > nt)
		return -1;
	return j;
}