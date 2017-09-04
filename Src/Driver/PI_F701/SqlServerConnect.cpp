#include "StdAfx.h"
#include "SqlServerConnect.h"
#include "pidbdrv.h"
#include "TagValue.h"
#include "..\..\Common\HashTableUtils.h"
#include "..\..\Common\TimeUtils.h"
#include "..\..\Common\DebugUtils.h"
#include "..\..\Common\DBDrvConnectString.h"

/*
错误信息如下：分为两行，第一行为ODBC API错误信息，第二行为ODBC驱动错误信息
判断断开连接只需判断第一行State值为
08001	无法连接到数据源,一般在Open DB时的错误信息
08003	连接未打开
08007	在执行事务的过程中连接失败
08S01	通讯链接失败，一般在打开记录集或调用直接执行SQL语句时出错。

State:08S01,Native:11,Origin:[Microsoft][ODBC SQL Server Driver][TCP/IP Sockets]
State:01000,Native:10054,Origin:[Microsoft][ODBC SQL Server Driver][TCP/IP Sockets]

SQLSTATE	ODBC API（驱动程序管理器）错误
00000	成功
01000	常规警告
01002	断开连接错误
01004	数据被截断
01006	未废除特权
01S00	无效连接字符串特性
01S01	行中的错误
01S02	选项值已更改
01S03	未更新或删除任何行
01S04	更新或删除了多行
01S05	取消操作被当成 FreeStmt/Close
01S06	设法在结果返回第一个行集之前进行提取
07001	参数个数错误
07006	违反受限制的数据类型特性
07S01	默认参数使用无效
08001	无法连接到数据源
08002	连接正在使用
08003	连接未打开
08004	数据源拒绝建立连接
08007	在执行事务的过程中连接失败
08S01	通讯链接失败
21S01	插入值列表与列列表不匹配
21S02	派生表的程度与列列表不匹配
22001	字符串数据右截断
22002	需要指示符变量，但未提供
22003	数字值超出范围
22005	分配时出错
22008	Datetime 字段溢出
22012	被零除错误
22026	字符串数据，长度不匹配
23000	违反完整性约束
24000*	无效的游标状态
25000	无效的事务状态
28000	无效的授权规范
34000	无效的游标名称
37000	语法错误或违规访问
3C000	重复的游标名称
40001	串行错误
42000	语法错误或违规访问
70100	操作终止
IM001	驱动程序不支持本函数
IM002	未找到源名称并且未指定默认驱动程序
IM003	无法装载指定的驱动程序
IM004	驱动程序的 SQLAllocEnv 失败
IM005	驱动程序的 SQLAllocConnect 失败
IM006	驱动程序的 SQLSetConnectOption 失败
IM007	未指定数据源或驱动程序；禁止对话
IM008	对话失败
IM009	无法装载转换 DLL
IM010	数据源名称过长
IM011	驱动程序名称过长
IM012	DRIVER 关键字语法错误
IM013	跟踪文件错误
S0001	基表或视图已存在
S0002	未找到基表
S0011	索引已经存在
S0012	未找到索引
S0021	列已存在
S0022	未找到列
S0023	列无默认值
S1000	常规错误
S1001	内存分配失败
S1002	列编号无效
S1003	程序类型超出范围
S1004	SQL 数据类型超出范围
S1008	已取消操作
S1009	参数值无效
S1010	函数序列错误
S1011	操作在此时无效
S1012	所指定的事务操作代码无效
S1015	无可用的游标名称
S1090	字符串或缓冲区长度无效
S1091	描述符类型超出范围
S1092	选项类型超出范围
S1093	参数编号无效
S1094	小数位数值无效
S1095	函数类型超出范围
S1096	信息类型超出范围
S1097	列类型超出范围
S1098	作用域类型超出范围
S1099	可为空类型超出范围
S1100	唯一性选项类型超出范围
S1101	准确性选项类型超出范围
S1103	方向选项超出范围
S1104	精度值无效
S1105	参数类型无效
S1106	提取类型超出范围
S1107	行值超出范围
S1108	并发选项超出范围
S1109	游标位置无效
S1110	驱动程序完成无效
S1111	书签值无效
S1C00	驱动程序无法执行
S1DE0	执行值挂起时无数据
S1T00	超时失效
*/

CString StrToQuoteQuery(LPCTSTR lpszSql, TCHAR chQuote/* = _T('\'')*/)
{
	CString szt;
	szt.AppendChar(chQuote);

	TCHAR ch = *lpszSql;
	while(ch != '\0')
	{
		szt.AppendChar(ch);

		if (ch == chQuote)
			szt.AppendChar(chQuote);

		lpszSql++;
		ch = *lpszSql;
	}
	szt.AppendChar(chQuote);

	return szt;
}

CSqlServerConnect::CSqlServerConnect()
:	m_bAutoCommit(TRUE)
{	
	m_db.otl_initialize(TRUE); // 以线程安全模式初始化OCI环境

#ifdef OTL_STREAM_POOLING_ON
	//m_db.set_stream_pool_size(2);
	// set the maximum stream pool size and actually initializes
	// the stream pool.
	// if this function is not called, the stream pool
	// gets initialized anyway, with the default size of 32 entries.
#endif
}

CSqlServerConnect::~CSqlServerConnect()
{
	Disconnect();
}

DWORD CSqlServerConnect::Error2DBPError(otl_exception& p)
{
	CString szSQLState(p.sqlstate);

	/*
	08001	无法连接到数据源,一般在Open DB时的错误信息
	08003	连接未打开
	08007	在执行事务的过程中连接失败
	08S01	通讯链接失败，一般在打开记录集或调用直接执行SQL语句时出错。
	*/

	if ((szSQLState == "08S01") ||
		(szSQLState == "08001") ||
		(szSQLState == "08003") ||
		(szSQLState == "08007")
		)
		return DBERR_CONNECT;
	else
		return DBERR_FAIL;
}

BOOL CSqlServerConnect::Connect(LPCTSTR lpszInstance, LPCTSTR lpszServer, UINT nPort, LPCTSTR lpszDatabase, LPCTSTR lpszUser, LPCTSTR lpszPassword, BOOL bAutoCommit/* = TRUE*/)
{
	m_bAutoCommit = bAutoCommit;

	CString szConnectString;

	if (!lpszInstance || !*lpszInstance)
		szConnectString.Format("DRIVER={SQL Server};DSN='';Server=%s;Database=%s;Network=DBMSSOCN;Address=%s,%d;UID=%s;PWD=%s",
			lpszServer,
			lpszDatabase,
			lpszServer,
			nPort,
			lpszUser,
			lpszPassword
			);
	else
		szConnectString.Format("DRIVER={SQL Server};DSN='';Server=%s\\%s;Database=%s;Network=DBMSSOCN;Address=%s,%d;UID=%s;PWD=%s",
			lpszServer, 
			lpszInstance, 
			lpszDatabase,
			lpszServer,
			nPort,
			lpszUser,
			lpszPassword
			);

	try
	{
		//m_db.rlogon(szConnectString, (int)bAutoCommit, NULL, NULL, true);
		OutputDebug(_T("CSqlServerConnect: %s)"), szConnectString);

		m_db.rlogon(szConnectString, (int)bAutoCommit);

		return IsConnected();
	}
	catch (otl_exception& p)
	{
		OutputDebug(_T("CSqlServerConnect::Connect error(msg:%s, stm_text: %s, sqlstate: %s, var_info: %s)"), p.msg, p.stm_text, p.sqlstate, p.var_info);
		return FALSE;
	}
	catch(...)
	{
		// 由于未知原因，会发生以下异常：
		// OraOciMTDrv!otl_conn::server_attach+0xc1 [d:\workspace\sisdbdrv\trunk\thirdparty\otl\include\otlv4.h @ 22878]

		//m_pdrv->AddError(LOG_OPT_DISPLAY, _T("无法连接Oracle，可能Oracle服务器未启动或连接配置错误！"));
		OutputDebug(_T("CSqlServerConnect::Connect error: Unknown error"));
		return FALSE;
	}
}

void CSqlServerConnect::Disconnect()
{
	try
	{
		m_db.logoff();
	}
	catch (otl_exception& p)
	{
		OutputDebug(_T("CSqlServerConnect::Disconnect error(msg:%s, stm_text: %s, sqlstate: %s, var_info: %s)"), p.msg, p.stm_text, p.sqlstate, p.var_info);
	}
	catch(...)
	{
		// 由于未知原因，会发生以下异常：
		// OraOciMTDrv!otl_conn::server_attach+0xc1 [d:\workspace\sisdbdrv\trunk\thirdparty\otl\include\otlv4.h @ 22878]

		OutputDebug(_T("CSqlServerConnect::Connect error: Unknown error"));
	}
}

BOOL CSqlServerConnect::ExecSQL(LPCTSTR lpszSQL, int *pRowsAffected/* = NULL*/)
{
	try
		{
		long lret = otl_cursor::direct_exec(
			m_db,							// OCI连接器
			lpszSQL,						// SQL语句
			otl_exception::enabled			// 禁止OTL异常
			);

		if (pRowsAffected)
			*pRowsAffected = lret;

		return TRUE;
	}
	catch (otl_exception& p)
	{
		OutputDebug(_T("CSqlServerConnect::ExecSQL error(msg:%s, stm_text: %s, sqlstate: %s, var_info: %s)"), p.msg, p.stm_text, p.sqlstate, p.var_info);
		return FALSE;
	}
	catch(...)
	{
		OutputDebug(_T("CSqlServerConnect::ExecSQL error: Unknown error"));
		return FALSE;
	}
}

BOOL CSqlServerConnect::SaveBlob(LPCTSTR lpszSQL, void *pBlob, int nLen)
{
	try
	{
		otl_stream o(1, // buffer size has to be set to 1 for operations with LOBs
			lpszSQL,	// SQL statement
			m_db // connect object
			);

		otl_lob_stream blob;

		o.set_commit(0); // 禁止自动提交

		o << blob; // 为写操作初始化otl_lob_stream

		blob.set_len(nLen);
		if (nLen > 0)
		{
			otl_long_string buf(pBlob, nLen);
			blob << buf;
		}

		blob.close();
		m_db.commit();

		if (m_bAutoCommit)
			m_db.auto_commit_on();
		else
			m_db.auto_commit_off();

		return TRUE;
	}
	catch (otl_exception& p)
	{
		// 在处理回滚时可能会发生异常，这里使用嵌套try-catch来处理
		try
		{
			m_db.rollback();

			if (m_bAutoCommit)
				m_db.auto_commit_on();
			else
				m_db.auto_commit_off();

			return DBERR_FAIL;
		}
		catch(...)
		{
			//OutputDebug(_T("COdbcDrv::WriteHis error: Unknown error"));
			return FALSE;
		}

		OutputDebug(_T("CSqlServerConnect::SaveBlob error(msg:%s, stm_text: %s, sqlstate: %s, var_info: %s)"), p.msg, p.stm_text, p.sqlstate, p.var_info);
		return FALSE;
	}
	catch(...)
	{
		if (m_bAutoCommit)
			m_db.auto_commit_on();
		else
			m_db.auto_commit_off();

		// 由于未知原因，会发生以下异常：
		// OraOciMTDrv!otl_conn::server_attach+0xc1 [d:\workspace\sisdbdrv\trunk\thirdparty\otl\include\otlv4.h @ 22878]

		OutputDebug(_T("CSqlServerConnect::SaveBlob error: Unknown error"));
		return FALSE;
	}
}

long CSqlServerConnect::GetTimeZone()
{
	long lTimeZone = 28800; // 默认+8时区

	try
	{
		otl_stream o(1, "SELECT DATEDIFF(second,GetUTCDate(),GetDate()) AS TimeZone", m_db);

		o.set_commit(0);

		if (!o.eof())
		{
			o >> lTimeZone;
		}

		return lTimeZone;
	}
	catch (otl_exception& p)
	{
		OutputDebug(_T("CSqlServerConnect::GetTimeZone error(msg:%s, stm_text: %s, sqlstate: %s, var_info: %s)"), p.msg, p.stm_text, p.sqlstate, p.var_info);
		return lTimeZone;
	}
	catch(...)
	{
		OutputDebug(_T("CSqlServerConnect::GetTimeZone error: Unknown error"));
		return lTimeZone;
	}
}

BOOL CSqlServerConnect::GetLastCount(int nID, int& iCount)
{
	CString strSQL;
	strSQL.Format("select top 1 ZDS from T_Chen where id=\'%04d\' order by ltime desc", nID);

	try
	{
		otl_stream o(1, // buffer size has to be set to 1 for operations with LOBs
			strSQL,	// SQL statement
			m_db // connect object
			);

		if (!o.eof())
		{
			o >> iCount;
		}
		return TRUE;
	}
	catch (otl_exception& p)
	{
		OutputDebug(_T("CSqlServerConnect::GetTimeZone error(msg:%s, stm_text: %s, sqlstate: %s, var_info: %s)"), p.msg, p.stm_text, p.sqlstate, p.var_info);
		return FALSE;
	}
	catch(...)
	{
		OutputDebug(_T("CSqlServerConnect::GetTimeZone error: Unknown error"));
		return FALSE;
	}
}


BOOL CSqlServerConnect::GetLastCountTime(int nID, int& iCount, long& lTime)
{
	CString strSQL;
	strSQL.Format("select top 1 ZDS, ltime from T_Chen where id=\'%04d\' order by ltime desc", nID);

	try
	{
		otl_stream o(1, // buffer size has to be set to 1 for operations with LOBs
			strSQL,	// SQL statement
			m_db // connect object
			);

		if (!o.eof())
		{
			o >> iCount;
			o >> lTime;
		}
		return TRUE;
	}
	catch (otl_exception& p)
	{
		OutputDebug(_T("CSqlServerConnect::GetTimeZone error(msg:%s, stm_text: %s, sqlstate: %s, var_info: %s)"), p.msg, p.stm_text, p.sqlstate, p.var_info);
		return FALSE;
	}
	catch(...)
	{
		OutputDebug(_T("CSqlServerConnect::GetTimeZone error: Unknown error"));
		return FALSE;
	}
}