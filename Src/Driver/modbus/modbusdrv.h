//////////////////////////////////////////////////////////////
// modbusdrv.h
#pragma once

#include <afxtempl.h>
#include <afxmt.h>
#include <afxsock.h>		// MFC socket extensions

#include "../sdk/drvsdk.h"
#include "../sdk/sdktool.h"
//modbus驱动错误码
#define RESULT_OPENCOMPORTERR			1005	// 打开COM错
#define RESULT_NOCOMTYPEERR				1006	// 不支持的通信方式
#define RESULT_TCPCONNECTSERVERERR		1007	// 连接服务器错误

#include "..\sdk\DrvVersion.h"
#define DRVCFG_VERSION		(DRVCFG_VERSION_USER + 3)	// 第一版

#define MODBUS_REGISTER_MAX	100			// 寄存器最大48个

// 是否允许超时重连
// 对于甘肃公司华家岭电厂的Modbus，运行一段时间后，会出现数据错乱的问题，断开重连可解决该问题
//#define _ENABLED_RECONNECT_TIMEOUT
#define _RECONNECT_TIMEOUT				(3600 * 1) // 两小时

//------------------------------------------------------------------------
// 随机发送队列

#define MAXMODFRMLEN  256

typedef struct tagMODFRM {	// 通信帧
	BYTE	cbuf[MAXMODFRMLEN];//
	int		nlen;

}MODFRM, *LPMODFRM;

class CRandomFrmBuf //随机发送队列，循环队列,取出后不再存在
{
protected:
	MODFRM	frms[256];
	int     nmaxsize;//最大空间 256帧;
	int		nhead;//头
	int		ntail;//尾
public:
	CRandomFrmBuf()
	{
		nmaxsize = 256;
		nhead =0;
		ntail = 0;
	};
	~CRandomFrmBuf(){};

	// nhead == ntail 空
	// (ntail + 1) % nmaxsize == nhead 满

	void Clear() //清空
	{
		nhead = ntail;
	};

	BOOL AddFrm(BYTE *s,int nlen)//添加,尾进头出,只对尾操作，不用锁
	{
		int nh,nt;
		nh = nhead;
		nt = ntail;

		if( ((nt + 1) % nmaxsize) == nh )//满
			return FALSE;
		if((nlen <= 0)||(nlen > MAXMODFRMLEN))
			return FALSE;

		memcpy(frms[nt].cbuf,s,nlen);
		frms[nt].nlen = nlen;

		nt = (nt +1)%nmaxsize;
		ntail = nt;
		return TRUE;
	};

	int GetFrm(BYTE *buf,int nbuflen)//取回,从头取，只对头操作，不用锁
	{
		int nh,nt;
		nh = nhead;
		nt = ntail;
		if(nh == nt)//空
			return 0;

		int nret = frms[nh].nlen;
		if((nret >0) && (nret <= nbuflen))
			memcpy(buf,frms[nh].cbuf,nret);
		else 
			nret = 0;

		nh = (nh + 1)%nmaxsize;
		nhead = nh;
		
		return nret;
	};
};

//------------------------------------------------------------------------
// 循环发送帧缓冲
class CLoopFrmBuf //循环发送帧缓冲,结构数组,只取不删
{
protected:
	MODFRM frms[256];
	int     nmaxsize;//最大空间 256帧;
	int     nsize;//当前帧数
	int		npos;//当前位置
public:
	CLoopFrmBuf()
	{
		nmaxsize = 256;
		nsize=0;
		npos = 0;
	};
	~CLoopFrmBuf(){};

	void Clear()
	{
		nsize=0;
		npos = 0;
	};

	BOOL AddFrm(BYTE *s,int nlen)
	{
		if((nlen > MAXMODFRMLEN)||(nlen <= 0))
			return FALSE;
		if(nsize >= nmaxsize)
			return FALSE;
		memcpy(frms[nsize].cbuf,s,nlen);
		frms[nsize].nlen = nlen;
		nsize++;
		return TRUE;
	};

	int GetNextFrm(BYTE *buf,int nbuflen)//返回字节数
	{
		if(nsize == 0)
			return 0;
		if(npos == nsize)
			npos=0;
		if(frms[npos].nlen > nbuflen)
			return 0;
		memcpy(buf,frms[npos].cbuf,frms[npos].nlen);
		int nret = frms[npos].nlen;
		npos = (npos + 1)%nsize;
		return nret;
	};
};

//------------------------------------------------------------------------
// modbus数据项
class CModItem//
{
public:
	enum datatype { SHORTV = 2,WORDV = 3,LONGV = 4,DWORDV = 5,FLOATV = 6 };//和ECD_DT_XXXX相同

	BYTE    m_ucorder;	//modbus命令号，第一排序 
	WORD	m_waddr;	//寄存器地址，第二排序
	char	m_uctype;	// 采集值类型datatype，DRT_DIGITAL,

	CString m_sname;	//名称
	CString m_szdes;	//描述
	CString m_szunit;	//备注

	CModItem(){
		m_sname = "";
		m_ucorder = 4;	
		m_waddr = 0;		
		m_uctype = SHORTV; 
		m_szdes = "";
		m_szunit = "";
	};
	CModItem(LPCTSTR lpszname,LPCTSTR lpszname2,BYTE ucorder,WORD waddr,BYTE uctype,LPCTSTR lpszdes,LPCTSTR lpszunit){
		m_sname = lpszname;
		m_ucorder = ucorder;	
		m_waddr = waddr;
		m_uctype = uctype; 
		m_szdes = lpszdes;
		m_szunit = lpszunit;
	};
	~CModItem(){};
	virtual void	Serialize(CArchive& ar)
	{
		if(ar.IsStoring()){
			ar << m_waddr ;
			ar << m_ucorder ;	
			ar << m_uctype ; 
			ar << m_sname ;	
			ar << m_szdes;
			ar << m_szunit ;	
		}
		else{
			ar >> m_waddr ;
			ar >> m_ucorder ;	
			ar >> m_uctype ; 
			ar >> m_sname ;
			ar >> m_szdes;
			ar >> m_szunit ;	
		}
	};
	void Copy(CModItem *pi)
	{
		m_sname = pi->m_sname;
		m_szdes = pi->m_szdes;
		m_ucorder = pi->m_ucorder;	
		m_waddr = pi->m_waddr;		
		m_uctype = pi->m_uctype; 
		m_szunit  = pi->m_szunit;
	};
	void SetType(LPCTSTR lpszType)
	{
		CString szType = lpszType;
		szType.MakeUpper();
		if(szType == "SHORT")
			m_uctype = SHORTV;
		else if((szType == "WORD")||(szType == "BIT"))
			m_uctype = WORDV;
		else if(szType == "LONG")
			m_uctype = LONGV;
		else if(szType == "DWORD")
			m_uctype = DWORDV;
		else
			m_uctype = FLOATV;
	};
	CString GetType()
	{
		CString szt;
		if(m_uctype == SHORTV)
			szt = "SHORT";
		else if(m_uctype == WORDV)
		{
			if((m_ucorder == 1)||(m_ucorder == 2)||(m_ucorder == 5))
				szt = "BIT";
			else
				szt = "WORD";
		}
		else if(m_uctype == LONGV)
			szt = "LONG";
		else if(m_uctype == DWORDV)
			szt = "DWORD";
		else
			szt  ="FLOAT";
		return szt;
	};
	int GetCVLen(){
		if(m_uctype < LONGV)
			return 2;
		return 4;
	};
	
};//数据项的定义只管定义，分析计算另外建立数据结构。
typedef CModItem*	LPMODITEM;

//----------------------------------------------------------------------------------
//  MODBUS设备
class CModDec//MODBUS设备
{
public:
	CString m_szname;//设备名称
	BYTE	m_ucaddr;//设备地址
	
	int		m_ncrc16seq;//crc16字节顺序
	int		m_nwordseq;//两字节整数顺序
	int		m_ndwordseq;//4字节整数顺序
	int		m_nfloatseq;//FLOAT字节顺序

	CLoopFrmBuf m_sendfrm;//预置的固定发送包

protected:
	LPMODITEM* m_pbuf;
	long	m_ndatas;//实际数据定义数
	long	m_nmaxitems;
protected:
	void	InitSendFrm0102(int norder);//初始化固定发送3,4命令包
	void	InitSendFrm0304(int norder);//初始化固定发送1,2命令包
public:

	CModDec(){
		m_pbuf = NULL;
		m_ndatas=0;
		m_nmaxitems=0;
		m_ncrc16seq = 12;//默认低字节在前
		m_nwordseq = 21;//两字节整数顺序,默认高字节在前
		m_ndwordseq = 4321;//4字节整数顺序，默认4321
		m_nfloatseq = 2134;//FLOAT字节顺序,默认2134
		m_szname = "";
	};

	~CModDec(){
		if(m_pbuf == NULL)
			return;
		LPMODITEM pi;
		int i;
		for(i=0;i<m_ndatas;i++)
		{
			pi =  m_pbuf[i];
			delete pi;
		}
		free(m_pbuf);
		m_pbuf = NULL;
	};
	virtual void	Serialize(CArchive& ar)
	{
		int i,n;
		CModItem item;
		if(ar.IsStoring()){			
			ar << m_szname;
			ar << m_ucaddr;
			ar << m_ncrc16seq;
			ar << m_nwordseq;
			ar << m_ndwordseq;
			ar << m_nfloatseq;

			ar << m_ndatas;
			for(i=0;i<m_ndatas;i++)
				m_pbuf[i]->Serialize(ar);
		}
		else{
			DeleteObjects();

			ar >> m_szname;
			ar >> m_ucaddr;
			ar >> m_ncrc16seq;
			ar >> m_nwordseq;
			ar >> m_ndwordseq;
			ar >> m_nfloatseq;

			ar >> n;
			for(i=0;i<n;i++)
			{
				item.Serialize(ar);
				Add(&item);
			}
		}
	};
	int	GetItems()//去对象个数
	{
		return m_ndatas;
	};

	LPMODITEM GetAt(int nIndex) //取对象
	{
		if((nIndex < 0)||(nIndex >= m_ndatas))
			return FALSE;
		return m_pbuf[nIndex];
	};

	BOOL  Add(LPMODITEM pi){//添加
		if(	m_ndatas >= m_nmaxitems)
		{
			LPMODITEM* pnew = (LPMODITEM*)malloc( (m_nmaxitems + 256) * sizeof(LPMODITEM));//每次增加256个
			if(pnew == NULL)
				return FALSE;
			if(m_pbuf != NULL)
			{
				memcpy(pnew,m_pbuf,m_nmaxitems * sizeof(LPMODITEM));
				free(m_pbuf);
			}
			m_pbuf = pnew;
			m_nmaxitems+=256;
		}
		LPMODITEM pt = new CModItem;
		if( pt == NULL)
			return FALSE;
		pt->Copy(pi);
		m_pbuf[m_ndatas]=pt;
		m_ndatas++;
		return TRUE;
	};

	static int Comp(const void *p1,const void *p2 )//比较
	{
		LPMODITEM pi1 ,pi2;

		pi1 = *((LPMODITEM *)p1);
		pi2 = *((LPMODITEM *)p2);
		
		if(pi2->m_ucorder > pi1->m_ucorder)
			return -1;
		if(pi2->m_ucorder < pi1->m_ucorder)
			return 1;

		if(pi2->m_waddr > pi1->m_waddr)
			return -1;
		if(pi2->m_waddr < pi1->m_waddr)
			return 1;
		return 0;
	};

	void  sort()//排序
	{
		qsort(m_pbuf, m_ndatas,sizeof(LPMODITEM),Comp);
	};
	void DeleteObjects()//删除所有对象
	{
		if(m_pbuf == NULL)
			return;
		LPMODITEM pi;
		int i;
		for(i=0;i<m_ndatas;i++)
		{
			pi =  m_pbuf[i];
			delete pi;
		}
		free(m_pbuf);
		m_pbuf = NULL;
		m_ndatas=0;
		m_nmaxitems=0;
	};
	LPMODITEM FindItem(LPCTSTR lpszname)//查找
	{
		LPMODITEM pi;
		int i;
		for(i=0;i<m_ndatas;i++)
		{
			pi =  m_pbuf[i];
			if(pi->m_sname == lpszname)
				return pi;
		}
		return NULL;
	};

	BOOL	DelItem(LPCTSTR lpszname)//删除
	{
		LPMODITEM pi;
		int i;
		for(i=0;i<m_ndatas;i++)
		{
			pi =  m_pbuf[i];
			if(pi->m_sname == lpszname)
			{
				delete pi;
				m_pbuf[i] = NULL;
				while(i<m_ndatas - 1)
				{
					m_pbuf[i] = m_pbuf[i+1];
					i++;
				}
				m_ndatas--;
				return TRUE;
			}
		}
		return FALSE;
	};

	void	InitSendFrm();//初始化定时发送
};

//------------------------------------------------------------------------
// MODBUS配置
#define COM_TYPE_485		0
#define COM_TYPE_TCP		1

#define MOD_TYPE_RTU		0
#define MOD_TYPE_ASCII		1

#define MAX_MODBUSDEC  128	//最大设备个数

class CModBusCfg//MODBUS配置
{
protected:
	int m_nsize;//设备个数
	CModDec	*m_pdec[MAX_MODBUSDEC];
	BOOL m_bModify;
public:

	//通信参数
	int		m_nComType;				// 0:COM485, 1:TCP
	int		m_nModType;				//modbus模式 MOD_TYPE_RTU,MOD_TYPE_ASCII,以后备用
	WORD    m_wtcpport;				//TCP端口,标准是502
	CString	m_szIP;					//服务器IP;

	CString m_szPort;				// 端口名，如"\\\\.\\com1","\\\\.\\com2"
	DWORD	m_dwBaudRate;			//波特率,CBR_1200,CBR_2400,CBR_4800,CBR_9600,
									//CBR_14400,CBR_19200,CBR_38400,CBR_57600,CBR_115200,
									//CBR_14400,CBR_19200,CBR_38400,CBR_57600,CBR_115200,
	BYTE	m_cByteSize;			//数据位 number of bits/byte, 4-8
	BYTE	m_cParity;				//0-4对应no,odd,even,mark,space 
	BYTE	m_cStopBits;			// 0,1,2 对应 1, 1.5, 2
	HANDLE  m_hComDev;				//通信端口句柄

	BOOL	m_bTcpCrc16;			//TCP是否带CRC16
	int		m_nTimeOut;				//读超时
	//{{新增属性 by heqing 2010-7-28
	BOOL	m_bSaveLog;				//是否保存调试日志
	UINT	m_nUpdateRate;			//刷新周期
	//}}
	CString m_szRes;				//保留以后使用
	CModBusCfg(){
		m_nComType  = COM_TYPE_485;		//485
		m_nModType =  MOD_TYPE_RTU;		//RTU

		m_wtcpport  = 502;
		m_szIP = "127.0.0.1";

		m_szPort="\\\\.\\com1";			// 端口名，如"\\\\.\\com1","\\\\.\\com2"
		m_dwBaudRate=CBR_9600;			//波特率,CBR_1200,CBR_2400,CBR_4800,CBR_9600,
										//CBR_14400,CBR_19200,CBR_38400,CBR_57600,CBR_115200,
		m_cByteSize=8;					//数据位 number of bits/byte, 4-8
		m_cParity=0;					//0-4对应no,odd,even,mark,space 
		m_cStopBits=0;					// 0,1,2 对应 1, 1.5, 2

		m_nsize = 0;
		for(int i=0;i<MAX_MODBUSDEC;i++)
			m_pdec[i]= NULL;

		m_bModify = FALSE;
		m_bTcpCrc16 = FALSE;
		m_nTimeOut = 200;
		m_szRes = "";

		m_bSaveLog = FALSE;
		m_nUpdateRate = 1000;
	};
	~CModBusCfg(){
		int i;
		for(i=0;i<m_nsize;i++)
			delete m_pdec[i];
	};
	void SetModify(BOOL bModify){m_bModify = bModify;};
	BOOL IsModify(){return m_bModify;};
	CString GetComMsg()
	{
		CString szt;
		if(m_nComType == COM_TYPE_485)
		{
			szt.Format("%s,%d,%d",m_szPort,m_dwBaudRate,m_cByteSize);
			if(m_cStopBits == 0)
				szt+=",1";
			else if(m_cStopBits == 1)
				szt+=",1.5";
			else if(m_cStopBits == 2)
				szt+=",2";
			if(m_cParity == 0)
				szt+=",无校验";
			else if(m_cParity == 1)
				szt+=",odd";
			else if(m_cParity == 2)
				szt+=",even";
			else if(m_cParity == 3)
				szt+=",mark";
			else szt+=",space";
		}
		else
			szt.Format("%s;%d", m_szIP,m_wtcpport);
		return szt;
	};
	BOOL	Add(CModDec *pi)
	{
		if(m_nsize >= MAX_MODBUSDEC)
			return FALSE;
		m_pdec[m_nsize] = pi;
		m_nsize++;
		return TRUE;
	};

	BOOL Delete(LPCTSTR lpszname)//删除对象
	{
		int i;
		for(i=0;i<m_nsize;i++)
		{
			if(m_pdec[i]->m_szname == lpszname)
			{
				delete m_pdec[i];
				m_pdec[i] = NULL;
				while(i< (m_nsize -1))
				{
					m_pdec[i] = m_pdec[i+1];
					i++;
				}
				m_nsize--;
				return TRUE;
			}
		}
		return FALSE;
	};
	BOOL Delete(BYTE ucaddr)//删除对象
	{
		int i;
		for(i=0;i<m_nsize;i++)
		{
			if(m_pdec[i]->m_ucaddr == ucaddr)
			{
				delete m_pdec[i];
				m_pdec[i] = NULL;
				while(i< (m_nsize -1))
				{
					m_pdec[i] = m_pdec[i+1];
					i++;
				}
				m_nsize--;
				return TRUE;
			}
		}
		return FALSE;
	};
	void DeleteObjects()//删除所有对象
	{
		int i;
		for(i=0;i<m_nsize;i++)
		{
			delete m_pdec[i];
			m_pdec[i] = NULL;
		}
		m_nsize = 0;
	}

	int	GetSize()//取设备个数
	{
		return m_nsize;
	};

	CModDec	*GetAt(int nIndex)//取对象
	{
		if((nIndex < 0)||(nIndex >= m_nsize))
			return NULL;
		return m_pdec[nIndex];
	};

	CModDec	*GetDecByAddr(BYTE ucaddr)
	{
		int i;
		for(i=0;i<m_nsize;i++)
		{
			if(m_pdec[i]->m_ucaddr == ucaddr)
				return m_pdec[i];
		}
		return NULL;
	};

	virtual void	Serialize(CArchive& ar, DWORD dwVersion)
	{
		int i,n;
		CModDec *pi;
		if(ar.IsStoring()){		
			ar << m_nComType;	
			ar << m_nModType;
			ar << m_wtcpport;
			ar << m_szIP;
			ar << m_szPort;			
			ar << m_dwBaudRate;								
			ar << m_cByteSize;					
			ar << m_cParity;					
			ar << m_cStopBits;	
			ar << m_bTcpCrc16;
			ar << m_nTimeOut;
			ar << m_nUpdateRate;
			ar << m_bSaveLog;
			ar << m_szRes;

			ar << m_nsize;
			for(i=0;i<m_nsize;i++)
				m_pdec[i]->Serialize(ar);
		}
		else{
			ar >> m_nComType;
			ar >> m_nModType;
			ar >> m_wtcpport;
			ar >> m_szIP;
			ar >> m_szPort;			
			ar >> m_dwBaudRate;								
			ar >> m_cByteSize;					
			ar >> m_cParity;					
			ar >> m_cStopBits;	
			ar >> m_bTcpCrc16;
			ar >> m_nTimeOut;
			if (dwVersion >= DRVCFG_VERSION_USER + 2)
			{
				ar >> m_nUpdateRate;
			}
			if (dwVersion >= DRVCFG_VERSION)
			{
				ar >> m_bSaveLog;
			}
			ar >> m_szRes;

			DeleteObjects();
			ar >> n;
			for(i=0;((i<n) &&(i<MAX_MODBUSDEC));i++)
			{	
				pi = new CModDec;
				pi->Serialize(ar);
				m_pdec[m_nsize] = pi;
				m_nsize++;
			}
		}
	};

	int GetCfgSize()//取配置字节数
	{
		CMemFile mf(4096);
		CArchive ar(&mf,CArchive::store);
		Serialize(ar, DRVCFG_VERSION);
		ar.Close();
		int nl = (int)mf.GetLength();
		mf.Close();
		return nl;
	};
};

//------------------------------------------------------------------------
//
class CModItemIndex //用于CMAP的CModItem索引，用于快速查找
{
public:
	CModItemIndex(){};
	CModItemIndex(CModDec *pdec,CModItem *pitem)
	{
		m_szID = pdec->m_szname + "." + pitem->m_sname;
		m_pdec = pdec;
		m_pitem = pitem;
	};

	~CModItemIndex(){};

	CString		m_szID;		//点名，设备名 + "." + 数据项名
	CModDec		*m_pdec;	//设备指针
	CModItem	*m_pitem;	//数据项指针
};
typedef CMap<CString,LPCTSTR, CModItemIndex*, CModItemIndex*> CModItemMap; //CModItem索引MAP

class CModDrv;

//------------------------------------------------------------------------
//
class CModCom : public CObject//modbus通信基类,其他比如485，TCP从此基类集成而来，同一接口
{
public:
	CModCom(){};
	~CModCom(){};

	CModDrv* m_pDrv;
public:

	//下面定义的纯虚函数，必须重载后才能用
	virtual BOOL IsPortOpen() = 0;//端口是否打开
	virtual BOOL OpenPort() = 0;//打开端口
	virtual void ClosePort() = 0;//关闭端口

	virtual int WriteFrm(BYTE *pb,int nsize) = 0;//发送modbus通信包,返回发送的字节数
				//发送的通信包为标准的MODBUS包，如果是TCP通信，则要由通信模块自己加上头部6字节

	virtual int ReadFrm(BYTE *pbuf,int nbufsize,int ncrc16seq) = 0;//接受modbus通信包,返回接收已验证包的字节数
				//标准MODBUS数据包，如果是TCP包，则要去掉前面的6个字节
};

//------------------------------------------------------------------------
//
class CModCom485 : public CModCom //485通信方式
{
public:
	CModCom485();
	~CModCom485();
protected:
	//通信参数
	CString m_szPort;				// 端口名，如"\\\\.\\com1","\\\\.\\com2"
	DWORD	m_dwBaudRate;			//波特率,CBR_1200,CBR_2400,CBR_4800,CBR_9600,
									//CBR_14400,CBR_19200,CBR_38400,CBR_57600,CBR_115200,
									//CBR_14400,CBR_19200,CBR_38400,CBR_57600,CBR_115200,
	BYTE	m_cByteSize;			//数据位 number of bits/byte, 4-8
	BYTE	m_cParity;				//0-4对应no,odd,even,mark,space 
	BYTE	m_cStopBits;			// 0,1,2 对应 1, 1.5, 2
	HANDLE  m_hComDev;				//通信端口句柄
	
	int		m_nTimeOut;				//超时
	int		m_nerrnum;//错误次数,用于监测串口错误，自复位用
public:
	
	void InitComPort(
		LPCTSTR lpszComName,// 端口名，如"\\\\.\\com1","\\\\.\\com2"
		DWORD dwBaudRate,	// 波特率,
		BYTE cByteSize,		// 数据位 number of bits/byte, 4-8
		BYTE cParity,		// 0-4=no,odd,even,mark,space 
		BYTE cStopBits,		// 0,1,2 = 1, 1.5, 2 )
		int  n_TimeOut
	);//初始化通信口

	//{{必须重载的函数
	virtual BOOL IsPortOpen();//端口是否打开
	virtual BOOL OpenPort();//打开端口
	virtual void ClosePort();//关闭端口

	virtual int WriteFrm(BYTE *pb,int nsize);
	virtual int ReadFrm(BYTE *pbuf,int nbufsize,int ncrc16seq);	
	//}}

protected:
	BOOL	SendData(BYTE *pc,int nSize);
	int		ReadData(BYTE *pc,int nSize);
};

//------------------------------------------------------------------------
// 
class CModComTcp : public CModCom //TCP通信方式
{
public:
	CModComTcp();
	~CModComTcp();
protected:
	//通信参数
	CString m_szSvrIp;	//服务器IP
	int		m_nSvrPort;	//服务端口
	BOOL	m_bTcpCrc16;//是否使用CRC16
	int		m_nTimeOut; //读超时
	SOCKET	m_sClient;
public:
	
	void	InitArgs(LPCTSTR lpszip,int nport,BOOL bTcpCrc16,int nTimeOut)//初始化参数
	{
		m_szSvrIp = lpszip;
		m_nSvrPort = nport;
		m_bTcpCrc16 = bTcpCrc16;
		m_nTimeOut = nTimeOut;
	};

	//{{必须重载的函数
	virtual BOOL IsPortOpen();//端口是否打开
	virtual BOOL OpenPort();//打开端口
	virtual void ClosePort();//关闭端口

	virtual int WriteFrm(BYTE *pb,int nsize);
	virtual int ReadFrm(BYTE *pbuf,int nbufsize,int ncrc16seq);
	//}}

protected:
	int		Read(SOCKET s,BYTE *pbuf,int size,int ndelaymsec);
	int		Write(SOCKET s,BYTE *pbuf,int size,int ndelaymsec);
};

class CModDrv : public CDrvObj//Modbus 驱动类,必须从CEcdDrv继承而来
{
protected:
	//{{驱动特有的属性
	int		m_ndecsize;				//设备个数
	CModDec	*m_pdec[MAX_MODBUSDEC];	//设备对象数组
	//}}
#ifdef _ENABLED_RECONNECT_TIMEOUT
	time_t	m_tmLastReconnectTime;
#endif
public:
	CModDrv();
	~CModDrv();

	void WriteDebugLog(BOOL nReadMode, BYTE *pb, int nsize); // 写Debug日志
protected:
	WORD m_wVersion;
	//{{驱动特有的属性
	CModCom				*m_pmodcom;		//modbus通信类
	CModBusCfg			m_cfg;			//配置
	CModItemMap			m_mapitem;		//所有数据点定义MAP
	CRandomFrmBuf		m_randfrm;		//随机发送包,用于缓冲5,6数据下行发送包
	//}}
	void	TransCfg();//将配置文件转换为运行对象

	//以下重载基类的函数
public: //虚函数
	virtual DRVRET  GetErrMsg(CString& szMsg,DRVRET dwcode);			//获取错误信息
	virtual DRVRET  WriteVar(LPDRREC lpval);			//写变量
	virtual DRVRET  GetTagDef(int nindex,LPDRVTAG  lptag);
					
protected: //纯虚函数
	virtual BOOL	OnConfig();					//配置和配置界面,返回TRUE表示修改了配置
	virtual void	Serialize(CArchive& ar);	//串行化保存，读取配置
	virtual DRVRET	OnStart();					//准备运行
	virtual void	OnStop();					//停止运行后的清理工作
	virtual UINT	DoRun();					//正真的运行过程，被RunThread中调用

protected:
	void	DoReadFrm(BYTE ucod,WORD wregaddr,CModDec *pdec,BYTE *pb,int nsize);//处理接收到的数据包，数据包已验证
	
	void	Send05(CModDec *pdec,WORD regaddr,int nVal);//发送05命令
	void	Send06(CModDec *pdec,WORD regaddr,WORD wVal);//发送06命令
	void	Send16(CModDec *pdec,WORD regaddr,BYTE *pb,int nsize);//发送16命令

	void	Send16(CModDec *pdec,WORD regaddr,DWORD dwVal);//发送16命令
	void	Send16(CModDec *pdec,WORD regaddr,float fVal);//发送16命令

public:
	//{{ 字节顺序转换
	static	short	GetShort(BYTE *pb,int nseq);//
	static	WORD	GetWord(BYTE *pb,int nseq);//
	static	long	GetLong(BYTE *pb,int nseq);//
	static	DWORD	GetDWord(BYTE *pb,int nseq);//
	static	float	GetFloat(BYTE *pb,int nseq);//

	static  void	SetWord(void* pv,BYTE *pb,int nseq);
	static  void	SetDWord(void* pv,BYTE *pb,int nseq);
	static  void	SetFloat(void* pv,BYTE *pb,int nseq);
	static  WORD	GetRegAddr(BYTE *pb,int nsize)//取寄存器地址
	{
		if(nsize < 4)
			return 0;
		WORD wAddr = 0;
		wAddr = pb[2];
		wAddr <<= 8 ;
		wAddr += pb[3];

		return wAddr;
	};
	//}}
	BOOL	Adddec(CModDec *pi)
	{
		if(m_ndecsize >= MAX_MODBUSDEC)
			return FALSE;
		m_pdec[m_ndecsize] = pi;
		m_ndecsize++;
		return TRUE;
	};

	void DeleteAllDec()//删除所有对象
	{
		int i;
		for(i=0;i<m_ndecsize;i++)
		{
			delete m_pdec[i];
			m_pdec[i] = NULL;
		}
		m_ndecsize = 0;
	};

	int	GetSize()//取设备个数
	{
		return m_ndecsize;
	};

	CModDec	*GetAt(int nIndex)//取对象
	{
		if((nIndex < 0)||(nIndex >= m_ndecsize))
			return NULL;
		return m_pdec[nIndex];
	};

	CModDec	*GetDecByAddr(BYTE ucaddr)
	{
		int i;
		for(i=0;i<m_ndecsize;i++)
		{
			if(m_pdec[i]->m_ucaddr == ucaddr)
				return m_pdec[i];
		}
		return NULL;
	};
	FILETIME		m_ftStartTime;				// 开始时间
	FILETIME		m_ftEndTime;				// 终止时间
	
	long GetElapsedTime() {	// 计算毫秒级时差
		ULONGLONG qst = *((ULONGLONG*)(&m_ftStartTime));
		ULONGLONG qet = *((ULONGLONG*)(&m_ftEndTime));
		return (long)((qet - qst) / 10000);
	}
};
