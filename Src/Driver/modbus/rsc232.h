/*
rsc232.h
  */
#pragma once

class CRsc232
{
public:
	CRsc232();
	~CRsc232();

	
	CString szPortName;//="\\\\.\\com1";	//端口名，如"\\\\.\\com1","\\\\.\\com2"
	DWORD dwBaudRate;//=CBR_1200;	//波特率,CBR_1200,CBR_2400,CBR_4800,CBR_9600,
									//CBR_14400,CBR_19200,CBR_38400,CBR_57600,CBR_115200,
	BYTE  cByteSize;//=	8;			//数据位 number of bits/byte, 4-8
	BYTE  cParity;//=		0;			//0-4对应no,odd,even,mark,space 
	BYTE  cStopBits;//=	0;			// 0,1,2 对应 1, 1.5, 2

	HANDLE  idComDev;//=INVALID_HANDLE_VALUE;//通信端口句柄

	
	int Open();
	void Close();

	int  Read(char *lpbuf,int size);//
	int  Write(char *lpbuf,int size);
	BOOL IsOpen();
};