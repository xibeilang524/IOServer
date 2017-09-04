#include "StdAfx.h"
#include <time.h>
#include <fstream>
#include "KimiCore.h"
#include "KimiCore/License/LicenseFile.h"
#include "KimiCore/stream/MemoryStream.h"
#include "KimiCore/Utility/Utility.h"

#define LICENSE_FILE_VERSION	1
#define LICENSE_FILE_FLAGS		"TPRI-LIC"

_KIMI_NAMESPACE_BEGIN


#pragma pack(push)
#pragma pack(1)
typedef struct
{
	char	szLicFileFlags[8];	// 标识头（TPRI-LIC）
	DWORD	dwCRC32;			// CRC32校验码
	DWORD	dwVersion;			// 版本号
	BYTE	btEncryptKeys[256];	// 加密用的随机Key
	DWORD	dwDataLen;			// 数据长度
}LICENSE_FILE_HEAD, *LPLICENSE_FILE_HEAD;

#pragma pack(pop)

#pragma warning( push )
#pragma warning(disable : 4267)

//////////////////////////////////////////////////////////////////////////
// CLicenseProperty - 授权属性项
//////////////////////////////////////////////////////////////////////////

CLicenseProperty::CLicenseProperty()
	: m_strName("")
	, m_strValue("")
	, m_nDataType(LIC_DT_STRING)
	, m_nMinValue(0)
	, m_nMaxValue(0)
{

}

CLicenseProperty::CLicenseProperty(const char *lpszName, const char * lpszDisplayName, const char *lpszValue, int nDataType/* = LIC_DT_STRING*/, 
	const char *lpszDescr/* = NULL*/, const char *lpszCategory/* = NULL*/, int nMin/* = 0*/, int nMax/* = 0*/, const char *lpszOptions/* = NULL*/)
	: m_strName(lpszName)
	, m_nDataType(nDataType)
	, m_strDescription(lpszDescr ? lpszDescr : "")
	, m_strCategory(lpszCategory ? lpszCategory : "")
	, m_nMinValue(nMin)
	, m_nMaxValue(nMax)
	, m_strOptions(lpszOptions ? lpszOptions : "")
{
	SetDisplayName(lpszDisplayName ? lpszDisplayName : lpszName);

	if (lpszValue)
		m_strValue = lpszValue;
}

CLicenseProperty::CLicenseProperty(const CLicenseProperty &obj)
{
	m_strName = obj.m_strName;
	m_strDisplayName = obj.m_strDisplayName;
	m_strValue = obj.m_strValue;
	m_nDataType = obj.m_nDataType;
	m_strDescription = obj.m_strDescription;
	m_strCategory = obj.m_strCategory;
	m_nMinValue = obj.m_nMinValue;
	m_nMaxValue = obj.m_nMaxValue;
	m_strOptions = obj.m_strOptions;
}

CLicenseProperty::~CLicenseProperty()
{

}

/*
属性的数据类型：
	BOOL	- 布尔类型，true或false
	NUMBER	- 整形类型，long
	FLOAT	- 浮点类型，double
	STRING	- 字符串类型
*/
int CLicenseProperty::String2DataType(const char *s)
{
	if (_stricmp(s, "BOOL") == 0)
		return LIC_DT_BOOL;
	else if (_stricmp(s, "NUMBER") == 0)
		return LIC_DT_NUMBER;
	else if (_stricmp(s, "FLOAT") == 0)
		return LIC_DT_FLOAT;
	else if (_stricmp(s, "DATE") == 0)
		return LIC_DT_DATE;
	else
		return LIC_DT_STRING;
}

string CLicenseProperty::DataType2String(int nType)
{
	switch(nType)
	{
	case LIC_DT_BOOL:
		return "BOOL";
		break;
	case LIC_DT_NUMBER:
		return "NUMBER";
		break;
	case LIC_DT_FLOAT:
		return "FLOAT";
		break;
	case LIC_DT_DATE:
		return "DATE";
		break;
	default:
		return "STRING";
		break;
	}
}

string CLicenseProperty::ToString()
{
	string s;

	switch(m_nDataType)
	{
	case LIC_DT_BOOL:
		s = AsBool() ? "True" : "False";
		break;
	case LIC_DT_DATE:
		{
			char szBuffer[16];
			__time64_t tt = _atoi64(m_strValue.c_str());

			struct tm ptmTemp;
			errno_t err = _localtime64_s(&ptmTemp, &tt);
			if (err != 0 || !strftime(szBuffer, 16, "%Y-%m-%d", &ptmTemp))
			{
				szBuffer[0] = '\0';
			}

			s = szBuffer;
			break;
		}
	case LIC_DT_NUMBER:
	case LIC_DT_FLOAT:
	case LIC_DT_STRING:
	default:
		s = m_strValue;
		break;
	}

	return s;
}

BOOL CLicenseProperty::Load(CMemoryStream &stream, DWORD dwVersion)
{
	stream >> m_nDataType >> m_strName >> m_strValue;

	stream >> m_strCategory >> m_strDisplayName >> m_strDescription;
	stream >> m_nMinValue >> m_nMaxValue >> m_strOptions;

	return TRUE;
}

BOOL CLicenseProperty::Save(CMemoryStream &stream, DWORD dwVersion)
{
	stream << m_nDataType << m_strName << m_strValue;
	stream << m_strCategory << m_strDisplayName << m_strDescription;
	stream << m_nMinValue << m_nMaxValue << m_strOptions;

	return TRUE;
}

CLicenseProperty &CLicenseProperty::operator=(const CLicenseProperty &obj)
{
	if (this != &obj)
	{
		m_strName = obj.m_strName;
		m_strDisplayName = obj.m_strDisplayName;
		m_strValue = obj.m_strValue;
		m_nDataType = obj.m_nDataType;
		m_strDescription = obj.m_strDescription;
		m_strCategory = obj.m_strCategory;
		m_nMinValue = obj.m_nMinValue;
		m_nMaxValue = obj.m_nMaxValue;
		m_strOptions = obj.m_strOptions;
	}

	return *this;
}

bool CLicenseProperty::AsBool(bool bDefault/* = false*/)
{
	if (m_strValue == "1")
		return true;
	else if (m_strValue == "0")
		return false;
	else if (_stricmp(m_strValue.c_str(), "true") == 0)
		return true;
	else if (_stricmp(m_strValue.c_str(), "false") == 0)
		return false;
	else
		return bDefault;
}

short CLicenseProperty::AsInt16(short shDefault/* = 0*/)
{
	if (m_strValue.empty())
		return shDefault;

	return (short)atoi(m_strValue.c_str());
}

WORD CLicenseProperty::AsUInt16(WORD wDefault/* = 0*/)
{
	if (m_strValue.empty())
		return wDefault;

	return (WORD)atoi(m_strValue.c_str());
}

int CLicenseProperty::AsInt(int nDefault/* = 0*/)
{
	if (m_strValue.empty())
		return nDefault;

	return atoi(m_strValue.c_str());
}

long CLicenseProperty::AsLong(long lDefault/* = 0*/)
{
	if (m_strValue.empty())
		return lDefault;

	return atol(m_strValue.c_str());
}

DWORD CLicenseProperty::AsULong(DWORD dwDefault/* = 0*/)
{
	if (m_strValue.empty())
		return dwDefault;

	return (DWORD)atol(m_strValue.c_str());
}

__int64 CLicenseProperty::AsInt64(__int64 i64Default/* = 0*/)
{
	if (m_strValue.empty())
		return i64Default;

	return _atoi64(m_strValue.c_str());
}

float CLicenseProperty::AsFloat(float fDefault/* = 0.0*/)
{
	if (m_strValue.empty())
		return fDefault;

	return (float)atof(m_strValue.c_str());
}

double CLicenseProperty::AsDouble(double dblDefault/* = 0.0*/)
{
	if (m_strValue.empty())
		return dblDefault;

	return atof(m_strValue.c_str());
}

void CLicenseProperty::SetValue(bool bVal)
{
	if (bVal)
		m_strValue = "1";
	else
		m_strValue = "0";
}

void CLicenseProperty::SetValue(short shVal)
{
	char buf[64];
	sprintf_s(buf, 64, "%d", (int)shVal);
	m_strValue = buf;
}

void CLicenseProperty::SetValue(WORD wVal)
{
	char buf[64];
	sprintf_s(buf, 64, "%u", (DWORD)wVal);
	m_strValue = buf;
}

void CLicenseProperty::SetValue(int nVal)
{
	char buf[64];
	sprintf_s(buf, 64, "%d", nVal);
	m_strValue = buf;
}

void CLicenseProperty::SetValue(long lVal)
{
	char buf[64];
	sprintf_s(buf, 64, "%d", lVal);
	m_strValue = buf;
}

void CLicenseProperty::SetValue(DWORD dwVal)
{
	char buf[64];
	sprintf_s(buf, 64, "%u", dwVal);
	m_strValue = buf;
}

void CLicenseProperty::SetValue(__int64 i64Val)
{
	char buf[64];
	sprintf_s(buf, 64, "%llu", i64Val);
	m_strValue = buf;
}

void CLicenseProperty::SetValue(float fVal)
{
	char buf[64];
	sprintf_s(buf, 64, "%f", fVal);
	m_strValue = buf;
}

void CLicenseProperty::SetValue(double dblVal)
{
	char buf[64];
	sprintf_s(buf, 64, "%f", dblVal);
	m_strValue = buf;
}

//////////////////////////////////////////////////////////////////////////
// 软件授权项
//////////////////////////////////////////////////////////////////////////

CLicenseSoftware::CLicenseSoftware()
	: m_dwUserData(0)
{

}

CLicenseSoftware::CLicenseSoftware(const CLicenseSoftware &lsi)
	: m_dwUserData(0)
{
	CLicenseSoftware::operator=(lsi);
}

CLicenseSoftware::~CLicenseSoftware()
{

}

CLicenseSoftware& CLicenseSoftware::operator=(const CLicenseSoftware &lsi)
{
	if (&lsi != this)
	{
		m_dwUserData = lsi.m_dwUserData;
		m_strSoftwareName = lsi.m_strSoftwareName;
		m_strDescription = lsi.m_strDescription;

		m_arrProperties.clear();

		for (size_t i = 0; i < lsi.m_arrProperties.size(); i++)
		{
			m_arrProperties.push_back( lsi.m_arrProperties[i] );
		}
	}

	return *this;
}

BOOL CLicenseSoftware::Load(CMemoryStream& stream, DWORD dwVersion)
{
	stream >> m_strSoftwareName;

	int n;
	stream >> n;

	string strName, strValue;

	m_arrProperties.clear();
	for (int i = 0; i < n; i++)
	{
		CLicenseProperty pty;
		pty.Load(stream, dwVersion);

		m_arrProperties.push_back( pty ); 
	}

	return TRUE;
}

BOOL CLicenseSoftware::Save(CMemoryStream& stream, DWORD dwVersion)
{
	stream << m_strSoftwareName;

	stream << (int)m_arrProperties.size();

	for (size_t i = 0; i < m_arrProperties.size(); i++)
	{
		m_arrProperties[i].Save(stream, dwVersion);
	}

	return TRUE;
}

CLicenseProperty* CLicenseSoftware::AddProperty(const char *lpszName, const char * lpszDisplayName, const char *lpszDataType, const char *lpszDefault, const char *lpszDescr, 
												const char *lpszCategory, int nMin, int nMax, const char *lpszOptions)
{
	CLicenseProperty *pPty = GetProperty(lpszName);
	if (pPty)
	{
		pPty->SetDisplayName(lpszDisplayName);
		pPty->SetDescription(lpszDescr);
		pPty->SetCategory(lpszCategory);
		pPty->SetMinValue(nMin);
		pPty->SetMaxValue(nMax);
		pPty->SetOptions(lpszOptions);

		return pPty;
	}
	else {
		CLicenseProperty pty(lpszName, lpszDisplayName, lpszDefault, CLicenseProperty::String2DataType(lpszDataType), lpszDescr, lpszCategory, nMin, nMax, lpszOptions);
		m_arrProperties.push_back( pty ); 

		return &m_arrProperties[m_arrProperties.size() - 1];
	}
}

CLicenseProperty* CLicenseSoftware::GetProperty(const char *lpszPropertyName)
{
	for (size_t i = 0; i < m_arrProperties.size(); i++)
	{
		if (_stricmp(m_arrProperties[i].GetName(), lpszPropertyName) == 0)
			return &m_arrProperties[i];
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 软件授权文件
//////////////////////////////////////////////////////////////////////////

CLicenseFile::CLicenseFile(void)
	: m_bExpiredTime(TRUE)
	, m_lExpiredTime( (long)::time(NULL) + 15552000) // 默认大约半年有效期
{
}

CLicenseFile::~CLicenseFile(void)
{
}

CLicenseFile& CLicenseFile::operator=(const CLicenseFile &file)
{
	if (&file != this)
	{
		m_dwVersion = file.m_dwVersion;
		m_strUserName = file.m_strUserName;
		m_strMachineCode = file.m_strMachineCode;
		m_bExpiredTime = file.m_bExpiredTime;
		m_lExpiredTime = file.m_lExpiredTime;

		m_arrLicenseSoftwares.clear();
		for (size_t i = 0; i < file.m_arrLicenseSoftwares.size(); i++)
		{
			m_arrLicenseSoftwares.push_back( file.m_arrLicenseSoftwares[i] ); 
		}
	}

	return *this;
}

void CLicenseFile::Clear()
{
	m_strUserName.clear();
	m_strMachineCode.clear();
	m_lExpiredTime = (long)::time(NULL) + 15552000;

	m_arrLicenseSoftwares.clear();
}

BOOL CLicenseFile::Load(const char *lpszFileName)
{
	FILE *pf;
	errno_t err = fopen_s(&pf, lpszFileName, "rb+");
	if (err || !pf)
		return FALSE;

	CMemoryStream stream;
	char buf[1024];

	size_t nRead = fread(buf, 1, 1024, pf);

	while(nRead)
	{
		stream.Write(buf, (long)nRead);

		nRead = fread(buf, 1, 1024, pf);
	}
	fclose(pf);

	return DoLoad(stream);
}

BOOL CLicenseFile::Load(const unsigned char *lpszLicBuffer, const unsigned long dwLicBufferSize)
{
	if ((!lpszLicBuffer) || (dwLicBufferSize <= 0))
		return FALSE;

	CMemoryStream stream;
	stream.Write((void*)lpszLicBuffer, dwLicBufferSize);

	return DoLoad(stream);
}

BOOL CLicenseFile::DoLoad(CMemoryStream &stream)
{
	stream.Seek(0, STREAM_START);

	if (stream.GetLength() <= sizeof(LICENSE_FILE_HEAD) + 16)
		return FALSE;

	LPLICENSE_FILE_HEAD pLicFileHead = (LPLICENSE_FILE_HEAD)stream.GetDataPtr();
	if (memcmp(pLicFileHead->szLicFileFlags, LICENSE_FILE_FLAGS, 8) != 0)
		return FALSE;

	size_t nSize = stream.GetLength() - sizeof(LICENSE_FILE_HEAD) + 16;
	if (nSize <= 0 || nSize > 1024 * 1024)
		return FALSE;

	u1byte *pData = new u1byte[nSize];
	CRijndael Rijndael;
	Rijndael.SetKey(pLicFileHead->btEncryptKeys, 256);

	u4byte dwLen = stream.GetLength() - sizeof(LICENSE_FILE_HEAD);
	Rijndael.DecryptData( (BYTE*)stream.GetDataPtr() + sizeof(LICENSE_FILE_HEAD), pData, dwLen );

	stream.Seek(sizeof(LICENSE_FILE_HEAD), STREAM_START);
	stream.Write(pData, dwLen);
	pLicFileHead = (LPLICENSE_FILE_HEAD)stream.GetDataPtr();

	delete[] pData;

	DWORD dwDataLen = sizeof(LICENSE_FILE_HEAD) + pLicFileHead->dwDataLen;

	DWORD dwCRC32 = Utility::GetCRC32((BYTE*)stream.GetDataPtr() + 8 + sizeof(DWORD), dwDataLen - 8 - sizeof(DWORD));
	if (pLicFileHead->dwCRC32 != dwCRC32)
	{
		return FALSE;
	}

	stream.Seek(sizeof(LICENSE_FILE_HEAD), STREAM_START);

	stream >> m_strUserName;
	stream >> m_strMachineCode;
	stream >> m_bExpiredTime;
	stream >> m_lExpiredTime;

	int n;
	stream >> n;

	string strName, strValue;

	m_arrLicenseSoftwares.clear();
	for (int i = 0; i < n; i++)
	{
		CLicenseSoftware item;
		item.Load(stream, LICENSE_FILE_VERSION);

		m_arrLicenseSoftwares.push_back( item ); 
	}

	return TRUE;
}

BOOL CLicenseFile::Save(const char *lpszFileName)
{
	LICENSE_FILE_HEAD LicFileHead;

	memcpy_s(LicFileHead.szLicFileFlags, 8, LICENSE_FILE_FLAGS, 8);
	LicFileHead.dwCRC32 = 0;
	LicFileHead.dwVersion = LICENSE_FILE_VERSION;

	InitKeys(LicFileHead.btEncryptKeys, 256);

	CMemoryStream stream;

	stream.Write(&LicFileHead, sizeof(LICENSE_FILE_HEAD));

	stream << m_strUserName;
	stream << m_strMachineCode;
	stream << m_bExpiredTime;
	stream << m_lExpiredTime;

	stream << (int)m_arrLicenseSoftwares.size();

	for (size_t i = 0; i < m_arrLicenseSoftwares.size(); i++)
	{
		m_arrLicenseSoftwares[i].Save(stream, LICENSE_FILE_VERSION);
	}

	char *pBuffer = stream.GetDataPtr();
	LPLICENSE_FILE_HEAD pLicFileHead = (LPLICENSE_FILE_HEAD)pBuffer;
	pLicFileHead->dwDataLen = stream.GetLength() - sizeof(LICENSE_FILE_HEAD);
	pLicFileHead->dwCRC32 = Utility::GetCRC32((BYTE*)pBuffer + 8 + sizeof(DWORD), stream.GetLength() - 8 - sizeof(DWORD));

	u1byte *pData = new u1byte[pLicFileHead->dwDataLen + 16];
	CRijndael Rijndael;
	Rijndael.SetKey(LicFileHead.btEncryptKeys, 256);

	u4byte dwLen = pLicFileHead->dwDataLen;
	Rijndael.EncryptData( (BYTE*)pBuffer + sizeof(LICENSE_FILE_HEAD), pData, dwLen );

// 汉字文件名有问题
// 	fstream filestream;
// 	filestream.open(lpszFileName, ios::out | ios::binary); 
// 
// 	filestream.write(pBuffer, sizeof(LPLICENSE_FILE_HEAD));
// 	filestream.write((const char *)pData, dwLen);
	FILE *pf;
	errno_t err = fopen_s(&pf, lpszFileName, "wb+");
	if (!err && pf)
	{
		fwrite(pBuffer, 1, sizeof(LICENSE_FILE_HEAD), pf);
		fwrite(pData, 1, dwLen, pf);
		fclose(pf);
	}

	delete[] pData;

	return TRUE;
}

void CLicenseFile::InitKeys(BYTE *btKeys, int nLen)
{
	::srand((UINT)time(NULL));

	for(int i = 0; i < nLen; i++)
		btKeys[i] = (BYTE)(256 * rand() / (RAND_MAX + 1.0));
}

CLicenseSoftware* CLicenseFile::GetLicenseSoftware(const char *lpszSoftwareName, bool bNew/* = false*/)
{
	for (size_t i = 0; i < m_arrLicenseSoftwares.size(); i++)
	{
		if (_stricmp(m_arrLicenseSoftwares[i].GetSoftwareName(), lpszSoftwareName) == 0)
			return &m_arrLicenseSoftwares[i];
	}
	if (bNew)
	{
		CLicenseSoftware software;
		software.SetSoftwareName(lpszSoftwareName);

		m_arrLicenseSoftwares.push_back(software);
		return &m_arrLicenseSoftwares[m_arrLicenseSoftwares.size() - 1];
	}
	else
		return NULL;
}

BOOL CLicenseFile::CheckMachineCode()
{
	// 机器码为空，表示可对任意计算机授权
	if (m_strMachineCode.empty())
		return TRUE;

	// 授权文件机器码
	CMachineCodeHelper mch(FALSE);
	mch.SetMachineBase64Encode( m_strMachineCode.c_str() );

	return (mch == m_mchLocal);
}

long CLicenseFile::CheckExpiredTime()
{
	if (!m_bExpiredTime)
		return 1;

	long lNow = (long)::time(NULL);

	return m_lExpiredTime - lNow;
}

#pragma warning( pop )

_KIMI_NAMESPACE_END