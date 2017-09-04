//////////////////////////////////////////////////////////////////
// demodrv.cpp
#include "stdafx.h"
#include "modbus.h"
#include "modbusdrv.h"
#include <math.h>
#include "..\sdk\HashTableUtils.h"

/////////////////////////////////////////////////////////////////
//实现静态对象类工厂
CDrvObj*	CDrvFactory::NewDrv()
{
	return new CModDrv();
}

DRVRET CDrvFactory::GetDriverInfo(LPDRVINFO lpDrvInfo)
{
	memset(lpDrvInfo,0,sizeof(T_DRVINFO));
	strcpy(lpDrvInfo->sname,"Modbus driver for IOServer");
	strcpy(lpDrvInfo->sdes,"CopyRight By 双鱼人科技");
	strcpy(lpDrvInfo->sver,"2.0.0.1"); //程序版本
	lpDrvInfo->dwver = 0x0200;//规范版本
	lpDrvInfo->dwres = 0;
	return DRERR_OK;
}

/*
	CRC-16码的计算步骤
1、	置16位寄存器为十六进制FFFF(即全为1)。称此寄存器为CRC寄存器。
2、	把一个8位数据与16位CRC寄存器的低位相异或，把结果放于CRC寄存器。
3、	把寄存器的内容右移一位(朝低位)，用0填补最高位，检查最低位(移出位)。
4、	如果最低位为0：复第3步(再次移位)。
如果最低位为1：CRC寄存器与多项式A001(1010 0000 0000 0001)进行异或。
5、	重复步骤3和4，直到右移8次，这样整个8位数据全部进行了处理。
6、	重复步骤2到步骤5，进行下一个8位的处理。
7、	最后得到的CRC寄存器即为CRC码，低字节在前，高字节在后。
  */

//CRC16，modbus RTU专用
unsigned short calccrc(unsigned char crcbuf,unsigned short crc)
{
   int i; 
   crc=crc ^ crcbuf;
   for(i=0;i<8;i++)
   {
       BYTE chk;
       chk=crc&1;
       crc=crc>>1;
       crc=crc&0x7fff;
       if (chk==1)
	       crc=crc^0xa001;
       crc=crc&0xffff;
   }
   return crc; 
}

unsigned short CRC_16(unsigned char *buf,int len)//结构为INTEL顺序，低字节在前，高字节在后
{
	int  i;
	unsigned short crc;
	crc=0xFFFF;
	for (i=0;i<len;i++)
	{	
		crc=calccrc(*buf,crc);
		buf++;
	}
	return crc;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//CModCom485

CModCom485::CModCom485()
{
	m_szPort="\\\\.\\com1";			// 端口名，如"\\\\.\\com1","\\\\.\\com2"
	m_dwBaudRate=CBR_9600;			//波特率,CBR_1200,CBR_2400,CBR_4800,CBR_9600,
									//CBR_14400,CBR_19200,CBR_38400,CBR_57600,CBR_115200,

	m_cByteSize=8;					//数据位 number of bits/byte, 4-8
	m_cParity=0;					//0-4对应no,odd,even,mark,space 
	m_cStopBits=0;					// 0,1,2 对应 1, 1.5, 2

	m_hComDev=INVALID_HANDLE_VALUE;				//通信端口句柄
	m_nerrnum = 0;

	m_nTimeOut = 200;
}

CModCom485::~CModCom485()
{
	ClosePort();
}

void CModCom485::InitComPort(
		LPCTSTR lpszComName,// 端口名，如"\\\\.\\com1","\\\\.\\com2"
		DWORD dwBaudRate,	// 波特率,
		BYTE cByteSize,		// 数据位 number of bits/byte, 4-8
		BYTE cParity,		// 0-4=no,odd,even,mark,space 
		BYTE cStopBits,		// 0,1,2 = 1, 1.5, 2 )
		int	 nTimeOut
	)//初始化通信口
{
	m_szPort=lpszComName;			// 端口名，如"\\\\.\\com1","\\\\.\\com2"
	m_dwBaudRate=dwBaudRate;			//波特率,CBR_1200,CBR_2400,CBR_4800,CBR_9600,
									//CBR_14400,CBR_19200,CBR_38400,CBR_57600,CBR_115200,
	m_cByteSize=cByteSize;			//数据位 number of bits/byte, 4-8
	m_cParity=cParity;				//0-4对应no,odd,even,mark,space 
	m_cStopBits=cStopBits;			// 0,1,2 对应 1, 1.5, 2
	m_nTimeOut = nTimeOut;
	ClosePort();
}

BOOL CModCom485::SendData(BYTE *pc,int nSize)
{
	if(m_hComDev == INVALID_HANDLE_VALUE)
		return FALSE;
	COMSTAT comstat;
	DWORD dwErrorMask;
	DWORD dwWriteNum=0;
	BOOL bret;
	bret=::WriteFile(
			m_hComDev,      // handle to file
			pc,             // data buffer
			nSize,				// number of bytes to write
			&dwWriteNum,		// number of bytes written
			NULL			// overlapped buffer
		);
	if(!bret)
	{
		ClearCommError(m_hComDev,&dwErrorMask,&comstat);//清除错误
		PurgeComm(m_hComDev,PURGE_TXABORT|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_RXCLEAR);//清空驱动程序中的缓冲区
		
		m_nerrnum++;
		if(m_nerrnum > 10)
		{
			ClosePort();
			Sleep(50);
			OpenPort();
			m_nerrnum = 0;
		}
		return bret;
	}
	if(dwWriteNum == 0)
		return FALSE;
	
	m_nerrnum = 0;
	return TRUE;
}

int  CModCom485::ReadData(BYTE *pc,int nSize)
{
	if(m_hComDev == INVALID_HANDLE_VALUE)
		return 0;
	BOOL bret;
	DWORD dwReadNum=0;
	COMSTAT comstat;
	DWORD dwErrorMask;

	bret = ::ReadFile(m_hComDev,pc,nSize,&dwReadNum,NULL);
	if(!bret)
	{
		ClearCommError(m_hComDev,&dwErrorMask,&comstat);//清除错误
		PurgeComm(m_hComDev,PURGE_TXABORT|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_RXCLEAR);//清空驱动程序中的缓冲区

		m_nerrnum++;
		if(m_nerrnum > 10)
		{
			ClosePort();
			Sleep(50);
			OpenPort();
			m_nerrnum = 0;
		}
		return 0;
	}
	m_nerrnum = 0;
	return (int)dwReadNum;
}

//--------------------下面是需要重载的函数
BOOL CModCom485::OpenPort()
{
	m_hComDev =CreateFile( m_szPort, GENERIC_READ | GENERIC_WRITE,
                  0,                    // exclusive access
                  NULL,                 // no security attrs
                  OPEN_EXISTING,
                  FILE_ATTRIBUTE_NORMAL,
                  NULL ); 
	if(m_hComDev==INVALID_HANDLE_VALUE)
		return FALSE;//CreateFile通信口失败！

	COMMTIMEOUTS  CommTimeOuts ;
	DCB dcb;
	COMMCONFIG cfg;

	cfg.dcb.DCBlength = sizeof( DCB ) ;
	BOOL bRet=FALSE;
	bRet=GetCommState( m_hComDev, &(cfg.dcb) ) ;

	SetupComm( m_hComDev, 4096, 4096 ) ;
    
	PurgeComm( m_hComDev, PURGE_TXABORT | PURGE_RXABORT |
                                      PURGE_TXCLEAR | PURGE_RXCLEAR ) ;
	dcb.DCBlength = sizeof( DCB ) ;

	bRet=GetCommState( m_hComDev, &dcb ) ;
	if(!bRet)
	{
		TRACE("GetCommState error!---------\n");
		return FALSE;
	}
	dcb.BaudRate =m_dwBaudRate;
	dcb.ByteSize =m_cByteSize;
	dcb.Parity =m_cParity;
	dcb.StopBits =m_cStopBits;

   // setup hardware flow control硬件流控制无
	dcb.fDtrControl = DTR_CONTROL_ENABLE;//DTR_CONTROL_DISABLE ;// 485转接器用DTR_CONTROL_ENABLE
	dcb.fOutxCtsFlow = FALSE ;
    dcb.fRtsControl = RTS_CONTROL_DISABLE ;

   // setup software flow control软件流控制无
	dcb.fInX = dcb.fOutX = FALSE ;

   // other various settings
	dcb.fBinary = TRUE ;
	dcb.fParity = TRUE ;
	
   // set up for overlapped I/O
	int ntm = (2*9600)/dcb.BaudRate;
	if(ntm <= 0)
		ntm = 1;
	int nConstant = m_nTimeOut - 128 * ntm;
	if(nConstant < 0)
		nConstant = 0;
	CommTimeOuts.ReadIntervalTimeout			=	5;//字符间超时3毫秒,
	CommTimeOuts.ReadTotalTimeoutMultiplier		=	1+ntm;
	CommTimeOuts.ReadTotalTimeoutConstant		=	100;//静态超时
	CommTimeOuts.WriteTotalTimeoutMultiplier	=   ntm;
	CommTimeOuts.WriteTotalTimeoutConstant		=  200;//静态100毫秒

	if(!SetCommTimeouts(m_hComDev , &CommTimeOuts ))
	{
		TRACE("SetCommTimeouts error!---------\n");
		return FALSE;
	}
	if (!SetCommState( m_hComDev, &dcb ))
	{
		TRACE("SetCommState error!---------\n");
		return FALSE;//
	}
	m_nerrnum=0;
	return TRUE;
}

void CModCom485::ClosePort()
{
	if(m_hComDev != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hComDev);
		m_hComDev = INVALID_HANDLE_VALUE;
	}
}
BOOL CModCom485::IsPortOpen()//端口是否打开
{
	return (m_hComDev != INVALID_HANDLE_VALUE);
}

int CModCom485::WriteFrm(BYTE *pb,int nsize)////发送modbus通信包
{
	if(m_hComDev == INVALID_HANDLE_VALUE)
		return FALSE;
	//专为modbus 485半双工设计
	PurgeComm(m_hComDev,PURGE_TXABORT|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_RXCLEAR);//清空驱动程序中的缓冲区

	COMSTAT comstat;
	DWORD dwErrorMask;
	DWORD dwWriteNum=0;
	BOOL bret;
	bret=::WriteFile(
			m_hComDev,      // handle to file
			pb,             // data buffer
			nsize,				// number of bytes to write
			&dwWriteNum,		// number of bytes written
			NULL			// overlapped buffer
			);
	m_pDrv->WriteDebugLog(FALSE, pb, nsize);
	if(!bret)
	{
		ClearCommError(m_hComDev,&dwErrorMask,&comstat);//清除错误
		PurgeComm(m_hComDev,PURGE_TXABORT|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_RXCLEAR);//清空驱动程序中的缓冲区
		
		m_nerrnum++;
		if(m_nerrnum > 10)
		{
			ClosePort();
			Sleep(50);
			OpenPort();
			m_nerrnum = 0;
		}
		return 0;
	}
	m_nerrnum = 0;
	return (int)dwWriteNum;
}		
int CModCom485::ReadFrm(BYTE *pbuf,int nbufsize,int ncrc16seq)//接受modbus通信包,返回字节数,缓冲区数据包已验证
{
	BYTE sread[512];//缓冲
	int  nrsize =0;

	//等待接受
	int i=0;

	int  nr=0;
	BYTE rt[256];
			
	for(i=0;i<5;i++)
	{
		Sleep(20);//延时50毫秒
		nr = ReadData(rt,128);//读128字节,每次最多读60个寄存器，因此最大接受数据包 3 + 60 * 2  + 2 = 125
		if(nr == 0)
			continue;
		if(nrsize + nr > 512)
			nrsize=0;
		int k;
		for(k=0;k<nr;k++)
		{
			sread[nrsize] = rt[k];
			nrsize++;
		}

		//检查完整帧
		if(nrsize < 3)
			continue;
					
		WORD wcrc=CRC_16(sread,nrsize-2);//低在前

		if(wcrc ==  CModDrv::GetWord(sread + nrsize - 2,ncrc16seq))//比较CRC16值
		{
			if(nrsize > nbufsize)
				return 0;
			int j;
			for(j=0;j<nrsize;j++)
				pbuf[j] = sread[j];	//复制完整帧

			m_pDrv->WriteDebugLog(FALSE, pbuf, nrsize);
			return nrsize;
		}
	}
	return 0;
}	

///////////////////////////////////////////////////////////////////////////////////
// CModComTcp

CModComTcp::CModComTcp()
{
	m_szSvrIp = "172.0.0.1";//服务器IP
	m_nSvrPort = 502;//服务端口
	m_sClient = INVALID_SOCKET;
	m_nTimeOut = 200;
	m_bTcpCrc16 = FALSE;
}

CModComTcp::~CModComTcp()
{
	ClosePort();
}

BOOL CModComTcp::IsPortOpen()//端口是否打开
{
	return (m_sClient != INVALID_SOCKET);
}
BOOL CModComTcp::OpenPort()//打开端口
{
	int nRet;
	if(m_sClient != INVALID_SOCKET)
	{
		::closesocket(m_sClient);
		m_sClient = INVALID_SOCKET;
	}

	unsigned long ul = 1;//非阻塞方式
	struct sockaddr_in sServer;

	sServer.sin_family=AF_INET;
	sServer.sin_port=::htons(m_nSvrPort);
	sServer.sin_addr.s_addr=::inet_addr(m_szSvrIp);

	m_sClient=::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(m_sClient==INVALID_SOCKET)
		return 1;
	
	if(SOCKET_ERROR == ioctlsocket(m_sClient, FIONBIO, (unsigned long*)&ul))//设置非阻塞方式连接
	{
		::closesocket(m_sClient);
		m_sClient = INVALID_SOCKET;
		return FALSE;
	}

	connect(m_sClient,(const struct sockaddr *)&sServer,sizeof(sServer));
	
	TIMEVAL tv01 = {2,0};//2秒钟延迟
	FD_SET fdw = {1, m_sClient};
	nRet=::select(0, NULL, &fdw, NULL, &tv01);//测试可写
	if(nRet != 1)
	{
		::closesocket(m_sClient);
		m_sClient = INVALID_SOCKET;
		return  FALSE;
	}
	return TRUE;
}

void CModComTcp::ClosePort()//关闭端口
{
	if(m_sClient != INVALID_SOCKET)
	{
		::closesocket(m_sClient);
		m_sClient = INVALID_SOCKET;
	}
}

// ndelaysec超时秒数
// 返回读的字节数，不含报头,-1发生严重错
int CModComTcp::Read(SOCKET s,BYTE *pbuf,int size,int ndelaymsec)
{
	TIMEVAL tv01 = {ndelaymsec/1000,1000 *(ndelaymsec%1000)};
	FD_SET fdr = {1, s};
	int nRet;
	char buf[512];
	int  nfrmlen=0;

	nRet=::select(0, &fdr, NULL, NULL, &tv01);
	if(SOCKET_ERROR==nRet)//发生网络错误
	{
		int nErrorCode=WSAGetLastError();
		TRACE("服务器线程设置读超时错误！nErrorCode=%d\n",nErrorCode);
		return -1;
	}
	if(nRet==0)//超时发生，无可读数据
	{
		TRACE("Read timeout\n");
		return 0;
	}

	nfrmlen=::recv(s, buf, 264 , 0);
	if(SOCKET_ERROR==nfrmlen)//接受错误
	{
		TRACE("接收数据错误！\n");
		return -1;
	}
	if(nfrmlen==0)//连接被关闭
		return -1;

	if(nfrmlen > size)
		return 0;
	memcpy(pbuf,buf,nfrmlen);

	return nfrmlen;
}

// ndelaysec超时秒数
// 返回发送字节数，不包括报头,发生严重错误时返回 -1；
int CModComTcp::Write(SOCKET s,BYTE *pbuf,int size,int ndelaymsec)
{
	TIMEVAL tv01 = {ndelaymsec/1000,1000 * (ndelaymsec%1000)};//1秒钟延迟
	FD_SET fdw = {1, s};

	if(1 != ::select(0, NULL, &fdw, NULL, &tv01))//测试可写
		return  0;
	
	int nret = ::send(s, (char*)pbuf,size,0);
	if(SOCKET_ERROR == nret)//发送错误
		return -1;
		
	return nret;
}

int CModComTcp::WriteFrm(BYTE *pb,int nsize)
{
	BYTE buf[512];
	if(nsize > 256)
		return 0;

	int nlen = nsize;
	//填写报头
	buf[0] = 0xF5;//任意的，用于识别双方的字
	buf[1] = 0x5F;
	buf[2] = 0;//协议
	buf[3] = 0;
	buf[4] = 0;
	if(m_bTcpCrc16)
	{
		buf[5] = BYTE(nsize);
		nlen = nsize;
	}
	else
	{
		buf[5] = BYTE(nsize - 2);
		nlen = nsize - 2;
	}
	
	memcpy(buf + 6,pb,nlen);
	
	int nret = Write(m_sClient,buf,nlen + 6,400);//400ms延时
	m_pDrv->WriteDebugLog(FALSE, buf, nlen + 6);
	if(nret == -1)
	{
		ClosePort();
		return 0;
	}
	return nret;
}

int CModComTcp::ReadFrm(BYTE *pbuf,int nbufsize,int ncrc16seq)
{
	BYTE buf[512];
	int n = Read(m_sClient,buf,264,m_nTimeOut);
	if(n == -1)
	{
		ClosePort();
		return 0;
	}

	if(n < 6)//
		return 0;

	m_pDrv->WriteDebugLog(TRUE, buf, n);
	
	WORD wlen = CModDrv::GetWord(buf + 4,21);//TCP头部填写的数据长度
	if( n < wlen + 6)//长度不够
	{
		int n2 = Read(m_sClient,buf+n,512 - n,m_nTimeOut);
		if(n2 == -1)
		{
			ClosePort();
			return 0;
		}
		m_pDrv->WriteDebugLog(TRUE, buf + n, n2);
		n+=n2;
	}
	int nf = wlen;
	if( n != wlen + 6)
	{
		m_pDrv->AddDebugFmt(_T("接收数据长度无效，接收：%d字节，期待：%d字节"), n, wlen + 6);
		return 0;
	}

	BYTE *pb = buf + 6;
	if(m_bTcpCrc16)//带CRC16
	{
		WORD wcrc=CRC_16(pb,nf-2);//低在前
		if(wcrc ==  CModDrv::GetWord(pb + nf - 2,ncrc16seq))//比较CRC16值
		{
			if(nf > nbufsize)
				return 0;
			memcpy(pbuf,pb,nf);
			return nf;
		}
	}
	else //不带CRC16,则加上,为了后面统一处理
	{
		if((buf[0] != 0xF5)||(buf[1] != 0x5F))
		{
			m_pDrv->AddDebugFmt(_T("无效数据包标识，接收：%02X %02X，期待：F5 5F"), (int)buf[0], (int)buf[1]);
			return 0;
		}
		//加CRC16
		WORD wcrc=CRC_16(pb,nf);//低在前
		CModDrv::SetWord(&wcrc,pb+nf,ncrc16seq);
		if(nf+2 > nbufsize)
			return 0;
		memcpy(pbuf,pb,nf+2);
		return nf+2;
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////
//
void CModDec::InitSendFrm0102(int norder)//初始化发送缓冲区,只存储定时读取命令01,02
{
	BYTE ucorder;
	WORD wregaddr;

	WORD wt;
	BOOL bs=FALSE;
	CModItem *pi;
	int nbytes=0;
	// 子机地址1 功能码1 起始地址2 读取个数2 CRC2
	BYTE fb[256];
	int  size=0;


	int i;
	for(i=0;i<m_ndatas;i++)
	{
		pi = m_pbuf[i];
		if(pi->m_ucorder != norder)
			continue;
		if(bs == FALSE)//起始
		{
			bs=TRUE;
			ucorder = pi->m_ucorder;
			wregaddr = pi->m_waddr;
			nbytes++;
		}
		else
		{

			if(		((nbytes + wregaddr) == pi->m_waddr) && //地址连续
					(nbytes < 48)  //48个寄存器
				)
				nbytes++;
			else
			{
				//组帧,不包括当前pi
				BYTE *pb = (BYTE *)fb;

				pb[0] = m_ucaddr;//设备地址
				pb[1] = ucorder;//命令

				CModDrv::SetWord(&wregaddr,pb+2,21);//填写起始地址

				wt = (WORD)(nbytes);//个数
				CModDrv::SetWord(&wt,pb+4,21);//填写数据个数

				wt = CRC_16( pb, 6 );
				CModDrv::SetWord(&wt,pb+6,m_ncrc16seq);//填写CRC16

				m_sendfrm.AddFrm(fb,8);//加入发送帧缓冲区
				
				//将当前pi组到下一帧
				ucorder = pi->m_ucorder;
				wregaddr = pi->m_waddr;
				nbytes = 1;
			}
		}
	}
	if(nbytes>0)
	{
		BYTE *pb = (BYTE *)fb;

		pb[0] = m_ucaddr;//设备地址
		pb[1] = ucorder;//命令

		CModDrv::SetWord(&wregaddr,pb+2,21);//填写起始地址

		wt = (WORD)(nbytes);//个数
		CModDrv::SetWord(&wt,pb+4,21);//填写数据个数
		
		wt = CRC_16( pb, 6 );
		CModDrv::SetWord(&wt,pb+6,m_ncrc16seq);//填写CRC16
		
		m_sendfrm.AddFrm(fb,8);//加入发送帧缓冲区
	}
}
void CModDec::InitSendFrm0304(int norder)//初始化发送缓冲区,只存储定时读取命令03,04
{
	BYTE ucorder;
	WORD wregaddr;

	WORD wt;
	BOOL bs=FALSE;
	CModItem *pi;
	int nbytes=0;
	// 子机地址1 功能码1 起始地址2 读取个数2 CRC2
	BYTE fb[256];
	int  size=0;

	int i,n;
	for(i=0;i<m_ndatas;i++)
	{
		pi = m_pbuf[i];
		if(pi->m_ucorder != norder)
			continue;
		if(bs == FALSE)//起始
		{
			bs=TRUE;
			ucorder = pi->m_ucorder;
			wregaddr = pi->m_waddr;
			n=pi->GetCVLen();
			nbytes += n;
		}
		else
		{
			n=pi->GetCVLen();//字节数

			if(	((nbytes/2 + wregaddr) == pi->m_waddr) && //地址连续,按字编排的寄存器地址
					((nbytes/2) < MODBUS_REGISTER_MAX)  //
				)
				nbytes+=n;
			else
			{
				;//组帧,不包括当前pi
				BYTE *pb = (BYTE *)fb;

				pb[0] = m_ucaddr;//设备地址
				pb[1] = ucorder;//命令
				CModDrv::SetWord(&wregaddr,pb+2,21);//设置起始地址
				

				wt = (WORD)(nbytes/2);//按字处理
				CModDrv::SetWord(&wt,pb+4,21);//设置数据个数
				
				wt = CRC_16( pb, 6 );
				CModDrv::SetWord(&wt,pb+6,m_ncrc16seq);//填写CRC16
				
				m_sendfrm.AddFrm(fb,8);//加入发送帧缓冲区
				
				//将当前pi组到下一帧
				ucorder = pi->m_ucorder;
				wregaddr = pi->m_waddr;
				n=pi->GetCVLen();
				nbytes = n;
			}
		}
	}

	if(nbytes>0)
	{
		BYTE *pb = (BYTE *)fb;

		pb[0] = m_ucaddr;//设备地址
		pb[1] = ucorder;//命令
		CModDrv::SetWord(&wregaddr,pb+2,21);//设置起始地址

		wt = (WORD)(nbytes/2);//按字处理
		CModDrv::SetWord(&wt,pb+4,21);//设置数据个数

		wt = CRC_16( pb, 6 );
		CModDrv::SetWord(&wt,pb+6,m_ncrc16seq);//填写CRC16

		m_sendfrm.AddFrm(fb,8);//加入发送帧缓冲区
	}
}

void CModDec::InitSendFrm()//初始化定时发送
{
	sort();//排序
	m_sendfrm.Clear();//先清空
	InitSendFrm0102(1);
	InitSendFrm0102(2);
	InitSendFrm0304(3);
	InitSendFrm0304(4);
}

////////////////////////////////////////////////////////////////////////////
//CModDrv 2.0
short CModDrv::GetShort(BYTE *pb,int nseq)//读字，nseq = 0低字节在前,1:高字节在前
{
	BYTE ca[4];
	if(nseq != 12)//高字节在前
	{
		ca[0] = pb[1];		ca[1] = pb[0];
	}
	else
	{
		ca[0] = pb[0];		ca[1] = pb[1];
	}
	return *((short*)ca);
}

WORD CModDrv::GetWord(BYTE *pb,int nseq)//读字，nseq = 0低字节在前,1:高字节在前
{
	BYTE ca[4];
	if(nseq != 12)//高字节在前
	{
		ca[0] = pb[1];		ca[1] = pb[0];
	}
	else
	{
		ca[0] = pb[0];		ca[1] = pb[1];
	}
	return *((WORD*)ca);
}

//nseq = 字节顺序 0:1234; 1:2143; 2:4321; 3:3412;1为最低字节，4为最高字节,即4321的排列顺序
long CModDrv::GetLong(BYTE *pb,int nseq)//
{
	BYTE ca[4];
	if(nseq == 1234)//
	{
		ca[0] = pb[0];		ca[1] = pb[1];		ca[2] = pb[2];		ca[3] = pb[3];
	}
	else if(nseq == 2143)//
	{
		ca[0] = pb[1];		ca[1] = pb[0];		ca[2] = pb[3];		ca[3] = pb[2];
	}

	else if(nseq == 4321)//
	{
		ca[0] = pb[3];		ca[1] = pb[2];		ca[2] = pb[1];		ca[3] = pb[0];
	}
	else
	{
		ca[0] = pb[2];		ca[1] = pb[3];		ca[2] = pb[0];		ca[3] = pb[1];
	}
	return *((int*)ca);
}

DWORD CModDrv::GetDWord(BYTE *pb,int nseq)//
{
	BYTE ca[4];
	if(nseq == 1234)//
	{
		ca[0] = pb[0];		ca[1] = pb[1];		ca[2] = pb[2];		ca[3] = pb[3];
	}
	else if(nseq == 2143)//
	{
		ca[0] = pb[1];		ca[1] = pb[0];		ca[2] = pb[3];		ca[3] = pb[2];
	}

	else if(nseq == 4321)//
	{
		ca[0] = pb[3];		ca[1] = pb[2];		ca[2] = pb[1];		ca[3] = pb[0];
	}
	else
	{
		ca[0] = pb[2];		ca[1] = pb[3];		ca[2] = pb[0];		ca[3] = pb[1];
	}
	return *((DWORD*)ca);
}

//nseq = 字节顺序 0:1234; 1:2143; 2:4321; 3:3412;1为最低字节，4为最高字节,即4321的排列顺序
float CModDrv::GetFloat(BYTE *pb,int nseq)//
{
	BYTE ca[4];
	if(nseq == 1234)//
	{
		ca[0] = pb[0];		ca[1] = pb[1];		ca[2] = pb[2];		ca[3] = pb[3];
	}
	else if(nseq == 2143)//
	{
		ca[0] = pb[1];		ca[1] = pb[0];		ca[2] = pb[3];		ca[3] = pb[2];
	}

	else if(nseq == 4321)//
	{
		ca[0] = pb[3];		ca[1] = pb[2];		ca[2] = pb[1];		ca[3] = pb[0];
	}
	else
	{
		ca[0] = pb[2];		ca[1] = pb[3];		ca[2] = pb[0];		ca[3] = pb[1];
	}
	return *((float*)ca);
}

void CModDrv::SetWord(void* pv,BYTE *pb,int nseq)
{
	BYTE *p = (BYTE*)pv;
	if(nseq != 12)//高字节在前
	{
		pb[0] = p[1];		pb[1] = p[0];
	}
	else
	{
		pb[0] = p[0];		pb[1] = p[1];
	}
}
void CModDrv::SetDWord(void* pv,BYTE *pb,int nseq)
{
	BYTE *p = (BYTE*)pv;
	if(nseq == 1234)//
	{
		pb[0] = p[0];		pb[1] = p[1];		pb[2] = p[2];		pb[3] = p[3];
	}
	else if(nseq == 2143)//
	{
		pb[0] = p[1];		pb[1] = p[0];		pb[2] = p[3];		pb[3] = p[2];
	}

	else if(nseq == 4321)//
	{
		pb[0] = p[3];		pb[1] = p[2];		pb[2] = p[1];		pb[3] = p[0];
	}
	else
	{
		pb[0] = p[2];		pb[1] = p[3];		pb[2] = p[0];		pb[3] = p[1];
	}
}
void CModDrv::SetFloat(void* pv,BYTE *pb,int nseq)
{
	BYTE *p = (BYTE*)pv;
	if(nseq == 1234)//
	{
		pb[0] = p[0];		pb[1] = p[1];		pb[2] = p[2];		pb[3] = p[3];
	}
	else if(nseq == 2143)//
	{
		pb[0] = p[1];		pb[1] = p[0];		pb[2] = p[3];		pb[3] = p[2];
	}

	else if(nseq == 4321)//
	{
		pb[0] = p[3];		pb[1] = p[2];		pb[2] = p[1];		pb[3] = p[0];
	}
	else
	{
		pb[0] = p[2];		pb[1] = p[3];		pb[2] = p[0];		pb[3] = p[1];
	}
}

CModDrv::CModDrv()
{
	m_ndecsize = 0;//设备个数
	int i;
	for(i=0;i<MAX_MODBUSDEC;i++)
		m_pdec[i] = 0;

	m_pmodcom = NULL;
	m_wVersion = 0;
}

CModDrv::~CModDrv()
{
	CString szid;
	CModItemIndex *pmi;
	POSITION pos = m_mapitem.GetStartPosition();
	while(pos!=NULL)
	{
		m_mapitem.GetNextAssoc( pos, szid, pmi );
		delete pmi;
	}
	m_mapitem.RemoveAll();//清空MAP

	int i;
	for(i=0;i<m_ndecsize;i++)
		delete m_pdec[i];//删除设备表

}

void	CModDrv::Serialize(CArchive& ar)//串行化保存，读取配置
{
	if(ar.IsLoading()) //如果是读入
	{
		DWORD dwFlag = 0;
		ar >> dwFlag;
		if(dwFlag != DRVCFG_FLAG_MODBUS)
			return;
		
		// 增加版本号信息
		ar >> m_wVersion;
		if (m_wVersion > DRVCFG_VERSION)
			return;		

		m_cfg.Serialize(ar, m_wVersion);
		TransCfg();//将配置文件转换为运行对象
	}
	else {
		ar << (DWORD)DRVCFG_FLAG_MODBUS;		
		// 增加版本号信息
		m_wVersion = DRVCFG_VERSION;
		ar << m_wVersion;

		m_cfg.Serialize(ar, m_wVersion);
	}
}

DRVRET  CModDrv::GetErrMsg(CString& szMsg,DRVRET dwcode)			//获取错误信息
{
	if(dwcode < 100)
		return CDrvObj::GetErrMsg(szMsg,dwcode);

	//>100是驱动自定义的，这里没有自定义错误信息，直接返回无此错误代码
	DRVRET lret = DRERR_OK;
	if(dwcode == RESULT_OPENCOMPORTERR)
		szMsg = "打开COM通信口错!";
	else if(dwcode == RESULT_NOCOMTYPEERR)
		szMsg = "不支持的通信方式!";
	else if(dwcode == RESULT_TCPCONNECTSERVERERR)
		szMsg = "连接TCP服务器错误!";
	else 
		lret = DRERR_NOERRCODE;
	return lret;
}

#include "DlgConfig.h"
BOOL	CModDrv::OnConfig()	//配置和配置界面,返回TRUE表示配置修改了
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CDlgConfig dlg;
	dlg.m_pcfg = &(m_cfg);
	dlg.DoModal(); 
	if(m_cfg.IsModify())//配置修改
	{
		TransCfg();//重新将配置文件转换为运行对象
		m_cfg.SetModify(FALSE);
		return TRUE;
	}
	return FALSE;
}

DRVRET	CModDrv::OnStart()			//准备运行,返回
{
	//初始化固定发送包
	int i;
	for(i=0;i<m_ndecsize;i++)
		m_pdec[i]->InitSendFrm();

	//创建通信对象
	if(m_pmodcom != NULL)
	{
		delete m_pmodcom;
		m_pmodcom = NULL;
		return RESULT_OPENCOMPORTERR;
	}
	if(m_cfg.m_nComType == COM_TYPE_485)
	{
		CModCom485 *pcom = new CModCom485;
		//初始化参数
		pcom->InitComPort(m_cfg.m_szPort,
			m_cfg.m_dwBaudRate,
			m_cfg.m_cByteSize,
			m_cfg.m_cParity,
			m_cfg.m_cStopBits,
			m_cfg.m_nTimeOut);
		if(!pcom->OpenPort())
		{
		//	delete pcom;
		//	return RESULT_OPENCOMPORTERR;
			if(m_cbOnLogOk)
			{
				AddErrorFmt("Modbus 485不能打开<%s>!",m_cfg.m_szPort);
			}
		}
		m_pmodcom = pcom;
	}
	else
	{
		CModComTcp *pcom  = new CModComTcp;
		pcom->InitArgs(m_cfg.m_szIP,m_cfg.m_wtcpport,m_cfg.m_bTcpCrc16,m_cfg.m_nTimeOut);
		if(!pcom->OpenPort())
		{
		//	delete pcom;
		//	return RESULT_TCPCONNECTSERVERERR;
			if(m_cbOnLogOk)
			{
				AddErrorFmt("Modbus TCP不能连接到服务器<%s;%d>!",m_cfg.m_szIP,m_cfg.m_wtcpport);
			}
		}
		m_pmodcom = pcom;
	}
	m_pmodcom->m_pDrv = this;
#ifdef _ENABLED_RECONNECT_TIMEOUT
	m_tmLastReconnectTime = ::time(NULL);
#endif

	return DRERR_OK;
}

void CModDrv::WriteDebugLog(BOOL nReadMode, BYTE *pb, int nsize)
{
	if (!m_cfg.m_bSaveLog)
		return;
	
	CString szLog = (nReadMode) ? _T("recv: ") : _T("send: ");
	CString szt;
	for(int i = 0 ; i < nsize ; i++)
	{
		szt.Format("%02X ",pb[i]);
		szLog += szt;
	}
	AddDebug(szLog);
}

UINT	CModDrv::DoRun()//通信处理
{
	m_bRun = TRUE; //不能少这一句

	int		nsendsize;	//发送字节数
	BYTE	ssend[512];	//发送缓冲
	BYTE	sread[512];	//接受缓冲
	int		nrsize;		//接受字节数
	WORD	wAddr;		//寄存器地址
	int i;
	CModDec	*pdec = NULL;	
	
	::CoFileTimeNow(&m_ftStartTime);
	while(!m_bKillThread)//先发送完需要立即发送的包
	{
		if(m_pmodcom == NULL)
		{
			Sleep(50);
			continue;
		}

		// 计算是否达到更新周期
		::CoFileTimeNow(&m_ftEndTime);
		if (GetElapsedTime() < (long)m_cfg.m_nUpdateRate)
		{
			Sleep(50);
			continue;
		}
		memcpy(&m_ftStartTime, &m_ftEndTime, sizeof(FILETIME));

#ifdef _ENABLED_RECONNECT_TIMEOUT
		// 每2个小时断开Socket连接，并重新连接
		time_t tmNow = ::time(NULL);
		if ((tmNow - m_tmLastReconnectTime) > _RECONNECT_TIMEOUT)
		{
			m_pmodcom->ClosePort();
			m_tmLastReconnectTime = tmNow;
			AddDebug(_T("Modbus超时重连"));
		}
#endif

		if(!m_pmodcom->IsPortOpen())
		{
			m_pmodcom->OpenPort();
			continue;
		}

		//{{
		/*
		pdec = m_pdec[0];
		nrsize = m_pmodcom->ReadFrm(sread,512,pdec->m_ncrc16seq);//接收
		if(nrsize > 0)//处理接受数据
		{
			//WriteDebugLog(TRUE, sread, nrsize);
			wAddr = 0x150;
			DoReadFrm(3,wAddr,pdec,sread,nrsize);//处理接收到的数据包，数据包已验证
		}
		Sleep(1);
		continue;
		*/
		//}}

		//检查是否有立即发送帧
		nsendsize = m_randfrm.GetFrm(ssend,512);//取回后删除的
		if(nsendsize > 0)
		{
			//获得设备指针
			pdec = GetDecByAddr(ssend[0]);
			if(pdec == NULL)
				continue;
			wAddr = GetRegAddr(ssend,nsendsize);//取寄存器地址;
			m_pmodcom->WriteFrm(ssend,nsendsize);//发送
			//WriteDebugLog(FALSE, ssend, nsendsize);

			nrsize = m_pmodcom->ReadFrm(sread,512,pdec->m_ncrc16seq);//接收
			if(nrsize > 0)//处理接受数据
			{
				//WriteDebugLog(TRUE, sread, nrsize);
				DoReadFrm(ssend[1],wAddr,pdec,sread,nrsize);//处理接收到的数据包，数据包已验证
			}
			Sleep(1);
			continue;//继续处理需要立即发送的包
		}
		Sleep(1);
		
		//固定的发送包
		for(i = 0; i< m_ndecsize; i++)
		{
			if(m_bKillThread)
				break;
			pdec = m_pdec[i];
			nsendsize = pdec->m_sendfrm.GetNextFrm(ssend,512);//取回后删除的
			if(nsendsize > 0)
			{
				wAddr = GetRegAddr(ssend,nsendsize);//取寄存器地址;
				m_pmodcom->WriteFrm(ssend,nsendsize);//发送
				//WriteDebugLog(FALSE, ssend, nsendsize);

				nrsize = m_pmodcom->ReadFrm(sread,512,pdec->m_ncrc16seq);//接收
				if(nrsize > 0)//处理接受数据
				{
					AddDebugFmt(_T("接收数据包：ucod=%d, wAddr=%d, decidx=%d, size=%d"), (int)ssend[1], (int)wAddr, i, nrsize);

					//WriteDebugLog(TRUE, sread, nrsize);
					DoReadFrm(ssend[1],wAddr,pdec,sread,nrsize);//处理接收到的数据包，数据包已验证
				}
			}
			Sleep(1);
			continue;//继续处理固定发送包
		}
		Sleep(1);
	}
	m_bRun = FALSE; //不能少这一句
	return DRERR_OK;
}

void	CModDrv::OnStop()				//停止运行后的清理工作
{
	if(m_pmodcom != NULL)
	{
		delete m_pmodcom;
		m_pmodcom = NULL;
	}
}

void	CModDrv::Send05(CModDec *pdec,WORD regaddr,int nVal)//发送05命令
{
	WORD wVal;
	if(nVal == 0)
		wVal = 0x0000;
	else
		wVal = 0xFF00;
	BYTE pbuf[256];
	
	pbuf[0] = pdec->m_ucaddr;//设备地址
	pbuf[1] = 5;//命令
	
	WORD wt = regaddr;
	SetWord(&wt,pbuf+2,21);//填写寄存器地址

	SetWord(&wVal,pbuf+4,pdec->m_nwordseq);//填写数据

	WORD crc = CRC_16(pbuf,6);
	SetWord(&crc,pbuf+6,pdec->m_ncrc16seq);//填写CRC16
	
	m_randfrm.AddFrm(pbuf,8);
}

void	CModDrv::Send06(CModDec *pdec,WORD regaddr,WORD wVal)//发送06命令
{
	BYTE pbuf[256];
	
	pbuf[0] = pdec->m_ucaddr;//设备地址
	pbuf[1] = 6;//命令
	
	WORD wt = regaddr;
	SetWord(&wt,pbuf+2,21);//填写寄存器地址

	wt = wVal;
	SetWord(&wt,pbuf+4,pdec->m_nwordseq);//填写数据

	WORD crc = CRC_16(pbuf,6);
	SetWord(&crc,pbuf+6,pdec->m_ncrc16seq);//填写CRC16

	m_randfrm.AddFrm(pbuf,8);
}

void	CModDrv::Send16(CModDec *pdec,WORD regaddr,DWORD dwVal)//发送16命令
{
	BYTE pbuf[256];
	
	pbuf[0] = pdec->m_ucaddr;//设备地址
	pbuf[1] = 16;//命令
	
	WORD wt = regaddr;
	SetWord(&wt,pbuf+2,21);//填写寄存器起始地址 ,2,3

	wt = 2;
	SetWord(&wt,pbuf+4,21);//填写寄存器个数,4,5

	pbuf[6] = 4;//填写字节数,6

	DWORD dwt = dwVal;

	SetDWord(&dwt,pbuf+7,pdec->m_ndwordseq);//填写数据 ,7,8,9,10,

	WORD crc = CRC_16(pbuf,11);
	SetWord(&crc,pbuf+ 11,pdec->m_ncrc16seq);//填写CRC16,11,12

	m_randfrm.AddFrm(pbuf,13);
}
void	CModDrv::Send16(CModDec *pdec,WORD regaddr,float fVal)//发送16命令
{
	BYTE pbuf[256];
	
	pbuf[0] = pdec->m_ucaddr;//设备地址
	pbuf[1] = 16;//命令
	
	WORD wt = regaddr;
	SetWord(&wt,pbuf+2,21);//填写寄存器起始地址 ,2,3

	wt = 2;
	SetWord(&wt,pbuf+4,21);//填写寄存器个数,4,5

	pbuf[6] = 4;//填写字节数,6

	float ft = fVal;

	SetFloat(&ft,pbuf+7,pdec->m_nfloatseq);//填写数据,7,8,9,10

	WORD crc = CRC_16(pbuf,11);
	SetWord(&crc,pbuf+ 11,pdec->m_ncrc16seq);//填写CRC16 ,11,12

	m_randfrm.AddFrm(pbuf,13);
}

void	CModDrv::Send16(CModDec *pdec,WORD regaddr,BYTE *pb,int nsize)//发送16命令
{
	BYTE pbuf[256];
	
	pbuf[0] = pdec->m_ucaddr;//设备地址
	pbuf[1] = 16;//命令
	
	WORD wt = regaddr;
	SetWord(&wt,pbuf+2,21);//填写寄存器起始地址 ,2,3

	wt = nsize/2;
	SetWord(&wt,pbuf+4,21);//填写寄存器个数,4,5

	pbuf[6] = (BYTE)nsize;//填写字节数,6

	for(int i=0;i<nsize;i++)
		pbuf[7 + i] = pb[i];//填写数据
	
	WORD crc = CRC_16(pbuf,7 + nsize);
	SetWord(&crc,pbuf+ 7 + nsize,pdec->m_ncrc16seq);//填写CRC16

	m_randfrm.AddFrm(pbuf,9 + nsize);
}

void CModDrv::TransCfg()//将配置文件转换为运行对象
{
	//删除map里对象
	CString szid;
	CModItemIndex *pmi;
	POSITION pos = m_mapitem.GetStartPosition();
	while(pos!=NULL)
	{
		m_mapitem.GetNextAssoc( pos, szid, pmi );
		delete pmi;
	}
	m_mapitem.RemoveAll();//清空MAP
	DeleteAllDec();//删除所有MODDEC对象
	int i;

	//再重建
	int nitems = 0;
	CModDec *pdec;
	CModDec *pcfgdec;
	int ndecs = m_cfg.GetSize();
	for(i=0;i<ndecs;i++)
	{
		pcfgdec = m_cfg.GetAt(i);
		if(pcfgdec == NULL)
			continue;
		pdec = new CModDec;
		if(pdec == NULL)
			continue;
		//设备参数
		pdec->m_szname = pcfgdec->m_szname;
		pdec->m_ucaddr = pcfgdec->m_ucaddr;
		pdec->m_ncrc16seq = pcfgdec->m_ncrc16seq;
		pdec->m_ndwordseq = pcfgdec->m_ndwordseq;
		pdec->m_nfloatseq = pcfgdec->m_nfloatseq;
		
		//数据项
		int j,n;
		CModItem *pi;
		CModItem	mi;
		n = pcfgdec->GetItems();
		for(j=0;j<n;j++)
		{
			pi = pcfgdec->GetAt(j);
			if(pi == NULL)
				continue;
			mi.m_sname = pi->m_sname;
			mi.m_ucorder = pi->m_ucorder;
			mi.m_uctype = pi->m_uctype;
			mi.m_waddr = pi->m_waddr;
			pdec->Add(&mi);
			nitems++;
		}
		Adddec(pdec);
	}

	if(nitems >0)//重建MAP
	{
		m_mapitem.InitHashTable( ::CalcHashTableSize(nitems) );
	}
	
	int j,n; 
	CModItem	*pi;
	for(i=0;i<m_ndecsize;i++)
	{
		if(m_pdec[i] == NULL)
			continue;
		pdec = m_pdec[i];
		n = pdec->GetItems();
		for(j=0;j<n;j++)
		{
			pi = pdec->GetAt(j);
			if(pi == NULL)
				continue;
			szid = pdec->m_szname + "." + pi->m_sname;
			pmi = new CModItemIndex(pdec,pi);
			if(pmi != NULL)
				m_mapitem.SetAt(szid,pmi);
		}
	}
}

DRVRET  CModDrv::GetTagDef(int nindex,LPDRVTAG  lptag)
{
	CString szID;
	CModDec	*pd;
	CModItem *pi;
	int ni=0,n,i,j,ndec = m_cfg.GetSize();
	for(i=0;i<ndec;i++)
	{
		pd  = m_cfg.GetAt(i);
		if(pd == NULL)
			continue;
		n = pd->GetItems();
		for(j=0;j<n;j++)
		{
			pi = pd->GetAt(j);
			if(pi == NULL)
				continue;
			if(ni < nindex)
			{
				ni++;
				continue;
			}
			else if(ni > nindex)
				return DRERR_NOMORETAG;
			
			

			if(pi->m_ucorder <5)
				lptag->waccess = 0;//设置只读
			else //5, 6,16
				lptag->waccess = 2;//设置只写

			if(pi->m_uctype != CModItem::FLOATV) 
			{
				if((pi->m_ucorder < 3)||(pi->m_ucorder == 5))
					lptag->wtype = DRT_DIGITAL;
				else
					lptag->wtype = DRT_INT32;
			}
			else
				lptag->wtype = DRT_FLOAT32;
			strncpy(lptag->sname,pd->m_szname + "." +pi->m_sname,63);
			strncpy(lptag->sdes,pi->m_szdes,79);
			strncpy(lptag->sunit,pi->m_szunit,15);

			return DRERR_OK;
		}
	}
	return DRERR_NOMORETAG;
}

DRVRET  CModDrv::WriteVar(LPDRREC lpval)
{
	return DRERR_OK;
	/* 暂不支持下写
	//先找到变量定义
	CString szid = lpval->sname;
	CModItemIndex *pi;
	
	if(!m_mapitem.Lookup(szid,pi))
	{
		TRACE("Err: No WriteVar VAR! \n");
		return FALSE;
	}
	
	if((pi->m_pitem == NULL)||(pi->m_pdec == NULL))
		return FALSE;
	if(pi->m_pitem->m_ucorder == 5)
	{
		CString szVal = EcdApi::EcdItemVal2StrDec(lpVal);  // 数据项转换为字符串10进制
		Send05(pi->m_pdec,pi->m_pitem->m_waddr,atoi(szVal));//发送
	}
	else if(pi->m_pitem->m_ucorder == 6)
	{
		if(pi->m_pitem->GetCVLen() != 2)
			return FALSE;//不是单寄存器
		CString szVal = EcdApi::EcdItemVal2StrDec(lpVal);  // 数据项转换为字符串10进制
		Send06(pi->m_pdec,pi->m_pitem->m_waddr,WORD(atoi(szVal)));//发送
	}
	else if(pi->m_pitem->m_ucorder == 16) //写多寄存器
	{
		if(pi->m_pitem->GetCVLen() != 4)
			return FALSE;//不是双寄存器
	
		DWORD dwt;
		float ft;
		CString szVal = EcdApi::EcdItemVal2StrDec(lpVal);  // 数据项转换为字符串10进制

		if(pi->m_pitem->m_uctype != CModItem::FLOATV) //4字节整数
		{
			dwt = (DWORD)atol(szVal);
			Send16(pi->m_pdec,pi->m_pitem->m_waddr,dwt);//发送
		}
		else		//浮点数
		{
			ft = (float)atof(szVal);
			Send16(pi->m_pdec,pi->m_pitem->m_waddr,ft);//发送
		}
	}
	return TRUE;
	*/
}
/*
BOOL CModDrv::WriteVar(LPCTSTR lpszid,LPECDITEMVAL	lpVal)//写变量,用MAP查找，高效率
{
	//先找到变量定义
	CString szid = lpszid;
	CModItemIndex *pi;
	
	if(!m_mapitem.Lookup(szid,pi))
	{
		TRACE("Err: No WriteVar VAR! \n");
		return FALSE;
	}
	
	if((pi->m_pitem == NULL)||(pi->m_pdec == NULL))
		return FALSE;
	if(pi->m_pitem->m_ucorder == 5)
	{
		CString szVal = EcdApi::EcdItemVal2StrDec(lpVal);  // 数据项转换为字符串10进制
		Send05(pi->m_pdec,pi->m_pitem->m_waddr,atoi(szVal));//发送
	}
	else if(pi->m_pitem->m_ucorder == 6)
	{
		if(pi->m_pitem->GetCVLen() != 2)
			return FALSE;//不是单寄存器
		CString szVal = EcdApi::EcdItemVal2StrDec(lpVal);  // 数据项转换为字符串10进制
		Send06(pi->m_pdec,pi->m_pitem->m_waddr,WORD(atoi(szVal)));//发送
	}
	else if(pi->m_pitem->m_ucorder == 16) //写多寄存器
	{
		if(pi->m_pitem->GetCVLen() != 4)
			return FALSE;//不是双寄存器
	
		DWORD dwt;
		float ft;
		CString szVal = EcdApi::EcdItemVal2StrDec(lpVal);  // 数据项转换为字符串10进制

		if(pi->m_pitem->m_uctype != CModItem::FLOATV) //4字节整数
		{
			dwt = (DWORD)atol(szVal);
			Send16(pi->m_pdec,pi->m_pitem->m_waddr,dwt);//发送
		}
		else		//浮点数
		{
			ft = (float)atof(szVal);
			Send16(pi->m_pdec,pi->m_pitem->m_waddr,ft);//发送
		}
	}
	return TRUE;
}
*/

void	CModDrv::DoReadFrm(BYTE ucod,WORD wregaddr,CModDec *pdec,BYTE *pb,int nsize)//处理接收到的数据包，数据包已验证
{
	WORD wAddr = wregaddr;

	BYTE ucorder = pb[1];//命令

	if((pdec->m_ucaddr != pb[0])||(pb[1] != ucod)) // 还要加上命令检查,mydebug 20090713
		return;//不是本设备的通信包

	//BYTE val[8];
	CModItem *pi;
	int i,nIndex = -1,ndatas = pdec->GetItems();
	for(i=0;i<ndatas;i++)
	{
		pi = pdec->GetAt(i);
		if(	(pi->m_ucorder == ucorder)&&
			(pi->m_waddr == wAddr))
		{
			nIndex = i;
			break;
		}
	}
	if(nIndex == -1)
		return ;//没找到开始处

	BYTE* pdata = pb+3;		//数据开始位置
	int ndatabytes = pb[2];	//数据字节数

	long ltime = (long)::time(NULL);
	CRdRec rv;
	CString szvarid;
	if((ucorder == 1)||(ucorder == 2))//读开关量
	{
		WORD wa = wAddr;
		int j,k;
		i = nIndex;
		for(j=0;j<ndatabytes;j++)//字节循环
		{
			for(k=0;k<8;k++)//位循环
			{
				if(i >= ndatas)
					break;
				pi =  pdec->GetAt(i);
			
				if( pi->m_ucorder != ucorder)
					break;//命令不同
				if(pi->m_waddr != wAddr + j * 8 + k)
					break;//地址不对
				
				szvarid = pdec->m_szname + "." + pi->m_sname;
				rv.SetName(szvarid);
				rv.ReSetType(DRT_DIGITAL);

				rv.val.snqa = 0;
				rv.val.ltime = ltime;
				if((pdata[j] & (0x01 << k)) != 0)
					rv.val.lval = 1;
				else
					rv.val.lval = 0;
				if(m_cbOnDataOk)
					m_cbOnDataOk(m_handle,&rv,1);
				i++;//下一数据项
			}
		}
	}
	else if((ucorder == 3)||(ucorder == 4))//读模拟量
	{
		int n=0;
		int ndx=0;				//地址增量
		i = nIndex;
		while(( i < ndatas)&&(n<nsize)&& (n<ndatabytes))
		{
			pi =  pdec->GetAt(i);
			
			if(pi->m_ucorder != ucorder)
				break;

			ndx = pi->GetCVLen();

			if(n+ndx > nsize-3)
				break;
			
			if(ndx == 2)//双字节
			{
				szvarid = pdec->m_szname + "." + pi->m_sname;
				rv.SetName(szvarid);
				rv.ReSetType(DRT_INT32);

				rv.val.snqa = 0;
				rv.val.ltime = ltime;	

				//填写数据
				if (pi->m_uctype == CModItem::SHORTV)
				{
					rv.val.lval = (long)GetShort(pdata + n,pdec->m_nwordseq);
				}
				else {
					rv.val.lval = (long)GetWord(pdata + n,pdec->m_nwordseq);
				}

				if(m_cbOnDataOk)
					m_cbOnDataOk(m_handle,&rv,1);
			}
			else 
			{
				if(pi->m_uctype == CModItem::LONGV)//整数
				{
					//填写数据
					szvarid = pdec->m_szname + "." + pi->m_sname;
					rv.SetName(szvarid);
					rv.ReSetType(DRT_INT32);
					rv.val.snqa = 0;
					rv.val.ltime = ltime;	
					rv.val.lval = GetLong(pdata + n,pdec->m_ndwordseq);
					if(m_cbOnDataOk)
						m_cbOnDataOk(m_handle,&rv,1);
				}
				else if(pi->m_uctype != CModItem::FLOATV)//整数
				{
					//填写数据
					szvarid = pdec->m_szname + "." + pi->m_sname;
					rv.SetName(szvarid);
					rv.ReSetType(DRT_INT32);
					rv.val.snqa = 0;
					rv.val.ltime = ltime;	
					rv.val.lval = (long)GetDWord(pdata + n,pdec->m_ndwordseq);
					if(m_cbOnDataOk)
						m_cbOnDataOk(m_handle,&rv,1);
				}
				else	// float
				{
					//填写数据
					szvarid = pdec->m_szname + "." + pi->m_sname;
					rv.SetName(szvarid);
					rv.ReSetType(DRT_FLOAT32);
					rv.val.snqa = 0;
					rv.val.ltime = ltime;	
					rv.val.fval = GetFloat(pdata + n,pdec->m_nfloatseq);
					if(m_cbOnDataOk)
						m_cbOnDataOk(m_handle,&rv,1);
				}
			}
			i++;
			n += ndx;
		}
	}
	
	//其它不需要处理
}


