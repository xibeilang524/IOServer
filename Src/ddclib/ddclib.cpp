// ddclib.cpp: implementation of the CDDCDrvObj class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ddclib.h"

#include "CfgDlg.h"
#include "ShowHelpDlg.h"
#include "DrvVersion.h"
#include "HashTableUtils.h"

#include <shlwapi.h>
#pragma comment( lib, "shlwapi.lib")

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define DRVCFG_VERSION		(DRVCFG_VERSION_USER + 4)

CString CConfigInfo::m_szHelpFile = _T("ddclib.chm");
CString CConfigInfo::m_szParamDisplayName = _T("连接参数");
CString CConfigInfo::m_szParamHintText = _T("（除非特殊说明，该项一般不用填）");
BOOL CConfigInfo::m_bSelectFileName = TRUE;
CString CConfigInfo::m_szSelectFileFilter = _T("文件(*.*)|*.*||");
CString CConfigInfo::m_szSelectFileHint = _T("请选择目录");

CString GetSystemErrorMessage(DWORD dwErrorCode)
{
	TCHAR *pBuffer = NULL;

	::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				dwErrorCode,
				0,
				(LPTSTR)&pBuffer,
				0,
				NULL);

	CString szMsg;
	if (pBuffer)
	{
		szMsg = pBuffer;
		::LocalFree(pBuffer);
	}
	else {
		szMsg = _T("获取错误信息失败");
	}

	return szMsg;
}

//取应用程序所在目录
CString GetHomeDirectory()
{
	TCHAR sFilename[_MAX_PATH];
	TCHAR sDrive[_MAX_DRIVE];
	TCHAR sDir[_MAX_DIR];
	TCHAR sFname[_MAX_FNAME];
	TCHAR sExt[_MAX_EXT];
	GetModuleFileName(AfxGetInstanceHandle(), sFilename, _MAX_PATH);
	_tsplitpath(sFilename, sDrive, sDir, sFname, sExt);
	
	CString rVal(CString(sDrive) + CString(sDir));
	int nLen = rVal.GetLength();
	if (rVal.GetAt(nLen-1) != _T('\\'))
		rVal += _T("\\");
	
	return rVal;
}

BOOL EnabledOpenHelpFile()
{
	if (CConfigInfo::m_szHelpFile.Find(_T("res://")) == 0)
		return TRUE;
	else {
		CString szPath = GetHomeDirectory();
		szPath += CConfigInfo::m_szHelpFile;

		return ::PathFileExists( szPath );
	}
}

void OpenHelpFile()
{
	if (CConfigInfo::m_szHelpFile.Find(_T("res://")) == 0)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState()); //资源切换

		CShowHelpDlg dlg;
		dlg.DoModal();
	}
	else {
		CString szPath = GetHomeDirectory();
		szPath += CConfigInfo::m_szHelpFile;

		if (::PathFileExists( szPath ))
		{
			::ShellExecute(NULL, _T("open"), szPath, NULL, NULL, SW_SHOW);
		}
	}
}

//////////////////////////////////////////////////////////////////////
// CConfigInfo class Construction/Destruction
//////////////////////////////////////////////////////////////////////
CConfigInfo::CConfigInfo()
	: m_pdrv(NULL)
	, m_bModified(FALSE)
	, m_wComType(COM_TYPE_COM)
	, m_szIP(_T("127.0.0.1"))
	, m_wPort(10000)
	, m_szComPort(_T("\\\\.\\com1"))
	, m_dwBaudRate(CBR_9600)
	, m_cByteSize( 8)
	, m_cParity(0)
	, m_cStopBits(0)
	, m_nReadTimeout(200)
	, m_nWriteTimeout(200)
	, m_bDebug(FALSE)
	, m_nUpdateSec(1000)
	, m_bLocalTime(FALSE)
	, m_nTimeBias(0)
	, m_nTimeOffset(0)
	, m_bAutoDeleteFile(FALSE)
{
}

CConfigInfo::~CConfigInfo()
{

}

void CConfigInfo::Serialize(CArchive& ar, DWORD dwVersion)
{
	if (ar.IsLoading())
	{
		ar >> m_wComType >> m_szIP >> m_wPort >> m_szComPort >> m_dwBaudRate >> 
			m_cByteSize >> m_cParity >> m_cStopBits >> m_nReadTimeout >> 
			m_nWriteTimeout >> m_nUpdateSec >> m_bDebug;

			ar >> m_bLocalTime >> m_nTimeBias;

			ar >> m_szTcpUdpParam >> m_szComParam;
			ar >> m_szDataSource >> m_szPsw >> m_szID;
			
			CalcTimeOffset();

			if (dwVersion >= DRVCFG_VERSION)
			{
				ar >> m_szFileName >> m_bAutoDeleteFile >> m_szFileParam;
			}
			else {
				m_szFileName.Empty();
				m_szFileParam.Empty();
			}
	}
	else {
		ar << m_wComType << m_szIP << m_wPort << m_szComPort << m_dwBaudRate << 
			m_cByteSize << m_cParity << m_cStopBits << m_nReadTimeout << 
			m_nWriteTimeout << m_nUpdateSec << m_bDebug;

		// 2010-07-19新增属性
		ar << m_bLocalTime << m_nTimeBias;

		ar << m_szTcpUdpParam << m_szComParam;

		ar << m_szDataSource << m_szPsw << m_szID;

		// 2013-03-19增加新属性
		ar << m_szFileName << m_bAutoDeleteFile << m_szFileParam;
	}
	m_bModified = FALSE;
}	
	
CConfigInfo& CConfigInfo::operator=(const CConfigInfo& config)
{
	if (&config != this)
	{		
		m_bModified		= config.m_bModified;
		m_wComType		= config.m_wComType;	
		m_szIP			= config.m_szIP;
		m_wPort			= config.m_wPort;
		m_szTcpUdpParam	= config.m_szTcpUdpParam;
		m_szComPort		= config.m_szComPort;
		m_dwBaudRate	= config.m_dwBaudRate;
		m_cByteSize		= config.m_cByteSize;
		m_cParity		= config.m_cParity;
		m_cStopBits		= config.m_cStopBits;
		m_szComParam	= config.m_szComParam;
		m_nReadTimeout	= config.m_nReadTimeout;
		m_nWriteTimeout = config.m_nWriteTimeout;
		m_nUpdateSec	= config.m_nUpdateSec;
		m_bDebug		= config.m_bDebug;
		m_bLocalTime	= config.m_bLocalTime;
		m_nTimeBias		= config.m_nTimeBias;

		m_szDataSource	= config.m_szDataSource;
		m_szPsw			= config.m_szPsw;
		m_szID			= config.m_szID;

		m_szFileName	= config.m_szFileName;
		m_bAutoDeleteFile = config.m_bAutoDeleteFile;
		m_szFileParam	= config.m_szFileParam;

		CalcTimeOffset();
	}

	return *this;
}

BOOL CConfigInfo::operator==( const CConfigInfo& config)
{
	return  (m_wComType			== config.m_wComType)		&&
			(m_szIP				== config.m_szIP)			&&
			(m_wPort			== config.m_wPort)			&&
			(m_szTcpUdpParam	== config.m_szTcpUdpParam)	&&
			(m_szComPort		== config.m_szComPort)		&&
			(m_dwBaudRate		== config.m_dwBaudRate)		&&
			(m_cByteSize		== config.m_cByteSize)		&&
			(m_cParity			== config.m_cParity)		&&
			(m_cStopBits		== config.m_cStopBits)		&&
			(m_szComParam		== config.m_szComParam)		&&
			(m_nReadTimeout		== config.m_nReadTimeout)	&&
			(m_nWriteTimeout	== config.m_nWriteTimeout)	&&
			(m_nUpdateSec		== config.m_nUpdateSec)		&&
			(m_bDebug			== config.m_bDebug)			&&
			(m_bLocalTime		== config.m_bLocalTime)		&&
			(m_nTimeBias		== config.m_nTimeBias)		&&

			(m_szFileName		== config.m_szFileName)		&&
			(m_bAutoDeleteFile		== config.m_bAutoDeleteFile)&&
			(m_szFileParam		== config.m_szFileParam)	&&

			(m_szDataSource     == config.m_szDataSource)   &&
			(m_szPsw			== config.m_szPsw)          &&
			(m_szID				== config.m_szID)			;
}

BOOL CConfigInfo::operator!=( const CConfigInfo& config)
{
	return  (m_wComType		!= config.m_wComType)		||
		(m_szIP				!= config.m_szIP)			||
		(m_wPort			!= config.m_wPort)			||
		(m_szTcpUdpParam	!= config.m_szTcpUdpParam)	||
		(m_szComPort		!= config.m_szComPort)		||
		(m_dwBaudRate		!= config.m_dwBaudRate)		||
		(m_cByteSize		!= config.m_cByteSize)		||
		(m_cParity			!= config.m_cParity)		||
		(m_cStopBits		!= config.m_cStopBits)		||
		(m_szComParam		!= config.m_szComParam)		||
		(m_nReadTimeout		!= config.m_nReadTimeout)	||
		(m_nWriteTimeout	!= config.m_nWriteTimeout)	||
		(m_nUpdateSec		!= config.m_nUpdateSec)		||
		(m_bDebug			!= config.m_bDebug)			||
		(m_bLocalTime		!= config.m_bLocalTime)		||
		(m_nTimeBias		!= config.m_nTimeBias)		||

		(m_szFileName		!= config.m_szFileName)		||
		(m_bAutoDeleteFile		!= config.m_bAutoDeleteFile)||
		(m_szFileParam		!= config.m_szFileParam)	||

		(m_szDataSource     != config.m_szDataSource)   ||
		(m_szPsw			!= config.m_szPsw)          ||
		(m_szID				!= config.m_szID)			;
}

CString CConfigInfo::GetConnectString()
{
	CString szt;
	switch(m_wComType)
	{
	case COM_TYPE_COM:		
		szt.Format(_T("通讯协议：COM，端口：%s，波特率：%ld，数据位：%d，奇偶校验：%d，停止位：%d，更新周期：%d ms，本机时标：%s，时区偏移：%d"), 
			m_szComPort, m_dwBaudRate, m_cByteSize, m_cParity, m_cStopBits, m_nUpdateSec, (m_bLocalTime)?_T("yes"):_T("no"), m_nTimeBias);
		break;
	case COM_TYPE_UDP:	
		szt.Format(_T("通讯协议：UDP，IP地址：%s，端口：%d，更新周期：%d ms，本机时标：%s，时区偏移：%d"), 
			m_szIP, m_wPort, m_nUpdateSec, (m_bLocalTime)?_T("yes"):_T("no"), m_nTimeBias);
		break;
	case COM_TYPE_UDPSERV:
		szt.Format(_T("通讯协议：UDP Server，端口：%d，更新周期：%d ms，本机时标：%s，时区偏移：%d"), 
			m_wPort, m_nUpdateSec, (m_bLocalTime)?_T("yes"):_T("no"), m_nTimeBias);
		break;
	case COM_TYPE_TCP:
		szt.Format(_T("通讯协议：TCP Client，IP地址：%s，端口：%d，更新周期：%d ms，本机时标：%s，时区偏移：%d"), 
			m_szIP, m_wPort, m_nUpdateSec, (m_bLocalTime)?_T("yes"):_T("no"), m_nTimeBias);
		break;
	case COM_TYPE_TCPSERV:
		szt.Format(_T("通讯协议：TCP Server，端口：%d，更新周期：%d ms，本机时标：%s，时区偏移：%d"), 
			m_wPort, m_nUpdateSec, (m_bLocalTime)?_T("yes"):_T("no"), m_nTimeBias);
		break;
	case COM_TYPE_DB:
		szt.Format(_T("ODBC数据源：%s，用户名：%s，密码：%s，更新周期：%d ms，本机时标：%s，时区偏移：%d"), 
			m_szDataSource,m_szID,m_szPsw,
			m_nUpdateSec, (m_bLocalTime)?_T("yes"):_T("no"), m_nTimeBias);
		break;
	case COM_TYPE_FILE:
		szt.Format(_T("文件名：%s，自动删除：%s，更新周期：%d ms，本机时标：%s，时区偏移：%d"), 
			m_szFileName, (m_bAutoDeleteFile)?_T("yes"):_T("no"),  m_nUpdateSec, (m_bLocalTime)?_T("yes"):_T("no"), m_nTimeBias);
		break;
	default:
		szt = _T("Unknown");
	}

	return szt;
}

///////////////////////////////////////////////////////////////////////////
// CRC-32 校检算法
///////////////////////////////////////////////////////////////////////////

// 用于查表的数组
unsigned long CCheckAlgorithm::crc32_table[256]={0}; 
BOOL CCheckAlgorithm::b_crc32IninTab = FALSE;
DWORD CCheckAlgorithm::Reflect(unsigned long ref, char ch)
{	
	int i;
	unsigned long value = 0;
	
	// bit 0 与 bit 7 交换
	// bit 1 与 bit 6 交换，如此类推...
	for(i = 1; i < (ch + 1); i++)
	{
		if (ref & 1)
			value |= 1 << (ch - i);
		ref >>= 1;
	}
	return value;
}
// 创建查表数组
void CCheckAlgorithm::Init_CRC32_Table()
{	
	// 该 CRC-32 多项式算法广泛应用于 PKZip、WinZip 和 Ethernet 中
	if(b_crc32IninTab)
		return;
	b_crc32IninTab=TRUE;
	unsigned long ulPolynomial = 0x04c11db7;
	int i, j;
	
	for(i = 0; i <= 0xFF; i++)
	{
		crc32_table[i] = Reflect(i, 8) << 24;
		for (j = 0; j < 8; j++)
			crc32_table[i] = (crc32_table[i] << 1) ^ 
			(crc32_table[i] & (1 << 31) ? ulPolynomial : 0);
		crc32_table[i] = Reflect(crc32_table[i], 32);
	}
}

DWORD CCheckAlgorithm::TL_GetCRC32(unsigned char *szData, unsigned long dwSize) //计算CRC32
{
	if(!b_crc32IninTab)
		Init_CRC32_Table();
	unsigned long  crc = 0xffffffff;
	int len;
	unsigned char* buffer = szData;
	
	len = dwSize;
	while(len--)
		crc = (crc >> 8) ^ crc32_table[(crc & 0xFF) ^ *buffer++];
	return crc^0xffffffff;
}

/*
 *	用于流协议分析
 */
//////////////////////////////////////////////////////////////////////////
CMyByteArray::CMyByteArray()
{
	m_pData = NULL;		//数据段
	m_nSize = 0;		//数据大小
	m_nBufSize = 0;		//空间大小
	m_nGrown = 16384;	//增长增量16K
}
CMyByteArray::~CMyByteArray()
{
	if(m_pData != NULL)
		free(m_pData);
	m_pData = NULL;
}

BOOL CMyByteArray::PutData(BYTE *pBuf,int nSize)//加入数据
{
	if((nSize < 1) ||(pBuf == NULL))
		return TRUE;
	if((m_nSize + nSize) > MAX_BYTEARRAYSIZE)
		return FALSE;
	if((m_nBufSize - m_nSize) < nSize) //空间不够
	{
		int nsizet = m_nGrown;//计算增量
		if(m_nGrown < nSize){
			nsizet = (nSize/m_nGrown) * m_nGrown + m_nGrown;
			if(( m_nSize + nsizet) > MAX_BYTEARRAYSIZE)
				nsizet = MAX_BYTEARRAYSIZE - m_nSize;
		}
		BYTE *p = (BYTE *)malloc(m_nBufSize + nsizet);//分配新空间
		if(p==NULL)
			return FALSE;
		if(m_pData){
			memcpy(p,m_pData,m_nSize);//复制
			free(m_pData);	//释放旧空间
		}
		m_pData = p;
		m_nBufSize += nsizet;
	}
	memcpy(m_pData + m_nSize,pBuf,nSize);//复制数据
	m_nSize += nSize;
	return TRUE;
}

BOOL CMyByteArray::PutStr(LPCTSTR lpszData,int nSize)
{
	return PutData((BYTE*)lpszData,nSize);
}

BYTE* CMyByteArray::GetBuf()
{
	return m_pData;
}

int CMyByteArray::GetDataSize()
{
	return m_nSize;
}

void CMyByteArray::	ClearData()
{
	m_nSize = 0;
}

void CMyByteArray::SetGrown(int ngrown)
{
	m_nGrown = ngrown;
	if(m_nGrown < 1024)
		m_nGrown = 1024;
	if(m_nGrown > 65536)
		m_nGrown = 65536;
}

void CMyByteArray::LeftMove(int n)//左移n字节
{
	if(m_pData == NULL)
		return;
	if(n <= 0)
		return;
	if(n >= m_nSize)
	{
		m_nSize = 0;
		return;
	}
	memmove(m_pData,m_pData + n,m_nSize - n);
	m_nSize -= n; 
}

void CMyByteArray::CopyTo(CMyByteArray *pba)
{
	pba->PutData(GetBuf(),GetDataSize());	
}; //复制到另一个字节数组尾部.

/////////////////////////////////////////////////////////////////////////////////////////////
CFrmParse::CFrmParse()
{
	m_nStatus = 0;
	m_nFrmLen = 0;
}

CFrmParse::~CFrmParse()
{

}

BOOL CFrmParse::CheckCrc32(BYTE *pd)//按照16字节报头校验
{
	DWORD dwCRC32 = CCheckAlgorithm::TL_GetCRC32(pd,12);
	DWORD *pcrc = (DWORD *)(pd+12);
	if(*pcrc != dwCRC32)
		return FALSE;
	return TRUE;
}

BYTE* CFrmParse::FindFlag(BYTE *pDataBuffer, int nDataSize, BYTE *pFlag, int nFlagSize)
{
	if (nFlagSize == 0)
		return pDataBuffer;

	BYTE *cp = pDataBuffer;
	BYTE *s1, *s2;

	int n = 0;
	while(cp - pDataBuffer < nDataSize)
	{
		s1 = cp;
		s2 = pFlag;

		while((cp - pDataBuffer < nDataSize) && (s2 - pFlag < nFlagSize) && !(*s1-*s2))
			s1++, s2++;

		if (s2 - pFlag >= nFlagSize)
			return cp;

		cp++;
	}

	return NULL;
}

int	CFrmParse::PutData(BYTE *buf,int nsize)//放入未解析数据,nsize < 256K
{
	if(m_nStatus == FRAME_STATUS_DATA_NO_ENOUGH) //报头完整
	{
		int n=m_Frm.GetDataSize();
		if( ( n + nsize) < m_nFrmLen)
		{
			m_Frm.PutData(buf,nsize);
			return FRAME_STATUS_DATA_NO_ENOUGH;
		}
		else 
		{			
			m_Frm.PutData(buf,m_nFrmLen - n);
			m_buf.PutData(buf + (m_nFrmLen - n),nsize - (m_nFrmLen - n));//剩下的放到待处理区
			m_nStatus = FRAME_STATUS_OK;
			return FRAME_STATUS_OK;
		}
	}
	m_buf.PutData(buf,nsize);//放入缓冲区
	return Parse();
}

/*
分包处理，根据实际情况改动分包算法

  返回值：
	0:没找到报头标志
	1:找到报头标志,长度不够无法验证报头
	2:报头校验完成,数据长度不够
	3:报文完成,
  */
int CFrmParse::Parse()//分包,返回0，1，2，3
{
	/*
	int nsize=m_buf.GetDataSize();
	if(nsize < 16)
		return FRAME_STATUS_NO_HEAD;
	BYTE *pc = m_buf.GetBuf();
	
	//无回朔扫描比较
	int np=0;//np指示向前移动字节数
	BOOL bFindHead=FALSE;

lpFindHeadFlag:
	while(np <= (nsize - 4))
	{
		if(pc[np] != 0x5A){
			np++;	continue;
		}
		if(pc[np+1] != 0x90){
			np++;		continue;
		}
		if(pc[np+2] != 0xEB){
			np += 2;	continue;
		}
		if(pc[np+3] != 0xA5){
			np += 3;	continue;
		}
		bFindHead = TRUE;
		break;
	}
	if(!bFindHead)	//没找到报文头标志
	{
		m_buf.LeftMove(np);
		return FRAME_STATUS_NO_HEAD;
	}
	if((np +16) > nsize)
	{
		m_buf.LeftMove(np);//长度不够
		return FRAME_STATUS_HEAD_NO_ENOUGH;
	}

	//验证CRC32
	if(!CheckCrc32(pc+np))
	{
		np++;
		goto lpFindHeadFlag;
	}
	//
	BYTE *pf = pc+np;
	DWORD *pdw = (DWORD *)(pf + 4);
	if(*pdw > (1024 * 1024))//MAX_FRM_LEN) //报文过长，可能是错误的报文
	{
		np++;
		goto lpFindHeadFlag;
	}
	m_nFrmLen = (int)(*pdw);//重置报文长度
	if((np + m_nFrmLen) <= nsize)
	{
		m_Frm.ClearData();//清空报文
		m_Frm.PutData(pc+np,m_nFrmLen);//复制报文
		m_buf.LeftMove(np + m_nFrmLen);//缓冲中剩下的字节左移
		m_nStatus = FRAME_STATUS_OK;
		return FRAME_STATUS_OK;
	}

	m_Frm.ClearData();//清空报文
	m_Frm.PutData(pc+np,nsize - np);//复制余下的字节
	m_buf.ClearData();//清空待处理缓冲
	*/
	m_nStatus = FRAME_STATUS_OK;
	return FRAME_STATUS_OK;//报头正确
}

//////////////////////////////////////////////////////////////////////
// CDDCDrvProtocol class
//////////////////////////////////////////////////////////////////////
CDDCDrvProtocol::CDDCDrvProtocol()
{
	m_pDrvObj = NULL;
	m_bAutoUpdateSec = TRUE;
	m_bBlockingMode = FALSE;
}

CDDCDrvProtocol::~CDDCDrvProtocol()
{

}

void CDDCDrvProtocol::WriteDebugLog(BOOL nReadMode, BYTE *pb, int nsize)
{
	if (!m_config.m_bDebug || (NULL == m_pDrvObj))
		return;
	
	CString szLog = (nReadMode) ? _T("recv: ") : _T("send: ");
	CString szt;
	for(int i = 0 ; i < nsize ; i++)
	{
		szt.Format("%02X ",pb[i]);
		szLog += szt;
	}
	m_pDrvObj->AddDebug(szLog);
}

//////////////////////////////////////////////////////////////////////
// CSerialProtocol class - 串行口通讯协议
//////////////////////////////////////////////////////////////////////
CSerialProtocol::CSerialProtocol()
{
	m_hComDev = INVALID_HANDLE_VALUE;
	m_nerrnum = 0;
}

CSerialProtocol::~CSerialProtocol()
{
	Disconnect();
}

BOOL CSerialProtocol::IsConnect()
{
	return (m_hComDev != INVALID_HANDLE_VALUE);
}

BOOL CSerialProtocol::Connect()
{
	if (m_hComDev != INVALID_HANDLE_VALUE)
		Disconnect();

	m_hComDev = ::CreateFile(m_config.m_szComPort, 
		GENERIC_READ | GENERIC_WRITE,
		0,                    // exclusive access
		NULL,                 // no security attrs
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL ); 
	if (m_hComDev == INVALID_HANDLE_VALUE)
		return FALSE;//CreateFile通信口失败！
	
	COMMTIMEOUTS  CommTimeOuts ;
	DCB dcb;
	COMMCONFIG cfg;
	
	cfg.dcb.DCBlength = sizeof( DCB ) ;
	BOOL bRet = FALSE;
	bRet = ::GetCommState( m_hComDev, &(cfg.dcb) ) ;
	
	::SetupComm( m_hComDev, 4096, 4096 ) ;
    
	::PurgeComm( m_hComDev, PURGE_TXABORT | PURGE_RXABORT |
		PURGE_TXCLEAR | PURGE_RXCLEAR ) ; //清空缓冲
	dcb.DCBlength = sizeof( DCB ) ;
	
	bRet = ::GetCommState( m_hComDev, &dcb ) ;
	if(!bRet)
	{
		TRACE("GetCommState error!\n");
		return FALSE;
	}
	dcb.BaudRate	= m_config.m_dwBaudRate;
	dcb.ByteSize	= m_config.m_cByteSize;
	dcb.Parity		= m_config.m_cParity;
	dcb.StopBits	= m_config.m_cStopBits;
	
	// setup hardware flow control硬件流控制无
	dcb.fDtrControl = DTR_CONTROL_DISABLE ;// 485转接器用DTR_CONTROL_ENABLE
	dcb.fOutxCtsFlow = FALSE ;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;
	
	// setup software flow control软件流控制无
	dcb.fInX = dcb.fOutX = FALSE ;
	
	// other various settings
	dcb.fBinary = TRUE ;
	dcb.fParity = TRUE ;
	
	// set up for overlapped I/O
	int ntm = (2*9600)/dcb.BaudRate;
	if(ntm <= 0)
		ntm = 1;
	int nReadConstant = m_config.m_nReadTimeout - 128 * ntm;
	if(nReadConstant < 0)
		nReadConstant = 0;
	CommTimeOuts.ReadIntervalTimeout			=	3;//字符间超时3毫秒,
	CommTimeOuts.ReadTotalTimeoutMultiplier		=	ntm;//wzq: 3
	CommTimeOuts.ReadTotalTimeoutConstant		=	nReadConstant;//静态超时 wzq:2
	CommTimeOuts.WriteTotalTimeoutMultiplier	=   ntm;//wzq:3
	CommTimeOuts.WriteTotalTimeoutConstant		=	100;//静态100毫秒
	
	if(!::SetCommTimeouts(m_hComDev , &CommTimeOuts ))
	{
		TRACE("SetCommTimeouts error!\n");
		return FALSE;
	}
	if (!::SetCommState( m_hComDev, &dcb ))
	{
		TRACE("SetCommState error!\n");
		return FALSE;//
	}
	m_nerrnum = 0;
	return TRUE;
}

void CSerialProtocol::Disconnect()
{
	if (m_hComDev != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(m_hComDev);
		m_hComDev = INVALID_HANDLE_VALUE;
	}
}

int	CSerialProtocol::SendData(BYTE *pb, int nsize)
{
	if (!IsConnect())
		return FALSE;

	COMSTAT comstat;
	DWORD dwErrorMask;
	DWORD dwWriteNum = 0;
	BOOL bret = ::WriteFile(
		m_hComDev,      // handle to file
		pb,             // data buffer
		nsize,				// number of bytes to write
		&dwWriteNum,		// number of bytes written
		NULL			// overlapped buffer
		);
	if (!bret)
	{
		::ClearCommError(m_hComDev, &dwErrorMask, &comstat);//清除错误
		::PurgeComm(m_hComDev, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);//清空驱动程序中的缓冲区
		
		m_nerrnum++;
		if(m_nerrnum > 10)
		{
			Disconnect();
			Sleep(50);
			Connect();
			m_nerrnum = 0;
		}
		return bret;
	}
	if(dwWriteNum == 0)
		return FALSE;

	WriteDebugLog(FALSE, pb, nsize);

	m_nerrnum = 0;
	return TRUE;
}

int	CSerialProtocol::ReadData(BYTE *pb, int nsize)
{
	if (!IsConnect())
		return 0;

	BOOL bret;
	DWORD dwReadNum=0;
	COMSTAT comstat;
	DWORD dwErrorMask;
	//if (nsize > BUF_LENGTH_SERIAL) nsize = BUF_LENGTH_SERIAL;//wzq 20091009
	bret = ::ReadFile(m_hComDev, pb, nsize, &dwReadNum, NULL);
	if (!bret)
	{
		::ClearCommError(m_hComDev, &dwErrorMask, &comstat);//清除错误
		::PurgeComm(m_hComDev, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);//清空驱动程序中的缓冲区
		
		m_nerrnum++;
		if(m_nerrnum > 10)
		{
			Disconnect();
			Sleep(50);
			Connect();
			m_nerrnum = 0;
		}
		return 0;
	}
	WriteDebugLog(TRUE, pb, dwReadNum);
	m_nerrnum = 0;
	return (int)dwReadNum;
}

DRVRET  CSerialProtocol::WriteVar(LPDRREC lpval)
{
	return DRERR_NOTSUPPORT;
}

//////////////////////////////////////////////////////////////////////
// CUDPProtocolBase class - UDP通讯协议基类
//////////////////////////////////////////////////////////////////////
CUDPProtocolBase::CUDPProtocolBase()
{
	m_sClient = INVALID_SOCKET; // 本地SOCKET
}

CUDPProtocolBase::~CUDPProtocolBase()
{
	Disconnect();
}

int	CUDPProtocolBase::Read(BYTE *pbuf, int nsize, int ndelaymsec)
{
	int nClientAddrLen = sizeof(m_sockaddr);//客户地址长度
	
	if(m_sClient == INVALID_SOCKET)
		return SOCKET_ERROR;
	TIMEVAL tv01 = {ndelaymsec/1000,1000 *(ndelaymsec%1000)};
	FD_SET fdr;
	fdr.fd_count = 1;
	fdr.fd_array[0] = m_sClient;
	
	int nRet=::select(0, &fdr, NULL, NULL, &tv01);
	if(SOCKET_ERROR == nRet)//发生网络错误
		return SOCKET_ERROR;
	
	if(nRet==0)//超时发生，无可读数据
		return 0;
	
	//有可读数据 
	int nReadLen = ::recvfrom(m_sClient, (char*)pbuf, nsize, 0, &m_sockaddr, &nClientAddrLen);
	if(nReadLen == SOCKET_ERROR)//接受错误
	{
		TRACE("recvfrom Error! ErrCode=%d\n",WSAGetLastError());
		return SOCKET_ERROR;
	}
	else if(nReadLen==0)//连接被关闭
	{
		TRACE("recvfrom Error nReadLen=0\n");
		return SOCKET_ERROR;
	}
	
	return nReadLen;	
}

int	CUDPProtocolBase::Write(BYTE *pbuf, int nsize, int ndelaymsec)
{
	if (!IsConnect())
		return SOCKET_ERROR;
	
	if (SOCKET_ERROR == ::sendto(m_sClient, (LPCTSTR)pbuf, nsize, 0,
		&m_sockaddr, sizeof(struct sockaddr_in)))//发送失败
	{
		return SOCKET_ERROR;
	}
	
	return 1;	
}

BOOL CUDPProtocolBase::IsConnect()
{
	return (m_sClient != INVALID_SOCKET);
}

void CUDPProtocolBase::Disconnect()
{	
	if(m_sClient != INVALID_SOCKET)
	{
		::closesocket(m_sClient);
		m_sClient = INVALID_SOCKET;
	}
}

int CUDPProtocolBase::SendData(BYTE *pb, int nsize)
{	
	int nret = Write(pb, nsize, m_config.m_nWriteTimeout);//400ms延时
	if(nret == -1)
	{
		Disconnect();
		Sleep(50);
		Connect();
		return 0;
	}
	WriteDebugLog(FALSE, pb, nsize);
	return nret;
}

int CUDPProtocolBase::ReadData(BYTE *pb, int nsize)
{
	if (!IsConnect())
		return SOCKET_ERROR;

	int n = Read(pb, nsize, m_config.m_nReadTimeout);
	if(n == -1)
	{
		Disconnect();
		Sleep(50);
		Connect();
		return 0;
	}
	if(n > nsize)
		return 0;

	WriteDebugLog(TRUE, pb, n);

	return n;
}

DRVRET CUDPProtocolBase::WriteVar(LPDRREC lpval)
{
	return DRERR_NOTSUPPORT;
}
//设置接收缓冲区大小
BOOL CUDPProtocolBase::SetRcvBuf(int nSize)
{
	socklen_t optlen;        /* Option length */

	/*
	 * Set socket option SO_RCVBUF:
	 */
	optlen = sizeof nSize;
	int z = setsockopt(m_sClient,SOL_SOCKET,SO_RCVBUF,(char *)&nSize, optlen);
	if (z)
	{
		m_pDrvObj->AddDebugFmt("fail setsockopt SO_RCVBUF");
		return FALSE;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//获取接收缓冲区大小
BOOL CUDPProtocolBase::GetRcvBuf(int &nSize)
{
	socklen_t optlen;        /* Option length */

	/*
	 * Get socket option SO_RCVBUF:
	 */
	optlen = sizeof nSize;
	int z = getsockopt(m_sClient,SOL_SOCKET,SO_RCVBUF,(char *)&nSize,&optlen);
	if (z)
	{
		m_pDrvObj->AddDebugFmt("fail getsockopt SO_RCVBUF");
		return FALSE;
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
// CUDPClientProtocol class - UDP Client通讯协议
//////////////////////////////////////////////////////////////////////
BOOL CUDPClientProtocol::Connect()
{
	Disconnect();

	m_sClient = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (m_sClient==INVALID_SOCKET)
	{
		TRACE("m_sClient==INVALID_SOCKET\n");
		return FALSE;
	}

	struct sockaddr_in *pa;

	//填写本地sockaddr
	pa = (sockaddr_in *)(&m_sockaddr);
	pa->sin_family = AF_INET;
	pa->sin_port = ::htons(u_short(m_config.m_wPort));
	pa->sin_addr.s_addr = inet_addr(m_config.m_szIP);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CUDPServerProtocol class - UDP Server通讯协议
//////////////////////////////////////////////////////////////////////
CUDPServerProtocol::CUDPServerProtocol()
: CUDPProtocolBase()
{
	m_bAutoUpdateSec = FALSE; 
}

BOOL CUDPServerProtocol::Connect()
{
	Disconnect();

	m_sClient = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (m_sClient==INVALID_SOCKET)
	{
		TRACE("m_sClient==INVALID_SOCKET\n");
		return FALSE;
	}

	struct sockaddr_in *pa;
	struct sockaddr localaddr;

	//填写本地sockaddr
	pa = (sockaddr_in *)(&localaddr);
	pa->sin_family = AF_INET;
	pa->sin_port = ::htons(u_short(m_config.m_wPort));
	pa->sin_addr.s_addr = htonl(INADDR_ANY);//inet_addr("127.0.0.1");////所有地址

	//绑定端口
	if(SOCKET_ERROR == ::bind(m_sClient, (struct sockaddr *)(&(localaddr)),sizeof(SOCKADDR)))
	{
		CString szMsg;
		szMsg.Format("UDP port %d bind error! Please check net fireware!\n", m_config.m_wPort);
		//AfxMessageBox(szMsg);
		TRACE(szMsg);
		return FALSE;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CUDPProtocolBaseEx class - 阻塞方式的UDP通讯协议基类
//////////////////////////////////////////////////////////////////////
CUDPProtocolBaseEx::CUDPProtocolBaseEx()
{
	m_sClient = INVALID_SOCKET; // 本地SOCKET
	m_bBlockingMode = TRUE;
}

CUDPProtocolBaseEx::~CUDPProtocolBaseEx()
{
	Disconnect();
}

int	CUDPProtocolBaseEx::Read(BYTE *pbuf, int nsize, int ndelaymsec)
{
	int nClientAddrLen = sizeof(m_sockaddr);//客户地址长度

	if(m_sClient == INVALID_SOCKET)
		return SOCKET_ERROR;

	//有可读数据 
	int nReadLen = ::recvfrom(m_sClient, (char*)pbuf, nsize, 0, &m_sockaddr, &nClientAddrLen);
	if(nReadLen == SOCKET_ERROR)//接受错误
	{
		TRACE("recvfrom Error! ErrCode=%d\n",WSAGetLastError());
		return SOCKET_ERROR;
	}
	else if(nReadLen==0)//连接被关闭
	{
		TRACE("recvfrom Error nReadLen=0\n");
		return SOCKET_ERROR;
	}

	return nReadLen;	
}

int	CUDPProtocolBaseEx::Write(BYTE *pbuf, int nsize, int ndelaymsec)
{
	if (!IsConnect())
		return SOCKET_ERROR;

	if (SOCKET_ERROR == ::sendto(m_sClient, (LPCTSTR)pbuf, nsize, 0,
		&m_sockaddr, sizeof(struct sockaddr_in)))//发送失败
	{
		return SOCKET_ERROR;
	}

	return 1;	
}

BOOL CUDPProtocolBaseEx::IsConnect()
{
	return (m_sClient != INVALID_SOCKET);
}

void CUDPProtocolBaseEx::Disconnect()
{	
	if(m_sClient != INVALID_SOCKET)
	{
		::closesocket(m_sClient);
		m_sClient = INVALID_SOCKET;
	}
}

int CUDPProtocolBaseEx::SendData(BYTE *pb, int nsize)
{	
	int nret = Write(pb, nsize, m_config.m_nWriteTimeout);//400ms延时
	if(nret == -1)
	{
		Disconnect();
		Sleep(50);
		Connect();
		return 0;
	}
	WriteDebugLog(FALSE, pb, nsize);
	return nret;
}

int CUDPProtocolBaseEx::ReadData(BYTE *pb, int nsize)
{
	if (!IsConnect())
		return SOCKET_ERROR;

	int n = Read(pb, nsize, m_config.m_nReadTimeout);
	if(n == -1)
	{
		Disconnect();
		Sleep(50);
		Connect();
		return 0;
	}
	if(n > nsize)
		return 0;

	WriteDebugLog(TRUE, pb, n);

	return n;
}

DRVRET CUDPProtocolBaseEx::WriteVar(LPDRREC lpval)
{
	return DRERR_NOTSUPPORT;
}



//设置接收缓冲区大小
BOOL CUDPProtocolBaseEx::SetRcvBuf(int nSize)
{
	socklen_t optlen;        /* Option length */

	/*
	 * Set socket option SO_RCVBUF:
	 */
	optlen = sizeof nSize;
	int z = setsockopt(m_sClient,SOL_SOCKET,SO_RCVBUF,(char *)&nSize, sizeof nSize);
	if (z)
	{
		m_pDrvObj->AddDebugFmt("fail setsockopt SO_RCVBUF");
		return FALSE;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//获取接收缓冲区大小
BOOL CUDPProtocolBaseEx::GetRcvBuf(int &nSize)
{
	socklen_t optlen;        /* Option length */

	/*
	 * Get socket option SO_RCVBUF:
	 */
	optlen = sizeof nSize;
	int z = getsockopt(m_sClient,SOL_SOCKET,SO_RCVBUF,(char *)&nSize,&optlen);
	if (z)
	{
		m_pDrvObj->AddDebugFmt("fail getsockopt SO_RCVBUF");
		return FALSE;
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
// CUDPClientProtocolEx class - 阻塞方式的UDP Client通讯协议
//////////////////////////////////////////////////////////////////////
BOOL CUDPClientProtocolEx::Connect()
{
	Disconnect();
	
	m_sClient = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (m_sClient==INVALID_SOCKET)
	{
		TRACE("m_sClient==INVALID_SOCKET\n");
		return FALSE;
	}
	
	struct sockaddr_in *pa;
	
	//填写本地sockaddr
	pa = (sockaddr_in *)(&m_sockaddr);
	pa->sin_family = AF_INET;
	pa->sin_port = ::htons(u_short(m_config.m_wPort));
	pa->sin_addr.s_addr = inet_addr(m_config.m_szIP);
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CUDPServerProtocolEx class - 阻塞方式的UDP Server通讯协议
//////////////////////////////////////////////////////////////////////
CUDPServerProtocolEx::CUDPServerProtocolEx()
: CUDPProtocolBaseEx()
{
	m_bAutoUpdateSec = FALSE; 
}

BOOL CUDPServerProtocolEx::Connect()
{
	Disconnect();
	
	m_sClient = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (m_sClient==INVALID_SOCKET)
	{
		TRACE("m_sClient==INVALID_SOCKET\n");
		return FALSE;
	}
	
	struct sockaddr_in *pa;
	struct sockaddr localaddr;
	
	//填写本地sockaddr
	pa = (sockaddr_in *)(&localaddr);
	pa->sin_family = AF_INET;
	pa->sin_port = ::htons(u_short(m_config.m_wPort));
	pa->sin_addr.s_addr = htonl(INADDR_ANY);//inet_addr("127.0.0.1");////所有地址
	
	//绑定端口
	if(SOCKET_ERROR == ::bind(m_sClient, (struct sockaddr *)(&(localaddr)),sizeof(SOCKADDR)))
	{
		CString szMsg;
		szMsg.Format("UDP port %d bind error! Please check net fireware!\n", m_config.m_wPort);
		//AfxMessageBox(szMsg);
		TRACE(szMsg);
		return FALSE;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CTCPClientProtocol class - TCP Client通讯协议
//////////////////////////////////////////////////////////////////////

CTCPClientProtocol::CTCPClientProtocol()
{
	m_sClient = INVALID_SOCKET;
}

CTCPClientProtocol::~CTCPClientProtocol()
{
	Disconnect();
}

// ndelaysec超时秒数
// 返回读的字节数,-1发生严重错
int	CTCPClientProtocol::Read(SOCKET s, BYTE *pbuf, int nsize, int ndelaymsec)
{
	TIMEVAL tv01 = {ndelaymsec/1000,1000 *(ndelaymsec%1000)};
	FD_SET fdr = {1, s};
	int nRet;
	int  nfrmlen=0;
	
	nRet = ::select(0, &fdr, NULL, NULL, &tv01);
	if(SOCKET_ERROR == nRet)//发生网络错误
	{
		m_pDrvObj->AddDebugFmt(_T("select()调用失败，错误码为%d"), WSAGetLastError());
		return -1;
	}
	if(nRet == 0)//超时发生，无可读数据
	{
		TRACE("Read timeout\n");
		return 0;
	}
	
	nfrmlen = ::recv(s, (char *)pbuf, nsize , 0);
	if(SOCKET_ERROR == nfrmlen)//接受错误
	{
		m_pDrvObj->AddDebugFmt(_T("recv()调用失败，错误码为%d"), WSAGetLastError());
		return -1;
	}
	if(nfrmlen == 0)//连接被关闭
		return -1;
	
	if(nfrmlen > nsize)
		return 0;

	return nfrmlen;
}

// ndelaysec超时秒数
// 返回发送字节数,发生严重错误时返回 -1；
int	CTCPClientProtocol::Write(SOCKET s, BYTE *pbuf, int nsize, int ndelaymsec)
{
	TIMEVAL tv01 = {ndelaymsec/1000,1000 * (ndelaymsec%1000)};//1秒钟延迟
	FD_SET fdw = {1, s};
	
	if(1 != ::select(0, NULL, &fdw, NULL, &tv01))//测试可写
	{
		return  0;
	}
	
	int nret = ::send(s, (char*)pbuf, nsize,0);
	if(SOCKET_ERROR == nret)//发送错误
		return -1;
	
	return nret;
}
	
BOOL CTCPClientProtocol::IsConnect()
{
	return (m_sClient != INVALID_SOCKET);
}

BOOL CTCPClientProtocol::Connect()
{	
	int nRet;
	
	Disconnect();
	unsigned long ul = 1;//非阻塞方式
	struct sockaddr_in sServer;
	
	sServer.sin_family=AF_INET;
	sServer.sin_port=::htons(m_config.m_wPort);
	sServer.sin_addr.s_addr=::inet_addr(m_config.m_szIP);
	
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
	return TRUE;
}

void CTCPClientProtocol::Disconnect()
{
	if(m_sClient != INVALID_SOCKET)
	{
		::closesocket(m_sClient);
		m_sClient = INVALID_SOCKET;
	}
}

int	CTCPClientProtocol::SendData(BYTE *pb,int nsize)
{
// 	if(nsize > BUF_LENGTH_TCP)
// 		return 0;
		
	int nret = Write(m_sClient, pb, nsize, m_config.m_nWriteTimeout); //400ms延时
	if(nret == -1)
	{
		Disconnect();
		return 0;
	}
	WriteDebugLog(FALSE, pb, nret);
	return nret;
}

int	CTCPClientProtocol::ReadData(BYTE *pb, int nsize)
{
	int n = Read(m_sClient, pb, nsize, m_config.m_nReadTimeout);
	if(n == -1)
	{
		Disconnect();
		return 0;
	}
	if(n > nsize)
		return 0;

	WriteDebugLog(TRUE, pb, n);
	return n;
}

DRVRET CTCPClientProtocol::WriteVar(LPDRREC lpval)
{
	return DRERR_NOTSUPPORT;
}

//////////////////////////////////////////////////////////////////////
// CTCPClientProtocolEx class - 阻塞TCP Client通讯协议
//////////////////////////////////////////////////////////////////////

CTCPClientProtocolEx::CTCPClientProtocolEx()
{
	m_sClient = INVALID_SOCKET;
	m_bBlockingMode = TRUE;
}

CTCPClientProtocolEx::~CTCPClientProtocolEx()
{
	Disconnect();
}

// ndelaysec超时秒数
// 返回读的字节数,-1发生严重错
int	CTCPClientProtocolEx::Read(SOCKET s, BYTE *pbuf, int nsize, int ndelaymsec)
{
	int  nfrmlen=0;

	nfrmlen = ::recv(s, (char *)pbuf, nsize , 0);
	if(SOCKET_ERROR == nfrmlen)//接受错误
	{
		m_pDrvObj->AddDebugFmt(_T("recv()调用失败，错误码为%d"), WSAGetLastError());
		return -1;
	}
	if(nfrmlen == 0)//连接被关闭
		return -1;

	if(nfrmlen > nsize)
		return 0;

	return nfrmlen;
}

// ndelaysec超时秒数
// 返回发送字节数,发生严重错误时返回 -1；
int	CTCPClientProtocolEx::Write(SOCKET s, BYTE *pbuf, int nsize, int ndelaymsec)
{
	int nret = ::send(s, (char*)pbuf, nsize,0);
	if(SOCKET_ERROR == nret)//发送错误
		return -1;

	return nret;
}

BOOL CTCPClientProtocolEx::IsConnect()
{
	return (m_sClient != INVALID_SOCKET);
}

BOOL CTCPClientProtocolEx::Connect()
{
	Disconnect();
	struct sockaddr_in sServer;

	sServer.sin_family=AF_INET;
	sServer.sin_port=::htons(m_config.m_wPort);
	sServer.sin_addr.s_addr=::inet_addr(m_config.m_szIP);

	m_sClient = ::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (m_sClient==INVALID_SOCKET)
		return 1;

	int nRet = ::connect(m_sClient,(const struct sockaddr *)&sServer,sizeof(sServer));

	if(nRet != 0)
	{
		m_pDrvObj->AddDebugFmt(_T("connect()调用失败，错误码为%d"), WSAGetLastError());
		Disconnect();
		return  FALSE;
	}

	return TRUE;
}

void CTCPClientProtocolEx::Disconnect()
{
	if(m_sClient != INVALID_SOCKET)
	{
		::closesocket(m_sClient);
		m_sClient = INVALID_SOCKET;
	}
}

int	CTCPClientProtocolEx::SendData(BYTE *pb,int nsize)
{
	// 	if(nsize > BUF_LENGTH_TCP)
	// 		return 0;

	int nret = Write(m_sClient, pb, nsize, m_config.m_nWriteTimeout); //400ms延时
	if(nret == -1)
	{
		Disconnect();
		return 0;
	}
	WriteDebugLog(FALSE, pb, nret);
	return nret;
}

int	CTCPClientProtocolEx::ReadData(BYTE *pb, int nsize)
{
	int n = Read(m_sClient, pb, nsize, m_config.m_nReadTimeout);
	if(n == -1)
	{
		Disconnect();
		return 0;
	}
	if(n > nsize)
		return 0;

	WriteDebugLog(TRUE, pb, n);
	return n;
}

DRVRET CTCPClientProtocolEx::WriteVar(LPDRREC lpval)
{
	return DRERR_NOTSUPPORT;
}

//////////////////////////////////////////////////////////////////////
// CTCPServerProtocol class - TCP Server通讯协议
//////////////////////////////////////////////////////////////////////

CTCPServerProtocol::CTCPServerProtocol()
{
	m_hSocket = INVALID_SOCKET;
	m_hConnect = INVALID_SOCKET;

	m_bRun = FALSE;	
	m_nConn  = 0 ;

	// 由用户自己处理更新周期
	m_bAutoUpdateSec = FALSE; 
}

CTCPServerProtocol::~CTCPServerProtocol()
{
	Disconnect();
}

BOOL CTCPServerProtocol::IsAccept()
{
	return (m_hConnect != INVALID_SOCKET);
}

BOOL CTCPServerProtocol::ListenClient()
{
	CString szErr;
	if(SOCKET_ERROR == ::listen(m_hSocket, 1)) 
	{
		szErr.Format("监听实时数据服务端口[%d]失败! ErrCode = %d", m_config.m_wPort, WSAGetLastError());
		return FALSE;
	}
	//////////////////////////////////////////////////////////////////////////
	//用select超时方式Accept
	int	nClientAddrLen = sizeof(SOCKADDR);//客户地址长度
	
	
	TIMEVAL tv01 = {0, 1000 * 100}; //100ms
	FD_SET fdr;
	
	int nRet;
	DWORD dwkkk=0;
	
	fdr.fd_count = 1;
	fdr.fd_array[0] = m_hSocket;
	nRet = ::select(0, &fdr, NULL, NULL, &tv01); //select返回时要修改fdr中的个数
	
	m_hConnect = ::accept(m_hSocket, (struct sockaddr *)(&m_addrClient), &nClientAddrLen);
	
	if(INVALID_SOCKET == m_hConnect)
		return FALSE;
	
	m_nConn = 1;

	return TRUE;
}

void CTCPServerProtocol::DisconnectClient()
{
	if(m_hConnect != INVALID_SOCKET)
	{
		::closesocket(m_hConnect);
		m_hConnect = INVALID_SOCKET;
		m_nConn = 0;
	}
}

/*
TCP读
参数:
pbuf		:接受缓冲区
nsize	:缓冲区长度
ntimeout:超时，毫秒数
返回值：
>0	:数据字节数
-1	:网络错误
*/
int	CTCPServerProtocol::Read(SOCKET s, BYTE *pbuf, int nsize, int ndelaymsec)
{
	TIMEVAL tv01 = {ndelaymsec / 1000, 1000 *(ndelaymsec % 1000)};
	FD_SET fdr;
	fdr.fd_count = 1;
	fdr.fd_array[0] = s;
	
	int nRet = ::select(0, &fdr, NULL, NULL, &tv01);
	if(SOCKET_ERROR == nRet)//发生网络错误
		return -1;
	
	if(nRet==0)//超时发生，无可读数据
		return 0;
	
	nRet = ::recv(s, (char*)pbuf, nsize , 0);
	if(nRet == 0)//连接关闭
		return -1;

	return nRet;	
}

/*
TCP发送
参数:
pbuf		:发送缓冲区
nsize	:发送字节数
ntimeout:超时，毫秒数
返回值：
0	:超时，网络忙
-1	:网络错误
*/
int	CTCPServerProtocol::Write(SOCKET s, BYTE *pbuf, int nsize, int ndelaymsec)
{
	TIMEVAL tv01 = {ndelaymsec / 1000, 1000 * (ndelaymsec % 1000)};//1秒钟延迟
	FD_SET fdw;
	fdw.fd_count = 1;
	fdw.fd_array[0] = s;
	
	int nRet = ::select(0, NULL, &fdw, NULL, &tv01);//测试可写
	if(SOCKET_ERROR == nRet)//发生网络错误
		return -1;
	
	if(nRet==0)//超时发生，不能发送数据
		return 0;
	nRet = ::send(s, (char*)pbuf,nsize,0);
	return nRet;
}

BOOL CTCPServerProtocol::IsConnect()
{
	return (m_hSocket != INVALID_SOCKET);
}

BOOL CTCPServerProtocol::Connect()
{
	CString szErr;
	if(m_bRun)
		return TRUE;
	
	m_HostAddr.sin_family = AF_INET;
	m_HostAddr.sin_port = ::htons(m_config.m_wPort);
	m_HostAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	m_hSocket = ::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	
	if(m_hSocket == INVALID_SOCKET)
	{
		szErr = "不能创建实时数据服务SOCKET,可能该端口被使用!";
		return FALSE;
	}
	
	if(SOCKET_ERROR==::bind(m_hSocket,(struct sockaddr *)(&(m_HostAddr)),sizeof(SOCKADDR)))
	{
		szErr.Format("绑定实时数据服务端口[%d]失败! ErrCode = %d", m_config.m_wPort, WSAGetLastError());
		return FALSE;
	}
	
	m_bRun = TRUE;
	return TRUE;
}

void CTCPServerProtocol::Disconnect()
{
	if(m_hSocket != INVALID_SOCKET)
	{
		::closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}

	DisconnectClient();

	m_bRun = FALSE;
	m_nConn  = 0 ;
}

int	CTCPServerProtocol::SendData(BYTE *pb, int nsize)
{
	if(nsize > BUF_LENGTH_TCP)
		return 0;	
	
	int nret = Write(m_hConnect, pb, nsize, m_config.m_nWriteTimeout);//400ms延时
	if(nret == -1)
	{
		Disconnect();
		return 0;
	}
	WriteDebugLog(FALSE, pb, nsize);
	return nret;
}

int	CTCPServerProtocol::ReadData(BYTE *pb, int nsize)
{
	int n = Read(m_hConnect, pb, nsize, m_config.m_nReadTimeout);
	if(n == -1)
	{
		Disconnect();
		return 0;
	}
	if(n > nsize)
		return 0;

	WriteDebugLog(TRUE, pb, n);

	return n;	
}

DRVRET CTCPServerProtocol::WriteVar(LPDRREC lpval)
{
	return DRERR_NOTSUPPORT;
}

//////////////////////////////////////////////////////////////////////
// CTCPServerProtocolEx class - 阻塞方式的TCP Server通讯协议
//////////////////////////////////////////////////////////////////////

CTCPServerProtocolEx::CTCPServerProtocolEx()
{
	m_hSocket = INVALID_SOCKET;
	m_hConnect = INVALID_SOCKET;

	m_bRun = FALSE;	
	m_nConn  = 0 ;

	// 由用户自己处理更新周期
	m_bAutoUpdateSec = FALSE;
	m_bBlockingMode = TRUE;
}

CTCPServerProtocolEx::~CTCPServerProtocolEx()
{
	Disconnect();
}

BOOL CTCPServerProtocolEx::IsAccept()
{
	return (m_hConnect != INVALID_SOCKET);
}

BOOL CTCPServerProtocolEx::ListenClient()
{
	CString szErr;
	if(SOCKET_ERROR == ::listen(m_hSocket, 1)) 
	{
		szErr.Format("监听实时数据服务端口[%d]失败! ErrCode = %d", m_config.m_wPort, WSAGetLastError());
		return FALSE;
	}
	//////////////////////////////////////////////////////////////////////////
	//用select超时方式Accept
	int	nClientAddrLen = sizeof(SOCKADDR);//客户地址长度

	m_hConnect = ::accept(m_hSocket, (struct sockaddr *)(&m_addrClient), &nClientAddrLen);

	if(INVALID_SOCKET == m_hConnect)
		return FALSE;

	m_nConn = 1;

	return TRUE;
}

void CTCPServerProtocolEx::DisconnectClient()
{
	if(m_hConnect != INVALID_SOCKET)
	{
		::closesocket(m_hConnect);
		m_hConnect = INVALID_SOCKET;
		m_nConn = 0;
	}
}

/*
TCP读
参数:
pbuf		:接受缓冲区
nsize	:缓冲区长度
ntimeout:超时，毫秒数
返回值：
>0	:数据字节数
-1	:网络错误
*/
int	CTCPServerProtocolEx::Read(SOCKET s, BYTE *pbuf, int nsize, int ndelaymsec)
{
	int nRet = ::recv(s, (char*)pbuf, nsize , 0);
	if(nRet == 0)//连接关闭
		return -1;

	return nRet;	
}

/*
TCP发送
参数:
pbuf		:发送缓冲区
nsize	:发送字节数
ntimeout:超时，毫秒数
返回值：
0	:超时，网络忙
-1	:网络错误
*/
int	CTCPServerProtocolEx::Write(SOCKET s, BYTE *pbuf, int nsize, int ndelaymsec)
{
	int nRet = ::send(s, (char*)pbuf,nsize,0);
	return nRet;
}

BOOL CTCPServerProtocolEx::IsConnect()
{
	return (m_hSocket != INVALID_SOCKET);
}

BOOL CTCPServerProtocolEx::Connect()
{
	CString szErr;
	if(m_bRun)
		return TRUE;

	m_HostAddr.sin_family = AF_INET;
	m_HostAddr.sin_port = ::htons(m_config.m_wPort);
	m_HostAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	m_hSocket = ::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	if(m_hSocket == INVALID_SOCKET)
	{
		szErr = "不能创建实时数据服务SOCKET,可能该端口被使用!";
		return FALSE;
	}

	if(SOCKET_ERROR==::bind(m_hSocket,(struct sockaddr *)(&(m_HostAddr)),sizeof(SOCKADDR)))
	{
		szErr.Format("绑定实时数据服务端口[%d]失败! ErrCode = %d", m_config.m_wPort, WSAGetLastError());
		return FALSE;
	}

	m_bRun = TRUE;
	return TRUE;
}

void CTCPServerProtocolEx::Disconnect()
{
	if(m_hSocket != INVALID_SOCKET)
	{
		::closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}

	DisconnectClient();

	m_bRun = FALSE;
	m_nConn  = 0 ;
}

int	CTCPServerProtocolEx::SendData(BYTE *pb, int nsize)
{
	if(nsize > BUF_LENGTH_TCP)
		return 0;	

	int nret = Write(m_hConnect, pb, nsize, m_config.m_nWriteTimeout);//400ms延时
	if(nret == -1)
	{
		Disconnect();
		return 0;
	}
	WriteDebugLog(FALSE, pb, nsize);
	return nret;
}

int	CTCPServerProtocolEx::ReadData(BYTE *pb, int nsize)
{
	int n = Read(m_hConnect, pb, nsize, m_config.m_nReadTimeout);
	if(n == -1)
	{
		Disconnect();
		return 0;
	}
	if(n > nsize)
		return 0;

	WriteDebugLog(TRUE, pb, n);

	return n;	
}

DRVRET CTCPServerProtocolEx::WriteVar(LPDRREC lpval)
{
	return DRERR_NOTSUPPORT;
}

//////////////////////////////////////////////////////////////////////
// CODBCProtocol class - ODBC
//////////////////////////////////////////////////////////////////////
/*
CODBCProtocol::CODBCProtocol()
{
	
}

CODBCProtocol::~CODBCProtocol()
{
	Disconnect();
}


BOOL CODBCProtocol::Connect()
{
	if (m_db.IsOpen())
		m_db.Close();

	CString szConnect = "";
	szConnect.Format("Driver={SYBASE SYSTEM 11};Srvr=%s;Uid=%s;Pwd=%s",this->m_config.m_szDataSource,
		this->m_config.m_szID,this->m_config.m_szPsw);
//	szConnect.Format("DSN=%s;Uid=%s;Pwd=%s",this->m_config.m_szDataSource,
//		this->m_config.m_szID,this->m_config.m_szPsw);
	m_db.SetLoginTimeout(8); // 登录秒超时，默认15s
	BOOL bret = FALSE;
	try
	{
		bret = m_db.OpenEx(szConnect,CDatabase::noOdbcDialog); 
		m_db.IsOpen();
	}
	catch (CDBException *e)
	{
		CString szt="ODBC连接错误:";
		szt+=e->m_strError;
		e->Delete();
		TRACE("%s\n",szt);
		return FALSE;
	}
	return bret;
}

void CODBCProtocol::Disconnect()
{
	if(m_db.IsOpen())
		m_db.Close();
}

BOOL CODBCProtocol::IsNeedReConnect(const CString& szStatus)
{
	//取index为6-10
	CString szval = szStatus.Left(11);
	szval = szval.Right(5);
	
// 	08001	无法连接到数据源,一般在Open DB时的错误信息
// 	08003	连接未打开
// 	08007	在执行事务的过程中连接失败
// 	08S01	通讯链接失败，一般在打开记录集或调用直接执行SQL语句时出错。
	
	if((szval == "08S01")||
		(szval == "08001")||
		(szval == "08003")||
		(szval == "08007")
		)
		return TRUE;
	return FALSE;
}
*/
//////////////////////////////////////////////////////////////////////
// CDDCDrvObj class
//////////////////////////////////////////////////////////////////////

CDDCDrvObj::CDDCDrvObj()
{
	m_dwSerializeID = 81022803;
	m_wComTypes = COM_TYPE_COM | COM_TYPE_UDP | COM_TYPE_UDPSERV | COM_TYPE_TCP | COM_TYPE_TCPSERV | COM_TYPE_DB;
	m_pProtocol = NULL;
	m_config.m_pdrv = this;
	m_bCanTestConnect = FALSE;
	m_bIgnoreCase = FALSE;
	m_bConnectionFailedStartup = FALSE;

	OnInitConfig();
}

CDDCDrvObj::~CDDCDrvObj()
{

}

DRVRET  CDDCDrvObj::GetErrMsg(CString& szMsg, DRVRET dwcode)			//获取错误信息
{
	if(dwcode < 100)
		return CDrvObj::GetErrMsg(szMsg, dwcode);

	//>100是驱动自定义的，这里没有自定义错误信息，直接返回无此错误代码
	return DRERR_NOERRCODE;
}

DWORD CDDCDrvObj::OnDataOk(	//数据回调函数
			   T_DRREC  recs[],		//记录集
			   int nrecs			//记录个数
			   )
{
	if (NULL != m_cbOnDataOk)
		return m_cbOnDataOk(m_handle, recs, nrecs);
	else
		return 0;
}
/*
DWORD CDDCDrvObj::OnLogOk( //日志回调函数
			  const char *plog			//日志字符串，0结束的字符串
			  )
{
	if (NULL != m_cbOnLogOk)
		return m_cbOnLogOk(m_handle, ltime, plog);
	else
		return 0;
}
*/

DWORD CDDCDrvObj::OnSoeOk( //SOE事件回调函数,每次回调一个事件
			  LPSOE psoe			//事件
		)
{	
	if (NULL != m_cbOnSoeOk)
		return m_cbOnSoeOk(m_handle, psoe);
	else
		return 0;
}

DRVRET  CDDCDrvObj::WriteVar(LPDRREC lpval)			//写变量
{
	if (m_pProtocol == NULL)
		return DRERR_CREATE;

	return m_pProtocol->WriteVar(lpval);
}

DRVRET  CDDCDrvObj::GetTagDef(int nindex,LPDRVTAG  lptag)
{
	CTagItem tag;
	if(m_tagtab.GetAt(nindex,&tag))
	{
		tag.CopyToDrTag(lptag);
		return DRERR_OK;
	}

	return DRERR_NOMORETAG;
}


void CDDCDrvObj::OnInitConfig()				// 初始化配置信息
{
	// 初始化m_config
}
				
BOOL CDDCDrvObj::OnConfig()					//配置和配置界面,返回TRUE表示修改了配置
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); //资源切换

	OnBeginConfig();

	CCfgDlg dlg;
	dlg.m_pDrvObj = this;
	dlg.m_wComTypes = m_wComTypes;
	TRACE("CDDCDrvObj::OnConfig() dlg.m_wComTypes = m_wComTypes%d\n",m_wComTypes);
	dlg.m_config = m_config;
	m_tagtab.CopyTo(dlg.m_tagtab, TRUE);
	dlg.m_tagtab.SetModified(FALSE);

	if (dlg.DoModal() == IDOK)
	{
		m_config = dlg.m_config;
		if (dlg.m_tagtab.IsModified())
		{
			dlg.m_tagtab.CopyTo(m_tagtab, FALSE);
			dlg.m_tagtab.GetSortObArray().RemoveAll();
			dlg.m_tagtab.GetTagItemMap().RemoveAll();
			m_tagtab.SetModified(TRUE);
		}
		
		OnEndConfig();

		if(m_config.IsModified() || m_tagtab.IsModified())
			return TRUE; //修改了
	}

	return FALSE;
}

void CDDCDrvObj::OnBeginConfig()
{

}

void CDDCDrvObj::OnEndConfig()
{

}

void CDDCDrvObj::Serialize(CArchive& ar)	//串行化保存，读取配置
{
	if(ar.IsLoading())
	{
		DWORD dwFlag = 0;
		ar >> dwFlag;
		if(dwFlag != m_dwSerializeID)
			return;
		
		// 增加版本号信息
		ar >> m_wVersion;
		if (m_wVersion != DRVCFG_VERSION)
			return;		

		m_config.Serialize(ar, m_wVersion);
		m_tagtab.Serialize(ar, m_wVersion);
	}
	else
	{
		ar << m_dwSerializeID;		
		// 增加版本号信息
		m_wVersion = DRVCFG_VERSION;
		ar << m_wVersion;

		m_config.Serialize(ar, m_wVersion);
		m_tagtab.Serialize(ar, m_wVersion);
	}
}

DRVRET CDDCDrvObj::OnStart()					//准备运行
{
	if (m_pProtocol != NULL)
		OnStop();

	m_nRecCount = 0;

	// 创建通讯协议处理类实例
	m_pProtocol = CreateProtocol();	
	if (m_pProtocol == NULL)
		return DRERR_CREATE;

	// 设置通讯协议是否为阻塞模式
	SetBlockingMode( m_pProtocol->GetBlockingMode() );

	// 设置协议处理类实例的相关参数
	m_pProtocol->SetConfig(m_config);
	m_pProtocol->m_pDrvObj = this;
	if (m_pProtocol->Connect())
		return DRERR_OK;
	else {
		if (m_bConnectionFailedStartup)
		{
			AddError(_T("驱动连接失败，但已启动并自动尝试连接"));
			return DRERR_OK;
		}
		else
			return DRERR_CONFIGDATA;
	}
}

void CDDCDrvObj::OnStop()					//停止运行后的清理工作
{
	if (m_pProtocol != NULL)
	{
		m_pProtocol->Disconnect();
		delete m_pProtocol;
		m_pProtocol = NULL;
	}
}

void CDDCDrvObj::OnDisconnect()
{
	if (m_pProtocol != NULL)
		m_pProtocol->Disconnect();
}

void CDDCDrvObj::InitTagIdMap(BOOL bIgnoreCase/* = FALSE*/)
{
	m_mapTagIDs.RemoveAll();

	int nHashSize = ::CalcHashTableSize(m_tagtab.GetSize());
	m_mapTagIDs.InitHashTable(nHashSize);

	CString szID;
	for(int i = 0; i < m_tagtab.GetSize(); i++)
	{
		CTagItem *pItem = m_tagtab.GetTag(i);

		if (bIgnoreCase)
		{
			// 如果忽略大小写，则所有Key值转换为小写
			szID = pItem->m_szID;
			szID.MakeLower();
			m_mapTagIDs.SetAt(szID, pItem);
		}
		else
			m_mapTagIDs.SetAt(pItem->m_szID, pItem);
	}
}

CTagItem *CDDCDrvObj::FindByID(LPCTSTR lpszID)  // 根据标签ID名称从Map表中快速查找标签
{
	CTagItem *pItem = NULL;

	if (m_bIgnoreCase)
	{
		CString szID(lpszID);
		szID.MakeLower();

		if (m_mapTagIDs.Lookup(szID, pItem))
			return pItem;
		else
			return NULL;
	}
	else {
		if (m_mapTagIDs.Lookup(lpszID, pItem))
			return pItem;
		else
			return NULL;
	}
}

BOOL CDDCDrvObj::AddValueByID(LPCTSTR lpszID, long ltime, short snqa, long val)
{
	CTagItem *pItem = NULL;
	if (!m_mapTagIDs.Lookup(lpszID, pItem))
		return FALSE;
	
	m_recs[m_nRecCount].SetName(pItem->m_szName);
	m_recs[m_nRecCount].ReSetType(DT_VOID);//清空类型
	m_recs[m_nRecCount].val.vt = pItem->m_wType;
	m_recs[m_nRecCount].val.ltime = GetValueTimestmap(ltime);
	m_recs[m_nRecCount].val.snqa = snqa;
	m_recs[m_nRecCount].val.snerr = 0;
	switch(pItem->m_wType)
	{		
	case DRT_DIGITAL:		
	case DRT_INT32:
		m_recs[m_nRecCount].val.lval = val;
		break;
	case DRT_FLOAT32:
		m_recs[m_nRecCount].val.fval = (float)val;
		break;
	case DRT_INT64:
		m_recs[m_nRecCount].val.llval = (__int64)val;
		break;
	case DRT_FLOAT64:
		m_recs[m_nRecCount].val.dblval = (double)val;
		break;
	case DRT_STRING: {
		CString szValue;
		szValue.Format(_T("%d"), val);
		*((CVarVal*)(&m_recs[m_nRecCount].val)) = szValue;
		break;
	}
	case DRT_BLOB: {
		CString szValue;
		szValue.Format(_T("%d"), val);
		((CVarVal*)(&m_recs[m_nRecCount].val))->SetBlob((void*)szValue.GetBuffer(szValue.GetLength()), szValue.GetLength()+1);
		szValue.ReleaseBuffer();
		break;
	}
	}

	m_nRecCount++;
	if (UPDATEVALTAGS == m_nRecCount)
		FlushValue();

	return TRUE;
}

BOOL CDDCDrvObj::AddValueByID(LPCTSTR lpszID, long ltime, short snqa, __int64 val)
{
	CTagItem *pItem = NULL;
	if (!m_mapTagIDs.Lookup(lpszID, pItem))
		return FALSE;
	
	m_recs[m_nRecCount].SetName(pItem->m_szName);
	m_recs[m_nRecCount].ReSetType(DT_VOID);//清空类型
	m_recs[m_nRecCount].val.vt = pItem->m_wType;
	m_recs[m_nRecCount].val.ltime = GetValueTimestmap(ltime);
	m_recs[m_nRecCount].val.snqa = snqa;
	m_recs[m_nRecCount].val.snerr = 0;
	switch(pItem->m_wType)
	{		
	case DRT_DIGITAL:		
	case DRT_INT32:
		m_recs[m_nRecCount].val.lval = (long)val;
		break;
	case DRT_FLOAT32:
		m_recs[m_nRecCount].val.fval = (float)val;
		break;
	case DRT_INT64:
		m_recs[m_nRecCount].val.llval = (__int64)val;
		break;
	case DRT_FLOAT64:
		m_recs[m_nRecCount].val.dblval = (double)val;
		break;
	case DRT_STRING: {
		CString szValue;
		szValue.Format(_T("%I64"), val);
		*((CVarVal*)(&m_recs[m_nRecCount].val)) = szValue;
		break;
					 }
	case DRT_BLOB: {
		CString szValue;
		szValue.Format(_T("%I64"), val);
		((CVarVal*)(&m_recs[m_nRecCount].val))->SetBlob((void*)szValue.GetBuffer(szValue.GetLength()), szValue.GetLength()+1);
		szValue.ReleaseBuffer();
		break;
				   }
	}
	
	m_nRecCount++;
	if (UPDATEVALTAGS == m_nRecCount)
		FlushValue();
	
	return TRUE;	
}

BOOL CDDCDrvObj::AddValueByID(LPCTSTR lpszID, long ltime, short snqa, float val)
{
	CTagItem *pItem = NULL;
	if (!m_mapTagIDs.Lookup(lpszID, pItem))
		return FALSE;
	
	m_recs[m_nRecCount].SetName(pItem->m_szName);
	m_recs[m_nRecCount].ReSetType(DT_VOID);//清空类型
	m_recs[m_nRecCount].val.vt = pItem->m_wType;
	m_recs[m_nRecCount].val.ltime = GetValueTimestmap(ltime);
	m_recs[m_nRecCount].val.snqa = snqa;
	m_recs[m_nRecCount].val.snerr = 0;
	switch(pItem->m_wType)
	{		
	case DRT_DIGITAL:		
	case DRT_INT32:
		m_recs[m_nRecCount].val.lval = (long)val;
		break;
	case DRT_FLOAT32:
		m_recs[m_nRecCount].val.fval = (float)val;
		break;
	case DRT_INT64:
		m_recs[m_nRecCount].val.llval = (__int64)val;
		break;
	case DRT_FLOAT64:
		m_recs[m_nRecCount].val.dblval = (double)val;
		break;
	case DRT_STRING: {
		CString szValue;
		szValue.Format(_T("%f"), val);
		*((CVarVal*)(&m_recs[m_nRecCount].val)) = szValue;
		break;
					 }
	case DRT_BLOB: {
		CString szValue;
		szValue.Format(_T("%f"), val);
		((CVarVal*)(&m_recs[m_nRecCount].val))->SetBlob((void*)szValue.GetBuffer(szValue.GetLength()), szValue.GetLength()+1);
		szValue.ReleaseBuffer();
		break;
				   }
	}
	
	m_nRecCount++;
	if (UPDATEVALTAGS == m_nRecCount)
		FlushValue();
	
	return TRUE;
}

BOOL CDDCDrvObj::AddValueByID(LPCTSTR lpszID, long ltime, short snqa, double val)
{
	CTagItem *pItem = NULL;
	if (!m_mapTagIDs.Lookup(lpszID, pItem))
		return FALSE;
	
	m_recs[m_nRecCount].SetName(pItem->m_szName);
	m_recs[m_nRecCount].ReSetType(DT_VOID);//清空类型
	m_recs[m_nRecCount].val.vt = pItem->m_wType;
	m_recs[m_nRecCount].val.ltime = GetValueTimestmap(ltime);
	m_recs[m_nRecCount].val.snqa = snqa;
	m_recs[m_nRecCount].val.snerr = 0;
	switch(pItem->m_wType)
	{		
	case DRT_DIGITAL:		
	case DRT_INT32:
		m_recs[m_nRecCount].val.lval = (long)val;
		break;
	case DRT_FLOAT32:
		m_recs[m_nRecCount].val.fval = (float)val;
		break;
	case DRT_INT64:
		m_recs[m_nRecCount].val.llval = (__int64)val;
		break;
	case DRT_FLOAT64:
		m_recs[m_nRecCount].val.dblval = (double)val;
		break;
	case DRT_STRING: {
		CString szValue;
		szValue.Format(_T("%f"), val);
		*((CVarVal*)(&m_recs[m_nRecCount].val)) = szValue;
		break;
					 }
	case DRT_BLOB: {
		CString szValue;
		szValue.Format(_T("%f"), val);
		((CVarVal*)(&m_recs[m_nRecCount].val))->SetBlob((void*)szValue.GetBuffer(szValue.GetLength()), szValue.GetLength()+1);
		szValue.ReleaseBuffer();
		break;
				   }
	}
	
	m_nRecCount++;
	if (UPDATEVALTAGS == m_nRecCount)
		FlushValue();
	
	return TRUE;
}

BOOL CDDCDrvObj::AddValueByID(LPCTSTR lpszID, long ltime, short snqa, LPCTSTR val)
{
	if ((NULL == val) || (*val == '\0'))
		return FALSE;
	CTagItem *pItem = NULL;
	if (!m_mapTagIDs.Lookup(lpszID, pItem))
		return FALSE;
	
	m_recs[m_nRecCount].SetName(pItem->m_szName);
	m_recs[m_nRecCount].ReSetType(DT_VOID);//清空类型
	m_recs[m_nRecCount].val.vt = pItem->m_wType;
	m_recs[m_nRecCount].val.ltime = GetValueTimestmap(ltime);
	m_recs[m_nRecCount].val.snqa = snqa;
	m_recs[m_nRecCount].val.snerr = 0;
	switch(pItem->m_wType)
	{		
	case DRT_DIGITAL:		
	case DRT_INT32:
		m_recs[m_nRecCount].val.lval = (long)atoi(val);
		break;
	case DRT_FLOAT32:
		m_recs[m_nRecCount].val.fval = (float)atof(val);
		break;
	case DRT_INT64:
		m_recs[m_nRecCount].val.llval = (__int64)atof(val);
		break;
	case DRT_FLOAT64:
		m_recs[m_nRecCount].val.dblval = (double)atof(val);
		break;
	case DRT_STRING: {
		*((CVarVal*)(&m_recs[m_nRecCount].val)) = val;
		break;
					 }
	case DRT_BLOB: {
		((CVarVal*)(&m_recs[m_nRecCount].val))->SetBlob((void*)val, strlen(val)+1);
		break;
				   }
	}
	
	m_nRecCount++;
	if (UPDATEVALTAGS == m_nRecCount)
		FlushValue();
	
	return TRUE;	
}

BOOL CDDCDrvObj::AddValueByID(LPCTSTR lpszID, long ltime, short snqa, void *val, int nsize)
{
	if ((NULL == val) || (nsize <= 0))
		return FALSE;
	CTagItem *pItem = NULL;
	if (!m_mapTagIDs.Lookup(lpszID, pItem))
		return FALSE;

	if (pItem->m_wType != DRT_BLOB)
		return FALSE;
	
	m_recs[m_nRecCount].SetName(pItem->m_szName);
	m_recs[m_nRecCount].ReSetType(DT_VOID);//清空类型
	m_recs[m_nRecCount].val.vt = pItem->m_wType;
	m_recs[m_nRecCount].val.ltime = GetValueTimestmap(ltime);
	m_recs[m_nRecCount].val.snqa = snqa;
	m_recs[m_nRecCount].val.snerr = 0;
	((CVarVal*)(&m_recs[m_nRecCount].val))->SetBlob(val, nsize);
	
	m_nRecCount++;
	if (UPDATEVALTAGS == m_nRecCount)
		FlushValue();
	
	return TRUE;	
}

BOOL CDDCDrvObj::AddValueByName(LPCTSTR lpszTagName, long ltime, short snqa, long val)
{
	CTagItem *pItem = m_tagtab.GetTag(lpszTagName);
	if (NULL == pItem)
		return FALSE;
	
	m_recs[m_nRecCount].SetName(pItem->m_szName);
	m_recs[m_nRecCount].ReSetType(DT_VOID);//清空类型
	m_recs[m_nRecCount].val.vt = pItem->m_wType;
	m_recs[m_nRecCount].val.ltime = GetValueTimestmap(ltime);
	m_recs[m_nRecCount].val.snqa = snqa;
	m_recs[m_nRecCount].val.snerr = 0;
	switch(pItem->m_wType)
	{		
	case DRT_DIGITAL:		
	case DRT_INT32:
		m_recs[m_nRecCount].val.lval = (long)val;
		break;
	case DRT_FLOAT32:
		m_recs[m_nRecCount].val.fval = (float)val;
		break;
	case DRT_INT64:
		m_recs[m_nRecCount].val.llval = (__int64)val;
		break;
	case DRT_FLOAT64:
		m_recs[m_nRecCount].val.dblval = (double)val;
		break;
	case DRT_STRING: {
		CString szValue;
		szValue.Format(_T("%f"), val);
		*((CVarVal*)(&m_recs[m_nRecCount].val)) = szValue;
		break;
					 }
	case DRT_BLOB: {
		CString szValue;
		szValue.Format(_T("%f"), val);
		((CVarVal*)(&m_recs[m_nRecCount].val))->SetBlob((void*)szValue.GetBuffer(szValue.GetLength()), szValue.GetLength()+1);
		szValue.ReleaseBuffer();
		break;
				   }
	}
	
	m_nRecCount++;
	if (UPDATEVALTAGS == m_nRecCount)
		FlushValue();
	
	return TRUE;
}

BOOL CDDCDrvObj::AddValueByName(LPCTSTR lpszTagName, long ltime, short snqa, __int64 val)
{
	CTagItem *pItem = m_tagtab.GetTag(lpszTagName);
	if (NULL == pItem)
		return FALSE;
	
	m_recs[m_nRecCount].SetName(pItem->m_szName);
	m_recs[m_nRecCount].ReSetType(DT_VOID);//清空类型
	m_recs[m_nRecCount].val.vt = pItem->m_wType;
	m_recs[m_nRecCount].val.ltime = GetValueTimestmap(ltime);
	m_recs[m_nRecCount].val.snqa = snqa;
	m_recs[m_nRecCount].val.snerr = 0;
	switch(pItem->m_wType)
	{		
	case DRT_DIGITAL:		
	case DRT_INT32:
		m_recs[m_nRecCount].val.lval = (long)val;
		break;
	case DRT_FLOAT32:
		m_recs[m_nRecCount].val.fval = (float)val;
		break;
	case DRT_INT64:
		m_recs[m_nRecCount].val.llval = (__int64)val;
		break;
	case DRT_FLOAT64:
		m_recs[m_nRecCount].val.dblval = (double)val;
		break;
	case DRT_STRING: {
		CString szValue;
		szValue.Format(_T("%f"), val);
		*((CVarVal*)(&m_recs[m_nRecCount].val)) = szValue;
		break;
					 }
	case DRT_BLOB: {
		CString szValue;
		szValue.Format(_T("%f"), val);
		((CVarVal*)(&m_recs[m_nRecCount].val))->SetBlob((void*)szValue.GetBuffer(szValue.GetLength()), szValue.GetLength()+1);
		szValue.ReleaseBuffer();
		break;
				   }
	}
	
	m_nRecCount++;
	if (UPDATEVALTAGS == m_nRecCount)
		FlushValue();
	
	return TRUE;	
}

BOOL CDDCDrvObj::AddValueByName(LPCTSTR lpszTagName, long ltime, short snqa, float val)
{
	CTagItem *pItem = m_tagtab.GetTag(lpszTagName);
	if (NULL == pItem)
		return FALSE;
	
	m_recs[m_nRecCount].SetName(pItem->m_szName);
	m_recs[m_nRecCount].ReSetType(DT_VOID);//清空类型
	m_recs[m_nRecCount].val.vt = pItem->m_wType;
	m_recs[m_nRecCount].val.ltime = GetValueTimestmap(ltime);
	m_recs[m_nRecCount].val.snqa = snqa;
	m_recs[m_nRecCount].val.snerr = 0;
	switch(pItem->m_wType)
	{		
	case DRT_DIGITAL:		
	case DRT_INT32:
		m_recs[m_nRecCount].val.lval = (long)val;
		break;
	case DRT_FLOAT32:
		m_recs[m_nRecCount].val.fval = (float)val;
		break;
	case DRT_INT64:
		m_recs[m_nRecCount].val.llval = (__int64)val;
		break;
	case DRT_FLOAT64:
		m_recs[m_nRecCount].val.dblval = (double)val;
		break;
	case DRT_STRING: {
		CString szValue;
		szValue.Format(_T("%f"), val);
		*((CVarVal*)(&m_recs[m_nRecCount].val)) = szValue;
		break;
					 }
	case DRT_BLOB: {
		CString szValue;
		szValue.Format(_T("%f"), val);
		((CVarVal*)(&m_recs[m_nRecCount].val))->SetBlob((void*)szValue.GetBuffer(szValue.GetLength()), szValue.GetLength()+1);
		szValue.ReleaseBuffer();
		break;
				   }
	}
	
	m_nRecCount++;
	if (UPDATEVALTAGS == m_nRecCount)
		FlushValue();
	
	return TRUE;
}

BOOL CDDCDrvObj::AddValueByName(LPCTSTR lpszTagName, long ltime, short snqa, double val)
{
	CTagItem *pItem = m_tagtab.GetTag(lpszTagName);
	if (NULL == pItem)
		return FALSE;
	
	m_recs[m_nRecCount].SetName(pItem->m_szName);
	m_recs[m_nRecCount].ReSetType(DT_VOID);//清空类型
	m_recs[m_nRecCount].val.vt = pItem->m_wType;
	m_recs[m_nRecCount].val.ltime = GetValueTimestmap(ltime);
	m_recs[m_nRecCount].val.snqa = snqa;
	m_recs[m_nRecCount].val.snerr = 0;
	switch(pItem->m_wType)
	{		
	case DRT_DIGITAL:		
	case DRT_INT32:
		m_recs[m_nRecCount].val.lval = (long)val;
		break;
	case DRT_FLOAT32:
		m_recs[m_nRecCount].val.fval = (float)val;
		break;
	case DRT_INT64:
		m_recs[m_nRecCount].val.llval = (__int64)val;
		break;
	case DRT_FLOAT64:
		m_recs[m_nRecCount].val.dblval = (double)val;
		break;
	case DRT_STRING: {
		CString szValue;
		szValue.Format(_T("%f"), val);
		*((CVarVal*)(&m_recs[m_nRecCount].val)) = szValue;
		break;
					 }
	case DRT_BLOB: {
		CString szValue;
		szValue.Format(_T("%f"), val);
		((CVarVal*)(&m_recs[m_nRecCount].val))->SetBlob((void*)szValue.GetBuffer(szValue.GetLength()), szValue.GetLength()+1);
		szValue.ReleaseBuffer();
		break;
				   }
	}
	
	m_nRecCount++;
	if (UPDATEVALTAGS == m_nRecCount)
		FlushValue();
	
	return TRUE;
}

BOOL CDDCDrvObj::AddValueByName(LPCTSTR lpszTagName, long ltime, short snqa, LPCTSTR val)
{
	if ((NULL == val) || (*val == '\0'))
		return FALSE;	
	CTagItem *pItem = m_tagtab.GetTag(lpszTagName);
	if (NULL == pItem)
		return FALSE;
	
	m_recs[m_nRecCount].SetName(pItem->m_szName);
	m_recs[m_nRecCount].ReSetType(DT_VOID);//清空类型
	m_recs[m_nRecCount].val.vt = pItem->m_wType;
	m_recs[m_nRecCount].val.ltime = GetValueTimestmap(ltime);
	m_recs[m_nRecCount].val.snqa = snqa;
	m_recs[m_nRecCount].val.snerr = 0;
	switch(pItem->m_wType)
	{		
	case DRT_DIGITAL:		
	case DRT_INT32:
		m_recs[m_nRecCount].val.lval = (long)atoi(val);
		break;
	case DRT_FLOAT32:
		m_recs[m_nRecCount].val.fval = (float)atof(val);
		break;
	case DRT_INT64:
		m_recs[m_nRecCount].val.llval = (__int64)atof(val);
		break;
	case DRT_FLOAT64:
		m_recs[m_nRecCount].val.dblval = (double)atof(val);
		break;
	case DRT_STRING: {
		*((CVarVal*)(&m_recs[m_nRecCount].val)) = val;
		break;
					 }
	case DRT_BLOB: {
		((CVarVal*)(&m_recs[m_nRecCount].val))->SetBlob((void*)val, strlen(val)+1);
		break;
				   }
	}
	
	m_nRecCount++;
	if (UPDATEVALTAGS == m_nRecCount)
		FlushValue();
	
	return TRUE;	
}

BOOL CDDCDrvObj::AddValueByName(LPCTSTR lpszTagName, long ltime, short snqa, void *val, int nsize)
{
	if ((NULL == val) || (nsize <= 0))
		return FALSE;
	
	CTagItem *pItem = m_tagtab.GetTag(lpszTagName);
	if (NULL == pItem)
		return FALSE;
	
	if (pItem->m_wType != DRT_BLOB)
		return FALSE;
	
	m_recs[m_nRecCount].SetName(pItem->m_szName);
	m_recs[m_nRecCount].ReSetType(DT_VOID);//清空类型
	m_recs[m_nRecCount].val.vt = pItem->m_wType;
	m_recs[m_nRecCount].val.ltime = GetValueTimestmap(ltime);
	m_recs[m_nRecCount].val.snqa = snqa;
	m_recs[m_nRecCount].val.snerr = 0;
	((CVarVal*)(&m_recs[m_nRecCount].val))->SetBlob(val, nsize);
	
	m_nRecCount++;
	if (UPDATEVALTAGS == m_nRecCount)
		FlushValue();
	
	return TRUE;	
}

BOOL CDDCDrvObj::FlushValue()
{
	BOOL bNeedFlush = (m_nRecCount > 0);
	if (bNeedFlush)
		OnDataOk(m_recs, m_nRecCount);

	m_nRecCount = 0;
	return bNeedFlush;
}

DWORD CDDCDrvObj::WriteStreamMessageCB( const char* lpszTagName, DWORD dwCommandID, BYTE* pBuffer, int nSize )
{
	if (m_cbStreamMessage == NULL)
		return DRERR_NOTSUPPORT;

	return m_cbStreamMessage(m_handle, lpszTagName, dwCommandID, pBuffer, nSize);
}

DWORD CDDCDrvObj::WriteHistoryCB( T_DRREC recs[], int nrecs )
{
	if (m_cbWriteHistory == NULL)
		return DRERR_NOTSUPPORT;

	return m_cbWriteHistory(m_handle, recs, nrecs);
}

// bForceUpdateSec == TRUE表示强制进行周期检测
BOOL CDDCDrvObj::CheckUpdateSec(BOOL bForceUpdateSec/* = TRUE*/)
{	
	// 是否自动处理更新周期？
	if (bForceUpdateSec  || m_pProtocol->m_bAutoUpdateSec)
	{
		::CoFileTimeNow(&m_ftEndTime);
		if (GetElapsedTime() < m_config.m_nUpdateSec)
		{
			Sleep(100);
			return FALSE;
		}
		memcpy(&m_ftStartTime, &m_ftEndTime, sizeof(FILETIME));
	}

	return TRUE;
}

UINT CDDCDrvObj::DoRun()					//正真的运行过程，被RunThread中调用
{
	m_bRun = TRUE; //不能少这一句

	::CoFileTimeNow(&m_ftStartTime);
	while(!m_bKillThread)
	{		
		if (m_pProtocol != NULL)
		{	
			// 是否自动处理更新周期？
			if (!CheckUpdateSec(FALSE))
				continue;
			
			m_pProtocol->DoRun();
		}
	}
	m_mapTagIDs.RemoveAll();
	m_bRun = FALSE; //不能少这一句

	return DRERR_OK;
}

BOOL CDDCDrvObj::TestConnect(CConfigInfo &config)
{
	WORD wType = m_config.m_wComType; // 先保留原协议类型
	m_config.m_wComType = config.m_wComType; // 设置该协议类型用于调用CreateProtocol()函数
	CDDCDrvProtocol	*pProtocol = CreateProtocol();
	m_config.m_wComType = wType;
	
	if (pProtocol == NULL)
	{
		AfxMessageBox(_T("连接测试失败！"), MB_OK | MB_ICONINFORMATION);
		return FALSE;
	}
	
	BOOL bOK = FALSE;
	// 设置协议处理类实例的相关参数
	pProtocol->SetConfig(config);
	pProtocol->m_pDrvObj = this;
	if (pProtocol->Connect())
	{
		bOK = TRUE;
		pProtocol->Disconnect();
		AfxMessageBox(_T("连接测试成功！"), MB_OK | MB_ICONINFORMATION);
	}
	else {
		AfxMessageBox(_T("连接测试失败！"), MB_OK | MB_ICONINFORMATION);
	}
	delete pProtocol;
	return bOK;
}