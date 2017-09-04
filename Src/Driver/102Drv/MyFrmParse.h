// MyFrmParse.h: interface for the CMyFrmParse class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYFRMPARSE_H__1AD52466_5898_4762_B5F7_DA665B9B6484__INCLUDED_)
#define AFX_MYFRMPARSE_H__1AD52466_5898_4762_B5F7_DA665B9B6484__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\..\SDK\ddclib.h"

// 程序执行状态
enum STATE_OPER{
	S_INIT,			                     // 初始化状态

	S_TIME_REQ,                         // 登陆请求
	S_TIME_REQ_OK,		              // 登陆确认

	S_REQ_LINK,                     //  请求链路
	S_REQ_LINK_ACK,           //  请求链路应答

	S_RESENT_LINK,              //  复位链路
	S_RESENT_LINK_ACK,   //  复位链路应答

	S_DATA_REQ,                                  // 读取数据
	S_DATA_REQ_OK,	                          // 收到数据OK

	S_LEVER1_DATA,              //  发送一级用户数据请求
	S_LEVER2_DATA,              //  发送二级用户数据请求

	S_LOGOFF_REQ,                           // 退出请求
	S_LOGOFF_OK, 	                         // 退出确认

	S_CHEK_PSW_REQ,                       // 校对密码请求
	S_CHEK_PSW_OK 	                      // 校对密码确认
};

// 类型码
enum TYPE_CODE{
	TC_ELEC_0X86            = 0x86,	      // l.	即时定时任务数据库电量数据
};

enum SEND_MSG_LEN{
	LEN_FIX_FRM            = 6,	       // 固定帧长
	LEN_SET_TIME          = 16,          // 退出状态
	LEN_E5                        = 1 		    // E5应答
};

#define  REQ_SUCCESS        0x0
#define  SEND_MAX_FRM   0xAA
#define  DEVISE_FOCTOR  10000
//*************************************DIFINE****************************************
#pragma pack(push)
#pragma pack(1)

typedef struct tagENDFRM //结束帧结构
{
	BYTE btCS;                //CS校验
	BYTE btEFlag;            //结束标识
}ENDFRM, *LPENDFRM;

typedef struct tagFIXFRM //固定帧结构
{
	BYTE btSFlag;            //报文标识
	BYTE btCtrl;               //控制域
	WORD     wAddr ;      //地址域
	ENDFRM pEndFrm;   //结束帧结构
}FIXFRM, *LPFIXFRM;

typedef struct tagVARIHEADER //报头结构
{
	BYTE btSFlag1;           //启动字符
	BYTE btLen;               //帧长
	BYTE btLen2;             //确认帧长
	BYTE btSFlag2;           //启动字符
	BYTE btCtrl;              //控制域
	WORD  wAddr ;            //地址域
}VARIHEADER, *LPVARIHEADER;

typedef struct tagASDUHEADER //ASDU报头结构
{
	BYTE btTI;                 //类型标识
	BYTE btVSQ;            //帧长
	BYTE btCOT;             //确认帧长
	WORD   wElecAddr;   //电能量远方终端设备地址
	BYTE     byItemNum ;  //电量数据项数目
}ASDUHEADER, *LPASDUHEADER;

typedef struct tagTIME_A     //时间信息 a（分至年）
{
	BYTE  btMinute ;                      //分
	BYTE  btHour ;                         //时
	BYTE  btDate;                          //天
	BYTE  btMonth;                       //月
	BYTE  btYear;                          //年
}TIME_A, *LPTIME_A;

typedef struct tagTIME_B     //时间信息 b（毫秒至年）
{
	BYTE  btMS ;                           //毫秒
	BYTE  btSecond ;                     //秒
	BYTE  btMinute ;                      //分
	BYTE  btHour ;                         //时
	BYTE  btDate;                          //天
	BYTE  btMonth;                       //月
	BYTE  btYear;                          //年
}TIME_B, *LPTIME_B;

typedef struct tagITEM_DATA     //带时标的单点信息数据帧---数据类型
{
	DWORD      dwValue;                       //值  
	BYTE       bySqua;                        //质量
}ITEM_DATA, *LPITEM_DATA;

#define  ELE_NUM  5
typedef struct tagELEC_DATA     //电量数据
{
	BYTE      wElecTI;                      //电量数据标识
	ITEM_DATA  Value[ELE_NUM];                      //电量数据数值0
	BYTE       btCheckSum;                  // 保护位
}ELEC_DATA, *LPELEC_DATA;

typedef struct tagINFO_Addr //定时任务标识号
{
	WORD  wAddr;                         //定时任务标识号
}INFO_Addr, *LPINFO_Addr;

typedef struct tagELECNO //定时任务标识号
{
	BYTE  btElecNo;                         //定时任务标识号
}ELECNO, *LPELECNO;



#pragma pack()
//*************************************DIFINE****************************************
#define TCP_BUFFER_SIZE		 1024*8  // 

enum GET_BYTE{
	GET_BYTE_HIGH ,	       // 高位
	GET_BYTE_LOW  ,          // 低位
};

class CMyFrmParse : public CFrmParse  
{
public:
	CMyFrmParse();
	virtual ~CMyFrmParse();
	virtual  int   Parse();//分包,返回m_nStatus
	virtual  int   Parse_E5();//分包,返回m_nStatus
	virtual  int   Parse_Fix();//分包,返回m_nStatus
	virtual  int   Parse_Vari();//分包,返回m_nStatus

	static BYTE        CalaCS(BYTE *pData, int iSize);
	static BYTE        GetByte(BYTE btData, char cType);
	static short   SwitchBytes(short wBytes,BOOL bSwitchBytes);
	static float    SwitchBytes(float fltBytes,BOOL bSwitchBytes);
	static DWORD   SwitchBytes(DWORD wBytes,BOOL bSwitchBytes);
	static DWORD   SwitchBytes(BYTE *pBytes,BOOL bSwitchBytes);
};

#endif // !defined(AFX_MYFRMPARSE_H__1AD52466_5898_4762_B5F7_DA665B9B6484__INCLUDED_)
