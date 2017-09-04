//232端口通信参数在下面修改
//奇校验传地址,偶校验传输据

#include "stdafx.h"
#include "rsc232.h"
CRsc232::CRsc232()
{
	szPortName="\\\\.\\com1";	//端口名，如"\\\\.\\com1","\\\\.\\com2"
	dwBaudRate=CBR_9600;	//波特率,CBR_1200,CBR_2400,CBR_4800,CBR_9600,
									//CBR_14400,CBR_19200,CBR_38400,CBR_57600,CBR_115200,
	cByteSize=	8;			//数据位 number of bits/byte, 4-8
	cParity=		0;			//0-4对应no,odd,even,mark,space 
	cStopBits=	0;			// 0,1,2 对应 1, 1.5, 2
	idComDev=INVALID_HANDLE_VALUE;//通信端口句柄
}
CRsc232::~CRsc232()
{
	Close();
}

BOOL CRsc232::IsOpen()
{
	return (idComDev != INVALID_HANDLE_VALUE);
}

BOOL CRsc232::Open()
{
	if(idComDev!=INVALID_HANDLE_VALUE)
		return 0;
	idComDev =CreateFile( szPortName, GENERIC_READ | GENERIC_WRITE,
                  0,                    // exclusive access
                  NULL,                 // no security attrs
                  OPEN_EXISTING,
                  FILE_ATTRIBUTE_NORMAL,
                  NULL ); 
	if(idComDev==INVALID_HANDLE_VALUE)
		return 1;//CreateFile通信口失败！

	COMMTIMEOUTS  CommTimeOuts ;
	DCB dcb;
	COMMCONFIG cfg;

	cfg.dcb.DCBlength = sizeof( DCB ) ;
	BOOL bRet=FALSE;
	bRet=GetCommState( idComDev, &(cfg.dcb) ) ;

	SetupComm( idComDev, 4096, 4096 ) ;

    // purge any information in the buffer
	PurgeComm( idComDev, PURGE_TXABORT | PURGE_RXABORT |
                                      PURGE_TXCLEAR | PURGE_RXCLEAR ) ;
	dcb.DCBlength = sizeof( DCB ) ;

	bRet=GetCommState( idComDev, &dcb ) ;
	if(!bRet)
	{
		TRACE("GetCommState error!---------\n");
		Close();
		return 2;
	}
	dcb.BaudRate =dwBaudRate;
	dcb.ByteSize =cByteSize;
	dcb.Parity =cParity;
	dcb.StopBits =cStopBits;

   // setup hardware flow control硬件流控制无
	dcb.fDtrControl = DTR_CONTROL_ENABLE;//DTR_CONTROL_DISABLE ; 485转接器专用
	dcb.fOutxCtsFlow = FALSE ;
    dcb.fRtsControl = RTS_CONTROL_DISABLE ;

   // setup software flow control软件流控制无
	dcb.fInX = dcb.fOutX = FALSE ;

   // other various settings
	dcb.fBinary = TRUE ;
	dcb.fParity = TRUE ;
	
   // set up for overlapped I/O
	CommTimeOuts.ReadIntervalTimeout			=	20;//0;字符间超时
	CommTimeOuts.ReadTotalTimeoutMultiplier		=	(2*9600)/dcb.BaudRate;
	CommTimeOuts.ReadTotalTimeoutConstant		=	100;//静态加超时
	CommTimeOuts.WriteTotalTimeoutMultiplier	=(2*9600)/dcb.BaudRate ;
	CommTimeOuts.WriteTotalTimeoutConstant		= 50 ;

	if(!SetCommTimeouts(idComDev , &CommTimeOuts ))
	{
		TRACE("SetCommTimeouts error!---------\n");
		Close();
		return 3;
	}
	if (!SetCommState( idComDev, &dcb ))
	{
		TRACE("SetCommState error!---------\n");
		Close();
		return 4;//
	}
	
	return 0;//设置通信口失败！
}

void CRsc232::Close(void)
{
	if(idComDev!=INVALID_HANDLE_VALUE)
	{
		CloseHandle(idComDev);
		idComDev=INVALID_HANDLE_VALUE;
	}
}

int  CRsc232::Read(char *lpbuf,int size)//
{
	if(idComDev == INVALID_HANDLE_VALUE)
	{
		if(0!=Open())
			return 0;
	}

	COMSTAT comstat;
	DWORD dwErrorMask,dwActRead=0;
	
	ClearCommError(idComDev,&dwErrorMask,&comstat);//清除错误
	PurgeComm(idComDev,PURGE_TXABORT|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_RXCLEAR);//清空驱动程序中的缓冲区

	if(!::ReadFile(idComDev,lpbuf,size,&dwActRead,NULL))	
	{
		ClearCommError(idComDev,&dwErrorMask,&comstat);//清除错误
		PurgeComm(idComDev,PURGE_TXABORT|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_RXCLEAR);//清空驱动程序中的缓冲区
		return 0;
	}
	
	return int(dwActRead);
}

int  CRsc232::Write(char *lpbuf,int size)
{
	if(idComDev == INVALID_HANDLE_VALUE)
	{
		if(0!=Open())
			return 0;
	}

	COMSTAT comstat;
	DWORD dwErrorMask,dwActRead=0;
	
	ClearCommError(idComDev,&dwErrorMask,&comstat);//清除错误
	PurgeComm(idComDev,PURGE_TXABORT|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_RXCLEAR);//清空驱动程序中的缓冲区

	if(!::WriteFile(idComDev,lpbuf,size,&dwActRead,NULL))	
	{
		ClearCommError(idComDev,&dwErrorMask,&comstat);//清除错误
		PurgeComm(idComDev,PURGE_TXABORT|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_RXCLEAR);//清空驱动程序中的缓冲区
		return 0;
	}
	
	return int(dwActRead);
}