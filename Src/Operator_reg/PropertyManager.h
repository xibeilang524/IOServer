#ifndef __TPRI_PROPERTY_MANAGER_H__
#define __TPRI_PROPERTY_MANAGER_H__

#include <afxtempl.h> //模版

#define DATATYPE_BOOL	0
#define DATATYPE_NUMBER	1
#define DATATYPE_FLOAT	2
#define DATATYPE_STRING	3

// CLicenseProperty::ToString()转换方式定义
#define TOSTRING_DEFAULT			0	// 默认方式，不作处理
#define TOSTRING_ZERO_TO_NOLIMIT	1	// 将零视为“无限制”

// 默认的授权属性名称，该项是必须存在的
#define LICENSE_PROPERTY_DEFAULT _T("授权")

/// 授权属性项
class CLicenseProperty
{
public:
	CLicenseProperty();
	CLicenseProperty(LPCTSTR lpszName, LPCTSTR lpszValue, int nDataType = DATATYPE_STRING, int nToStringType = TOSTRING_DEFAULT);
	CLicenseProperty(const CLicenseProperty &obj);
	~CLicenseProperty();

	BOOL Serialize(CArchive &ar, DWORD dwVersion);
	CLicenseProperty &operator=(const CLicenseProperty &obj);

	LPCTSTR GetName() { return m_szName; }
	int GetDataType() { return m_nDataType; }
	void SetDataType(int nDataType) { m_nDataType = nDataType; }

	CString ToString();
	LPCTSTR AsString() { return m_szValue; }
	bool AsBool(bool bDefault = false);
	short AsInt16(short shDefault = 0);
	WORD AsUInt16(WORD wDefault = 0);
	int AsInt(int nDefault = 0);
	long AsLong(long lDefault = 0);
	DWORD AsULong(DWORD dwDefault = 0);
	__int64 AsInt64(__int64 i64Default = 0);
	float AsFloat(float fDefault = 0.0);
	double AsDouble(double dblDefault = 0.0);

	void SetValue(LPCTSTR lpszVal) { m_szValue = (lpszVal) ? lpszVal : _T(""); }
	void SetValue(bool bVal);
	void SetValue(short shVal);
	void SetValue(WORD wVal);
	void SetValue(int nVal);
	void SetValue(long lVal);
	void SetValue(DWORD dwVal);
	void SetValue(__int64 i64Val);
	void SetValue(float fVal);
	void SetValue(double dblVal);

protected:
private:
	int		m_nDataType;
	int		m_nToStringType;
	CString m_szName;
	CString m_szValue;
};

typedef CArray<CLicenseProperty> CLicensePropertyArray;

/// 授权产品项
class CLicenseProduct
{
public:
	CLicenseProduct(LPCTSTR lpszProductName = NULL);
	~CLicenseProduct();

	BOOL Serialize(CArchive &ar, DWORD dwVersion);
	BOOL IsAuthorized() {
		CLicenseProperty *pProperty = FindProperty(LICENSE_PROPERTY_DEFAULT);
		if (pProperty && pProperty->AsBool())
			return TRUE;
		else
			return FALSE;
	}
public:
	LPCTSTR GetProductName() { return m_szProduct; }
	void SetProductName(LPCTSTR lpszName) { m_szProduct = lpszName; }
	int GetCount() { return (int)m_arrProperty.GetCount(); }
	void RemoveAll() { m_arrProperty.RemoveAll(); }

	CLicenseProperty &GetAt(int nIndex) { return m_arrProperty[nIndex]; }
	CLicenseProperty *FindProperty(LPCTSTR lpszName);
	void AddProperty(LPCTSTR lpszName, LPCTSTR lpszValue, int nDataType = DATATYPE_STRING, int nToStringType = TOSTRING_DEFAULT);
	CString GetPropertyString();

protected:
private:
	CString m_szProduct;
	CLicensePropertyArray	m_arrProperty;
};

typedef CArray<CLicenseProduct*, CLicenseProduct*> CLicenseProductArray;

/// 授权属性管理
class CLicensePropertyManager
{
public:
	CLicensePropertyManager();
	~CLicensePropertyManager();

public:
	int GetCount() { return (int)m_arrProduct.GetCount(); }
	CLicenseProduct *GetAt(int nIndex) { return m_arrProduct[nIndex]; }
	void RemoveAll();
	CLicenseProduct *FindProduct(LPCTSTR lpszProductName);

	LPCTSTR GetUserName() { return m_szUserName; }
	void SetUserName(LPCTSTR lpszUserName) { m_szUserName = lpszUserName; }
	LPCTSTR GetMachineNo() { return m_szMachineNo; }
	void SetMachineNo(LPCTSTR lpszMachineNo) { m_szMachineNo = lpszMachineNo; }
	long GetStartDate() { return m_lStartDate; }
	void SetStartDate(long lStartDate) { m_lStartDate = lStartDate; }
	long GetEndDate() { return m_lEndDate; }
	void SetEndDate(long lEndDate) { m_lEndDate = lEndDate; }

	BOOL Load(LPCTSTR lpszFileName);
	BOOL Save(LPCTSTR lpszFileName);

protected:
	void Init();
	// 随机生成加密KEY
	void InitKeys(BYTE *btKeys, int nLen);
	BOOL Serialize(CArchive &ar);

private:
	DWORD m_dwVersion;
	CString m_szUserName;
	CString m_szMachineNo;
	long	m_lStartDate;
	long	m_lEndDate;
	CLicenseProductArray m_arrProduct;
};

#endif // end of __TPRI_PROPERTY_MANAGER_H__