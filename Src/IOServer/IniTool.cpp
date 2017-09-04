#include "StdAfx.h"
#include "IniTool.h"


CIniTool::CIniTool(void)
{
	m_szFile = "";
}
CIniTool::CIniTool(LPCTSTR sFile)
{
	m_szFile = sFile;
}

CIniTool::~CIniTool(void)
{
}

BOOL CIniTool::Write()//写入
{
	FILE *f = fopen (m_szFile, "wt");
	if(f==NULL)
	{
		CString szMsg;
		szMsg.Format("<%s>不能写!",m_szFile);
		AfxMessageBox(szMsg,MB_OK|MB_ICONWARNING);
		return FALSE;
	}
	//注释
	CString sl = ";IOServer V1配置文件\n";
	fwrite(sl,sl.GetLength(),1,f);
	sl = ";双鱼人科技\n\n";
	fwrite(sl,sl.GetLength(),1,f);

	//////////////////////
	sl = "[local]\n";
	fwrite(sl,sl.GetLength(),1,f);

	sl.Format("ProjectName = %s	;工程名\n",m_szProjectName);
	fwrite(sl,sl.GetLength(),1,f);

	if(m_bAutoRun)
		sl = "AutoRun = yes		;启动后自动启动驱动\n";
	else
		sl = "AutoRun = no		;启动后自动启动驱动\n";
	fwrite(sl,sl.GetLength(),1,f);

	if(m_bSaveLog)
		sl = "savelog = yes		;保存日志\n";
	else
		sl = "savelog = no		;保存日志\n";
	fwrite(sl,sl.GetLength(),1,f);

	if(m_bExitPass)
		sl = "exitpassword = yes	;退出时密码验证\n";
	else
		sl = "exitpassword = no	;退出时密码验证\n";
	fwrite(sl,sl.GetLength(),1,f);

	if(m_bCashe)
		sl = "cashe = yes		;使用缓存\n";
	else
		sl = "cashe = no		;使用缓存\n";
	fwrite(sl,sl.GetLength(),1,f);

	if(m_bSkipSameTime)
		sl = "skipsametime = yes	;仅更新时标大于当前时标值的快照\n";
	else
		sl = "skipsametime = no		;仅更新时标大于当前时标值的快照\n";
	fwrite(sl,sl.GetLength(),1,f);

	sl.Format("m_nTimeZero = %d		;显示时区，范围-12到12,默认为东8区\n",m_nTimeZero);
	fwrite(sl,sl.GetLength(),1,f);


	sl.Format("upthread = %d		;范围1到10\n",m_nupthreads);
	fwrite(sl,sl.GetLength(),1,f);

	sl.Format("loglevel = %d		;日志级别，0-错误、1-警告、2-提示、3-调试\n",m_nLogLevel);
	fwrite(sl,sl.GetLength(),1,f);

	sl.Format("title = %s		;DDClient附加显示标题，以区分DDClient的功能\n",m_szTitle);
	fwrite(sl,sl.GetLength(),1,f);//--

	if(m_bWaitTcpAck)
		sl = "wait_tcp_ack = yes	;TCP是否有响应包，部分网闸是不允许有响应包的\n";
	else
		sl = "wait_tcp_ack = no	;TCP是否有响应包，部分网闸是不允许有响应包的\n";
	fwrite(sl,sl.GetLength(),1,f);

	sl.Format("udp_send_delay = %d	;UDP发包延时时间，可设置为0-50毫秒\n\n", m_nUdpSendDelay);
	fwrite(sl,sl.GetLength(),1,f);

	//////////////////////////////
	sl = "[heart]			;双前置心跳配置\n";
	fwrite(sl,sl.GetLength(),1,f);

	sl.Format("heartport = %d	;心跳端口\n",m_wheartport);
	fwrite(sl,sl.GetLength(),1,f);

	m_szHeartIP.TrimLeft();
	m_szHeartIP.TrimRight();
	if(m_szHeartIP.IsEmpty())
		sl = "HeartIP = NULL		;NULL表示本机是主服务，填写IP表示本机是备用服务\n";
	else
		sl.Format("HeartIP = %s		;NULL表示本机是主主服务，填写IP表示本机是备用服务\n",m_szHeartIP);
	fwrite(sl,sl.GetLength(),1,f);
	//
	fclose(f);
	return TRUE;
}
