#pragma once

class CDecVal;
class CDec2TagVal
{
public:
	CDec2TagVal(LPCTSTR lpszName){
		m_szname = lpszName;
		m_tab.SetSize(0,8);
		m_pNext = NULL;
	};
	~CDec2TagVal()
	{
		m_tab.RemoveAll();
	};

	CDec2TagVal* GetNext() {
		return m_pNext;
	}

	void AddItem(CDec2TagVal* pTagVal);
	void OnDrvTagDataOk(CDecVal* pdecval, LPDRREC pdv); //驱动数据OK
public:
	CString						m_szname; //驱动标签名
	CArray<CTagVal*,CTagVal*>	m_tab;
	CVarVal						m_val;//保存上次的值，用于过滤

protected:
	CDec2TagVal*				m_pNext;
	BOOL IsDataEqual(TVVAL *pval1, TVVAL *pval2);
	/*
	static		DWORD WINAPI GetExternVar( 
					CExpItem *pi,//in/out
					void *lpParam1,//参数 CDecVal* 用于查找设备内变量
					void *lpParam2//参数 LPDRREC* 用于取cx,px
					);//计算公式用于获取外部变量用的回调函数
	*/
	friend class			CDecVal;
};

