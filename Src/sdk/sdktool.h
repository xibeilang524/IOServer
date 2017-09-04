//sdktool.h
#pragma once
#include "drvdef.h"

#ifndef DT_VOID
#define DT_VOID		DRT_VOID	//空类型
#define DT_DIGITAL	DRT_DIGITAL	//开关量	,也是4字节整数		
#define DT_INT32	DRT_INT32	//4字节整数
#define DT_FLOAT32	DRT_FLOAT32	//4字节浮点数
#define DT_INT64	DRT_INT64	//8字节整数
#define DT_FLOAT64	DRT_FLOAT64	//8字节浮点数
#define DT_STRING	DRT_STRING	//字符串
#define DT_BLOB		DRT_BLOB	//二进制数据块
#endif

//辅助类
class CVarVal : public TVVAL  //变体对象
{
public:
	CVarVal();
	CVarVal(DWORD dwtype);
	~CVarVal();
public:
	void ResetType(WORD wtype);
	void Init(DWORD dwtype);
	void ClearData(); //清除可变长度数据内存pstr,pblob,调用前保证是经过初始化的。
	void Clear(); //清除可变长度数据内存pstr,pblob,调用前保证是经过初始化的。
	void SetVoid();
	CVarVal& operator=(const CVarVal& vv); //重载=符号
	CVarVal& operator=(const long lv); //重载=符号
	CVarVal& operator=(const __int64 llv); //重载=符号
	CVarVal& operator=(const float fv); //重载=符号
	CVarVal& operator=(const double dblv); //重载=符号
	CVarVal& operator=(const char* ps); //重载=符号

	BOOL	 operator==(const CVarVal& vv); //重载==符号,比较相等


	BOOL	SetBlob(void *pdata,int nlen); //设置BLOB数据

	int		GetInt();
	__int64 GetInt64();
	long	GetLong(){return GetInt();};
	float   GetFloat();
	double  GetDouble();

	BOOL ChangeType(DWORD dwtype); //改变类型

	CString GetValStr(); //取值，转换为字符串形式
	CString GetTimeStr(long ltimeadjust = 0);
	CString GetQaStr();

protected:
	
	void ToVoid();
	void ToDigital();
	void ToInt32();
	void ToInt64();
	void ToFloat32();
	void ToFloat64();
	void ToString();
	void ToBlob();
};

class CRdRec : public T_DRREC
{
public:
	CRdRec();
	CRdRec(int ntype);
	~CRdRec();
public:
	void SetName(const char* ps);
	void SetVal(TVVAL* pval);
	void SetVoid();
	void ClearData();
	void ReSetType(int ntype){GetVarVal()->ResetType(ntype);};
	CVarVal* GetVarVal(){return (CVarVal*)&val;};

};

class CSoe : public TSOE //事件对象
{
public:
	CSoe();
	~CSoe();
public:
	void	Init();
	void	Clear(); //清除里面的可变长度数据m_pbinargs,调用前一定要保证是经过初始化的.
	CSoe& operator=(CSoe& soe); //重载=符号
	void	SetBinArgs(void* pargs,int nsize); //设置二进制参数
};

