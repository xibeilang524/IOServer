#include "StdAfx.h"
#include "PropertyManager.h"
#include "Rijndael.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CLicenseProperty - 授权属性项
//////////////////////////////////////////////////////////////////////////

CLicenseProperty::CLicenseProperty()
:	m_szName(_T("")),
	m_szValue(_T("")),
	m_nDataType(DATATYPE_STRING),
	m_nToStringType(TOSTRING_DEFAULT)
{

}

CLicenseProperty::CLicenseProperty(LPCTSTR lpszName, LPCTSTR lpszValue, int nDataType/* = DATATYPE_STRING*/, int nToStringType/* = TOSTRING_DEFAULT*/)
:	m_szName(lpszName),
	m_szValue(lpszValue),
	m_nDataType(nDataType),
	m_nToStringType(nToStringType)
{

}

CLicenseProperty::CLicenseProperty(const CLicenseProperty &obj)
{
	m_szName = obj.m_szName;
	m_szValue = obj.m_szValue;
	m_nDataType = obj.m_nDataType;
	m_nToStringType = obj.m_nToStringType;
}

CLicenseProperty::~CLicenseProperty()
{

}

CString CLicenseProperty::ToString()
{
	CString szt;

	switch(m_nDataType)
	{
	case DATATYPE_BOOL:
		szt = AsBool() ? _T("YES") : _T("NO");
		break;
	case DATATYPE_NUMBER:
	case DATATYPE_FLOAT:
	case DATATYPE_STRING:
	default:
		szt = m_szValue;
		break;
	}

	szt.Trim();
	switch (m_nToStringType)
	{
	case TOSTRING_ZERO_TO_NOLIMIT:
		if (szt == _T("0"))
			szt = _T("无限制");
		break;
	default:
		break;
	}

	return szt;
}

BOOL CLicenseProperty::Serialize(CArchive &ar, DWORD dwVersion)
{
	if (ar.IsLoading())
	{
		ar >> m_szName >> m_szValue >> m_nDataType >> m_nToStringType;
	}
	else {
		ar << m_szName << m_szValue << m_nDataType << m_nToStringType;
	}

	return TRUE;
}

CLicenseProperty &CLicenseProperty::operator=(const CLicenseProperty &obj)
{
	if (this != &obj)
	{
		m_szName = obj.m_szName;
		m_szValue = obj.m_szValue;
		m_nDataType = obj.m_nDataType;
		m_nToStringType = obj.m_nToStringType;
	}

	return *this;
}

bool CLicenseProperty::AsBool(bool bDefault/* = false*/)
{
	if (m_szValue == _T("1"))
		return true;
	else if (m_szValue == _T("0"))
		return false;
	else
		return bDefault;
}

short CLicenseProperty::AsInt16(short shDefault/* = 0*/)
{
	if (m_szValue.IsEmpty())
		return shDefault;

	return (short)_tstoi(m_szValue);
}

WORD CLicenseProperty::AsUInt16(WORD wDefault/* = 0*/)
{
	if (m_szValue.IsEmpty())
		return wDefault;

	return (WORD)_tstoi(m_szValue);
}

int CLicenseProperty::AsInt(int nDefault/* = 0*/)
{
	if (m_szValue.IsEmpty())
		return nDefault;

	return _tstoi(m_szValue);
}

long CLicenseProperty::AsLong(long lDefault/* = 0*/)
{
	if (m_szValue.IsEmpty())
		return lDefault;

	return _tstol(m_szValue);
}

DWORD CLicenseProperty::AsULong(DWORD dwDefault/* = 0*/)
{
	if (m_szValue.IsEmpty())
		return dwDefault;

	return (DWORD)_tstol(m_szValue);
}

__int64 CLicenseProperty::AsInt64(__int64 i64Default/* = 0*/)
{
	if (m_szValue.IsEmpty())
		return i64Default;

	return _atoi64((LPCSTR)(LPCTSTR)m_szValue);
}

float CLicenseProperty::AsFloat(float fDefault/* = 0.0*/)
{
	if (m_szValue.IsEmpty())
		return fDefault;

	return (float)_tstof(m_szValue);
}

double CLicenseProperty::AsDouble(double dblDefault/* = 0.0*/)
{
	if (m_szValue.IsEmpty())
		return dblDefault;

	return _tstof(m_szValue);
}

void CLicenseProperty::SetValue(bool bVal)
{
	if (bVal)
		m_szValue = _T("1");
	else
		m_szValue = _T("0");
}

void CLicenseProperty::SetValue(short shVal)
{
	m_szValue.Format(_T("%d"), (int)shVal);
}

void CLicenseProperty::SetValue(WORD wVal)
{
	m_szValue.Format(_T("%u"), (DWORD)wVal);
}

void CLicenseProperty::SetValue(int nVal)
{
	m_szValue.Format(_T("%d"), nVal);
}

void CLicenseProperty::SetValue(long lVal)
{
	m_szValue.Format(_T("%d"), lVal);
}

void CLicenseProperty::SetValue(DWORD dwVal)
{
	m_szValue.Format(_T("%u"), dwVal);
}

void CLicenseProperty::SetValue(__int64 i64Val)
{
	m_szValue.Format(_T("%i64d"), i64Val);
}

void CLicenseProperty::SetValue(float fVal)
{
	m_szValue.Format(_T("%f"), fVal);
}

void CLicenseProperty::SetValue(double dblVal)
{
	m_szValue.Format(_T("%f"), dblVal);
}

//////////////////////////////////////////////////////////////////////////
// CLicenseProduct - 授权产品项
//////////////////////////////////////////////////////////////////////////

CLicenseProduct::CLicenseProduct(LPCTSTR lpszProductName/* = NULL*/)
:	m_szProduct((lpszProductName) ? lpszProductName : _T(""))
{
}

CLicenseProduct::~CLicenseProduct()
{

}

CLicenseProperty *CLicenseProduct::FindProperty(LPCTSTR lpszName)
{
	for (int i = 0; i < m_arrProperty.GetCount(); i++)
	{
		if (_tcsicmp(m_arrProperty[i].GetName(), lpszName) == 0)
			return &m_arrProperty[i];
	}

	return NULL;
}

void CLicenseProduct::AddProperty(LPCTSTR lpszName, LPCTSTR lpszValue, int nDataType/* = DATATYPE_STRING*/, int nToStringType/* = TOSTRING_DEFAULT*/)
{
	CLicenseProperty *pProperty = FindProperty(lpszName);
	if (pProperty == NULL)
	{
		m_arrProperty.Add( CLicenseProperty(lpszName, lpszValue, nDataType, nToStringType) );
	}
	else {
		pProperty->SetValue(lpszValue);
	}
}

CString CLicenseProduct::GetPropertyString()
{
	CString szt = _T("");

	for (int i = 0; i < m_arrProperty.GetCount(); i++)
	{
		//if (_tcsicmp(m_arrProperty[i].GetName(), LICENSE_PROPERTY_DEFAULT) == 0)
		//	continue;

		if (!szt.IsEmpty())
			szt.Append(_T("\n"));

		szt.Append(m_arrProperty[i].GetName());
		szt.Append(_T(":"));
		szt.Append(m_arrProperty[i].ToString());
	}

	return szt;	
}

BOOL CLicenseProduct::Serialize(CArchive &ar, DWORD dwVersion)
{
	int nCount;
	if (ar.IsLoading())
	{
		ar >> m_szProduct;
		ar >> nCount;
		for (int i = 0; i < nCount; i++)
		{
			CLicenseProperty property;
			property.Serialize(ar, dwVersion);
			m_arrProperty.Add(property);
		}
	}
	else {
		ar << m_szProduct;
		nCount = (int)m_arrProperty.GetCount();
		ar << nCount;
		for (int i = 0; i < nCount; i++)
		{
			if (!m_arrProperty[i].Serialize(ar, dwVersion))
				return FALSE;
		}
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// CLicensePropertyManager - 授权属性管理
//////////////////////////////////////////////////////////////////////////
CLicensePropertyManager::CLicensePropertyManager()
{
	Init();
}

CLicensePropertyManager::~CLicensePropertyManager()
{
	RemoveAll();
}

void CLicensePropertyManager::Init()
{
	CLicenseProduct *pProduct;

	pProduct = new CLicenseProduct(_T("DDClient"));
	pProduct->AddProperty(LICENSE_PROPERTY_DEFAULT, _T("1"), DATATYPE_BOOL);
	m_arrProduct.Add(pProduct);

	pProduct = new CLicenseProduct(_T("DDServer"));
	pProduct->AddProperty(LICENSE_PROPERTY_DEFAULT, _T("1"), DATATYPE_BOOL);
	m_arrProduct.Add(pProduct);

	pProduct = new CLicenseProduct(_T("RDBProxy"));
	pProduct->AddProperty(LICENSE_PROPERTY_DEFAULT, _T("1"), DATATYPE_BOOL);
	pProduct->AddProperty(_T("最大连接数"), _T("0"), DATATYPE_NUMBER, TOSTRING_ZERO_TO_NOLIMIT);
	pProduct->AddProperty(_T("最大标签数"), _T("0"), DATATYPE_NUMBER, TOSTRING_ZERO_TO_NOLIMIT);
	m_arrProduct.Add(pProduct);

	pProduct = new CLicenseProduct(_T("Graph"));
	pProduct->AddProperty(LICENSE_PROPERTY_DEFAULT, _T("1"), DATATYPE_BOOL);
	m_arrProduct.Add(pProduct);

	pProduct = new CLicenseProduct(_T("DDOPCServer"));
	pProduct->AddProperty(LICENSE_PROPERTY_DEFAULT, _T("1"), DATATYPE_BOOL);
	pProduct->AddProperty(_T("最大连接数"), _T("0"), DATATYPE_NUMBER, TOSTRING_ZERO_TO_NOLIMIT);
	pProduct->AddProperty(_T("最大标签数"), _T("0"), DATATYPE_NUMBER, TOSTRING_ZERO_TO_NOLIMIT);
	m_arrProduct.Add(pProduct);

	pProduct = new CLicenseProduct(_T("RDBPEquation"));
	pProduct->AddProperty(LICENSE_PROPERTY_DEFAULT, _T("1"), DATATYPE_BOOL);
	pProduct->AddProperty(_T("最大连接数"), _T("0"), DATATYPE_NUMBER, TOSTRING_ZERO_TO_NOLIMIT);
	pProduct->AddProperty(_T("最大计算任务数"), _T("0"), DATATYPE_NUMBER, TOSTRING_ZERO_TO_NOLIMIT);
	m_arrProduct.Add(pProduct);	
}

void CLicensePropertyManager::RemoveAll()
{
	CLicenseProduct *pProduct;
	for (int i = 0; i < m_arrProduct.GetCount(); i++)
	{
		pProduct = m_arrProduct[i];
		delete pProduct;
	}
	m_arrProduct.RemoveAll();
}

CLicenseProduct *CLicensePropertyManager::FindProduct(LPCTSTR lpszProductName)
{
	CString szName(lpszProductName);

	for(int i = 0; i < m_arrProduct.GetCount(); i++)
	{
		if (szName.CompareNoCase(m_arrProduct[i]->GetProductName()) == 0)
			return m_arrProduct[i];
	}

	return NULL;
}

void CLicensePropertyManager::InitKeys(BYTE *btKeys, int nLen)
{
	::srand((UINT)time(NULL));

	for(int i = 0; i < nLen; i++)
		btKeys[i] = (BYTE)(256 * rand() / (RAND_MAX + 1.0));
}

BOOL CLicensePropertyManager::Serialize(CArchive &ar)
{
	int nCount = 0;
	const DWORD CONST_FLAG = 0xAABBCCDD;
	DWORD dwValue;
	DWORD dwSize;
	BYTE btKeys[256];
	CLicenseProduct *pProduct;

	if (ar.IsLoading()) // 加载
	{
		RemoveAll();

		ar >> dwValue; // 标志位
		if (dwValue != CONST_FLAG)
			return FALSE;

		ar >> dwValue; // 版本号
		if (dwValue < 100)
			return FALSE;
		m_dwVersion = dwValue;

		// 加载加密钥匙
		ar.Read(btKeys, 256);

		ar >> dwSize;
		if (dwSize > 1024 * 10)
			return FALSE;

		BYTE *pEncryptData = new BYTE[dwSize];
		BYTE *pDecryptData = new BYTE[dwSize + 16];
		ar.Read(pEncryptData, dwSize);
		CRijndael rijndael;
		rijndael.SetKey(btKeys, 256);
		if (!rijndael.DecryptData(pEncryptData, pDecryptData, dwSize))
		{
			delete[] pEncryptData;
			delete[] pDecryptData;
			return FALSE;
		}
		delete[] pEncryptData;

		CMemFile mfile;
		mfile.Attach(pDecryptData, dwSize); // 直接挂勾内存块，不需要重新分配内存
		CArchive ar2(&mfile, CArchive::load);

		ar2 >> m_szUserName >> m_szMachineNo >> m_lStartDate >> m_lEndDate;

		ar2 >> nCount; // 有多少个产品授权？
		for(int i = 0; i < nCount; i++)
		{
			pProduct = new CLicenseProduct();
			if (pProduct == NULL)
				return FALSE;

			if (!pProduct->Serialize(ar2, m_dwVersion))
			{
				delete pProduct;
				return FALSE;
			}

			m_arrProduct.Add(pProduct);
		}
		ar2.Close();
		mfile.Detach();
		delete[] pDecryptData;
	}
	else { // 保存
		ar << CONST_FLAG;
		m_dwVersion = 100;
		ar << m_dwVersion;

		// 生成加密KEY
		InitKeys(btKeys, 256);
		ar.Write(btKeys, 256);

		CRijndael rijndael;
		rijndael.SetKey(btKeys, 256);

		CMemFile mfile;
		CArchive ar2(&mfile, CArchive::store);

		ar2 << m_szUserName << m_szMachineNo << m_lStartDate << m_lEndDate;

		nCount = (int)m_arrProduct.GetCount();
		ar2 << nCount;
		for(int i = 0; i < nCount; i++)
		{
			pProduct = m_arrProduct[i];

			if (!pProduct->Serialize(ar2, m_dwVersion))
				return FALSE;
		}

		ar2.Close();
		mfile.SeekToBegin();
		dwSize = (DWORD)mfile.GetLength();
		BYTE *pDecryptData = new BYTE[dwSize];
		BYTE *pEncryptData = new BYTE[dwSize + 16];
		mfile.Read(pDecryptData, dwSize);
		if (!rijndael.EncryptData(pDecryptData, pEncryptData, dwSize))
		{
			delete[] pEncryptData;
			delete[] pDecryptData;

			return FALSE;
		}
		mfile.Close();

		ar << dwSize;
		ar.Write(pEncryptData, dwSize);

		delete[] pEncryptData;
		delete[] pDecryptData;
	}

	return TRUE;
}

BOOL CLicensePropertyManager::Load(LPCTSTR lpszFileName)
{
	CFile file;
	try
	{
		if (!file.Open(lpszFileName, CFile::modeRead))
			return FALSE;

		CArchive ar(&file, CArchive::load);
		Serialize(ar);
		ar.Close();
		file.Close();

		return TRUE;
	}
	catch(CFileException *e)
	{
		delete e;
		return FALSE;
	}
}

BOOL CLicensePropertyManager::Save(LPCTSTR lpszFileName)
{
	CFile file;
	try
	{
		if (!file.Open(lpszFileName, CFile::modeCreate | CFile::modeWrite))
			return FALSE;

		CArchive ar(&file, CArchive::store);
		Serialize(ar);
		ar.Close();
		file.Close();

		return TRUE;
	}
	catch(CFileException *e)
	{
		delete e;
		return FALSE;
	}
}