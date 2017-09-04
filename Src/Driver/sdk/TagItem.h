// TagItem.h: interface for the CTagItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TAGITEM_H__F3C94662_16CD_48A5_8494_2471F3A9B49A__INCLUDED_)
#define AFX_TAGITEM_H__F3C94662_16CD_48A5_8494_2471F3A9B49A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define FIELD_ID		0	// 标签名
#define FIELD_NAME		1	// 标签名
#define FIELD_DESC		2	// 描述
#define FIELD_UNIT		3	// 单位
#define FIELD_TYPE		4	// 类型

#define FIELD_LAST		5	//

#define CUSTOMFIELD_TYPE_LONG		0
#define CUSTOMFIELD_TYPE_DWORD		1
#define CUSTOMFIELD_TYPE_DOUBLE		2
#define CUSTOMFIELD_TYPE_STRING		3

// 自定义字段属性类
class CCustomField
{
public:
	CCustomField(int nDataType = 0, int nFieldIndex = 0, LPCTSTR lpszDisplayName = _T(""), 
		LPCTSTR lpszDefaultValue = _T(""), int nColWidth = 100, DWORD dwAlignFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE) {
		m_nDataType = nDataType;
		m_nFieldIndex = nFieldIndex;
		m_nColWidth = nColWidth;
		m_szDefaultValue = lpszDefaultValue;
		m_szDisplayName = lpszDisplayName;
	}
	virtual ~CCustomField(){};

	const CCustomField& operator =( const CCustomField& src ) {
		if (this != &src)
		{
			m_nDataType			= src.m_nDataType;
			m_nFieldIndex		= src.m_nFieldIndex;
			m_nColWidth			= src.m_nColWidth;
			m_szDefaultValue	= src.m_szDefaultValue;
			m_szDisplayName		= src.m_szDisplayName;
		}

		return *this;
	}
	long DefaultValueAsLong() {
		return _ttol(m_szDefaultValue);
	}
	DWORD DefaultValueAsDWORD() {
		return (DWORD)_ttol(m_szDefaultValue);
	}
	double DefaultValueAsDouble() {
		return atof(m_szDefaultValue);
	}
	LPCTSTR DefaultValueAsString() {
		return m_szDefaultValue;
	}
	void GetDefaultValue(long &lValue) {
		lValue = DefaultValueAsLong();
	}
	void GetDefaultValue(DWORD &dwValue) {
		dwValue = DefaultValueAsDWORD();
	}
	void GetDefaultValue(double &dblValue) {
		dblValue = DefaultValueAsDouble();
	}
	void GetDefaultValue(CString &szValue) {
		szValue = m_szDefaultValue;
	}

public:
	int		m_nDataType;		// 自定义字段数据类型，0 - long、1 - DWORD、2 - double、3 - string
	int		m_nFieldIndex;		// 自定义字段索引号，0-3
	int		m_nColWidth;		// 自定义字段显示列宽
	DWORD	m_dwAlignFormat;	// 自定义字段显示对齐方式
	CString m_szDefaultValue;	// 自定义字段的默认值
	CString	m_szDisplayName;	// 自定义字段显示名
};

typedef CArray<CCustomField, CCustomField&> CCustomFieldArray;

class CTagItem : public CSortItem  
{
public:	
	CTagItem();
	virtual ~CTagItem();

public:
	CString	m_szID;
	CString m_szName; 
	CString m_szDes;
	CString m_szUnit;
	CString m_szNote;	
	WORD 	m_wType;		//数据类型

	//{{ 保留字段
	long	m_lField1;
	long	m_lField2;
	long	m_lField3;
	long	m_lField4;	
	DWORD	m_dwField1;
	DWORD	m_dwField2;
	DWORD	m_dwField3;
	DWORD	m_dwField4;
	double	m_dblField1;
	double	m_dblField2;
	double	m_dblField3;
	double	m_dblField4;
	CString	m_szField1;
	CString m_szField2;
	CString m_szField3;
	CString m_szField4;
	//}} 保留字段

	int		m_nError;		//引用Rdb标签错误,0正常,1:无此标签 2:类型错误
	BOOL	m_bSel;			//选择标志
public:
	CString GetText(int nField);//取数据字符串
	void	SetCustomField(int nField, LPCSTR lpszValue);// 设置自定义字段值
	void	CopyTo(CTagItem *pt);//复制
	void	CopyToDrTag(LPDRVTAG lpdrtag);//复制到接口格式标签定义
	void    Serialize(CArchive& ar, DWORD dwVersion);
	
	//排序比较重载
	virtual		int Compare(CSortItem *pOb);

	CString GetTypeStr();
	void	SetTypeVal(LPCTSTR lpszType);

public:
	// 获取自定义字段总数
	static int GetCustomFieldCount() {
		return m_arrCustomFields.GetSize();
	}
	// 添加一个自定义字段
	static BOOL AddCustomField(
		int nDataType,						// 自定义字段数据类型，可以为CUSTOMFIELD_TYPE_LONG、CUSTOMFIELD_TYPE_DWORD、CUSTOMFIELD_TYPE_DOUBLE、CUSTOMFIELD_TYPE_STRING
		int nFieldIndex,					// 自定义字段在该数据类型中的索引号，可取0-3，在同一个数据类型中，不能重复
		LPCTSTR lpszDisplayName,			// 自定义字段在列表中的显示名称
		LPCTSTR lpszDefaultValue = _T(""),	// 自定义字段默认值
		int nColWidth = 100,				// 自定义字段在列表中的显示宽度
		DWORD dwAlignFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE // 自定义字段在列表中的显示对齐方式
		) {
		for(int i = 0; i < m_arrCustomFields.GetSize(); i++)
		{
			CCustomField &field = m_arrCustomFields[i];
			if (field.m_nDataType == nDataType && field.m_nFieldIndex == nFieldIndex)
				return FALSE;
		}
		CCustomField field(nDataType, nFieldIndex, lpszDisplayName, lpszDefaultValue, nColWidth, dwAlignFormat);
		m_arrCustomFields.Add(field);
		return TRUE;
	}
	// 获取自定义字段
	static CCustomField &GetCustomField(int nIndex) {
		return m_arrCustomFields[nIndex];
	}
	// 设置标签表导入、导出的CSV文件中的格式标签名，默认是DDCDRV，如果使用了自定义字段，
	// 可以改变该名称，以免被其他驱动错误加载
	static void SetCSVFormatName(LPCTSTR lpszName) {
		m_szCSVFormatName = lpszName; 
	}
	// 获取CSV文件中的格式标签名
	static LPCTSTR GetCSVFormatName() {
		return m_szCSVFormatName;
	}

protected:
	static CCustomFieldArray m_arrCustomFields;
	static CString m_szCSVFormatName;
};

class CTagDefTab		//标签定义表
{
public:
	typedef CMap<CString, LPCTSTR, CTagItem*,CTagItem*>	CTagItemMap;
	CTagDefTab()
	{
		m_bModified = FALSE;
		m_mapTab.InitHashTable(225307);	//初始化MAP空间
		m_oaTag.SetGrown(8192);			//按8k增长
	};
	virtual ~CTagDefTab()
	{
		m_mapTab.RemoveAll();//m_oaTag析构时会删除对象,所以不必逐个删除m_mapTab的对象
	};
protected:
	//m_mapTab和m_oaTag里存储相同的对象指针
	CTagItemMap m_mapTab;	//标签MAP
	CSortObArray m_oaTag;									//标签数组
	BOOL m_bModified;
	friend class CRunTags;
	friend class CWinTagInfo;
public:
	void	SetModified(BOOL bModified = TRUE) { m_bModified = bModified; }
	BOOL	IsModified() { return m_bModified; }
	int		GetSize(){return m_oaTag.GetSize();};
	CSortObArray &GetSortObArray() { return m_oaTag; }
	CTagItemMap &GetTagItemMap() { return m_mapTab; }
	void	Serialize(CArchive& ar, DWORD dwVersion);
	BOOL	IsSelected(int nIndex);
	void	SetSel(int nIndex,BOOL bSel);
	void    Qsort() { m_oaTag.Qsort(); }
	int		DelSelect();//删除选择的标签
	int		DelBadTags();
	int		GetBadTagCount();

	void	CopyTo(CTagDefTab &table, BOOL bNewAlloc = TRUE);
	
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
	
	CTagItem*	GetTag(LPCTSTR lpszName); //返回标签对象
	CTagItem*	GetTag(int nIndex); //返回标签对象
	BOOL		ModifyItem(CTagItem* pTagItem, LPCTSTR lpszOldName);
	BOOL		GetAt(int nIndex,CTagItem *ptag); //复制方式取
	BOOL		AddTag(CTagItem *pTag); //添加标签定义，复制方式
	BOOL		IsExist(LPCTSTR lpszName);//是否存在
};

#endif // !defined(AFX_TAGITEM_H__F3C94662_16CD_48A5_8494_2471F3A9B49A__INCLUDED_)
