// 组包解包工具
#pragma once

#define FRM_SYN_WORD	0XA99AA99A
#define FRM_SYN_BYTE0	0x9A
#define FRM_SYN_BYTE1	0xA9
#define FRM_SYN_BYTE2	0x9A
#define FRM_SYN_BYTE3	0xA9
/*
	组包类
  */
#define MAX_FRM_LEN		(MAX_BYTEARRAYSIZE - 128)	//最大报文长度 4M

class CMakeFrm
{
public:
	CMakeFrm(){};
	~CMakeFrm(){};
public:
	BOOL MakeFrm(BYTE ucOrder,BYTE ucSubOrder,BYTE ucEndFlag,BYTE ucErrCode,BYTE *pData,int nDataSize)
	{
		if((nDataSize + 16) > MAX_FRM_LEN)
			return FALSE;

		m_ByteArray.ClearData();

		BYTE ucbuf[32];
		DWORD *pdw = (DWORD *)ucbuf;
		pdw[0] = FRM_SYN_WORD;
		pdw[1] = 16 + nDataSize;
		ucbuf[8] = ucOrder;
		ucbuf[9] = ucSubOrder;
		ucbuf[10] = ucEndFlag;
		ucbuf[11] = ucErrCode;
		pdw[3] = Utility::GetCRC32(ucbuf,12);

		m_ByteArray.PutData(ucbuf,16);

		m_ByteArray.PutData(pData,nDataSize);
		return TRUE;
	}; //组包

	BOOL MakeFrm(BYTE ucOrder,BYTE ucSubOrder,BYTE ucEndFlag,BYTE ucErrCode,LPCTSTR pData,int nDataSize)
	{
		return MakeFrm(ucOrder,ucSubOrder,ucEndFlag,ucErrCode,(BYTE *)pData,nDataSize);
	}; //组包

	BYTE *GetFrm(int *nsize)
	{
		*nsize = m_ByteArray.GetDataSize();
		if(*nsize > 0)
			return m_ByteArray.GetBuf();
		return NULL;
	}; //取报文

	static BOOL MakeFrm(CMyByteArray *pba,BYTE ucOrder,BYTE ucSubOrder,BYTE ucEndFlag,BYTE ucErrCode,BYTE *pData,int nDataSize)
	{
		if((nDataSize + 16) > MAX_FRM_LEN)
			return FALSE;

		pba->ClearData();

		BYTE ucbuf[32];
		DWORD *pdw = (DWORD *)ucbuf;
		pdw[0] = FRM_SYN_WORD;
		pdw[1] = 16 + nDataSize;
		ucbuf[8] = ucOrder;
		ucbuf[9] = ucSubOrder;
		ucbuf[10] = ucEndFlag;
		ucbuf[11] = ucErrCode;
		pdw[3] = Utility::GetCRC32(ucbuf,12);

		pba->PutData(ucbuf,16);
		pba->PutData(pData,nDataSize);
		return TRUE;
	}; //组包
protected:
	CMyByteArray m_ByteArray; //存放数据的动态数组

};

/*------------------------------------------------------------------------------------

  报文解析 CFrmParse,每个客户连接后就创建一个该对象
	0:没找到报头标志
	1:找到报头标志,长度不够无法验证报头
	2:报头校验完成,数据长度不够
	3:报文完成,
  */
#define FRMP_NOHEAD		0	//没找到报头
#define FRMP_HEADOK		1	//找到报头
#define FRMP_CHKOK		2	//校验正确
#define FRMP_FRMOK		3	//报文完成
class CFrmParse //报文解析,只是分包
{
public:
	CFrmParse()
	{
		m_nStatus = FRMP_NOHEAD;
		m_nFrmLen = 0;	
	};
	~CFrmParse(){};
public:
	CMyByteArray	m_Frm;	//已分离出的报文，含报头
public:
	int		PutData(BYTE *buf,int nsize)//放入未解析数据
	{
		if((buf == NULL)||(nsize == 0))
			return Parse();

		if(m_nStatus == 2) //报头完整
		{
			int n=m_Frm.GetDataSize();
			if( ( n + nsize) < m_nFrmLen)
			{
				m_Frm.PutData(buf,nsize);
				return 2;
			}
			else 
			{
				
				m_Frm.PutData(buf,m_nFrmLen - n);
				m_buf.PutData(buf + (m_nFrmLen - n),nsize - (m_nFrmLen - n));//剩下的放到待处理区
				m_nStatus = 3;
				return 3;
			}
		}
		m_buf.PutData(buf,nsize);//放入缓冲区
		return Parse();
	};

	int		GetStatus(){return m_nStatus;};
	void	SetStatus(int nst){m_nStatus = nst;};
	int		Parse()//分包,返回m_nStatus
	{
		int nsize=m_buf.GetDataSize();
		if(nsize < 16)
			return 0;
		BYTE *pc = m_buf.GetBuf();
		
		//无回朔扫描比较
		int np=0;//np指示向前移动字节数
		BOOL bFindHead=FALSE;

	lpFindHeadFlag:
		while(np <= (nsize - 4))
		{
			if(pc[np] != FRM_SYN_BYTE0){
				np++;	continue;
			}
			if(pc[np+1] != FRM_SYN_BYTE1){
				np++;		continue;
			}
			if(pc[np+2] != FRM_SYN_BYTE2){
				np += 2;	continue;
			}
			if(pc[np+3] != FRM_SYN_BYTE3){
				np += 3;	continue;
			}
			bFindHead = TRUE;
			break;
		}
		if(!bFindHead)	//没找到报文头标志
		{
			m_buf.LeftMove(np);
			return FRMP_NOHEAD;
		}
		if((np +16) > nsize)
		{
			m_buf.LeftMove(np);//长度不够
			return FRMP_HEADOK;
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
		if(*pdw > MAX_FRM_LEN) //报文过长，可能是错误的报文
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
			m_nStatus = FRMP_FRMOK;
			return FRMP_FRMOK;
		}

		m_Frm.ClearData();//清空报文
		m_Frm.PutData(pc+np,nsize - np);//复制余下的字节
		m_buf.ClearData();//清空待处理缓冲
		m_nStatus = FRMP_CHKOK;
		return FRMP_CHKOK;//报头正确
	};
	static	BOOL CheckCrc32(BYTE *pd) //按照16字节报头校验
	{
		DWORD dwCRC32 = Utility::GetCRC32(pd,12);
		DWORD *pcrc = (DWORD *)(pd+12);
		if(*pcrc != dwCRC32)
			return FALSE;
		return TRUE;
	};
protected:
	int				m_nFrmLen;		//报文长度
	int				m_nStatus;		//0:无，1：报头开始标志找到，2：报头完成，3：报文完成
	CMyByteArray	m_buf;			//待处理字节
	
};