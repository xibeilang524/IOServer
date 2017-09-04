//pagefile.cpp 页面文件 

#include "stdafx.h"
#include "pagefile.h"
#include <io.h>

//CLogger *GetLogger();
////////////////////////////////////////////////////
CPfRecVal::CPfRecVal()
{
	m_szdes = "";//描述
	m_ltime = 0;//更新时间
	m_ntype = 0; //类型
	m_nszie = 0; //数据字节数
	m_lbufsize =0 ;
	m_pdata = NULL;

	m_snqa = 0;  //质量
	m_wres = 0;		//保留
	m_dwres = 0;		//保留

}
CPfRecVal::~CPfRecVal()
{
	if(m_pdata)
		free(m_pdata);
}

void	CPfRecVal::ResetBufSize(long lsize) //重置大小
{
	if(m_pdata)
		free(m_pdata);
	m_nszie = 0;
	m_lbufsize = 0;
	if(lsize <= 0)
		return ;
	m_lbufsize = (lsize/8) * 8 + 8;
	m_pdata = malloc(m_lbufsize);
	if(m_pdata == NULL)
		m_lbufsize = 0;
}

BOOL	CPfRecVal::SetData(void* pdata,long lsize)//设置数据
{

	if(m_pdata)
		free(m_pdata);
	m_lbufsize = 0;
	m_nszie = 0;
	if((pdata == NULL) || (lsize <= 0))
		return TRUE;
	m_lbufsize = (lsize/8) * 8 + 8;
	m_pdata = malloc(m_lbufsize);
	if(m_pdata == NULL)
	{
		m_lbufsize = 0;
		return FALSE;
	}
	memcpy(m_pdata,pdata,lsize);
	m_nszie = lsize;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
CPageFile::CPageFile()
{
	m_lgrownsize = 16 * 1024 * 1024;//默认16M增量
	m_bOpen = FALSE;
	m_szFile = "";

	memset(&m_head,0,sizeof(PFHEAD));
	m_head.dwfalg = 0x81018202;
	m_head.lpages = 0;
	m_head.lindexs = 0;
	m_head.lgrownsize = m_lgrownsize;

	m_indexs = (LPPFINDEX)malloc(PF_MAXINDEXS * sizeof(PFINDEX));
	m_pages = (long*)malloc( PF_MAXPAGES * sizeof(long));

	if(m_indexs)
		memset(m_indexs,0,PF_MAXINDEXS * sizeof(PFINDEX));
	if(m_pages)
		memset(m_pages,0,PF_MAXPAGES * sizeof(long));
}


CPageFile::~CPageFile()
{
	if(m_indexs)
		free(m_indexs);
	if(m_pages)
		free(m_pages);

	m_indexs = NULL;
	m_pages = NULL;

	m_bNeedFlush = FALSE;
}

void CPageFile::Clear()	//清空索引和块标记
{
	m_szFile = "";
	memset(&m_head,0,sizeof(PFHEAD));
	//清空索引
	if(m_indexs)
		memset(m_indexs,0,PF_MAXINDEXS * sizeof(PFINDEX));

	//清空页面标记
	if(m_pages)
		memset(m_pages,0,PF_MAXPAGES * sizeof(long));
}

//void g_AddLog(long ltime,LPCTSTR lpszTxt);
void		CPageFile::EmptyFile()//清空，截取多余的文件尾
{
	if(m_file.GetLength() > 16 * 1024 * 1024)
	{
		Clear();
		//写入文件头
		m_head.dwfalg = 0x81018202;
		m_head.lgrownsize = m_lgrownsize;//文件增量
		m_file.SetLength(16 * 1024 * 1024);//16M 初始文件大小
		m_head.lpages = (16 * 1024 * 1024 - PF_DATAPOS)/PF_PAGESIZE;//初始页面数
		m_file.SeekToBegin();
		m_file.Write(&m_head,sizeof(PFHEAD));
		m_file.Write(m_indexs,PF_MAXINDEXS * sizeof(PFINDEX));
		m_file.Write(m_pages,PF_MAXPAGES * sizeof(long));
		m_file.Flush();
		TRACE("EmptyPageFile\n");
		GetLogger()->AddInfo("当前可变类型缓存cashe发送完成!");
	}
}

void	CPageFile::CloseAndDelFile()//关闭和删除文件
{
	CString szFile = m_szFile;
	Close();
	if(!szFile.IsEmpty())
	{
		::DeleteFile(szFile);

		GetLogger()->AddInfoFmt("<%s>页面文件发送完毕!",szFile);
	}
}

int	CPageFile::Close()					//关闭
{
	Clear();
	if(!m_bOpen)
		return PFR_OK;
	try
	{
		m_file.Close();
	}
	catch ( CFileException *ex ) 
	{
	   ex->Delete();
	}
	m_bOpen = FALSE;
	return PFR_OK;
}

void CPageFile::TraceIndex()
{
	int i;
	for(i=0;i<PF_MAXINDEXS;i++)
	{
		if(m_indexs[i].ltime)
			TRACE("index[%d].ltime = %d,sp = %d\n",i,m_indexs[i].ltime ,m_indexs[i].lpageno);
	}
}

void CPageFile::TracePageMark()
{
	int i;
	for(i=0;i<m_head.lpages;i++)
	{
		if(m_pages[i])
			TRACE("m_pagse[%d] = %d\n",i,m_pages[i]);
	}
}

/*
	打开页面文件,如果已经打开则先关闭。
	参数：
		lpszFile 文件名
	返回值：
		成功:PFR_OK，PFR_MEMERR，PFR_NOOPEN，PFR_ERR  

  */
int	CPageFile::Open(LPCTSTR lpszFile)		//打开
{
	if((m_indexs == NULL)||(m_pages == NULL))
		return PFR_MEMERR;
	
	Close();
	//读入文件头
	UINT uread = 0;
	try
	{
		if(!m_file.Open(lpszFile,CFile::modeReadWrite |  CFile::shareExclusive))
			return PFR_NOOPEN;

		uread = m_file.Read(&m_head,sizeof(PFHEAD));
		if(m_head.dwfalg != 0x81018202)
		{
			m_file.Close();
			return PFR_NOTPF;
		}
		//读入索引
		uread = m_file.Read(m_indexs,PF_MAXINDEXS * sizeof(PFINDEX));
		if(uread != PF_MAXINDEXS * sizeof(PFINDEX))
		{
			m_file.Close();
			return PFR_NOTPF;
		}
//		TraceIndex(); //mydebug
		uread = m_file.Read(m_pages,PF_MAXPAGES * sizeof(long));
		if(uread != PF_MAXPAGES * sizeof(long))
		{
			m_file.Close();
			return PFR_PAGEMARKERR;
		}
//		TracePageMark();//mydebug
		m_szFile = lpszFile;
		
		if(m_head.lgrownsize < (1024 * 1024))
			m_head.lgrownsize= 1024 * 1024;
		if(m_head.lgrownsize > 512 * 1024 * 1024)
			m_head.lgrownsize = 512 * 1024 * 1024;

		m_lgrownsize = m_head.lgrownsize;
		
		m_bOpen = TRUE;
		return PFR_OK;
	}
	catch ( CFileException *ex ) 
	{
	   ex->Delete();
	}
	m_file.Close();
	return PFR_ERR;
}

/*
	创建页面文件
	参数：
		lpszFile 页面文件名
	返回值：
		PFR_OK，PFR_FILEEXIST，PFR_ERR
  */
int	CPageFile::Create(LPCTSTR lpszFile)	//创建,初始分配16M
{
	//检查文件是否存在
	if(0 == _access(lpszFile,0))
		return PFR_FILEEXIST;//文件存在
	Close();
	try
	{
		if(!m_file.Open(lpszFile,CFile::modeReadWrite | CFile::shareExclusive | CFile::modeCreate))
			return PFR_ERR;//创建失败

		//写入文件头
		m_head.dwfalg = 0x81018202;
		m_head.lgrownsize = m_lgrownsize;//文件增量
		m_file.SetLength(16 * 1024 * 1024);//16M 初始文件大小
		m_head.lpages = (16 * 1024 * 1024 - PF_DATAPOS)/PF_PAGESIZE;//初始页面数
		m_file.SeekToBegin();
		m_file.Write(&m_head,sizeof(PFHEAD));
		m_file.Write(m_indexs,PF_MAXINDEXS * sizeof(PFINDEX));
		m_file.Write(m_pages,PF_MAXPAGES * sizeof(long));
		m_file.Flush();
		m_bOpen = TRUE;
		m_szFile = lpszFile;
		return PFR_OK;
	}
	catch ( CFileException *ex ) 
	{
	   ex->Delete();
	}
	return PFR_ERR;//创建失败
}

void CPageFile::Flush()
{
	if(!m_bOpen)
		return ;//没有打开
	if(!m_bNeedFlush)
		return;
	try
	{
		m_file.Flush();
		m_bNeedFlush = FALSE;
	}
	catch ( CFileException *ex ) 
	{
	   ex->Delete();
	}
}

long CPageFile::GetEmptyPages()//取空闲的页面数
{
	int i,n=0;
	for(i=0;i<m_head.lpages;i++)
	{
		if(m_pages[i] == 0)
			n++;
	}
	return n;
}

long CPageFile::GetOneEmptyIndex()//取一个空闲索引位置
{
	for(int i=0;i<PF_MAXINDEXS;i++)
	{
		if(m_indexs[i].ltime == 0)
			return i;
	}
	return -1;
}

int	CPageFile::GetNextEmptyPage(int nstartpos)
{
	int i;
	for(i=nstartpos;i<m_head.lpages;i++)
	{
		if(m_pages[i] == 0)
			return i;
	}
	//分配新磁盘空间
	if(m_head.lpages < PF_MAXPAGES)
	{
		long lnewpages = m_head.lpages + m_lgrownsize/PF_PAGESIZE;
		if(lnewpages > PF_MAXPAGES)
			lnewpages = PF_MAXPAGES;
		try
		{
			m_file.SetLength(lnewpages * PF_PAGESIZE + PF_DATAPOS);
			m_head.lpages = lnewpages;
			m_file.SeekToBegin();
			m_file.Write(&m_head,sizeof(PFHEAD));
		}
		catch ( CFileException *ex ) 
		{
		   ex->Delete();
		   return -1;
		}
		while(i<m_head.lpages)
		{
			if(m_pages[i] == 0)
				return i;
			i++;
		}
	}
	return -1;
}

int	CPageFile::GetNextEmptyIndex(int nstart)//取下一个可用的索引号
{
	int	i;
	for(i=nstart;i<PF_MAXINDEXS;i++)
	{
		if(m_indexs[i].ltime == 0)
			return i;
	}
	return -1;
}

int	CPageFile::PF_GetPages(LPPFINDEX pf)//取一个索引所占页面数
{
	int n=0;
	long lno = pf->lpageno;
	while(lno != PF_PAGEEND)
	{
		n++;
		lno = m_pages[lno];
	}
	return n;
}

/*
	添加页面，返回添加的页面号,-1表示错误
  */
int	CPageFile::PF_AddPage(PF_HANDLE h)//添加一个页面
{
	LPPFINDEX pf = m_indexs + h;
	long lpno = pf->lpageno;
	while(m_pages[lpno] != PF_PAGEEND)
		lpno = m_pages[lpno];

	long lnewp = GetNextEmptyPage(0);
	if(lnewp == -1)
		return -1;
	m_pages[lpno] = lnewp;
	m_pages[lnewp] = PF_PAGEEND;
	try
	{
		m_file.Seek(PF_PAGEMARKPOS + lpno * sizeof(long),CFile::begin);
		m_file.Write(&(m_pages[lpno]), sizeof(long));//写前一个

		m_file.Seek(PF_PAGEMARKPOS + lnewp * sizeof(long),CFile::begin);
		m_file.Write(&(m_pages[lnewp]), sizeof(long));//写最后一个
	}
	catch ( CFileException *ex ) 
	{
	   ex->Delete();
	   //恢复
	   m_pages[lpno] = PF_PAGEEND;
	   m_pages[lnewp] = 0;

	   return -1;
	}
	return lnewp;
}

/*
	定位页面文件offset处页面号
	返回页面号，-1表示失败
  */
int	CPageFile::PF_GetPageNo(LPPFINDEX pindex,long offset)
{
	long lpages = offset/PF_PAGESIZE;
	int n=0;
	long lpageno = pindex->lpageno;
	while(lpageno != PF_PAGEEND)
	{
		if(n == lpages)
			return lpageno;
		lpageno = m_pages[lpageno];
		n++;
	}
	return -1;
}

int	CPageFile::PF_Delete(PF_HANDLE h)//删除
{
	LPPFINDEX pf = m_indexs + h;

	//清除所占页面
	long lmk=0;
	int npr=-1,npno = pf->lpageno;
	try
	{
		while(npno != PF_PAGEEND)
		{
			npr = npno;
			npno = m_pages[npno];
			m_pages[npr] = 0; //标记为可用

			m_file.Seek(PF_PAGEMARKPOS + npr * sizeof(long),CFile::begin);
			m_file.Write(&lmk, sizeof(long)); //同步到磁盘
		}
		m_indexs[h].ltime = 0;//删除索引
		if(h == m_head.lindexs-1)
		{
			m_head.lindexs--;
			m_file.SeekToBegin();
			m_file.Write(&m_head,sizeof(PFHEAD));
		}
		m_file.Seek(PF_INDEXPOS + h * sizeof(PFINDEX),CFile::begin);
		m_file.Write(&(m_indexs[h]),sizeof(PFINDEX)); //同步到磁盘
		m_file.Flush();
		m_bNeedFlush = FALSE;
	}
	catch( CFileException *ex ) 
	{
	   ex->Delete();
	   return PFR_ERR;
	}
	
	return PFR_OK;
}

/*
	写文件
	h 页面文件句柄
	offset 位置: -1表示在文件尾加入
	pbuf: 数据
	nsize:数据字节数

	返回值:
	PFR_OK;
	PFR_ERR;
  */
int	CPageFile::PF_Write(PF_HANDLE h,long offset,const void *pbuf,long nsize,BOOL bFlush)//写
{
	if(nsize <= 0)
		return PFR_OK;

	LPPFINDEX pf = m_indexs + h;
	long loff = offset;
	if(loff == -1)
		loff = pf->lsize;

	int npno = PF_GetPageNo(pf,loff);
	if(npno == -1)
	{
		if((pf->lsize % PF_PAGESIZE) == 0) //排除整页结束情况
		{
			npno = PF_AddPage(h);
			if(npno == -1)
				return PFR_ERR;
		}
		else
		{
			return PFR_ERR;
		}
	}
	long lt = PF_PAGESIZE - (pf->lsize % PF_PAGESIZE);//当前页剩余空间
	long lw = 0;
	if(lt > nsize)
		lt = nsize;
	char *pc = (char *)pbuf;
	long lseek = PF_DATAPOS + npno * PF_PAGESIZE + (pf->lsize % PF_PAGESIZE);
	try
	{
		//写第一页
		m_file.Seek(lseek,CFile::begin);
		m_file.Write(pc,lt);
		lw += lt;
		
		//写剩下的页
		while(lw < nsize)
		{
			pc += lt;
			lt = PF_PAGESIZE;
			if((nsize - lw) < PF_PAGESIZE)
				lt = nsize - lw;
			npno = m_pages[npno];
			if(npno == PF_PAGEEND) //页面不够
			{
				npno = PF_AddPage(h); //添加页面
				if( npno == -1)
					return PFR_NOPAGE;//无空间
			}
			lseek = PF_DATAPOS + npno * PF_PAGESIZE;
			m_file.Seek(lseek,CFile::begin);
			m_file.Write(pc,lt);
			lw += lt;
		}
		
		if(loff + lw > pf->lsize)
		{
			pf->lsize = loff + lw; //更改页面文件大小
			//写入索引
			m_file.Seek(PF_INDEXPOS + h * sizeof(PFINDEX),CFile::begin);
			m_file.Write(pf,sizeof(PFINDEX));
		}
		if(bFlush)
		{
			m_file.Flush();
			m_bNeedFlush = FALSE;
		}
		else
			m_bNeedFlush = TRUE;
	}
	catch( CFileException *ex ) 
	{
	   ex->Delete();
	   return PFR_ERR;
	}
	
	return PFR_OK;
}

/*
	读
  */
int	CPageFile::PF_Read(PF_HANDLE h,long offset,void *pbuf,long nsize,long &nreadsize)//读
{
	LPPFINDEX pf = m_indexs + h;
	int npno = PF_GetPageNo(pf,offset);
	char *pc = (char *)pbuf;
	long lr = 0,lt;
	if(npno == -1)
		return PFR_ERR;
	try
	{
		m_file.Seek(PF_DATAPOS + npno * PF_PAGESIZE + (offset % PF_PAGESIZE),CFile::begin);

		lt = PF_PAGESIZE - (offset % PF_PAGESIZE);

		if(lt > nsize)
			lt = nsize;
		if((offset + lt) > pf->lsize)
			lt = pf->lsize - offset;
		
		m_file.Read(pc,lt);
		lr += lt;
		npno = m_pages[npno];
		while((lr < pf->lsize)&&(lr < nsize) && ( npno != PF_PAGEEND))
		{
			pc += lt;
			lt = PF_PAGESIZE;
			
			if(lt > (pf->lsize - offset - lr)) 
				lt = pf->lsize - offset - lr; //不超过文件大小
			
			if(lt > (nsize - lr))
				lt = nsize - lr;		//不超过空间大小
			m_file.Seek(PF_DATAPOS + npno * PF_PAGESIZE,CFile::begin);
			m_file.Read(pc,lt);
			lr += lt;
			npno = m_pages[npno];
		}
		nreadsize = lr; 
	}
	catch( CFileException *ex ) 
	{
	   ex->Delete();
	   return PFR_ERR;
	}
	return PFR_OK;
}


PF_HANDLE	CPageFile::PF_CreateRecDir(CPfRecVal* prec)//创建页面文件，返回索引号，-1表示错误
{
	int ni = GetNextEmptyIndex(0);
	if(ni == -1)
		return -2;
	int i,npr,npi = -1;
	long initpages = prec->GetDataSize() / PF_PAGESIZE;
	if(prec->GetDataSize() % PF_PAGESIZE)
		initpages++;
	if(initpages == 0)
		initpages++; //最少一个页面

	//填写索引
	m_indexs[ni].ltype = prec->m_ntype;
	m_indexs[ni].ltime = prec->m_ltime;
	strcpy(m_indexs[ni].sdes,prec->m_szdes.Left(PGFDESLEN - 1));

	m_indexs[ni].lsize = 0;
	m_indexs[ni].lpageno = PF_PAGEEND;
	m_indexs[ni].snqa = prec->m_snqa;
	m_indexs[ni].wres = prec->m_wres;
	m_indexs[ni].dwres = prec->m_dwres;

	for(i=0;i<initpages;i++)
	{
		npr = npi;
		npi = GetNextEmptyPage(npi+1);
		if(npi == -1)
		{
			m_indexs[ni].ltime = 0;
			return PF_ERR_HANDLE;
		}
		if(i==0)
			m_indexs[ni].lpageno = npi;
		if(npr != -1)
			m_pages[npr] = npi;

		m_pages[npi] = PF_PAGEEND;
	}
	
	//写入磁盘
	try
	{
		//写入索引
		if(ni >= m_head.lindexs)
		{
			m_head.lindexs++;
			m_file.SeekToBegin();
			m_file.Write(&m_head,sizeof(PFHEAD));
		}
		m_file.Seek(PF_INDEXPOS + ni * sizeof(PFINDEX),CFile::begin);
		m_file.Write(&(m_indexs[ni]),sizeof(PFINDEX)); //写入索引

		//页面标记从m_indexs[ni].lpageno 到 npi
		if(npi >= m_indexs[ni].lpageno)
		{
			m_file.Seek(PF_PAGEMARKPOS + m_indexs[ni].lpageno * sizeof(long),CFile::begin);
			m_file.Write(&(m_pages[m_indexs[ni].lpageno]),(npi - m_indexs[ni].lpageno + 1) * sizeof(long));
		}
		return ni;
	}
	catch ( CFileException *ex ) 
	{
	   ex->Delete();
	}
	m_indexs[ni].ltime = 0;//清除索引
	return PF_ERR_HANDLE;
}

int			CPageFile::WriteRec(CPfRecVal* prec)
{
	if(!m_bOpen)
		return PFR_NOOPEN;//没有打开
	PF_HANDLE h = PF_CreateRecDir(prec);
	
	// 如果没有空间，则返回-2，以下条件判断失败造成程序崩溃
	//if(h == -1)
	{
		if(h == -1)
			return PFR_ERR;
		else if(h == -2)
			return PFR_NOPAGE;//无空间
	}
	
	long  lsize = prec->GetDataSize();
	void *pdata = prec->GetBuf();
	return PF_Write(h,0,pdata,lsize,TRUE);//写
	
}
int			CPageFile::GetLastRec(PF_HANDLE &h,CPfRecVal* prec)
{
	int	i;
	h = -1;
	for(i = (m_head.lindexs - 1); i>= 0; i--) //优化了
	//for(i = (PF_MAXINDEXS - 1); i >= 0;i--)
	{
		if(m_indexs[i].ltime != 0)
		{
			h = i;
			break;
		}
	}
	if(h == -1)
		return PFR_NOINDEX;
	int nsize = m_indexs[i].lsize;
	
	prec->ResetBufSize(nsize); //
	
	//目录信息
	prec->m_ltime = m_indexs[i].ltime;
	prec->m_ntype = m_indexs[i].ltype;
	prec->m_snqa = m_indexs[i].snqa;
	prec->m_szdes = m_indexs[i].sdes;

	long ldatasize = 0;
	void *pdata = prec->GetBuf();
	int nbufsize = prec->GetBufSize();

	int	nret = PF_Read(h,0,pdata,nbufsize,ldatasize);//读
	if(nret == PFR_OK)
	{
		prec->m_nszie = ldatasize;
		return nret;
	}

	//PF_Delete(h); //删除
	h = -1;
	return PFR_ERR;

}

int			CPageFile::GetFirstRec(PF_HANDLE &h,CPfRecVal* prec)
{
	int	i;
	h = -1;
	for(i = 0; i < m_head.lindexs; i++) //优化了
		//for(i = (PF_MAXINDEXS - 1); i >= 0;i--)
	{
		if(m_indexs[i].ltime != 0)
		{
			h = i;
			break;
		}
	}
	if(h == -1)
		return PFR_NOINDEX;
	int nsize = m_indexs[i].lsize;

	prec->ResetBufSize(nsize); //

	//目录信息
	prec->m_ltime = m_indexs[i].ltime;
	prec->m_ntype = m_indexs[i].ltype;
	prec->m_snqa = m_indexs[i].snqa;
	prec->m_szdes = m_indexs[i].sdes;

	long ldatasize = 0;
	void *pdata = prec->GetBuf();
	int nbufsize = prec->GetBufSize();

	int	nret = PF_Read(h,0,pdata,nbufsize,ldatasize);//读
	if(nret == PFR_OK)
	{
		prec->m_nszie = ldatasize;
		return nret;
	}

	//PF_Delete(h); //删除
	h = -1;
	return PFR_ERR;

}