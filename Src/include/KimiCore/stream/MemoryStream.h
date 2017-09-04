#ifndef __KIMI_KERNEL_MEMORY_STREAM_H__
#define __KIMI_KERNEL_MEMORY_STREAM_H__

#include <string>
#include "DynamicArray.h"

_KIMI_NAMESPACE_BEGIN

#define STREAM_START   0
#define STREAM_CURRENT 1
#define STREAM_END     2

class KIMI_CORE_CLASS CMemoryStream
{
public :
	CMemoryStream();
	CMemoryStream(void *buffer,long len);
	CMemoryStream(char *str);
	~CMemoryStream();
public :	
	int GetBlockSize() { return m_pBuffer.GetBlockSize(); };
	void SetBlockSize(int factorial) { m_pBuffer.SetBlockSize(factorial);} // 使用价乘值设置块大小，有效值为1-32
	long GetLength();
	int Eof();
	long Read(void *buffer,long size);
	long Write(void *buffer,long size);
	long Seek(long pos,int mode);
	long GetPos();
	int GetCh();
	int PutCh(int ch);

	void Clear();
	char& operator[](long i);

	char *GetDataPtr();

public :  // 输入流
	CMemoryStream& operator<<(bool val);
	CMemoryStream& operator<<(char val);
	CMemoryStream& operator<<(unsigned char val);
	CMemoryStream& operator<<(short val);
	CMemoryStream& operator<<(unsigned short val);
	CMemoryStream& operator<<(int val);     // for BOOL
	CMemoryStream& operator<<(unsigned int val);
	CMemoryStream& operator<<(long val);
	CMemoryStream& operator<<(unsigned long val);
	CMemoryStream& operator<<(float val);
	CMemoryStream& operator<<(double val);
	CMemoryStream& operator<<(const char *val);	// 输出字符串
	CMemoryStream& operator<<(const wchar_t *val); // BSTR 字符串
	CMemoryStream& operator<<(const string &val);
	CMemoryStream& operator<<(const wstring &val);
#ifdef _KIMI_CORE_USE_MFC
	CMemoryStream& operator<<(const CStringA &val);
	CMemoryStream& operator<<(const CStringW &val);
#endif

public :  // 输出流
	CMemoryStream& operator>>(bool &val);
	CMemoryStream& operator>>(char &val);
	CMemoryStream& operator>>(unsigned char &val);
	CMemoryStream& operator>>(short &val);
	CMemoryStream& operator>>(unsigned short &val);
	CMemoryStream& operator>>(int &val);   // for BOOL
	CMemoryStream& operator>>(unsigned int &val);
	CMemoryStream& operator>>(long &val);
	CMemoryStream& operator>>(unsigned long &val);
	CMemoryStream& operator>>(float &val);
	CMemoryStream& operator>>(double &val);
	CMemoryStream& operator>>(char *&val);
	CMemoryStream& operator>>(wchar_t *&val); // BSTR 字符串
	CMemoryStream& operator>>(string &val);
	CMemoryStream& operator>>(wstring &val);
#ifdef _KIMI_CORE_USE_MFC
	CMemoryStream& operator>>(CStringA &val);
	CMemoryStream& operator>>(CStringW &val);
#endif
	
protected:
private :
	long m_nPos;     // 读写指针
#pragma warning(push)
#pragma warning(disable:4251)
	CDynamicArray<char> m_pBuffer;
#pragma warning(pop)
};

_KIMI_NAMESPACE_END

#endif // __KIMI_KERNEL_MEMORY_STREAM_H__