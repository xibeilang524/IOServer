// ParameterStringParser.cpp: implementation of the CDBDrvConnStr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GeneralAPI.h"
#include "General\ParameterStringParser.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

TRUNK_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParameterStringParser::CParameterStringParser(LPCTSTR lpszParameterString)
{
	m_mapValues.InitHashTable(107);


	if (NULL != lpszParameterString)
		m_szParameterString = lpszParameterString;
	else
		m_szParameterString.Empty();

	if (!m_szParameterString.IsEmpty())
		DoParse();
}

CParameterStringParser::~CParameterStringParser()
{

}

// 设置新的参数字符串，如果lpszParameterString==NULL，则清空连接参数表
BOOL CParameterStringParser::SetParameterString(LPCTSTR lpszParameterString)
{
	ASSERT(lpszParameterString != NULL);

	m_szParameterString = lpszParameterString;
	return DoParse();
}

CString CParameterStringParser::GetParameterString()
{
	CString szParam, szValue, szItem;
	CString szParameterString;

	POSITION pos = m_mapValues.GetStartPosition();
	while(pos) {
		m_mapValues.GetNextAssoc(pos, szParam, szValue);

		EncodeString(szValue, szValue);
		szItem.Format(_T(";%s=%s"), szParam, szValue);
		szParameterString += szItem;
	}

	return szParameterString;
}

BOOL CParameterStringParser::Lookup(LPCTSTR lpszParam, CString& szValue) const
{
	ASSERT(lpszParam != NULL);

	return m_mapValues.Lookup(lpszParam, szValue);
}

void CParameterStringParser::RemoveAll()
{
	m_mapValues.RemoveAll();
	m_szParameterString.Empty();
}

void CParameterStringParser::Remove(LPCTSTR lpszParam)
{
	ASSERT(lpszParam != NULL);

	CString szParam(lpszParam);
	szParam.MakeUpper();

	m_mapValues.RemoveKey(szParam);
}

CString& CParameterStringParser::operator []( LPCTSTR lpszParam )
{
	ASSERT(lpszParam != NULL);

	CString szParam(lpszParam);
	szParam.MakeUpper();

	return m_mapValues[szParam];
}

int CParameterStringParser::GetValueByInt(LPCTSTR lpszParam, int nDefault) 
{		
	ASSERT(lpszParam != NULL);

	CString szParam(lpszParam);
	szParam.MakeUpper();
	
	CString szValue;
	if (m_mapValues.Lookup(szParam, szValue))
		return _ttoi(szValue);
	else
		return nDefault;
}
long CParameterStringParser::GetValueByLong(LPCTSTR lpszParam, long nDefault) 
{		
	ASSERT(lpszParam != NULL);

	CString szParam(lpszParam);
	szParam.MakeUpper();
	
	CString szValue;
	if (m_mapValues.Lookup(szParam, szValue))
		return _ttol(szValue);
	else
		return nDefault;
}

float CParameterStringParser::GetValueByFloat(LPCTSTR lpszParam, float fDefault)
{
	ASSERT(lpszParam != NULL);

	CString szParam(lpszParam);
	szParam.MakeUpper();
	
	CString szValue;
	if (m_mapValues.Lookup(szParam, szValue))
		return (float)atof(szValue);
	else
		return fDefault;
}

double CParameterStringParser::GetValueByDouble(LPCTSTR lpszParam, double dbDefault)
{
	ASSERT(lpszParam != NULL);

	CString szParam(lpszParam);
	szParam.MakeUpper();
	
	CString szValue;
	if (m_mapValues.Lookup(szParam, szValue))
		return atof(szValue);
	else
		return dbDefault;
}

CString CParameterStringParser::GetValueByString(LPCTSTR lpszParam, LPCTSTR lpszDefault)
{
	ASSERT(lpszParam != NULL);
	ASSERT(lpszDefault != NULL);

	CString szParam(lpszParam);
	szParam.MakeUpper();
	
	CString szValue;
	if (m_mapValues.Lookup(szParam, szValue))
		return szValue;
	else
		return (NULL != lpszDefault) ? lpszDefault : _T("");
}

void CParameterStringParser::SetValue(LPCTSTR lpszParam, int nValue)
{
	ASSERT(lpszParam != NULL);
	
	CString szParam(lpszParam);
	szParam.MakeUpper();
	
	m_mapValues[szParam].Format(_T("%d"), nValue);
}

void CParameterStringParser::SetValue(LPCTSTR lpszParam, long nValue)
{
	ASSERT(lpszParam != NULL);
	
	CString szParam(lpszParam);
	szParam.MakeUpper();
	
	m_mapValues[szParam].Format(_T("%ld"), nValue);
}

void CParameterStringParser::SetValue(LPCTSTR lpszParam, float nValue)
{
	ASSERT(lpszParam != NULL);
	
	CString szParam(lpszParam);
	szParam.MakeUpper();
	
	m_mapValues[szParam].Format(_T("%f"), nValue);
}

void CParameterStringParser::SetValue(LPCTSTR lpszParam, double nValue)
{
	ASSERT(lpszParam != NULL);
	
	CString szParam(lpszParam);
	szParam.MakeUpper();
	
	m_mapValues[szParam].Format(_T("%f"), nValue);
}

void CParameterStringParser::SetValue(LPCTSTR lpszParam, LPCTSTR lpszValue)
{
	ASSERT(lpszParam != NULL);
	ASSERT(lpszValue != NULL);

	CString szParam(lpszParam);
	szParam.MakeUpper();

	m_mapValues[szParam] = lpszValue;
}

BOOL CParameterStringParser::DoParse()
{
	m_mapValues.RemoveAll();

	m_szParameterString.Trim();

	int nStart = 0;
	int nEnd = m_szParameterString.GetLength() - 1;
	if (nStart == -1 || nEnd == -1)
		return FALSE;

	LPCTSTR lpszptr = m_szParameterString.GetBuffer(m_szParameterString.GetLength());	
	LPCTSTR lpszend = lpszptr + nEnd;

	int n = 0, j = 0, l = 0, nSeparator = 0;

	TCHAR buf[256];
	TCHAR ch;
	while(lpszptr < lpszend)
	{
		ch = *lpszptr;
		if (ch == _T('\\')) {
			lpszptr++;
			buf[l++] = *lpszptr;
		}
		else if (ch == _T('='))
		{
			nSeparator = l;
			buf[l++] = ch;
		}
		else if (ch == _T(';')) {
			buf[l] = '\0';
			DoParseLine(buf, nSeparator);
			l = 0;
			nSeparator = 0;
		}
		else {
			buf[l++] = ch;
		}

		lpszptr++;
	}
	
	if (l > 0) {
		buf[l] = '\0';
		DoParseLine(buf, nSeparator);
	}
	m_szParameterString.ReleaseBuffer();
	
	
	return (m_mapValues.GetCount() > 0);
}

BOOL CParameterStringParser::DoParseLine(LPCTSTR lpszLine, int nSeparator)
{
	CString szline(lpszLine);
	if (nSeparator > 0) {
		CString szParam = szline.Mid(0, nSeparator);
		szParam.TrimLeft();
		szParam.TrimRight();
		szParam.MakeUpper();
		
		CString szValue = szline.Mid(nSeparator + 1);
		szValue.TrimLeft();
		szValue.TrimRight();

		m_mapValues[szParam] = szValue;
		return TRUE;
	}
	else {
		return FALSE;
	}
}

/* 编码字符串，进行如下编码：

	\ => \\
	; => \;
	= => \=
*/
void CParameterStringParser::EncodeString(LPCTSTR lpszStr, CString &szEncodeStr)
{
	szEncodeStr.Empty();
	size_t len = strlen(lpszStr);
	TCHAR ch;
	for(size_t i = 0; i < len; i++)
	{
		ch = lpszStr[i];

		if (ch == _T('\\'))
			szEncodeStr += _T("\\\\");
		else if (ch == _T(';'))
			szEncodeStr += _T("\\;");
		else if (ch == _T('='))
			szEncodeStr += _T("\\=");
		else
			szEncodeStr += ch;
	}
}


TRUNK_NAMESPACE_END