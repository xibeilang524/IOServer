#include "StdAfx.h"
#include "GeneralAPI.h"
#include "General\MyByteArray.h"

TRUNK_NAMESPACE_BEGIN

CMyByteArray::CMyByteArray()
:	m_pData(NULL),
	m_nSize(0),
	m_nBufSize(0),
	m_nGrown(1024 * 16)
{
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
	if((m_nBufSize - m_nSize) < nSize){ //空间不够
		int nsizet = m_nGrown;//计算增量
		if(m_nGrown < nSize){
			nsizet = (nSize/m_nGrown) * m_nGrown + m_nGrown;
			if(( m_nBufSize + nsizet) > MAX_BYTEARRAYSIZE)
				nsizet = MAX_BYTEARRAYSIZE - m_nBufSize;
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

void CMyByteArray::ClearData()
{
	m_nSize = 0;
	if(m_nBufSize > 64 * 1024) //大于64K则释放内存,优化内存的方式
	{
		if(m_pData != NULL)
			free(m_pData);
		m_pData = NULL;
		m_nBufSize = 0;	
	}
}

void CMyByteArray::SetGrown(int ngrown)
{
	m_nGrown = ngrown;
	if(m_nGrown < 1024)
		m_nGrown = 1024;
	if(m_nGrown > 1024 * 1024)
		m_nGrown = 1024 * 1024;
}

void CMyByteArray::LeftMove(int n)//左移n字节
{
	if(m_pData == NULL)
		return;
	if(n <= 0)
		return;
	if(n >= m_nSize){
		m_nSize = 0;
		return;
	}
	memmove(m_pData,m_pData + n,m_nSize - n);
	m_nSize -= n; 
}

//复制到另一个字节数组尾部.
void CMyByteArray::CopyTo(CMyByteArray *pba)
{
	pba->PutData(GetBuf(),GetDataSize());	
}

CMyByteArray& CMyByteArray::operator = (CMyByteArray& ba) //重载=符号
{
	ClearData();
	PutData(ba.GetBuf(),ba.GetDataSize());
	return *this;
}

CMyByteArray& CMyByteArray::operator += (CMyByteArray& ba) //重载+=符号
{
	PutData(ba.GetBuf(),ba.GetDataSize());
	return *this;
}

TRUNK_NAMESPACE_END