// MyDrvObj.cpp: implementation of the CMyDrvObj class.
//
//////////////////////////////////////////////////////////////////////
// 专版说明：
//		1. 南通专版：无时标浮点（数据类型13）连续ID的数据包的标签ID不需要减0x4000，否则得到的ID号为负值
//
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "104Drv.h"
#include "MyDrvObj.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// 电量指令，有0x05，0x45
#define ASK_POWER_COMMAND	0x05

/////////////////////////////////////////////////////////////////
//实现静态对象类工厂
CDrvObj* CDrvFactory::NewDrv()
{
	return new CMyDrvObj();
}

DRVRET CDrvFactory::GetDriverInfo(LPDRVINFO lpDrvInfo)
{
	memset(lpDrvInfo, 0, sizeof(T_DRVINFO));
	strcpy(lpDrvInfo->sname, "南瑞104接口驱动");
	strcpy(lpDrvInfo->sdes, "CopyRight By 无趣电子");
	strcpy(lpDrvInfo->sver, "2.0.0.0"); //程序版本
	lpDrvInfo->dwver = 0x0200;//规范版本
	lpDrvInfo->dwres = 0;
	return DRERR_OK;
}

//////////////////////////////////////////////////////////////////////
// CMyDrvObj class
//////////////////////////////////////////////////////////////////////

CMyDrvObj::CMyDrvObj()
{
	// 设置配置串行化存储时的标志ID，最好不同驱动之间设置不同值以示区分
	SetSerializeID(0x10000011);

	// 设置通讯协议类型，如果支持多协议，则可以用 | 的或操作连接多协议声明：
	// SetComTypes(COM_TYPE_UDP | COM_TYPE_TCP);
	// 然后在CreateProtocol()接口中根据用户选择的协议来创建协议处理类对象实例
	SetComTypes(COM_TYPE_TCP);
//	SetComTypes(COM_TYPE_COM);
//	SetComTypes(COM_TYPE_UDP);
	EnabledTestConnect();
	CConfigInfo::m_szHelpFile = _T("104接口驱动.chm");
	CConfigInfo::m_szParamHintText = _T("设备地址;IP:端口;遥信;遥测;遥脉。例如：1;127.0.0.1:1024;0;16384;25600。不需要配置项可使用空格或下划线代替");
	CTagItem::AddCustomField(CUSTOMFIELD_TYPE_DOUBLE, 0, _T("系数"), _T("1"), 100, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	CTagItem::AddCustomField(CUSTOMFIELD_TYPE_DOUBLE, 1, _T("偏差 "), _T("0"), 100, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	CTagItem::SetCSVFormatName(_T("104Drv"));
	SetConnectionFailedStartup(TRUE);
}

CMyDrvObj::~CMyDrvObj()
{

}

BOOL CMyDrvObj::TestConnect(CConfigInfo &config)
{
	return CDDCDrvObj::TestConnect(config);
}

DRVRET CMyDrvObj::GetErrMsg(CString& szMsg, DRVRET dwcode)
{	
	if (dwcode < 100)
		return CDrvObj::GetErrMsg(szMsg, dwcode);

	//>100是驱动自定义的，如果没有自定义错误信息，可不用实现该方法

	switch (dwcode)
	{
	case 1001:
		szMsg = _T("XXX错误");
		break;
	case 1002:
		szMsg = _T("XXX错误");
		break;
	default:
		return DRERR_NOERRCODE;
	}

	return DRERR_OK;
}

// 由CreateProtocol()创建的协议处理类实例不需要显式删除，
// 驱动管理对象会在适当时间自动删除
CDDCDrvProtocol	*CMyDrvObj::CreateProtocol()
{
	CDDCDrvProtocol *pObj = NULL;
	pObj = new CMyTCPClientProtocol();
//	pObj = new CMySerialProtocol();
//  	if (m_config.m_wComType == COM_TYPE_TCP)
//  		pObj = new CMyTCPClientProtocol();
//   	else if (m_config.m_wComType == COM_TYPE_TCPSERV)
//   		pObj = new CMyTCPServerProtocol();
//	pObj = new CMyUDPServerProtocol();
	if (pObj)
		InitTagIdMap();

	return pObj;
}

//////////////////////////////////////////////////////////////////////
// CMyTCPClientProtocol 协议处理类实现
//////////////////////////////////////////////////////////////////////
CMyTCPClientProtocol::CMyTCPClientProtocol()
{
	m_wSendFrameNum = 0;
	m_wReceiveFrameNum = 0;
	m_bEnd = FALSE;
	m_bLinkOK = FALSE;
	m_bSendTest = FALSE;
	m_nADPUAddr = 1;
	m_bCanAskPower = FALSE;
	m_bCanRecvPower = FALSE;
	m_bCanAskPowerAgain = FALSE;
	m_bUseIP2 = FALSE;

	m_lDiStartAddr = 0;
	m_lAiStartAddr = 16384;
	m_lPowerStartAddr = 25600;

	lStartTime = 0;
}

CMyTCPClientProtocol::~CMyTCPClientProtocol()
{
	
}

DRVRET CMyTCPClientProtocol::WriteVar(LPDRREC lpval)
{
	return DRERR_NOTSUPPORT;
}

BOOL CMyTCPClientProtocol::Connect()
{	
	int nRet;

	if (m_szIP2.IsEmpty() || (m_wPort2 == 0)) // 是否已经解析
		GetAddress();

	Disconnect();
	unsigned long ul = 1;//非阻塞方式
	struct sockaddr_in sServer;

	sServer.sin_family=AF_INET;

	CString szIP;
	WORD wPort;

	if (!m_bUseIP2 || m_szIP2.IsEmpty() || m_wPort2 == 0)
	{
		wPort= m_config.m_wPort;
		szIP = m_config.m_szIP;
	}
	else 
	{
		wPort = m_wPort2;
		szIP = m_szIP2;
	}

	sServer.sin_port=::htons(wPort);
	sServer.sin_addr.s_addr=::inet_addr(szIP);

	m_bUseIP2 = !m_bUseIP2;

	m_sClient = ::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (m_sClient==INVALID_SOCKET)
		return 1;

	if(SOCKET_ERROR == ::ioctlsocket(m_sClient, FIONBIO, (unsigned long*)&ul))//设置非阻塞方式连接
	{
		Disconnect();
		return FALSE;
	}

	::connect(m_sClient,(const struct sockaddr *)&sServer,sizeof(sServer));

	TIMEVAL tv01 = {2, 0};//2秒钟延迟
	FD_SET fdw = {1, m_sClient};
	nRet = ::select(0, NULL, &fdw, NULL, &tv01);//测试可写
	if(nRet != 1)
	{
		Disconnect();
		return  FALSE;
	}

	m_pDrvObj->AddInfoFmt(_T("104规约驱动连接成功，IP=%s，Port=%d"), szIP, (int)wPort);

	return TRUE;
}

/*******************************
//APCI 应用规约控制信息
起动字符 68H
APDU 长度(最大, 253)
控制域八位位组1
控制域八位位组2
控制域八位位组3
控制域八位位组4 

//ASDU 应用服务数据单元
类型标识，1个字节； 　
可变结构限定词，1个字节； 　
传送原因，2个字节； 　
ASDU公共地址，2个字节。 
信息对象地址，3个字节； 　
信息元素集，若干字节； 　
时标，7个字节(可选)。
*********************************/
void CMyTCPClientProtocol::DoRun()
{
	int nUpdateSec = m_pDrvObj->m_config.m_nUpdateSec / 1000;
	long lEndTime = (long)::time(NULL);
	if (lStartTime != 0)
	{		
		if (lEndTime - lStartTime < nUpdateSec)
		{
			Sleep(10);
			return;
		}
	}
	lStartTime = lEndTime;

	if (!m_tmAutoBreak.CheckUpdatePeriod(BREAK_PERIOD*1000 ))
	{	
		Disconnect();
		m_pDrvObj->AddInfo(_T("数据长时间(300s)没有刷新，启动自动重连机制"));
	}

	if (!IsConnect())
	{
		m_bCanAskPower = FALSE;
		m_bCanRecvPower = FALSE;
		
		m_wSendFrameNum = 0;
		m_wReceiveFrameNum = 0;
		m_bLinkOK = FALSE;
		if (!Connect())
			return;
	}



	if (!m_bLinkOK)
	{		
		if (!StartDT()) //激活链路
			return;

		m_bLinkOK = TRUE;
	}	
	DoRead();


#define  HAVE_ELECTR 
#ifdef   HAVE_ELECTR 

	if (m_bCanAskPower)
	{
		BYTE byCmd = ASK_POWER_COMMAND;
		if (!AskPower(byCmd))
		{
			m_bCanAskPower = FALSE;
			m_bCanRecvPower = FALSE;
			m_wSendFrameNum = 0;



			m_wReceiveFrameNum = 0;
			m_bLinkOK = FALSE;
			return;
		}
		else
		{
			DoRead();
		}
	}
	
	if (m_bCanRecvPower)
	{
		if (!AffirmRecv())
		{
			m_bCanAskPower = FALSE;
			m_bCanRecvPower = FALSE;
			m_wSendFrameNum = 0;
			m_wReceiveFrameNum = 0;
			m_bLinkOK = FALSE;
			return;
		}
		DoRead();
	}

#endif
}

BOOL CMyTCPClientProtocol::TestLink()
{
	//U帧 测试链路
	BYTE bufSend[LINKPACKETLENGTH] = {0x68, 0x04, 0x43, 0x00, 0x00, 0x00};	
	int nsize = SendData(bufSend,LINKPACKETLENGTH); 
	if (0 == nsize)
		return FALSE;
	return TRUE;
}

BOOL CMyTCPClientProtocol::AffirmTestLink()
{
	BYTE bufRecv[BUF_LENGTH_TCP];
	int nsize = ReadData(bufRecv,BUF_LENGTH_TCP);
	if (0 == nsize)
		return FALSE;
	
	int nStatus;
	nStatus = m_FrmParseStream.PutData(bufRecv,nsize);
	while (3 == nStatus) // 包含完整报文
	{
		BYTE *pFrm = m_FrmParseStream.m_Frm.GetBuf();
		LPAPCI lpapci = (LPAPCI)pFrm;
		if (lpapci->byHead[0] != 0x68 ||
			lpapci->byHead[1] != 0x04 ||
			lpapci->byHead[2] != 0x83 ||
			lpapci->byHead[3] != 0x00 ||
			lpapci->byHead[4] != 0x00 ||
			lpapci->byHead[5] != 0x00)
		{
			return FALSE;
		}
		nStatus = m_FrmParseStream.Parse(); 
	}

	return TRUE;
}

BOOL CMyTCPClientProtocol::StartDT()
{
	//U帧 启动数据传输
	BYTE sendBuf[LINKPACKETLENGTH] = {0x68,0x04,0x07,0x00,0x00,0x00};
	int nsize = SendData(sendBuf,LINKPACKETLENGTH); 
	if (0 == nsize)
		return FALSE;
	return TRUE;
}

BOOL CMyTCPClientProtocol::AffirmStartDT()
{
	//U帧 确认启动数据传输
	BYTE recvBuf[BUF_LENGTH_TCP];
	int nsize = ReadData(recvBuf,BUF_LENGTH_TCP); 
	if (0 == nsize)
		return FALSE;

	LPAPCI lplink = (LPAPCI)recvBuf;
	if (lplink->byHead[0] != 0x68 || 
		lplink->byHead[1] != 0x04 ||
		lplink->byHead[2] != 0x0B || 
		lplink->byHead[3] != 0x00 || 
		lplink->byHead[4] != 0x00 || 
		lplink->byHead[5] != 0x00)
	{
		return FALSE;
	}
	else
		return TRUE;
	return FALSE;
}

BOOL CMyTCPClientProtocol::Summon()
{
	//I帧 发送总召帧 传送原因 0x06激活
	BYTE byLow_Send = (m_wSendFrameNum & 0x7F) << 1;
	BYTE byHigh_Send = (m_wSendFrameNum >> 7) & 0x7F;
	BYTE byLow_Receive = (m_wReceiveFrameNum & 0x7F) << 1;
	BYTE byHigh_Receive = (m_wReceiveFrameNum >> 7) & 0x7F;
	BYTE byLow_ADPUAddr = LOBYTE(m_nADPUAddr);
	BYTE byHigh_ADPUAddr = HIBYTE(m_nADPUAddr);
	BYTE Buf[SUMMONPACKETLENGTH] = {0x68, 0x0E, byLow_Send, byHigh_Send, byLow_Receive, byHigh_Receive, 
		0x64, 0x01, 0x06, 0x00, byLow_ADPUAddr, byHigh_ADPUAddr, 0x00, 0x00, 0x00, 0x14};
	int nsize = SendData(Buf,SUMMONPACKETLENGTH); 
	if (0 == nsize)
		return FALSE;
	
	m_wSendFrameNum++;
	return TRUE;
}

BOOL CMyTCPClientProtocol::AffirmSummon()
{
	//I帧 确认总召帧 传送原因 0x07确认激活
	BYTE recvBuf[BUF_LENGTH_TCP];
	int nsize = ReadData(recvBuf,BUF_LENGTH_TCP); //收到总招帧
	//帧长度不对
	if (0 == nsize)
	{
		CloseSummon();
		return FALSE;
	}

	int nStatus;
	nStatus = m_FrmParseStream.PutData(recvBuf,nsize);
	if (3 == nStatus) // 包含完整报文
	{
		BYTE *pFrm = m_FrmParseStream.m_Frm.GetBuf();
		//报头标志不对
		LPAPCI lpapci = (LPAPCI)pFrm;
		WORD wReceiveFrameNum = MAKEWORD(lpapci->byHead[4],lpapci->byHead[5]);
		if (lpapci->byHead[0] != 0x68 || lpapci->byHead[1] != 0x0E)
		{
			CloseSummon();
			return FALSE;
		}
		
		BYTE PeerSendNumPart1 =  lpapci->byHead[2]/* >> 1*/;
		BYTE PeerSendNumPart2 = lpapci->byHead[3] /*& 0x7F*/;
		WORD PeerSendNum = MAKEWORD(PeerSendNumPart1,PeerSendNumPart2);
		
		BYTE *precvBuf = pFrm;
		precvBuf += sizeof(APCI);
		
		//确认激活帧信息不对
		BYTE byLow_ADPUAddr = LOBYTE(m_nADPUAddr);
		BYTE byHigh_ADPUAddr = HIBYTE(m_nADPUAddr);
		
		LPASDUHEAD lpasduhead = (LPASDUHEAD)precvBuf;
		if (lpasduhead->byType != 0x64 || 
			lpasduhead->byWord != 0x01 || 
			lpasduhead->byReason[0] != 0x07|| 
			lpasduhead->byReason[1] != 0x00|| 
			lpasduhead->byAddr[0] != byLow_ADPUAddr || 
			lpasduhead->byAddr[1] != byHigh_ADPUAddr)
		{
			CloseSummon();
			return FALSE;
		}
		
		precvBuf += sizeof(ASDUHEAD);
		//信息不对
		if (*precvBuf != 0x00 ||
			*(precvBuf + 1) != 0x00 ||
			*(precvBuf + 2) != 0x00 ||
			*(precvBuf + 3) != 0x14)
		{
			CloseSummon();
			return FALSE;
		}
		m_wReceiveFrameNum = PeerSendNum>>1;
		nStatus = m_FrmParseStream.Parse(); 
	}
	
	return TRUE; 
}

BOOL CMyTCPClientProtocol::CloseSummon()
{
	//I帧 发送总召帧 传送原因 0x07激活终止
	BYTE byLow_Send = LOBYTE(m_wSendFrameNum);
	BYTE byHigh_Send = HIBYTE(m_wSendFrameNum);
	BYTE byLow_Receive = (m_wReceiveFrameNum & 0x7F) << 1;
	BYTE byHigh_Receive = (m_wReceiveFrameNum >> 7) & 0x7F;
	BYTE byLow_ADPUAddr = LOBYTE(m_nADPUAddr);
	BYTE byHigh_ADPUAddr = HIBYTE(m_nADPUAddr);

	BYTE Buf[SUMMONPACKETLENGTH] = {0x68, 0x0E, byLow_Send, byHigh_Send, byLow_Receive, byHigh_Receive, 
		0x64, 0x01, 0x07, 0x00, byLow_ADPUAddr, byHigh_ADPUAddr, 0x00, 0x00, 0x00, 0x14};
	int nsize = SendData(Buf,SUMMONPACKETLENGTH); 
	if (0 == nsize)
		return FALSE;

//	m_wSendFrameNum++;

	m_wSendFrameNum = 0;
	m_wReceiveFrameNum = 0;
	m_bLinkOK = FALSE;
	return TRUE;
}


BOOL CMyTCPClientProtocol::AffirmRecv()
{
	BYTE byLow_Receive = (m_wReceiveFrameNum & 0x7F) << 1;
	BYTE byHigh_Receive = (m_wReceiveFrameNum >> 7) & 0x7F;
	
	BYTE sendBuf[LINKPACKETLENGTH] = {0x68,0x04,0x01,0x00,byLow_Receive,byHigh_Receive};
	int nsize = SendData(sendBuf,LINKPACKETLENGTH); 
	if (0 == nsize)
		return FALSE;
	return TRUE;
}

void CMyTCPClientProtocol::DoRead()
{
	BYTE recvBuf[BUF_LENGTH_TCP];
 	int nsize = ReadData(recvBuf,BUF_LENGTH_TCP);
	if (nsize == 0)
	{
		CloseSummon();	
		return;
	}

	int nStatus;
	nStatus = m_FrmParseStream.PutData(recvBuf,nsize);
	while (3 == nStatus) // 包含完整报文
	{
		BYTE *pFrm = m_FrmParseStream.m_Frm.GetBuf();
		LPAPCI lpapci = (LPAPCI)pFrm;
		if (0x68 == lpapci->byHead[0] &&   //接收的是测试报文则发送确认测试帧
			0x04 == lpapci->byHead[1] &&
			0x43 == lpapci->byHead[2] &&
			0x00 == lpapci->byHead[3] &&
			0x00 == lpapci->byHead[4] &&
			0x00 == lpapci->byHead[5])
		{
			m_bSendTest = TRUE;
		}
		else if (lpapci->byHead[0] == 0x68 &&  //确认链路
			lpapci->byHead[1] == 0x04 &&
			lpapci->byHead[2] == 0x0B && 
			lpapci->byHead[3] == 0x00 && 
			lpapci->byHead[4] == 0x00 && 
			lpapci->byHead[5] == 0x00)
		{
			m_bSendCall = TRUE;
		}
		else if (0x68 == lpapci->byHead[0] &&   //接收的是终止确认帧
			0x04 == lpapci->byHead[1] &&
			0x23 == lpapci->byHead[2] &&
			0x00 == lpapci->byHead[3] &&
			0x00 == lpapci->byHead[4] &&
			0x00 == lpapci->byHead[5])
		{
			m_wSendFrameNum = 0;
			m_wReceiveFrameNum = 0;
			m_bCanAskPower = FALSE;
			m_bCanRecvPower = FALSE;

			m_bLinkOK = FALSE;
		}
		else if (lpapci->byHead[0] == 0x68 && lpapci->byHead[1] == 0x0E)
		{
			WORD wReceiveFrameNum = MAKEWORD(lpapci->byHead[4],lpapci->byHead[5]);
			
			BYTE PeerSendNumPart1 =  lpapci->byHead[2]/* >> 1*/;
			BYTE PeerSendNumPart2 = lpapci->byHead[3] /*& 0x7F*/;
			WORD PeerSendNum = MAKEWORD(PeerSendNumPart1,PeerSendNumPart2);
			
			m_wReceiveFrameNum = PeerSendNum>>1;
// 			if (!AffirmRecv())
// 			{
// 				CloseSummon();
// 				return;
// 			}

			LPASDUHEAD lpasduhead = (LPASDUHEAD)(pFrm + sizeof(APCI));
			WORD wResult = MAKEWORD(lpasduhead->byReason[0], lpasduhead->byReason[1]);
			BYTE byteLast = *(pFrm + sizeof(APCI) + sizeof(ASDUHEAD) + 3);
			if (lpasduhead->byType == 0x46 && lpasduhead->byWord == 0x01 && wResult == 0x04 && byteLast == 0x00) //初始化结束
			{
		/*		if (m_config.m_bDebug)
				{
					CString szLog = _T("");
					szLog = "收到初始化结束帧";
					m_pDrvObj->OnLogOk(0,szLog);
				}*/
			}
			else if (lpasduhead->byType == 0x64 && lpasduhead->byWord == 0x01 && wResult == 0x07 && byteLast == 0x14) //总召确认
			{
			/*	if (m_config.m_bDebug)
				{
					CString szLog = _T("");
					szLog = "收到总召确认帧";
					m_pDrvObj->OnLogOk(0,szLog);
				}*/
				if (m_bCanAskPower)
				{
					m_bCanRecvPower = TRUE;
					return;
				}
			}
			else if (lpasduhead->byType == 0x64 && lpasduhead->byWord == 0x01 && wResult == 0x0A && byteLast == 0x14) //总召结束
			{
			/*	if (m_config.m_bDebug)
				{
					CString szLog = _T("");
					szLog = "收到总召结束帧";
					m_pDrvObj->OnLogOk(0,szLog);
				}*/
				m_bCanAskPower = TRUE;
				return;
			}
			else if (lpasduhead->byType == 0x65 && lpasduhead->byWord == 0x01 && wResult == 0x06 && byteLast == 0x45) //电量结束
			{
			/*	if (m_config.m_bDebug)
				{
					CString szLog = _T("");
					szLog = "收到电量结束帧";
					m_pDrvObj->OnLogOk(0,szLog);
				}*/
				m_bCanAskPower = FALSE;
				m_bCanRecvPower = FALSE;
				m_bCanAskPowerAgain = FALSE;
				return;
			}
			else if (lpasduhead->byType == 0x65 && lpasduhead->byWord == 0x01 && wResult == 0x07 && byteLast == ASK_POWER_COMMAND) //电量确认
			{
			/*	if (m_config.m_bDebug)
				{
					CString szLog = _T("");
					szLog = "收到电量冻结确认帧";
					m_pDrvObj->OnLogOk(0,szLog);
				}*/
				m_bCanAskPowerAgain = TRUE;
				//		return;
			}
			pFrm += lpapci->byHead[1] + 2;
			nsize -= lpapci->byHead[1] + 2;
		}
		else if (lpapci->byHead[0] == 0x68 && lpapci->byHead[1] != 0x0E)
		{
			BYTE PeerSendNumPart1 = lpapci->byHead[2] /*>> 1*/;
			BYTE PeerSendNumPart2 = lpapci->byHead[3] /*& 0x7F*/;
			WORD PeerSendNum = MAKEWORD(PeerSendNumPart1,PeerSendNumPart2);
			m_wReceiveFrameNum = PeerSendNum>>1;
			pFrm += sizeof(APCI);
			nsize -= sizeof(APCI);	

			//ASDU头信息
			LPASDUHEAD lpasduhead = (LPASDUHEAD)pFrm;
			BYTE byteType = lpasduhead->byType; //数据类型
			BOOL byteContinuous = (lpasduhead->byWord & 0x80) >> 7; //ID是否连续 0不连续 1连续
			BYTE byteNum = lpasduhead->byWord & 0x7F;  //数据个数
			WORD wResult = MAKEWORD(lpasduhead->byReason[0], lpasduhead->byReason[1]);
			BYTE byteResult = wResult & 0x3F;
			if (0x0A == byteResult) //激活终止
			{
				m_bEnd = TRUE;
			}
			pFrm += sizeof(ASDUHEAD);
			nsize -= sizeof(ASDUHEAD);

			double dValue = 0;
			//信息体信息
			CString szID = _T("");
			CTagItem *pTag = NULL;

			::CoFileTimeNow(&m_tmAutoBreak.m_ftStartTime);

			m_pDrvObj->AddDebugFmt(_T("接收到数据类型%d，是否地址连续：%s,数据个数:%d"), (int)byteType, (byteContinuous)?_T("YES"):_T("NO"), byteNum );
						
			if (1 == byteType) //不带时标单点遥信
			{
				if (!byteContinuous) //不连续  每个信息一个地址
				{
					LPIDENTPT1_0 lpidentpt1 = (LPIDENTPT1_0)pFrm;
					for (int ii = 0; ii < byteNum; ii++)
					{
						int nID = lpidentpt1[ii].byAddr[0] + ((lpidentpt1[ii].byAddr[1] & 0xFF) << 8) + ((lpidentpt1[ii].byAddr[2]) & 0xFF << 16);
						szID.Format("SP_NOTIME_%ld",nID - m_lDiStartAddr);

						pTag = m_pDrvObj->FindByID(szID);
						if (pTag)
						{
							dValue = long(lpidentpt1[ii].byVal & 0x01)*pTag->m_dblField1 + pTag->m_dblField2;
							m_pDrvObj->AddValueByID(szID, 0, QA_OK, dValue);
							m_pDrvObj->AddDebugFmt(_T("TagName=%s (%s), Value = %d"), szID,pTag->m_szName,dValue);	
						}
						else
						{
							m_pDrvObj->AddWarningFmt("ID = %s 不存在！", szID);	
						}
	
						nsize -= sizeof(IDENTPT1_0);
					}
					pFrm += sizeof(IDENTPT1_0) * byteNum;
				}
				else //连续
				{
					//信息对象地址 起始ID
					int nID = *pFrm + ((*(pFrm+1) & 0xFF) << 8) + ((*(pFrm+2) & 0xFF) << 16);
					nID -= m_lDiStartAddr;
					pFrm += 3;
					nsize -= 3;

					m_pDrvObj->AddDebugFmt(_T("TagName=SP_NOTIME_%ld to SP_NOTIME_%ld"), nID, nID + byteNum - 1);

					LPIDENTPT1_1 lpidentpt1 = (LPIDENTPT1_1)pFrm;
					for (int ii = 0; ii < byteNum; ii++)
					{
						szID.Format("SP_NOTIME_%ld",nID + ii);
						pTag = m_pDrvObj->FindByID(szID);
						if (pTag)
						{
							dValue = long(lpidentpt1[ii].byVal & 0x01)*pTag->m_dblField1 + pTag->m_dblField2;
							m_pDrvObj->AddValueByID(szID, 0, QA_OK,dValue);
							m_pDrvObj->AddDebugFmt(_T("TagName=%s (%s), Value = %d"), szID, pTag->m_szName, dValue);	
						}
						else
						{
							m_pDrvObj->AddWarningFmt("ID = %s 不存在！", szID);	
						}
						nsize -= sizeof(IDENTPT1_1);
					}
					pFrm += sizeof(IDENTPT1_1) * byteNum;
				}
			}
			else if (30 == byteType)//带时标单点遥信
			{
				if (!byteContinuous) //不连续  每个信息一个地址
				{
					LPIDENTPT30_0 lpidentpt30 = (LPIDENTPT30_0)pFrm;
					for (int ii = 0; ii < byteNum; ii++)
					{
						int nID = lpidentpt30[ii].byAddr[0] + ((lpidentpt30[ii].byAddr[1] & 0xFF) << 8) + ((lpidentpt30[ii].byAddr[2]) & 0xFF << 16);					
						szID.Format("SP_TIME_%ld",nID - m_lDiStartAddr);
						pTag = m_pDrvObj->FindByID(szID);
						if (pTag)
						{
							int nSec = MAKEWORD(lpidentpt30[ii].byTime[0],lpidentpt30[ii].byTime[1]) / 1000;
							COleDateTime dt;
							dt.SetDateTime(lpidentpt30[ii].byTime[6]+2000,lpidentpt30[ii].byTime[5],lpidentpt30[ii].byTime[4],
								lpidentpt30[ii].byTime[3],lpidentpt30[ii].byTime[2],nSec);
							
							SYSTEMTIME systime;
							VariantTimeToSystemTime(dt, &systime);
							CTime tm(systime);

							dValue = long(lpidentpt30[ii].byVal & 0x01)*pTag->m_dblField1 + pTag->m_dblField2;
							m_pDrvObj->AddValueByID(szID, (long)tm.GetTime(), QA_OK, dValue);
							m_pDrvObj->AddDebugFmt(_T("TagName=%s (%s), Value = %d"), szID,pTag->m_szName,dValue);	
						}
						else
						{
							m_pDrvObj->AddWarningFmt("ID = %s 不存在！", szID);	
						}

						nsize -= sizeof(IDENTPT30_0);
					}
					pFrm += sizeof(IDENTPT30_0) * byteNum;
				}
				else //连续
				{
					//信息对象地址 起始ID
					int nID = *pFrm + ((*(pFrm+1) & 0xFF) << 8) + ((*(pFrm+2) & 0xFF) << 16);
					nID -= m_lDiStartAddr;
					pFrm += 3;
					nsize -= 3;
					LPIDENTPT30_1 lpidentpt30 = (LPIDENTPT30_1)pFrm;
					for (int ii = 0; ii < byteNum; ii++)
					{
						szID.Format("SP_TIME_%ld",nID + ii);
						pTag = m_pDrvObj->FindByID(szID);
						if (pTag)
						{
							int nSec = MAKEWORD(lpidentpt30[ii].byTime[0],lpidentpt30[ii].byTime[1]) / 1000;
							COleDateTime dt;
							dt.SetDateTime(lpidentpt30[ii].byTime[6]+2000,lpidentpt30[ii].byTime[5],lpidentpt30[ii].byTime[4],
								lpidentpt30[ii].byTime[3],lpidentpt30[ii].byTime[2],nSec);
							
							SYSTEMTIME systime;
							VariantTimeToSystemTime(dt, &systime);
							CTime tm(systime);
							dValue = long(lpidentpt30[ii].byVal & 0x01)*pTag->m_dblField1 + pTag->m_dblField2;
							m_pDrvObj->AddValueByID(szID, (long)tm.GetTime(), QA_OK, dValue);
							m_pDrvObj->AddDebugFmt(_T("TagName=%s (%s), Value = %d"), szID,pTag->m_szName,dValue);	
						}
						else
						{
							m_pDrvObj->AddWarningFmt("ID = %s 不存在！", szID);	
						}
						nsize -= sizeof(IDENTPT30_1);
					}
					pFrm += sizeof(IDENTPT30_1) * byteNum;
				}
			}
			else if (3 == byteType) //不带时标双点遥信
			{
				if (!byteContinuous) //不连续  每个信息一个地址
				{
					LPIDENTPT3_0 lpidentpt3 = (LPIDENTPT3_0)pFrm;
					for (int ii = 0; ii < byteNum; ii++)
					{
						int nID = lpidentpt3[ii].byAddr[0] + ((lpidentpt3[ii].byAddr[1] & 0xFF) << 8) + ((lpidentpt3[ii].byAddr[2] & 0xFF) << 16);
						szID.Format("DP_NOTIME_%ld",nID - m_lDiStartAddr);
						pTag = m_pDrvObj->FindByID(szID);
						if (pTag)
						{
							long lVal = lpidentpt3[ii].byVal & 0x03;
							if (2 == lVal)
								lVal = 1;
							else
								lVal = 0;
							dValue = lVal*pTag->m_dblField1 + pTag->m_dblField2;
							m_pDrvObj->AddValueByID(szID, 0, QA_OK, dValue);
							m_pDrvObj->AddDebugFmt(_T("TagName=%s (%s), Value = %d"), szID,pTag->m_szName,dValue);	
						}
						else
						{
							m_pDrvObj->AddWarningFmt("ID = %s 不存在！", szID);	
						}

						nsize -= sizeof(IDENTPT3_0);
					}
					pFrm += sizeof(IDENTPT3_0) * byteNum;
				}
				else
				{
					//信息对象地址 起始ID
					int nID = *pFrm + ((*(pFrm+1) & 0xFF) << 8) + ((*(pFrm+2) & 0xFF) << 16);
					nID -= m_lDiStartAddr;
					pFrm += 3;
					nsize -= 3;

					LPIDENTPT3_1 lpidentpt3 = (LPIDENTPT3_1)pFrm;
					for (int ii = 0; ii < byteNum; ii++)
					{
						szID.Format("DP_NOTIME_%ld",nID + ii);
						pTag = m_pDrvObj->FindByID(szID);
						if (pTag)
						{	
							long lVal = lpidentpt3[ii].byVal & 0x03;
							if (2 == lVal)
								lVal = 1;
							else
								lVal = 0;
							dValue = lVal*pTag->m_dblField1 + pTag->m_dblField2;
							m_pDrvObj->AddValueByID(szID, 0, QA_OK, dValue);
							m_pDrvObj->AddDebugFmt(_T("TagName=%s (%s), Value = %f"), szID,pTag->m_szName,dValue);	
						}
						else
						{
							m_pDrvObj->AddWarningFmt("ID = %s 不存在！", szID);	
						}
						nsize -= sizeof(IDENTPT3_1);
					}
					pFrm += sizeof(IDENTPT3_1) * byteNum;
				}
			}
			else if (31 == byteType) //带时标双点遥信
			{
				if (!byteContinuous) //不连续  每个信息一个地址
				{
					LPIDENTPT31_0 lpidentpt31 = (LPIDENTPT31_0)pFrm;
					for (int ii = 0; ii < byteNum; ii++)
					{
						int nID = lpidentpt31[ii].byAddr[0] + ((lpidentpt31[ii].byAddr[1] & 0xFF) << 8) + ((lpidentpt31[ii].byAddr[2] & 0xFF) << 16);
						szID.Format("DP_TIME_%ld",nID - m_lDiStartAddr);
						pTag = m_pDrvObj->FindByID(szID);
						if (pTag)
						{
							long lVal = lpidentpt31[ii].byVal & 0x03;
							if (2 == lVal)
								lVal = 1;
							else
								lVal = 0;

							int nSec = MAKEWORD(lpidentpt31[ii].byTime[0],lpidentpt31[ii].byTime[1]) / 1000;
							COleDateTime dt;
							dt.SetDateTime(lpidentpt31[ii].byTime[6]+2000,lpidentpt31[ii].byTime[5],lpidentpt31[ii].byTime[4],
								lpidentpt31[ii].byTime[3],lpidentpt31[ii].byTime[2],nSec);
							
							SYSTEMTIME systime;
							VariantTimeToSystemTime(dt, &systime);
							CTime tm(systime);

							dValue = lVal*pTag->m_dblField1 + pTag->m_dblField2;
							m_pDrvObj->AddValueByID(szID, (long)tm.GetTime(), QA_OK,dValue);
							m_pDrvObj->AddDebugFmt(_T("TagName=%s (%s), Value = %f"), szID,pTag->m_szName, dValue);	
						}
						else
						{
							m_pDrvObj->AddWarningFmt("ID = %s 不存在！", szID);	
						}
						nsize -= sizeof(IDENTPT31_0);
					}
					pFrm += sizeof(IDENTPT31_0) * byteNum;
				}
				else
				{
					//信息对象地址 起始ID
					int nID = *pFrm + ((*(pFrm+1) & 0xFF) << 8) + ((*(pFrm+2) & 0xFF) << 16);
					nID -= m_lDiStartAddr;
					pFrm += 3;
					nsize -= 3;

					LPIDENTPT31_1 lpidentpt31 = (LPIDENTPT31_1)pFrm;
					for (int ii = 0; ii < byteNum; ii++)
					{
						szID.Format("DP_TIME_%ld",nID + ii);
						pTag = m_pDrvObj->FindByID(szID);
						if (pTag)
						{
							long lVal = lpidentpt31[ii].byVal & 0x03;
							if (2 == lVal)
								lVal = 1;
							else
								lVal = 0;
							int nSec = MAKEWORD(lpidentpt31[ii].byTime[0],lpidentpt31[ii].byTime[1]) / 1000;
							COleDateTime dt;
							dt.SetDateTime(lpidentpt31[ii].byTime[6]+2000,lpidentpt31[ii].byTime[5],lpidentpt31[ii].byTime[4],
								lpidentpt31[ii].byTime[3],lpidentpt31[ii].byTime[2],nSec);
							
							SYSTEMTIME systime;
							VariantTimeToSystemTime(dt, &systime);
							CTime tm(systime);

							dValue = lVal*pTag->m_dblField1 + pTag->m_dblField2;
							m_pDrvObj->AddValueByID(szID, (long)tm.GetTime(), QA_OK,dValue);
							m_pDrvObj->AddDebugFmt(_T("TagName=%s (%s), Value = %f"), szID,pTag->m_szName, dValue);	
						}
						else
						{
							m_pDrvObj->AddWarningFmt("ID = %s 不存在！", szID);	
						}
						nsize -= sizeof(IDENTPT31_1);
					}
					pFrm += sizeof(IDENTPT31_1) * byteNum;
				}
			}
			else if (9 == byteType) //不带时标规一化遥测
			{
				if (!byteContinuous)
				{
					LPIDENTPT9_0 lpident9 = (LPIDENTPT9_0)pFrm;
					for (int ii = 0; ii < byteNum; ii++)
					{
						int nID = lpident9[ii].byAddr[0] + ((lpident9[ii].byAddr[1] & 0xFF) << 8) + ((lpident9[ii].byAddr[2] & 0xFF) << 16);	
						szID.Format("GY_NOTIME_%ld", nID - m_lAiStartAddr);

						pTag = m_pDrvObj->FindByID(szID);
						if (pTag)
						{
							short fBuf = *(short*)lpident9[ii].byVal;
							float f1 = (float)atof(pTag->m_szNote);
							if (f1 == 0)
								f1 = 1;
							float fVal = f1 * fBuf;
						/*	int accum = 0; 
							for (int shiftBy = 0; shiftBy < sizeof(lpident9[ii].byVal)/sizeof(lpident9[ii].byVal[0]); shiftBy++) 
							{ 
								accum |= (lpident9[ii].byVal[shiftBy] & 0xff) << (shiftBy * 8); 
							} 

							BYTE btSign = ((accum >> 15) & 0X01);
							accum = (accum & 0x7FFF);
							if (btSign == 1)
								accum = 0 - (accum ^ 0x7FFF);
							float f1 = atof(pTag->m_szNote);
							if (f1 == 0)
								f1 = 1;
							float fVal = float(accum * f1);*/

							dValue = fVal*pTag->m_dblField1 + pTag->m_dblField2;
							m_pDrvObj->AddValueByID(szID, 0, QA_OK, dValue);
							m_pDrvObj->AddDebugFmt(_T("TagName=%s, Value = %f"), szID,dValue);		
						}
						else
						{
							m_pDrvObj->AddWarningFmt("ID = %s 不存在！", szID);	
						}
						nsize -= sizeof(IDENTPT9_0);
					}
					pFrm += sizeof(IDENTPT9_0) * byteNum;
				}
				else
				{
					//信息对象地址 起始ID
					int nID = *pFrm + ((*(pFrm+1) & 0xFF) << 8) + ((*(pFrm+2) & 0xFF) << 16);
					nID -= m_lAiStartAddr;
					pFrm += 3;
					nsize -= 3;

					m_pDrvObj->AddDebugFmt(_T("TagName=GY_NOTIME_%ld to GY_NOTIME_%ld"), nID, nID + byteNum - 1);

					LPIDENTPT9_1 lpident9 = (LPIDENTPT9_1)pFrm;
					for (int ii = 0; ii < byteNum; ii++)
					{
						szID.Format("GY_NOTIME_%ld",nID + ii);
						pTag = m_pDrvObj->FindByID(szID);
						if (pTag)
						{
							short fBuf = *(short*)lpident9[ii].byVal;
							float f1 = (float)atof(pTag->m_szNote);
							if (f1 == 0)
								f1 = 1;
							float fVal = f1 * fBuf;

						/*	int accum = 0; 
							for (int shiftBy = 0; shiftBy < sizeof(lpident9[ii].byVal)/sizeof(lpident9[ii].byVal[0]); shiftBy++) 
							{ 
								accum |= (lpident9[ii].byVal[shiftBy] & 0xff) << (shiftBy * 8); 
							} 

							BYTE btSign = ((accum >> 15) & 0X01);
							accum = (accum & 0x7FFF);
							if (btSign == 1)
								accum = 0 - (accum ^ 0x7FFF);

							float f1 = atof(pTag->m_szNote);
							if (f1 == 0)
								f1 = 1;
							float fVal = float(accum * f1);*/

							dValue = fVal*pTag->m_dblField1 + pTag->m_dblField2;
							m_pDrvObj->AddValueByID(szID, 0, QA_OK,dValue);
							m_pDrvObj->AddDebugFmt(_T("TagName=%s (%s), Value = %f"), szID,pTag->m_szName,dValue);	
						}
						else
						{
							m_pDrvObj->AddWarningFmt("ID = %s 不存在！", szID);	
						}
						nsize -= sizeof(IDENTPT9_1);
					}
					pFrm += sizeof(IDENTPT9_1) * byteNum;
				}
			}
			else if (34 == byteType) //带时标规一化遥测
			{
				if (!byteContinuous)
				{
					LPIDENTPT34_0 lpident34 = (LPIDENTPT34_0)pFrm;
					for (int ii = 0; ii < byteNum; ii++)
					{
						int nID = lpident34[ii].byAddr[0] + ((lpident34[ii].byAddr[1] & 0xFF) << 8) + ((lpident34[ii].byAddr[2] & 0xFF) << 16);				
						szID.Format("GY_TIME_%ld",nID - m_lAiStartAddr);
						pTag = m_pDrvObj->FindByID(szID);
						if (pTag)
						{
							short fBuf = *(short*)lpident34[ii].byVal;
							float f1 = (float)atof(pTag->m_szNote);
							if (f1 == 0)
								f1 = 1;
							float fVal = f1 * fBuf;
						/*	int accum = 0; 
							for (int shiftBy = 0; shiftBy < sizeof(lpident34[ii].byVal)/sizeof(lpident34[ii].byVal[0]); shiftBy++) 
							{ 
								accum |= (lpident34[ii].byVal[shiftBy] & 0xff) << (shiftBy * 8); 
							} 

							BYTE btSign = ((accum >> 15) & 0X01);
							accum = (accum & 0x7FFF);
							if (btSign == 1)
								accum = 0 - (accum ^ 0x7FFF);

							float f1 = atof(pTag->m_szNote);
							if (f1 == 0)
								f1 = 1;
							float fVal = float(accum * f1);*/

							COleDateTime dt;
							int nSec = MAKEWORD(lpident34[ii].byTime[0],lpident34[ii].byTime[1]) / 1000;
							dt.SetDateTime(lpident34[ii].byTime[6]+2000,lpident34[ii].byTime[5],lpident34[ii].byTime[4],
								lpident34[ii].byTime[3],lpident34[ii].byTime[2],nSec);
							
							SYSTEMTIME systime;
							VariantTimeToSystemTime(dt, &systime);
							CTime tm(systime);

							dValue = fVal*pTag->m_dblField1 + pTag->m_dblField2;
							m_pDrvObj->AddValueByID(szID, (long)tm.GetTime(), QA_OK, dValue);
							m_pDrvObj->AddDebugFmt(_T("TagName=%s (%s), Value = %f"), szID,pTag->m_szName,dValue);	
						}
						else
						{
							m_pDrvObj->AddWarningFmt("ID = %s 不存在！", szID);	
						}
						nsize -= sizeof(IDENTPT34_0);
					}
					pFrm += sizeof(IDENTPT34_0) * byteNum;
				}
				else
				{
					//信息对象地址 起始ID
					int nID = *pFrm + ((*(pFrm+1) & 0xFF) << 8) + ((*(pFrm+2) & 0xFF) << 16);
					nID -= m_lAiStartAddr;
					pFrm += 3;
					nsize -= 3;

					LPIDENTPT34_1 lpident34 = (LPIDENTPT34_1)pFrm;
					for (int ii = 0; ii < byteNum; ii++)
					{
						szID.Format("GY_TIME_%ld",nID + ii);
						pTag = m_pDrvObj->FindByID(szID);
						if (pTag)
						{
							short fBuf = *(short*)lpident34[ii].byVal;
							float f1 = (float)atof(pTag->m_szNote);
							if (f1 == 0)
								f1 = 1;
							float fVal = f1 * fBuf;
						/*	int accum = 0; 
							for (int shiftBy = 0; shiftBy < sizeof(lpident34[ii].byVal)/sizeof(lpident34[ii].byVal[0]); shiftBy++) 
							{ 
								accum |= (lpident34[ii].byVal[shiftBy] & 0xff) << (shiftBy * 8); 
							} 

							BYTE btSign = ((accum >> 15) & 0X01);
							accum = (accum & 0x7FFF);
							if (btSign == 1)
								accum = 0 - (accum ^ 0x7FFF);

							float f1 = atof(pTag->m_szNote);
							if (f1 == 0)
								f1 = 1;
							float fVal = float(accum * f1);*/
							COleDateTime dt;
							int nSec = MAKEWORD(lpident34[ii].byTime[0],lpident34[ii].byTime[1]) / 1000;
							dt.SetDateTime(lpident34[ii].byTime[6]+2000,lpident34[ii].byTime[5],lpident34[ii].byTime[4],
								lpident34[ii].byTime[3],lpident34[ii].byTime[2],nSec);
							
							SYSTEMTIME systime;
							VariantTimeToSystemTime(dt, &systime);
							CTime tm(systime);
							
							dValue = fVal*pTag->m_dblField1 + pTag->m_dblField2;
							m_pDrvObj->AddValueByID(szID, (long)tm.GetTime(), QA_OK, dValue);
							m_pDrvObj->AddDebugFmt(_T("TagName=%s (%s), Value = %f"), szID,pTag->m_szName,dValue);	
						}
						else
						{
							m_pDrvObj->AddWarningFmt("ID = %s 不存在！", szID);	
						}
						nsize -= sizeof(IDENTPT34_1);
					}
					pFrm += sizeof(IDENTPT34_1) * byteNum;
				}
			}
			else if (11 == byteType) //不带时标标度化遥测
			{
				if (!byteContinuous)
				{
					LPIDENTPT11_0 lpident11 = (LPIDENTPT11_0)pFrm;
					for (int ii = 0; ii < byteNum; ii++)
					{
						int nID = lpident11[ii].byAddr[0] + ((lpident11[ii].byAddr[1] & 0xFF) << 8) + ((lpident11[ii].byAddr[2]) & 0xFF << 16);		
						szID.Format("BD_NOTIME_%ld", nID - m_lAiStartAddr);
						pTag = m_pDrvObj->FindByID(szID);
						if (pTag)
						{
							short fBuf = *(short*)lpident11[ii].byVal;
							float f1 = (float)atof(pTag->m_szNote);
							if (f1 == 0)
								f1 = 1;
							float fVal = f1 * fBuf;
						/*	int accum = 0; 
							for (int shiftBy = 0; shiftBy < sizeof(lpident11[ii].byVal)/sizeof(lpident11[ii].byVal[0]); shiftBy++) 
							{ 
								accum |= (lpident11[ii].byVal[shiftBy] & 0xff) << (shiftBy * 8); 
							} 

							BYTE btSign = ((accum >> 15) & 0X01);
							accum = (accum & 0x7FFF);
							if (btSign == 1)
								accum = 0 - (accum ^ 0x7FFF);

							float f1 = atof(pTag->m_szNote);
							if (f1 == 0)
								f1 = 1;
							float fVal = float(accum * f1);*/
							dValue = fVal*pTag->m_dblField1 + pTag->m_dblField2;
							m_pDrvObj->AddValueByID(szID, 0, QA_OK, dValue);
							m_pDrvObj->AddDebugFmt(_T("TagName=%s (%s), Value = %f"), szID,pTag->m_szName,dValue);	
						}
						else
						{
							m_pDrvObj->AddWarningFmt("ID = %s 不存在！", szID);	
						}
						nsize -= sizeof(IDENTPT11_0);
					}
					pFrm += sizeof(IDENTPT11_0) * byteNum;
				}
				else
				{
					//信息对象地址 起始ID
					int nID = *pFrm + ((*(pFrm+1) & 0xFF) << 8) + ((*(pFrm+2) & 0xFF) << 16);
					nID -= m_lAiStartAddr;
					pFrm += 3;
					nsize -= 3;

					LPIDENTPT11_1 lpident11 = (LPIDENTPT11_1)pFrm;
					for (int ii = 0; ii < byteNum; ii++)
					{
						szID.Format("BD_NOTIME_%ld",nID + ii);
						pTag = m_pDrvObj->FindByID(szID);
						if (pTag)
						{
							short fBuf = *(short*)lpident11[ii].byVal;
							float f1 = (float)atof(pTag->m_szNote);
							if (f1 == 0)
								f1 = 1;
							float fVal = f1 * fBuf;
						/*	int accum = 0; 
							for (int shiftBy = 0; shiftBy < sizeof(lpident11[ii].byVal)/sizeof(lpident11[ii].byVal[0]); shiftBy++) 
							{ 
								accum |= (lpident11[ii].byVal[shiftBy] & 0xff) << (shiftBy * 8); 
							} 

							BYTE btSign = ((accum >> 15) & 0X01);
							accum = (accum & 0x7FFF);
							if (btSign == 1)
								accum = 0 - (accum ^ 0x7FFF);

							float f1 = atof(pTag->m_szNote);
							if (f1 == 0)
								f1 = 1;
							float fVal = float(accum * f1);*/
							dValue = fVal*pTag->m_dblField1 + pTag->m_dblField2;
							m_pDrvObj->AddValueByID(szID, 0, QA_OK, dValue);
							m_pDrvObj->AddDebugFmt(_T("TagName=%s (%s), Value = %f"), szID,pTag->m_szName,dValue);	
						}
						else
						{
							m_pDrvObj->AddWarningFmt("ID = %s 不存在！", szID);	
						}
						nsize -= sizeof(IDENTPT11_1);
					}
					pFrm += sizeof(IDENTPT11_1) * byteNum;
				}
			}
			else if (35 == byteType) //带时标标度化遥测
			{
				if (!byteContinuous)
				{
					LPIDENTPT35_0 lpident35 = (LPIDENTPT35_0)pFrm;
					for (int ii = 0; ii < byteNum; ii++)
					{
						int nID = lpident35[ii].byAddr[0] + ((lpident35[ii].byAddr[1] & 0xFF) << 8) + ((lpident35[ii].byAddr[2]) & 0xFF << 16);		
						szID.Format("BD_TIME_%ld", nID - m_lAiStartAddr);
						pTag = m_pDrvObj->FindByID(szID);
						if (pTag)
						{
							short fBuf = *(short*)lpident35[ii].byVal;
							float f1 = (float)atof(pTag->m_szNote);
							if (f1 == 0)
								f1 = 1;
							float fVal = f1 * fBuf;
						/*	int accum = 0; 
							for (int shiftBy = 0; shiftBy < sizeof(lpident35[ii].byVal)/sizeof(lpident35[ii].byVal[0]); shiftBy++) 
							{ 
								accum |= (lpident35[ii].byVal[shiftBy] & 0xff) << (shiftBy * 8); 
							} 

							BYTE btSign = ((accum >> 15) & 0X01);
							accum = (accum & 0x7FFF);
							if (btSign == 1)
								accum = 0 - (accum ^ 0x7FFF);

							float f1 = atof(pTag->m_szNote);
							if (f1 == 0)
								f1 = 1;
							float fVal = float(accum * f1);*/
							COleDateTime dt;
							int nSec = MAKEWORD(lpident35[ii].byTime[0],lpident35[ii].byTime[1]) / 1000;
							dt.SetDateTime(lpident35[ii].byTime[6]+2000,lpident35[ii].byTime[5],lpident35[ii].byTime[4],
								lpident35[ii].byTime[3],lpident35[ii].byTime[2],nSec);
							
							SYSTEMTIME systime;
							VariantTimeToSystemTime(dt, &systime);
							CTime tm(systime);
							
							dValue = fVal*pTag->m_dblField1 + pTag->m_dblField2;
							m_pDrvObj->AddValueByID(szID, (long)tm.GetTime(), QA_OK, dValue);
							m_pDrvObj->AddDebugFmt(_T("TagName=%s (%s), Value = %f"), szID,pTag->m_szName,dValue);	
						}
						else
						{
							m_pDrvObj->AddWarningFmt("ID = %s 不存在！", szID);	
						}
						nsize -= sizeof(IDENTPT35_0);
					}
					pFrm += sizeof(IDENTPT35_0) * byteNum;
				}
				else
				{
					//信息对象地址 起始ID
					int nID = *pFrm + ((*(pFrm+1) & 0xFF) << 8) + ((*(pFrm+2) & 0xFF) << 16);
					nID -= m_lAiStartAddr;
					pFrm += 3;
					nsize -= 3;

					LPIDENTPT35_1 lpident35 = (LPIDENTPT35_1)pFrm;
					for (int ii = 0; ii < byteNum; ii++)
					{
						szID.Format("BD_TIME_%ld",nID + ii);
						pTag = m_pDrvObj->FindByID(szID);
						if (pTag)
						{
							short fBuf = *(short*)lpident35[ii].byVal;
							float f1 = (float)atof(pTag->m_szNote);
							if (f1 == 0)
								f1 = 1;
							float fVal = f1 * fBuf;
						/*	int accum = 0; 
							for (int shiftBy = 0; shiftBy < sizeof(lpident35[ii].byVal)/sizeof(lpident35[ii].byVal[0]); shiftBy++) 
							{ 
								accum |= (lpident35[ii].byVal[shiftBy] & 0xff) << (shiftBy * 8); 
							} 

							BYTE btSign = ((accum >> 15) & 0X01);
							accum = (accum & 0x7FFF);
							if (btSign == 1)
								accum = 0 - (accum ^ 0x7FFF);

							float f1 = atof(pTag->m_szNote);
							if (f1 == 0)
								f1 = 1;
							float fVal = float(accum * f1);*/
							COleDateTime dt;
							int nSec = MAKEWORD(lpident35[ii].byTime[0],lpident35[ii].byTime[1]) / 1000;
							dt.SetDateTime(lpident35[ii].byTime[6]+2000,lpident35[ii].byTime[5],lpident35[ii].byTime[4],
								lpident35[ii].byTime[3],lpident35[ii].byTime[2],nSec);
							
							SYSTEMTIME systime;
							VariantTimeToSystemTime(dt, &systime);
							CTime tm(systime);
							
							dValue = fVal*pTag->m_dblField1 + pTag->m_dblField2;
							m_pDrvObj->AddValueByID(szID, (long)tm.GetTime(), QA_OK,dValue);
							m_pDrvObj->AddDebugFmt(_T("TagName=%s (%s), Value = %f"), szID,pTag->m_szName,dValue);	
						}
						else
						{
							m_pDrvObj->AddWarningFmt("ID = %s 不存在！", szID);	
						}
						nsize -= sizeof(IDENTPT35_1);
					}
					pFrm += sizeof(IDENTPT35_1) * byteNum;
				}
			}
			else if (13 == byteType) //不带时标短浮点数遥测
			{
				if (!byteContinuous)
				{			
					LPIDENTPT13_0 lpident13 = (LPIDENTPT13_0)pFrm;
					for (int ii = 0; ii < byteNum; ii++)
					{
						int nID = lpident13[ii].byAddr[0] + ((lpident13[ii].byAddr[1] & 0xFF) << 8) + ((lpident13[ii].byAddr[2] & 0xFF) << 16);		
						szID.Format("SF_NOTIME_%ld", nID - m_lAiStartAddr);

						pTag = m_pDrvObj->FindByID(szID);
						if (pTag)
						{
							float fBuf = *(float*)lpident13[ii].byVal;
							float f1 = (float)atof(pTag->m_szNote);
							if (f1 == 0)
								f1 = 1;
							float fVal = float(fBuf * f1);
						/*	int accum = 0; 
							for (int shiftBy = 0; shiftBy < sizeof(lpident13[ii].byVal)/sizeof(lpident13[ii].byVal[0]); shiftBy++) 
							{ 
								accum |= (lpident13[ii].byVal[shiftBy] & 0xff) << (shiftBy * 8); 
							} 

							float f0 = *(float*)lpident13[ii].byVal;
							BYTE btSign = ((accum >> 31) & 0X01);
							accum = (accum & 0x7FFF);
							if (btSign == 1)
								accum = 0 - (accum ^ 0x7FFF);

							float f1 = atof(pTag->m_szNote);
							if (f1 == 0)
								f1 = 1;
							float fVal = float(f0 * f1);*/
							dValue = fVal*pTag->m_dblField1 + pTag->m_dblField2;
							m_pDrvObj->AddValueByID(szID, 0, QA_OK, dValue);
							m_pDrvObj->AddDebugFmt(_T("TagName=%s (%s), Value = %f"), szID,pTag->m_szName,dValue);	
						}
						else
						{
							m_pDrvObj->AddWarningFmt("ID = %s 不存在！", szID);	
						}
						nsize -= sizeof(IDENTPT13_0);
					}
					pFrm += sizeof(IDENTPT13_0) * byteNum;
				}
				else
				{
					//信息对象地址 起始ID
					int nID = *pFrm + ((*(pFrm+1) & 0xFF) << 8) + ((*(pFrm+2) & 0xFF) << 16);
					//m_pDrvObj->AddDebugFmt(_T("无时标浮点：StartID=%d, Count=%d"), nID, byteNum);
					// 南通的104不需要减0x4000，否则计算出来的ID不正确
					nID -= m_lAiStartAddr;
					pFrm += 3;
					nsize -= 3;

					m_pDrvObj->AddDebugFmt(_T("TagName=SF_NOTIME_%ld to SF_NOTIME_%ld"), nID, nID + byteNum - 1);

					LPIDENTPT13_1 lpident13 = (LPIDENTPT13_1)pFrm;
					for (int ii = 0; ii < byteNum; ii++)
					{
						szID.Format("SF_NOTIME_%ld",nID + ii);
						pTag = m_pDrvObj->FindByID(szID);
						if (pTag)
						{
							float fBuf = *(float*)lpident13[ii].byVal;
							float f1 = (float)atof(pTag->m_szNote);
							if (f1 == 0)
								f1 = 1;
							float fVal = float(fBuf * f1);
						/*	int accum = 0; 
							for (int shiftBy = 0; shiftBy < sizeof(lpident13[ii].byVal)/sizeof(lpident13[ii].byVal[0]); shiftBy++) 
							{ 
								accum |= (lpident13[ii].byVal[shiftBy] & 0xff) << (shiftBy * 8); 
							} 

							float f0 = *(float*)lpident13[ii].byVal;
							BYTE btSign = ((accum >> 31) & 0X01);
							accum = (accum & 0x7FFF);
							if (btSign == 1)
								accum = 0 - (accum ^ 0x7FFF);

							float f1 = atof(pTag->m_szNote);
							if (f1 == 0)
								f1 = 1;
							float fVal = float(f0 * f1);*/

							dValue = fVal*pTag->m_dblField1 + pTag->m_dblField2;
							m_pDrvObj->AddValueByID(szID, 0, QA_OK,dValue);
							m_pDrvObj->AddDebugFmt(_T("TagName=%s (%s), Value = %f"), szID,pTag->m_szName,dValue);	
						}
						else
						{
							m_pDrvObj->AddWarningFmt("ID = %s 不存在！", szID);	
						}
						nsize -= sizeof(IDENTPT13_1);
					}
					pFrm += sizeof(IDENTPT13_1) * byteNum;
				}
			}
			else if (36 == byteType) //带时标短浮点数遥测
			{
				if (!byteContinuous)
				{					
					LPIDENTPT36_0 lpident36 = (LPIDENTPT36_0)pFrm;
					for (int ii = 0; ii < byteNum; ii++)
					{
						int nID = lpident36[ii].byAddr[0] + ((lpident36[ii].byAddr[1] & 0xFF) << 8) + ((lpident36[ii].byAddr[2] & 0xFF) << 16);		
						szID.Format("SF_TIME_%ld", nID - m_lAiStartAddr);
						pTag = m_pDrvObj->FindByID(szID);
						if (pTag)
						{
							float fBuf = *(float*)lpident36[ii].byVal;
							float f1 = (float)atof(pTag->m_szNote);
							if (f1 == 0)
								f1 = 1;
							float fVal = float(fBuf * f1);
						/*	int accum = 0; 
							for (int shiftBy = 0; shiftBy < sizeof(lpident36[ii].byVal)/sizeof(lpident36[ii].byVal[0]); shiftBy++) 
							{ 
								accum |= (lpident36[ii].byVal[shiftBy] & 0xff) << (shiftBy * 8); 
							} 

							BYTE btSign = ((accum >> 31) & 0X01);
							accum = (accum & 0x7FFF);
							if (btSign == 1)
								accum = 0 - (accum ^ 0x7FFF);

							float f1 = atof(pTag->m_szNote);
							if (f1 == 0)
								f1 = 1;
							float fVal = float(accum * f1);*/
							COleDateTime dt;
							int nSec = MAKEWORD(lpident36[ii].byTime[0],lpident36[ii].byTime[1]) / 1000;
							dt.SetDateTime(lpident36[ii].byTime[6]+2000,lpident36[ii].byTime[5],lpident36[ii].byTime[4],
								lpident36[ii].byTime[3],lpident36[ii].byTime[2],nSec);
							
							SYSTEMTIME systime;
							VariantTimeToSystemTime(dt, &systime);
							CTime tm(systime);
							
							dValue = fVal*pTag->m_dblField1 + pTag->m_dblField2;
							m_pDrvObj->AddValueByID(szID, (long)tm.GetTime(), QA_OK, dValue);
							m_pDrvObj->AddDebugFmt(_T("TagName=%s (%s), Value = %f"), szID,pTag->m_szName,dValue);	
						}
						else
						{
							m_pDrvObj->AddWarningFmt("ID = %s 不存在！", szID);	
						}
						nsize -= sizeof(IDENTPT36_0);
					}
					pFrm += sizeof(IDENTPT36_0) * byteNum;
				}
				else
				{
					//信息对象地址 起始ID
					int nID = *pFrm + ((*(pFrm+1) & 0xFF) << 8) + ((*(pFrm+2) & 0xFF) << 16);
					nID -= m_lAiStartAddr;
					pFrm += 3;
					nsize -= 3;

					LPIDENTPT36_1 lpident36 = (LPIDENTPT36_1)pFrm;
					for (int ii = 0; ii < byteNum; ii++)
					{
						szID.Format("SF_TIME_%ld",nID + ii);
						pTag = m_pDrvObj->FindByID(szID);
						if (pTag)
						{
							float fBuf = *(float*)lpident36[ii].byVal;
							float f1 = (float)atof(pTag->m_szNote);
							if (f1 == 0)
								f1 = 1;
							float fVal = float(fBuf * f1);
						/*	int accum = 0; 
							for (int shiftBy = 0; shiftBy < sizeof(lpident36[ii].byVal)/sizeof(lpident36[ii].byVal[0]); shiftBy++) 
							{ 
								accum |= (lpident36[ii].byVal[shiftBy] & 0xff) << (shiftBy * 8); 
							} 

							BYTE btSign = ((accum >> 31) & 0X01);
							accum = (accum & 0x7FFF);
							if (btSign == 1)
								accum = 0 - (accum ^ 0x7FFF);

							float f1 = atof(pTag->m_szNote);
							if (f1 == 0)
								f1 = 1;
							float fVal = float(accum * f1);*/
							COleDateTime dt;
							int nSec = MAKEWORD(lpident36[ii].byTime[0],lpident36[ii].byTime[1]) / 1000;
							dt.SetDateTime(lpident36[ii].byTime[6]+2000,lpident36[ii].byTime[5],lpident36[ii].byTime[4],
								lpident36[ii].byTime[3],lpident36[ii].byTime[2],nSec);
							
							SYSTEMTIME systime;
							VariantTimeToSystemTime(dt, &systime);
							CTime tm(systime);
							
							dValue = fVal*pTag->m_dblField1 + pTag->m_dblField2;
							m_pDrvObj->AddValueByID(szID, (long)tm.GetTime(), QA_OK,dValue);
							m_pDrvObj->AddDebugFmt(_T("TagName=%s (%s), Value = %f"), szID,pTag->m_szName,dValue);	
						}
						else
						{
							m_pDrvObj->AddWarningFmt("ID = %s 不存在！", szID);	
						}
						nsize -= sizeof(IDENTPT36_1);
					}
					pFrm += sizeof(IDENTPT36_1) * byteNum;
				}
			}
			else if (21 == byteType) //不带品质描述的规一化遥测帧
			{
				if (!byteContinuous)
				{
					LPIDENTPT21_0 lpident21 = (LPIDENTPT21_0)pFrm;
					for (int ii = 0; ii < byteNum; ii++)
					{
						int nID = lpident21[ii].byAddr[0] + ((lpident21[ii].byAddr[1] & 0xFF) << 8) + ((lpident21[ii].byAddr[2] & 0xFF) << 16);		
						szID.Format("GY_NOQUALITY_%ld", nID - m_lAiStartAddr);
						pTag = m_pDrvObj->FindByID(szID);
						if (pTag)
						{
							short fBuf = *(short*)lpident21[ii].byVal;
							float f1 = (float)atof(pTag->m_szNote);
							if (f1 == 0)
								f1 = 1;
							float fVal = f1 * fBuf;
						/*	int accum = 0;
							for (int shiftBy = 0; shiftBy < sizeof(lpident21[ii].byVal)/sizeof(lpident21[ii].byVal[0]); shiftBy++) 
							{ 
								accum |= (lpident21[ii].byVal[shiftBy] & 0xff) << (shiftBy * 8); 
							} 

							BYTE btSign = ((accum >> 15) & 0X01);
							accum = (accum & 0x7FFF);
							if (btSign == 1)
								accum = 0 - (accum ^ 0x7FFF);
							float f1 = atof(pTag->m_szNote);
								
							if (f1 == 0)
								f1 = 1;
							float fVal = float(accum * f1);*/
							dValue = fVal*pTag->m_dblField1 + pTag->m_dblField2;
							m_pDrvObj->AddValueByID(szID, 0, QA_OK, dValue);
							m_pDrvObj->AddDebugFmt(_T("TagName=%s (%s), Value = %f"), szID,pTag->m_szName,dValue);	
						}
						else
						{
							m_pDrvObj->AddWarningFmt("ID = %s 不存在！", szID);	
						}
						nsize -= sizeof(IDENTPT21_0);
					}
					pFrm += sizeof(IDENTPT21_0) * byteNum;
				}
				else
				{
					//信息对象地址 起始ID
					int nID = *pFrm + ((*(pFrm+1) & 0xFF) << 8) + ((*(pFrm+2) & 0xFF) << 16);
					nID -= m_lAiStartAddr;
					pFrm += 3;
					nsize -= 3;

					LPIDENTPT21_1 lpident21 = (LPIDENTPT21_1)pFrm;
					for (int ii = 0; ii < byteNum; ii++)
					{
						szID.Format("GY_NOQUALITY_%ld",nID + ii);
						pTag = m_pDrvObj->FindByID(szID);
						if (pTag)
						{
							short fBuf = *(short*)lpident21[ii].byVal;
							float f1 = (float)atof(pTag->m_szNote);
							if (f1 == 0)
								f1 = 1;
							float fVal = f1 * fBuf;
						/*	int accum = 0; 
							for (int shiftBy = 0; shiftBy < sizeof(lpident21[ii].byVal)/sizeof(lpident21[ii].byVal[0]); shiftBy++) 
							{ 
								accum |= (lpident21[ii].byVal[shiftBy] & 0xff) << (shiftBy * 8); 
							} 

							BYTE btSign = ((accum >> 15) & 0X01);
							accum = (accum & 0x7FFF);
							if (btSign == 1)
								accum = 0 - (accum ^ 0x7FFF);
						
							float f1 = atof(pTag->m_szNote);
							if (f1 == 0)
								f1 = 1;
							float fVal = float(accum * f1);*/

							dValue = fVal*pTag->m_dblField1 + pTag->m_dblField2;
							m_pDrvObj->AddValueByID(szID, 0, QA_OK, dValue);
							m_pDrvObj->AddDebugFmt(_T("TagName=%s (%s), Value = %f"), szID,pTag->m_szName,dValue);	
						}
						else
						{
							m_pDrvObj->AddWarningFmt("ID = %s 不存在！", szID);	
						}
						nsize -= sizeof(IDENTPT21_1);
					}
					pFrm += sizeof(IDENTPT21_1) * byteNum;
				}
				lpapci = (LPAPCI)pFrm;
			}
			else if (15 == byteType)
			{
				if (!byteContinuous) //不连续  每个信息一个地址
				{
					LPIDENTPT15_0 lpidentpt15 = (LPIDENTPT15_0)pFrm;
					for (int ii = 0; ii < byteNum; ii++)
					{
						int nID = lpidentpt15[ii].byAddr[0] + ((lpidentpt15[ii].byAddr[1] & 0xFF) << 8) + ((lpidentpt15[ii].byAddr[2]) & 0xFF << 16);
						szID.Format("POWER_%ld",nID - m_lPowerStartAddr);
						if(nID - m_lPowerStartAddr == 134)
						{
							int a=0;
						}
						pTag = m_pDrvObj->FindByID(szID);

						if (pTag)
						{
							long lVal = 0;
							memcpy(&lVal,lpidentpt15[ii].byVal,4);
							//平圩3期对于电量数据需要添加计算系数 王波 20150508
							//m_pDrvObj->AddValueByID(szID, 0, QA_OK, long(lVal));
							float f1 = (float)atof(pTag->m_szNote);
							if (f1 == 0)
								f1 = 1;
							float fVal = f1 * lVal;

							dValue = fVal*pTag->m_dblField1 + pTag->m_dblField2;
							m_pDrvObj->AddValueByID(szID, 0, QA_OK, dValue);
							m_pDrvObj->AddDebugFmt(_T("TagName=%s (%s), Value = %f"), szID,pTag->m_szName, dValue);	
						}
						else
						{
							m_pDrvObj->AddWarningFmt("ID = %s 不存在！", szID);	
						}
						nsize -= sizeof(IDENTPT15_0);
					}
					pFrm += sizeof(IDENTPT15_0) * byteNum;
				}
				else //连续
				{
					//信息对象地址 起始ID
					int nID = *pFrm + ((*(pFrm+1) & 0xFF) << 8) + ((*(pFrm+2) & 0xFF) << 16);
					nID -= m_lPowerStartAddr;
					pFrm += 3;
					nsize -= 3;

					m_pDrvObj->AddDebugFmt(_T("TagName: POWER_%ld to POWER_%ld"), nID, nID + byteNum - 1);
					LPIDENTPT15_1 lpidentpt15 = (LPIDENTPT15_1)pFrm;
					for (int ii = 0; ii < byteNum; ii++)
					{
						szID.Format("POWER_%ld",nID + ii);
						if((nID + ii) == 134)
						{
							int a = 0;
						}
						pTag = m_pDrvObj->FindByID(szID);
						if (pTag)
						{
							long lVal = 0;
							memcpy(&lVal,lpidentpt15[ii].byVal,4);
							//平圩3期对于电量数据需要添加计算系数 王波 20150508
							//m_pDrvObj->AddValueByID(szID, 0, QA_OK, long(lVal));
							float f1 = (float)atof(pTag->m_szNote);
							if (f1 == 0)
								f1 = 1;
							float fVal = f1 * lVal;

							dValue = fVal*pTag->m_dblField1 + pTag->m_dblField2;
							m_pDrvObj->AddValueByID(szID, 0, QA_OK, dValue);
							m_pDrvObj->AddDebugFmt(_T("TagName=%s (%s), Value = %f"), szID,pTag->m_szName, dValue);	
						}
						else
						{
							m_pDrvObj->AddWarningFmt("ID = %s 不存在！", szID);	
						}
						nsize -= sizeof(IDENTPT15_1);
					}
					pFrm += sizeof(IDENTPT15_1) * byteNum;
				}
			}
			else if (37 == byteType)//处理遥脉
			{
				if (!byteContinuous) //不连续  每个信息一个地址
				{
					LPIDENTPT37_0 lpidentpt15 = (LPIDENTPT37_0)pFrm;
					
					for (int ii = 0; ii < byteNum; ii++)
					{
						int nID = lpidentpt15[ii].byAddr[0] + ((lpidentpt15[ii].byAddr[1] & 0xFF) << 8) + ((lpidentpt15[ii].byAddr[2]) & 0xFF << 16);
						szID.Format("POWER_%ld",nID - m_lPowerStartAddr);
						m_pDrvObj->AddDebugFmt("遥脉 szID=%s, nID=%d", szID, nID);
						pTag = m_pDrvObj->FindByID(szID);
						if (pTag)
						{
							long lVal = lpidentpt15[ii].iValue;

							dValue = lVal*pTag->m_dblField1 + pTag->m_dblField2;
							m_pDrvObj->AddValueByID(szID,0, QA_OK,dValue);
							m_pDrvObj->AddDebugFmt(_T("TagName=%s (%s), Value = %d"), szID,pTag->m_szName, dValue);	
						}
						else
						{
							m_pDrvObj->AddWarningFmt("ID = %s 不存在！", szID);	
						}
						nsize -= sizeof(IDENTPT37_0);
					}
					pFrm += sizeof(IDENTPT37_0) * byteNum;
				}
				else //连续
				{
					//信息对象地址 起始ID
					int nID = *pFrm + ((*(pFrm+1) & 0xFF) << 8) + ((*(pFrm+2) & 0xFF) << 16);
					nID -= m_lPowerStartAddr;
					pFrm += 3;
					nsize -= 3;

					m_pDrvObj->AddDebugFmt(_T("遥脉 TagName: POWER_%ld to POWER_%ld"), nID, nID + byteNum - 1);
					LPIDENTPT37_1 lpidentpt15 = (LPIDENTPT37_1)pFrm;
					for (int ii = 0; ii < byteNum; ii++)
					{
						szID.Format("POWER_%ld",nID + ii);
						pTag = m_pDrvObj->FindByID(szID);
						if (pTag)
						{
							long lVal = lpidentpt15[ii].iValue;

							dValue = lVal*pTag->m_dblField1 + pTag->m_dblField2;
							m_pDrvObj->AddValueByID(szID,0, QA_OK, dValue);
							m_pDrvObj->AddDebugFmt(_T("TagName=%s (%s), Value = %d"), szID,pTag->m_szName,dValue);	
						}
						else
						{
							m_pDrvObj->AddWarningFmt("ID = %s 不存在！", szID);	
						}
						nsize -= sizeof(IDENTPT37_1);
					}
					pFrm += sizeof(IDENTPT37_1) * byteNum;
				}
			}
		}
		m_pDrvObj->FlushValue();
	
		nStatus = m_FrmParseStream.Parse(); // 继续解析待处理数据，其中没有完整报文时，下次从缓冲中读取并放入待处理数组中
	}

	if (!m_bEnd)
	{
		if (!AffirmRecv())
			CloseSummon();
	}
	else
	{
		m_bLinkOK = FALSE;
		m_wSendFrameNum = 0;
		m_wReceiveFrameNum = 0;
		m_bEnd = FALSE;
	}

	if (m_bSendTest)
	{
		BYTE bufSend[LINKPACKETLENGTH] = {0x68, 0x04, 0x83, 0x00, 0x00, 0x00};
		SendData(bufSend,LINKPACKETLENGTH);
		m_bSendTest = FALSE;
	}

	if (m_bSendCall)
	{
		Summon();//发送总召唤
		m_bSendCall = FALSE;
	}
	else
	{
		long lEndTime = (long)::time(NULL);
		if (lStartTime != 0)
		{
			if (lEndTime - lStartTime < 300)
				return;
			Summon();//发送总召唤
		}
		lStartTime = lEndTime;
	}
}

void CMyTCPClientProtocol::GetAddress()
{
	char *p = NULL, *pNext = NULL; 

	m_nADPUAddr = 1;
	m_szIP2.Empty();
	m_wPort2 = 0;
	m_lDiStartAddr = 0;
	m_lAiStartAddr = 16384;
	m_lPowerStartAddr = 25600;

	char szParam[128];
	strcpy_s(szParam, 128, (char*)(LPCTSTR)m_config.m_szTcpUdpParam);
	int i = 0;
	p = strtok_s(szParam, ";", &pNext);
	CString szValue;
	
	while(p != NULL)
	{  
		szValue = p;
		szValue.Trim();

		switch(i)
		{
		case 0:
			if (!szValue.IsEmpty() && (szValue != "_") && (szValue != "-"))
			{
				int nAddr = atoi(szValue);
				if (nAddr > 0 && nAddr < 65535)
					m_nADPUAddr = nAddr;
			}
			break;
		case 1:
			if (!szValue.IsEmpty() && (szValue != "_") && (szValue != "-"))
			{
				int nFind = szValue.Find(':');
				if (nFind != -1)
				{
					m_szIP2 = szValue.Left(nFind);
					m_wPort2 = atoi( szValue.Right(szValue.GetLength() - nFind - 1) );
				}
			}
			break;
		case 2:
			if (!szValue.IsEmpty() && (szValue != "_") && (szValue != "-"))
			{
				m_lDiStartAddr = atoi(szValue);
			}
			break;
		case 3:
			if (!szValue.IsEmpty() && (szValue != "_") && (szValue != "-"))
			{
				m_lAiStartAddr = atoi(szValue);
			}
			break;
		case 4:
			if (!szValue.IsEmpty() && (szValue != "_") && (szValue != "-"))
			{
				m_lPowerStartAddr = atoi(szValue);
			}
			break;
		}

		i++;
		p = strtok_s(NULL, ";", &pNext);
	}
}

BOOL CMyTCPClientProtocol::AskPower(BYTE byCmd)
{
	BYTE byLow_Send = (m_wSendFrameNum & 0x7F) << 1;
	BYTE byHigh_Send = (m_wSendFrameNum >> 7) & 0x7F;
	BYTE byLow_Receive = (m_wReceiveFrameNum & 0x7F) << 1;
	BYTE byHigh_Receive = (m_wReceiveFrameNum >> 7) & 0x7F;
	BYTE byLow_ADPUAddr = LOBYTE(m_nADPUAddr);
	BYTE byHigh_ADPUAddr = HIBYTE(m_nADPUAddr);
	BYTE Buf[SUMMONPACKETLENGTH] = {0x68, 0x0E, byLow_Send, byHigh_Send, byLow_Receive, byHigh_Receive, 
		0x65, 0x01, 0x06, 0x00, byLow_ADPUAddr, byHigh_ADPUAddr, 0x00, 0x00, 0x00, byCmd};
	int nsize = SendData(Buf,SUMMONPACKETLENGTH); 
	if (0 == nsize)
		return FALSE;
	
	m_wSendFrameNum++;
	return TRUE;
}
void	CMyTCPClientProtocol::Disconnect()
{
	if(m_sClient != INVALID_SOCKET)
	{
		// 优雅关闭Socket，但未考虑客户端读数据情况
		::shutdown(m_sClient, SD_BOTH);
		::closesocket(m_sClient);
		m_sClient = INVALID_SOCKET;
	}
}
