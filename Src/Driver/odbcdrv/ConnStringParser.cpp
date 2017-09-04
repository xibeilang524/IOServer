// ConnStringParser.cpp: implementation of the CConnStringParser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "odbcdrv.h"
#include "ConnStringParser.h"

#include <Atlbase.h>
#pragma comment( lib, "shlwapi.lib")	// PathFileExists()函数需要

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////////
// UNKNOWN初始化参数函数
void UNKNOWN_InitParamProc(CConnStringParser *parser)
{
	parser->RemoveAll();
}

// UNKNOWN构造连接字符串函数
CString UNKNOWN_MakeConnStringProc(CConnStringParser *parser)
{
	return _T("");
}

// UNKNOWN参数解析处理函数
BOOL UNKNOWN_ParserValueProc(CConnStringParser *parser, LPCTSTR lpszKey, LPCTSTR lpszValue)
{	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// MSSQL初始化参数函数
void MSSQL_InitParamProc(CConnStringParser *parser)
{
	if (!parser)
		return;

	parser->RemoveAll();
	
	parser->SetValue(_T("DRIVER"),		_T("{SQL Server}"));
	parser->SetValue(_T("Server"),		_T("localhost"));
	parser->SetValue(_T("Port"),		_T("1433"));
	parser->SetValue(_T("INSTANCE"),	_T(""));
	parser->SetValue(_T("Database"),	_T("odbcrdb"));
	parser->SetValue(_T("UID"),			_T("sa"));
	parser->SetValue(_T("PWD"),			_T(""));

	parser->SetValue(_T("DSN"),		_T("''"));
	parser->SetValue(_T("Network"),		_T("DBMSSOCN"));
}

// MSSQL构造连接字符串函数
CString MSSQL_MakeConnStringProc(CConnStringParser *parser)
{
	if (!parser)
		return _T("");

	CString szcon, szInst = parser->GetValue(_T("INSTANCE"));
	CString szServer = parser->GetValue(_T("SERVER"));

	if(szInst.IsEmpty())
		szcon.Format("DRIVER={SQL Server};DSN=%s;Server=%s;Database=%s;Network=%s;Address=%s,%s;UID=%s;PWD=%s",
			parser->GetValue(_T("DSN")), szServer, parser->GetValue(_T("Database")), parser->GetValue(_T("Network")), szServer, 
			parser->GetValue(_T("Port")), parser->GetValue(_T("UID")), parser->GetValue(_T("PWD")));
	else
		szcon.Format("DRIVER={SQL Server};DSN=%s;Server=%s\\%s;Database=%s;Network=%s;Address=%s,%s;UID=%s;PWD=%s",
			parser->GetValue(_T("DSN")), szServer, szInst, parser->GetValue(_T("Database")), parser->GetValue(_T("Network")), szServer, 
			parser->GetValue(_T("Port")), parser->GetValue(_T("UID")), parser->GetValue(_T("PWD")));

	CString szKey, szValue;
	for(int i = 0; i < parser->GetCount(); i++)
	{
		szKey = parser->GetKey(i);
		if ((szKey.CompareNoCase(_T("DRIVER")) != 0) &&
			(szKey.CompareNoCase(_T("Server")) != 0) &&
			(szKey.CompareNoCase(_T("Port")) != 0) &&
			(szKey.CompareNoCase(_T("INSTANCE")) != 0) &&
			(szKey.CompareNoCase(_T("DATABASE")) != 0) &&
			(szKey.CompareNoCase(_T("UID")) != 0) &&
			(szKey.CompareNoCase(_T("PWD")) != 0) &&
			(szKey.CompareNoCase(_T("DSN")) != 0) &&
			(szKey.CompareNoCase(_T("Network")) != 0) &&
			(szKey.CompareNoCase(_T("ADDRESS")) != 0) )
		{			
			szValue = parser->GetValue(i);
			szcon += _T(";") + szKey;
			szcon += _T("=") + szValue;
		}
	}

	return szcon;
}

// MSSQL参数解析处理函数
BOOL MSSQL_ParserValueProc(CConnStringParser *parser, LPCTSTR lpszKey, LPCTSTR lpszValue)
{
	CString szKey = lpszKey;
	if (szKey.CompareNoCase(_T("Server")) == 0)
	{
		CString szValue = lpszValue;
		CString szInst = _T("");
		int nPos = szValue.Find(_T('\\'));
		if (nPos != -1)
		{
			szInst = szValue.Mid(nPos + 1);
			szInst.TrimLeft();
			szInst.TrimRight();

			szValue = szValue.Mid(0, nPos);
			szValue.TrimLeft();
			szValue.TrimRight();
		}
		parser->SetValue(_T("INSTANCE"), szInst);
		parser->SetValue(_T("Server"), szValue);
	}
	else if (szKey.CompareNoCase(_T("ADDRESS")) == 0)
	{
		CString szValue = lpszValue;
		CString szPort;
		int nPos = szValue.Find(_T(','));
		if (nPos == -1)
			szPort = _T("1433");
		else {
			szPort = szValue.Mid(nPos + 1);
			szPort.TrimLeft();
			szPort.TrimRight();
			if (szPort.IsEmpty())
				szPort = _T("1433");
		}
		parser->SetValue(_T("Port"), szPort);
	}
	else
		parser->SetValue(lpszKey, lpszValue);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// MySQL初始化参数函数
void MySQL_InitParamProc(CConnStringParser *parser)
{
	if (!parser)
		return;

	parser->RemoveAll();
	
	parser->SetValue(_T("DRIVER"),		_T("{MySQL ODBC 5.1 Driver}"));
	parser->SetValue(_T("Server"),		_T("localhost"));
	parser->SetValue(_T("Port"),		_T("3306"));
	parser->SetValue(_T("Database"),	_T("odbcrdb"));
	parser->SetValue(_T("UID"),			_T("root"));
	parser->SetValue(_T("PWD"),			_T(""));
	
	// 必须设置Charset=gb2312，否则无法写入带汉字的记录
	parser->SetValue(_T("Charset"),		_T("gb2312"));
	parser->SetValue(_T("Option"),		_T("67108864"));
}

// MySQL构造连接字符串函数
CString MySQL_MakeConnStringProc(CConnStringParser *parser)
{
	if (!parser)
		return _T("");
	
	CString szcon;
	
	szcon.Format("DRIVER={MySQL ODBC 5.1 Driver};Server=%s;Port=%s;Database=%s;UID=%s;PWD=%s;Charset=%s;Option=%s",
		parser->GetValue(_T("Server")), parser->GetValue(_T("Port")), parser->GetValue(_T("Database")), 
		parser->GetValue(_T("UID")), parser->GetValue(_T("PWD")), parser->GetValue(_T("Charset")), 
		parser->GetValue(_T("Option")));
	
	CString szKey, szValue;
	for(int i = 0; i < parser->GetCount(); i++)
	{
		szKey = parser->GetKey(i);
		if ((szKey.CompareNoCase(_T("DRIVER")) != 0) &&
			(szKey.CompareNoCase(_T("Server")) != 0) &&
			(szKey.CompareNoCase(_T("Port")) != 0) &&
			(szKey.CompareNoCase(_T("Database")) != 0) &&
			(szKey.CompareNoCase(_T("UID")) != 0) &&
			(szKey.CompareNoCase(_T("PWD")) != 0) &&
			(szKey.CompareNoCase(_T("Charset")) != 0) &&
			(szKey.CompareNoCase(_T("Option")) != 0) )
		{			
			szValue = parser->GetValue(i);
			szcon += _T(";") + szKey;
			szcon += _T("=") + szValue;
		}
	}
	return szcon;
}

// MySQL参数解析处理函数
BOOL MySQL_ParserValueProc(CConnStringParser *parser, LPCTSTR lpszKey, LPCTSTR lpszValue)
{
	return (parser->SetValue(lpszKey, lpszValue) != -1);
}

//////////////////////////////////////////////////////////////////////////
// Oracle初始化参数函数
void Oracle_InitParamProc(CConnStringParser *parser)
{
	if (!parser)
		return;
	
	parser->RemoveAll();
	
	parser->SetValue(_T("DRIVER"),	_T("{Microsoft ODBC for Oracle}"));
	parser->SetValue(_T("Server"),	_T(""));
	parser->SetValue(_T("UID"),		_T(""));
	parser->SetValue(_T("PWD"),		_T(""));
}

// Oracle构造连接字符串函数
CString Oracle_MakeConnStringProc(CConnStringParser *parser)
{
	if (!parser)
		return _T("");
	
	CString szcon;
	
	szcon.Format("DRIVER={Microsoft ODBC for Oracle};Server=%s;UID=%s;PWD=%s",
		parser->GetValue(_T("Server")), parser->GetValue(_T("UID")), parser->GetValue(_T("PWD")));
	
	CString szKey, szValue;
	for(int i = 0; i < parser->GetCount(); i++)
	{
		szKey = parser->GetKey(i);
		if ((szKey.CompareNoCase(_T("DRIVER")) != 0) &&
			(szKey.CompareNoCase(_T("Server")) != 0) &&
			(szKey.CompareNoCase(_T("UID")) != 0) &&
			(szKey.CompareNoCase(_T("PWD")) != 0) )
		{			
			szValue = parser->GetValue(i);
			szcon += _T(";") + szKey;
			szcon += _T("=") + szValue;
		}
	}
	return szcon;
}

// Oracle参数解析处理函数
BOOL Oracle_ParserValueProc(CConnStringParser *parser, LPCTSTR lpszKey, LPCTSTR lpszValue)
{
	return (parser->SetValue(lpszKey, lpszValue) != -1);
}

//////////////////////////////////////////////////////////////////////////
// Sybase 11初始化参数函数
void Sybase11_InitParamProc(CConnStringParser *parser)
{
	if (!parser)
		return;
	
	parser->RemoveAll();
	
	parser->SetValue(_T("DRIVER"),	_T("{SYBASE SYSTEM 11}"));
	parser->SetValue(_T("Srvr"),	_T("127.0.0.1"));
	parser->SetValue(_T("UID"),		_T(""));
	parser->SetValue(_T("PWD"),		_T(""));
}

// Sybase11构造连接字符串函数
CString Sybase11_MakeConnStringProc(CConnStringParser *parser)
{
	if (!parser)
		return _T("");
	
	CString szcon;
	
	szcon.Format("DRIVER={SYBASE SYSTEM 11};Srvr=%s;UID=%s;PWD=%s",
		parser->GetValue(_T("Srvr")), parser->GetValue(_T("UID")), parser->GetValue(_T("PWD")));
	
	CString szKey, szValue;
	for(int i = 0; i < parser->GetCount(); i++)
	{
		szKey = parser->GetKey(i);
		if ((szKey.CompareNoCase(_T("DRIVER")) != 0) &&
			(szKey.CompareNoCase(_T("Srvr")) != 0) &&
			(szKey.CompareNoCase(_T("UID")) != 0) &&
			(szKey.CompareNoCase(_T("PWD")) != 0) )
		{			
			szValue = parser->GetValue(i);
			szcon += _T(";") + szKey;
			szcon += _T("=") + szValue;
		}
	}
	return szcon;
}

// Sybase11参数解析处理函数
BOOL Sybase11_ParserValueProc(CConnStringParser *parser, LPCTSTR lpszKey, LPCTSTR lpszValue)
{
	return (parser->SetValue(lpszKey, lpszValue) != -1);
}

//////////////////////////////////////////////////////////////////////////
// Sybase 12初始化参数函数
void Sybase12_InitParamProc(CConnStringParser *parser)
{
	if (!parser)
		return;
	
	parser->RemoveAll();
	
	parser->SetValue(_T("DRIVER"),	_T("{SYBASE ASE ODBC Driver}"));
	parser->SetValue(_T("Srvr"),	_T("127.0.0.1"));
	parser->SetValue(_T("UID"),		_T(""));
	parser->SetValue(_T("PWD"),		_T(""));
}

// Sybase12构造连接字符串函数
CString Sybase12_MakeConnStringProc(CConnStringParser *parser)
{
	if (!parser)
		return _T("");
	
	CString szcon;
	
	szcon.Format("DRIVER={SYBASE ASE ODBC Driver};Srvr=%s;UID=%s;PWD=%s",
		parser->GetValue(_T("Srvr")), parser->GetValue(_T("UID")), parser->GetValue(_T("PWD")));
	
	CString szKey, szValue;
	for(int i = 0; i < parser->GetCount(); i++)
	{
		szKey = parser->GetKey(i);
		if ((szKey.CompareNoCase(_T("DRIVER")) != 0) &&
			(szKey.CompareNoCase(_T("Srvr")) != 0) &&
			(szKey.CompareNoCase(_T("UID")) != 0) &&
			(szKey.CompareNoCase(_T("PWD")) != 0) )
		{			
			szValue = parser->GetValue(i);
			szcon += _T(";") + szKey;
			szcon += _T("=") + szValue;
		}
	}
	return szcon;
}

// Sybase12参数解析处理函数
BOOL Sybase12_ParserValueProc(CConnStringParser *parser, LPCTSTR lpszKey, LPCTSTR lpszValue)
{
	return (parser->SetValue(lpszKey, lpszValue) != -1);
}

//////////////////////////////////////////////////////////////////////////
// DB2初始化参数函数
void DB2_InitParamProc(CConnStringParser *parser)
{
	if (!parser)
		return;
	
	parser->RemoveAll();
	
	parser->SetValue(_T("DRIVER"),		_T("{IBM DB2 ODBC Driver}"));
	parser->SetValue(_T("HostName"),	_T("127.0.0.1"));
	parser->SetValue(_T("Database"),	_T(""));
	parser->SetValue(_T("Port"),		_T("50000"));
	parser->SetValue(_T("UID"),			_T(""));
	parser->SetValue(_T("PWD"),			_T(""));
	
	parser->SetValue(_T("Protocol"),	_T("TCPIP"));
}

// DB2构造连接字符串函数
CString DB2_MakeConnStringProc(CConnStringParser *parser)
{
	if (!parser)
		return _T("");
	
	CString szcon;
	
	szcon.Format("DRIVER={IBM DB2 ODBC Driver};HostName=%s;Port=%s;Database=%s;UID=%s;PWD=%s;Protocol=%s",
		parser->GetValue(_T("HostName")), parser->GetValue(_T("Port")), parser->GetValue(_T("Database")), 
		parser->GetValue(_T("UID")), parser->GetValue(_T("PWD")), parser->GetValue(_T("Protocol")));
	
	CString szKey, szValue;
	for(int i = 0; i < parser->GetCount(); i++)
	{
		szKey = parser->GetKey(i);
		if ((szKey.CompareNoCase(_T("DRIVER")) != 0) &&
			(szKey.CompareNoCase(_T("HostName")) != 0) &&
			(szKey.CompareNoCase(_T("Database")) != 0) &&
			(szKey.CompareNoCase(_T("Port")) != 0) &&
			(szKey.CompareNoCase(_T("UID")) != 0) &&
			(szKey.CompareNoCase(_T("PWD")) != 0) &&
			(szKey.CompareNoCase(_T("Protocol")) != 0) )
		{			
			szValue = parser->GetValue(i);
			szcon += _T(";") + szKey;
			szcon += _T("=") + szValue;
		}
	}
	return szcon;
}

// DB2参数解析处理函数
BOOL DB2_ParserValueProc(CConnStringParser *parser, LPCTSTR lpszKey, LPCTSTR lpszValue)
{
	return (parser->SetValue(lpszKey, lpszValue) != -1);
}

// 数据库选项配置
DBTYPEDATA CConnStringParser::m_arrDbTypeData[] =
{
	{	// 无驱动定义
		DBT_UNKNOWN, 
		_T(""),
		_T(""), 
		UNKNOWN_InitParamProc,
		UNKNOWN_MakeConnStringProc,
		UNKNOWN_ParserValueProc
	}, 
	{	// SQL2k驱动定义
		DBT_MSSQL, 
		_T("{SQL Server}"),
		_T("Microsoft SQL Server ODBC Driver"), 
		MSSQL_InitParamProc,
		MSSQL_MakeConnStringProc,
		MSSQL_ParserValueProc
	}, 
	{	// MySQL驱动定义
		DBT_MYSQL, 
		_T("{MySQL ODBC 5.1 Driver}"),
		_T("MySQL ODBC 5.1 Driver"),
		MySQL_InitParamProc,
		MySQL_MakeConnStringProc,
		MySQL_ParserValueProc
	}, 
	{	// Oracle驱动定义
		DBT_ORACLE, 
		_T("{Microsoft ODBC for Oracle}"),
		_T("Microsoft ODBC for Oracle"),
		Oracle_InitParamProc,
		Oracle_MakeConnStringProc,
		Oracle_ParserValueProc
	}, 
	{	// Sybase System 11 ODBC Driver驱动定义
		DBT_SYBASE11, 
		_T("{SYBASE SYSTEM 11}"),
		_T("Sybase System 11 ODBC Driver"),
		Sybase11_InitParamProc,
		Sybase11_MakeConnStringProc,
		Sybase11_ParserValueProc
	}, 
	{	// Sybase System 12 (12.5) ODBC Driver驱动定义
		DBT_SYBASE12, 
		_T("{SYBASE ASE ODBC Driver}"),
		_T("Sybase System 12 ODBC Driver"),
		Sybase12_InitParamProc,
		Sybase12_MakeConnStringProc,
		Sybase12_ParserValueProc
	},
	{	// DB2驱动定义
		DBT_DB2, 
		_T("{IBM DB2 ODBC Driver}"),
		_T("IBM DB2 ODBC Driver"),
		DB2_InitParamProc,
		DB2_MakeConnStringProc,
		DB2_ParserValueProc
	}
};

// 数据库选项个数
int CConnStringParser::m_nDbTypeCount = 4;	//7;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConnStringParser::CConnStringParser(LPCTSTR lpszConnStr/* = NULL*/)
{
	if (lpszConnStr)
	{		
		m_szConnStr = lpszConnStr;
		DoParser();
	}
	else {
		m_nDbType = DBT_MSSQL;
		m_bSuccess = FALSE;
		Init();
	}
}

CConnStringParser::CConnStringParser(const CString &szConnStr)
{
	m_szConnStr = szConnStr;
	DoParser();
}

CConnStringParser::~CConnStringParser()
{
	RemoveAll();
}

void CConnStringParser::SetDBType(int type)
{
	m_nDbType = type;
	Init();
}

LPDBTYPEDATA CConnStringParser::FindDBTypeData(int ntype)
{
	for(int i = 0; i < m_nDbTypeCount; i++)
	{
		if (m_arrDbTypeData[i].nTypeID == m_nDbType)
			return &m_arrDbTypeData[i];
	}

	return NULL;
}

LPDBTYPEDATA CConnStringParser::FindDBTypeData(LPCTSTR szDriver)
{
	CString szdriver = szDriver;

	int nsize = sizeof(m_arrDbTypeData) / sizeof(DBTYPEDATA);
	for(int i = 0; i < nsize; i++)
	{
		if (szdriver.CompareNoCase(m_arrDbTypeData[i].szDriver) == 0)
			return &m_arrDbTypeData[i];
	}
	
	return NULL;
}

LPCTSTR CConnStringParser::GetDBTypeStr()
{
	static TCHAR szUnknownType[] = _T("Unknown");

	LPDBTYPEDATA pData = FindDBTypeData(m_nDbType);
	if (pData)
		return pData->szName;
	else
		return szUnknownType;

}

LPCTSTR CConnStringParser::GetKey(int nIdex)
{ 
	ASSERT((nIdex >= 0) && (nIdex < m_arrKeys.GetCount()));

	POSITION pos = m_arrKeys.FindIndex(nIdex);
	if (pos)
		return m_arrKeys.GetAt(pos);
	else
		return NULL;
}

LPCTSTR CConnStringParser::GetValue(int nIdex)
{ 
	ASSERT((nIdex >= 0) && (nIdex < m_arrValues.GetCount()));

	POSITION pos = m_arrValues.FindIndex(nIdex);
	if (pos)
		return m_arrValues.GetAt(pos);
	else
		return NULL;
}

LPCTSTR CConnStringParser::GetValue(LPCTSTR lpszKey)
{
	return (*this)[lpszKey];
}

int CConnStringParser::IndexOf(LPCTSTR lpszKey)
{
	CString szKey = lpszKey;

	for(int i = 0; i < m_arrKeys.GetCount(); i++)
	{
		if (szKey.CompareNoCase(GetKey(i)) == 0)
			return i;
	}

	return -1;
}

LPCTSTR CConnStringParser::GetConnectString()
{
	LPDBTYPEDATA pData = FindDBTypeData(m_nDbType);
	if (!pData) {
		m_nDbType = DBT_MSSQL;
		pData = FindDBTypeData(m_nDbType);
	}
	if (pData) {
		m_szConnStr = pData->procMakeConnStr(this);
		m_bSuccess = TRUE;
	}
	else
		m_bSuccess = FALSE;

	return m_szConnStr;
}

BOOL CConnStringParser::SetConnectString(LPCTSTR lpszConnStr)
{
	m_szConnStr = lpszConnStr;
	DoParser();
	if (!IsSuccess())
	{
		//m_nDbType = DBT_MSSQL;
		m_nDbType = DBT_UNKNOWN;
		m_bSuccess = FALSE;
		Init();
	}

	return IsSuccess();
}

void CConnStringParser::RemoveAll()
{
	m_arrKeys.RemoveAll(); 
	m_arrValues.RemoveAll(); 
	m_nDbType = DBT_UNKNOWN;
	m_bSuccess = FALSE;
	m_szConnStr.Empty();
}

int CConnStringParser::SetValue(LPCTSTR lpszKey, LPCTSTR lpszValue)
{
	int nIndex = IndexOf(lpszKey);
	if (nIndex == -1)
	{
		m_arrKeys.AddTail( lpszKey );
		m_arrValues.AddTail( lpszValue );
		
		return m_arrKeys.GetCount() - 1;
	}
	else {
		POSITION pos = m_arrValues.FindIndex(nIndex);
		m_arrValues.SetAt(pos, lpszValue);
		return nIndex;
	}
}

int CConnStringParser::SetValue(LPCTSTR lpszKey, int nValue)
{
	CString szValue;
	szValue.Format(_T("%d"), nValue);
	return SetValue(lpszKey, szValue);
}

int CConnStringParser::SetValue(LPCTSTR lpszKey, UINT nValue)
{
	CString szValue;
	szValue.Format(_T("%u"), nValue);
	return SetValue(lpszKey, szValue);
}

int CConnStringParser::SetValue(LPCTSTR lpszKey, long lValue)
{
	CString szValue;
	szValue.Format(_T("%ld"), lValue);
	return SetValue(lpszKey, szValue);
}

int CConnStringParser::SetValue(LPCTSTR lpszKey, float fValue)
{
	CString szValue;
	szValue.Format(_T("%f"), fValue);
	return SetValue(lpszKey, szValue);
}

int CConnStringParser::SetValue(LPCTSTR lpszKey, double dbValue)
{
	CString szValue;
	szValue.Format(_T("%f"), dbValue);
	return SetValue(lpszKey, szValue);
}

void CConnStringParser::Init()
{
	LPDBTYPEDATA pData = FindDBTypeData(m_nDbType);
	if (!pData) {
		m_nDbType = DBT_MSSQL;
		pData = FindDBTypeData(m_nDbType);
	}
	if (pData) {
		int ntype = m_nDbType;
		pData->procInitParam(this);	// 这里调用RemoveAll会改变m_nDbType
		m_nDbType = ntype;
		m_szConnStr = pData->procMakeConnStr(this);
		m_bSuccess = TRUE;
	}
	else
		m_bSuccess = FALSE;
}

void CConnStringParser::DoParser()
{
	m_arrKeys.RemoveAll();
	m_arrValues.RemoveAll();

	CString szitem, szkey, szvalue;
	int nStart = 0;
	int nPos = m_szConnStr.Find(_T(';'), nStart);
	if (nPos == -1)
	{
		m_bSuccess = FALSE;
		return;
	}
	nStart = 0;
	LPDBTYPEDATA pData = NULL;
	while((nPos != -1) || (nStart < m_szConnStr.GetLength() - 1))
	{
		if (nPos == -1)
			nPos = m_szConnStr.GetLength();
		szitem = m_szConnStr.Mid(nStart, nPos - nStart);
		
		int nSubPos = szitem.Find(_T('='));
		if(nSubPos <= 0)
		{
			szkey = szitem;
			szkey.TrimLeft();
			szkey.TrimRight();
			szvalue = _T("");
		}
		else {
			szkey = szitem.Left(nSubPos);
			szkey.TrimLeft();
			szkey.TrimRight();

			szvalue = szitem.Right(szitem.GetLength() - nSubPos - 1);
			szvalue.TrimLeft();
			szvalue.TrimRight();
		}
		if (szkey.CompareNoCase(_T("DRIVER")) == 0)
		{
			SetValue(szkey, szvalue);
			pData = FindDBTypeData(szvalue);
		}
		if (pData)
			pData->procParserValue(this, szkey, szvalue);
		else
			SetValue(szkey, szvalue);

		nStart = nPos + 1;
		nPos = m_szConnStr.Find(_T(";"), nStart);
	}

	//LPDBTYPEDATA pData = FindDBTypeData(GetValue(_T("DRIVER")));
	if (pData) {
		m_nDbType = pData->nTypeID;
		m_bSuccess = TRUE;
	}
	else
		m_bSuccess = FALSE;
}

CConnStringParser& CConnStringParser::operator=(CConnStringParser &ConnStr)
{
	m_szConnStr = ConnStr.GetConnectString();
	DoParser();

	return *this;
}

LPCTSTR CConnStringParser::operator[](LPCTSTR lpszKey)
{
	CString szKey = lpszKey;

	for(int i = 0; i < m_arrKeys.GetCount(); i++)
	{
		if (szKey.CompareNoCase(GetKey(i)) == 0)
			return GetValue(i);
	}

	// 未找到
	m_arrKeys.AddTail(lpszKey);
	m_arrValues.AddTail(_T(""));
	return GetValue(m_arrValues.GetCount() - 1);
}

//////////////////////////////////////////////////////////////////////////
// 辅助函数

// 读取本地的Oracle配置表来获取数据库名称列表
int GetOracleServers(CStringArray &arrServers)
{
	CRegKey reg;
	TCHAR szFileName[1024];
	DWORD dwCount = 1024;
	
	arrServers.RemoveAll();
	if (reg.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\ORACLE")) != ERROR_SUCCESS)
		return 0;
	
#if _MSC_VER <= 1200 // MFC 6.0 or earlier
	if((reg.QueryValue(szFileName, _T("ORACLE_HOME"), &dwCount) != ERROR_SUCCESS) || (dwCount == 0))
#else
	if((reg.QueryStringValue(_T("ORACLE_HOME"), szFileName, &dwCount) != ERROR_SUCCESS) || (dwCount == 0))
#endif
	{
		reg.Close();
		return 0;
	}
	reg.Close();
	
	dwCount = (DWORD)_tcslen(szFileName);
	if (szFileName[dwCount - 1] != _T('\\'))
	{
		szFileName[dwCount] = _T('\\');
		szFileName[dwCount + 1] = _T('\0');
	}
	_tcscat(szFileName, _T("network\\admin\\tnsnames.ora"));	// Oracle本地配置文件名
	if (!PathFileExists( szFileName ))
		return 0;
	
	CString szLine, szServer;
	TCHAR ch;
	CStdioFile file;
	try
	{
		file.Open(szFileName, CFile::modeRead | CFile::shareDenyNone);
		while(file.ReadString(szLine))
		{
			szLine.TrimLeft();
			szLine.TrimRight();
			if (szLine.GetLength() == 0) // 跳过空行
				continue;
			
			ch = szLine[0];
			if (ch == _T('#')) // 跳过注释行
				continue;
			
			// 首字符在A~z之间的行被认为是Oracle服务名行
			if (ch >= 'A' && ch <= 'z')
			{
				int nPos = szLine.Find(_T("="));
				if (nPos > 0)
					szServer = szLine.Mid(0, nPos);
				else
					szServer = szLine;
				szServer.TrimRight();
				arrServers.Add(szServer);
			}
		}
		
		return arrServers.GetSize();
	}
	catch (CFileException* e)
	{
		e->Delete();
		return 0;
	}
}