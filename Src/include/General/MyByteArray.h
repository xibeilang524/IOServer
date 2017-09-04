#ifndef __MYBYTEARRAY_H__
#define __MYBYTEARRAY_H__

TRUNK_NAMESPACE_BEGIN

/*------------------------------------------------------------------------------------

  字节动态数组CMyByteArray，用于保存数据报文或待分析数据

  */
#define MAX_BYTEARRAYSIZE	(1024 * 4096 + 256)	// 最大字节动态数组大小4M

class _GENERAL_CLASS CMyByteArray //字节动态数组，用于保存数据报文或待分析数据
{
public:
	CMyByteArray();
	~CMyByteArray();

	BOOL PutData(BYTE *pBuf,int nSize); //加入数据
	BOOL PutData(void *pBuf,int nSize)//加入数据
	{
		return PutData((BYTE*)pBuf,nSize);
	};
	BOOL PutStr(LPCTSTR lpszData,int nSize){
		return PutData((BYTE*)lpszData,nSize);
	};

	BYTE *GetBuf(){return m_pData;};
	BYTE *GetBuf(int &nsize){
		nsize = m_nSize;
		if(m_pData == NULL)
			nsize = 0;
		return m_pData;
	};

	int GetDataSize(){return m_nSize;};
	void ClearData();
	void SetGrown(int ngrown);
	void LeftMove(int n); //左移n字节
	void CopyTo(CMyByteArray *pba); //复制到另一个字节数组尾部.
	CMyByteArray& operator = (CMyByteArray& ba); //重载=符号
	CMyByteArray& operator += (CMyByteArray& ba); //重载+=符号
	
protected:
	BYTE *m_pData;		//数据段
	int  m_nSize;		//数据大小
	int  m_nBufSize;	//空间大小
	int	 m_nGrown;		//增长增量
};

TRUNK_NAMESPACE_END

#endif // end of __MYBYTEARRAY_H__