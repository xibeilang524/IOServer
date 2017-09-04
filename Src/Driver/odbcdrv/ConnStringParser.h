// ConnStringParser.h: interface for the CConnStringParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONNSTRINGPARSER_H__9C588319_455A_41F5_B144_780BB80F13AD__INCLUDED_)
#define AFX_CONNSTRINGPARSER_H__9C588319_455A_41F5_B144_780BB80F13AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*
 * 数据库类型，使用大跨度值定义，以方便以后插入支持更多的数据库类型，
 * 比如将来可能会根据需要，将MSSQL细分为 DBT_MSSQL2k=1，DBT_MSSQL2005=2，DBT_MSSQL2008=3...
*/
#define DBT_UNKNOWN		0	// 未知数据库类型
#define DBT_MSSQL		1	// Microsoft SQL Server 2000/2005/2008
#define DBT_MYSQL		100	// MySQL
#define DBT_ORACLE		200	// Oracle
#define DBT_SYBASE11	300	// Sybase 11
#define DBT_SYBASE12	301	// Sybase 12
#define DBT_DB2			400	// DB2

class CConnStringParser;

typedef void (*InitParamProc)(CConnStringParser *parser);
typedef CString (*MakeConnStringProc)(CConnStringParser *parser);
typedef BOOL (*ParserValueProc)(CConnStringParser *parser, LPCTSTR lpszKey, LPCTSTR lpszValue);

typedef struct tagDBTYPEDATA {
	int		nTypeID;			// 数据库类型ID，DBT_XXXX
	TCHAR	szDriver[64];		// 驱动的连接字符串中DRIVER属性名
	TCHAR	szName[64];			// 数据库的显示名
	InitParamProc		procInitParam;		// 初始化连接字符串参数的函数指针
	MakeConnStringProc	procMakeConnStr;	// 从参数表构造连接字符串的函数指针
	ParserValueProc		procParserValue;
}DBTYPEDATA, *LPDBTYPEDATA;

class CConnStringParser  
{
public:
	CConnStringParser(LPCTSTR lpszConnStr = NULL);
	CConnStringParser(const CString &szConnStr);
	virtual ~CConnStringParser();

public:
	BOOL IsSuccess() { return m_bSuccess; }
	int GetDBType() { return m_nDbType; }
	void SetDBType(int type);
	LPCTSTR GetDBTypeStr();
	LPCTSTR GetConnectString();
	BOOL SetConnectString(LPCTSTR lpszConnStr);
	int GetCount() { return m_arrKeys.GetCount(); }
	LPCTSTR GetKey(int nIdex);
	LPCTSTR GetValue(int nIdex);
	LPCTSTR GetValue(LPCTSTR lpszKey);
	int SetValue(LPCTSTR lpszKey, LPCTSTR lpszValue);
	int SetValue(LPCTSTR lpszKey, int nValue);
	int SetValue(LPCTSTR lpszKey, UINT nValue);
	int SetValue(LPCTSTR lpszKey, long lValue);
	int SetValue(LPCTSTR lpszKey, float fValue);
	int SetValue(LPCTSTR lpszKey, double dbValue);
	int IndexOf(LPCTSTR lpszKey);
	void RemoveAll();

	CConnStringParser& operator=(CConnStringParser &ConnStr);
	LPCTSTR operator[](LPCTSTR lpszKey);

	static LPDBTYPEDATA GetDBTypeData() {
		return m_arrDbTypeData;
	}
	static int GetDBTypeCount() {
		return m_nDbTypeCount;
	}
	
protected:
	void Init();
	void DoParser();
	LPDBTYPEDATA FindDBTypeData(int ntype);
	LPDBTYPEDATA FindDBTypeData(LPCTSTR szDriver);

private:
	BOOL	m_bSuccess;
	int		m_nDbType;
	CString m_szConnStr;

	CStringList	m_arrKeys;
	CStringList	m_arrValues;
	static int m_nDbTypeCount;
	static DBTYPEDATA	m_arrDbTypeData[];
};

// 读取本地的Oracle配置表来获取数据库名称列表
int GetOracleServers(CStringArray &arrServers);

#endif // !defined(AFX_CONNSTRINGPARSER_H__9C588319_455A_41F5_B144_780BB80F13AD__INCLUDED_)
