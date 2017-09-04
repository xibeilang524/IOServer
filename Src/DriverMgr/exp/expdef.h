#pragma once

#define DT_LONG		1
#define DT_LLONG    2
#define DT_FLOAT	3
#define DT_DOUBLE	4

template <class T,int SIZE> class CArrayStackTemp
{
public:
	CArrayStackTemp (){top= -1;}; //缺省构造函数,构造一个空堆栈
	~CArrayStackTemp (){};//析构函数
	bool Push(T element); //入栈
	bool Pop(T& element);//出栈
	int GetSize(){return top+1;};//取元素个数
private:
	T Buffer[SIZE];
	int top;
};
template <class T, int SIZE> bool CArrayStackTemp<T, SIZE>:: Push (T element)
{
	top++;
	if (top>SIZE-1) {top--; return false;}//满
	Buffer[top]=element;
	return true;
}
template <class T, int SIZE> bool CArrayStackTemp<T, SIZE>:: Pop (T& element)
{
	if (top==-1) return false;//空
	element=Buffer[top];
	top--;
	return true;
}

class CExpItem;

// 获取外部变量函数
typedef DWORD (WINAPI *EXP_GetExternVar)(
					CExpItem *pi,//in/out
					void *lpParam1,//参数1
					void *lpParam2//参数2
					);//返回值0表示成功

//数据项
class CExpItem
{
public:
	CString m_szid;//为空表示值，不为空表示变量。
	
	int m_ndt;	//DT_CHAR
	union VAL
	{
		long	lVal;
		float	fltVal;
		__int64 llVal;
		double	dblVal;
	}m_val;

	CExpItem()
	{
		m_szid = "";
		m_ndt = DT_LONG;
		m_val.lVal = 0;
	};
	~CExpItem(){};

	BOOL ChangeType(int ndt);//改变数据类型

	//{{运算符
	CExpItem *Add(CExpItem *ps);// +
	CExpItem *Dec(CExpItem *ps);// -
	CExpItem *Mut(CExpItem *ps);// *
	CExpItem *Div(CExpItem *ps);// /
	CExpItem *Mod(CExpItem *ps);// %
	CExpItem *LeftMove(CExpItem *ps);// <<
	CExpItem *RightMove(CExpItem *ps);// >>
	CExpItem *BitAnd(CExpItem *ps);// &
	CExpItem *BitOr(CExpItem *ps);// |
	//}}

	//{{赋值
	void SetData(long val){m_ndt = DT_LONG;m_val.lVal = val;};
	void SetData(float val){m_ndt = DT_FLOAT;m_val.fltVal = val;};
	void SetData(__int64 val){m_ndt = DT_LLONG;m_val.llVal = val;};
	void SetData(double val){m_ndt = DT_DOUBLE;m_val.dblVal = val;};

	
	int SetData(LPCTSTR lpszVal,EXP_GetExternVar pfun,void *pParam1,void *pParam2);//字符串复制，转换为数值
	//}}

	BOOL GetVal(CString &szr);//取值
};

class CExpStack //堆栈
{
public:
	int nsize;//空间大小
	int ntop;//定位置，初始位置-1;
	DWORD dwbuf[1024];

	CExpStack()
	{
		nsize = 1024;
		ntop = -1;
	};
	~CExpStack()
	{
		CExpItem *pi;
		while(ntop >=0)
		{
			pi = (CExpItem*)dwbuf[ntop];
			delete pi;
			ntop--;
		}
	};
	int Getsize()
	{
		return ntop+1;
	};
	BOOL push(CExpItem *pi)
	{
		if(ntop >= nsize - 1)
			return FALSE;
		ntop++;
		dwbuf[ntop] = DWORD(pi);
		return TRUE;
	};

	CExpItem* pop()
	{
		if(ntop == -1)
			return NULL;
		CExpItem *pi;
		pi = (CExpItem*)dwbuf[ntop];
		ntop--;
		return pi;
	};
};

int			g_Mid2Behind(LPCTSTR lpszS,CString &szD,CString &szErr);//中置->后置 ret = 0 ok
CExpItem*	g_CalBehindParse(LPCTSTR lpszParse,EXP_GetExternVar pfun,void *pParam1,void *pParam2,long &ltags);//根据后置表达式计算,返回结果

