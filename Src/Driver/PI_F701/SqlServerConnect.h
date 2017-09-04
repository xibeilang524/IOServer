#ifndef __TPRI_SQL_SERVER_CONNECT_H__
#define __TPRI_SQL_SERVER_CONNECT_H__

#include <iostream>
#include <stdio.h>
#include "PiDBDrv.h"
using namespace std;

/*
#define OTL_NO_TMPL_MEMBER_FUNC_SUPPORT
#define OTL_BIGINT	__int64

#define OTL_STR_TO_BIGINT(str,n)                \
{                                               \
	n=_atoi64(str);                             \
}

#define OTL_BIGINT_TO_STR(n,str)                \
{                                               \
	_i64toa(n,str,10);                          \
}
*/

#define OTL_ODBC
//#define OTL_ODBC_MULTI_MODE
//#define OTL_ODBC_MSSQL_2005
//#define OTL_ODBC_MSSQL_2008
//#define OTL_ODBC_SQL_STATEMENT_WITH_DIAG_REC_OUTPUT


//#define OTL_ORA9I // Compile OTL 4.0/OCI9i
//#define OTL_ORA10G // Compile OTL 4.0/OCI10g
//#define OTL_ORA10G_R2 // Compile OTL 4.0/OCI10R2
//#define OTL_ORA11G // Compile OTL 4.0/OCI11g
//#define OTL_ORA11G_R2 // Compile OTL 4.0/OCI11gR2

//#define OTL_STL // turn on OTL in the STL compliance mode
//#define OTL_STREAM_POOLING_ON

#include "otlv4.h"
#include "../sdk/dbsdk.h"

class CTvObj;
class CSqlServerObject;

CString GetConnectString(LPCTSTR lpszInstance, LPCTSTR lpszServer, UINT nPort, LPCTSTR lpszUser, LPCTSTR lpszPassword);
CString StrToQuoteQuery(LPCTSTR lpszSql, TCHAR chQuote = _T('\''));

class CSqlServerConnect
{
public:
	CSqlServerConnect();
	~CSqlServerConnect();

	BOOL IsAutoCommit() { return m_bAutoCommit; }
	otl_connect &GetConnect() { return m_db; }
	BOOL IsConnected() {
		return (BOOL)m_db.connected;
	}
public:
	void SetOwner(CPiDBDrv *pdrv) { m_pdrv = pdrv; }
	BOOL Connect(LPCTSTR lpszInstance, LPCTSTR lpszServer, UINT nPort, LPCTSTR lpszDatabase, LPCTSTR lpszUser, LPCTSTR lpszPassword, BOOL bAutoCommit = TRUE);
	void Disconnect();

	long GetTimeZone();	// 获取InSQL服务器的时区

	BOOL ExecSQL(LPCTSTR lpszSQL, int *pRowsAffected = NULL);
	BOOL SaveBlob(LPCTSTR lpszSQL, void *pBlob, int nLen);
	BOOL GetLastCount(int nID, int& iCount);
	BOOL GetLastCountTime(int nID, int& iCount, long& lTime);
	static DWORD Error2DBPError(otl_exception& p);

private:
	otl_connect m_db;
	BOOL m_bAutoCommit;
	CPiDBDrv *m_pdrv;
	WORD m_wOciVersions[4];
};

#endif // end of __TPRI_SQL_SERVER_CONNECT_H__