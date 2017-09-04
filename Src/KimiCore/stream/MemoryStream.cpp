#include "StdAfx.h"
#include "KimiCore.h"
#include "KimiCore\stream\MemoryStream.h"


_KIMI_NAMESPACE_BEGIN

#pragma warning( push )
#pragma warning(disable : 4267)

/////////////////////////////////////////////////////////////////////
//  CMemoryStream
/////////////////////////////////////////////////////////////////////
CMemoryStream::CMemoryStream()
{
	m_nPos = 0; 
	m_pBuffer.SetLength(0); 
	m_pBuffer.SetAutoAlloc(1);
}

CMemoryStream::CMemoryStream(void *buffer,long len) 
{ 
	CMemoryStream::CMemoryStream(); 
	m_nPos = 0; 
	Write(buffer, len); 
}

CMemoryStream::CMemoryStream(char *str) 
{ 
	CMemoryStream::CMemoryStream(str, strlen(str)); 
}

CMemoryStream::~CMemoryStream()
{
}

long CMemoryStream::GetLength()
{
	return m_pBuffer.GetLength();
}

int CMemoryStream::Eof()
{
	if (m_nPos >= m_pBuffer.GetLength()) 
		return 1;
	return 0;
}

long CMemoryStream::Read(void *buffer, long size)
{
	if (m_nPos >= m_pBuffer.GetLength()) 
		return 0;

	size = (m_nPos + size <= m_pBuffer.GetLength())?size:(m_pBuffer.GetLength() - m_nPos);

	memcpy(buffer, &m_pBuffer[m_nPos], size);

	m_nPos += size;

	return size;
}

long CMemoryStream::Write(void *buffer, long size)
{
	if (m_pBuffer.SetLength(m_nPos + size)) 
		return 0;
	
	memcpy(&m_pBuffer[m_nPos],buffer,size);
	m_nPos += size;
	return size;
}

long CMemoryStream::Seek(long p, int mode)
{
	switch (mode)
	{
	case STREAM_START:   m_nPos = p; break;
	case STREAM_CURRENT: m_nPos += p; break;
	case STREAM_END:     m_nPos = m_pBuffer.GetLength() + p; break;
	}
	return 0;
}

long CMemoryStream::GetPos()
{
	return m_nPos;
}

int CMemoryStream::GetCh()
{
	if (m_nPos >= m_pBuffer.GetLength()) return -1;
	return m_pBuffer[m_nPos++];
}

int CMemoryStream::PutCh(int ch)
{
	m_pBuffer[m_nPos++]=ch;
	return 0;
}

void CMemoryStream::Clear() 
{ 
	m_pBuffer.Clear(); 
	m_nPos = 0; 
}

char& CMemoryStream::operator[](long i) 
{ 
	return m_pBuffer[i]; 
}

char *CMemoryStream::GetDataPtr()
{
	return &m_pBuffer[0];
}

// 输入流
CMemoryStream& CMemoryStream::operator<<(bool val)
{
	Write(&val, sizeof(bool));
	return *this;
}

CMemoryStream& CMemoryStream::operator<<(char val)
{
	Write(&val, sizeof(char));
	return *this;
}

CMemoryStream& CMemoryStream::operator<<(unsigned char val)
{
	Write(&val, sizeof(unsigned char));
	return *this;
}

CMemoryStream& CMemoryStream::operator<<(short val)
{
	Write(&val, sizeof(short));
	return *this;
}

CMemoryStream& CMemoryStream::operator<<(unsigned short val)
{
	Write(&val, sizeof(unsigned short));
	return *this;
}

CMemoryStream& CMemoryStream::operator<<(int val)
{
	Write(&val, sizeof(int));
	return *this;
}

CMemoryStream& CMemoryStream::operator<<(unsigned int val)
{
	Write(&val, sizeof(unsigned int));
	return *this;
}

CMemoryStream& CMemoryStream::operator<<(long val)
{
	Write(&val, sizeof(long));
	return *this;
}

CMemoryStream& CMemoryStream::operator<<(unsigned long val)
{
	Write(&val, sizeof(unsigned long));
	return *this;
}

CMemoryStream& CMemoryStream::operator<<(float val)
{
	Write(&val, sizeof(float));
	return *this;
}

CMemoryStream& CMemoryStream::operator<<(double val)
{
	Write(&val, sizeof(double));
	return *this;
}

CMemoryStream& CMemoryStream::operator<<(const char *val)
{
	DWORD len = 0;
	if (NULL != val)
		len = strlen(val);
	
	Write(&len, sizeof(DWORD)); // 写入字符串长度，不包括NULL长度

	if (NULL != val)
		Write((char *)val, len);

	return *this;
}

CMemoryStream& CMemoryStream::operator<<(const wchar_t *val) // BSTR 字符串
{
	DWORD len = wcslen(val);

	// ----BSTR
	// ^^^^ 该四个字节为字符串的长度
	Write((void *)(val-2), len*sizeof(wchar_t)+4);
	return *this;
}

CMemoryStream& CMemoryStream::operator<<(const string &val)
{
	CMemoryStream::operator<<(val.c_str());

	return *this;
}

CMemoryStream& CMemoryStream::operator<<(const wstring &val)
{
	CMemoryStream::operator<<(val.c_str());

	return *this;
}

#ifdef _KIMI_CORE_USE_MFC
CMemoryStream& CMemoryStream::operator<<(const CStringA &val)
{
	CMemoryStream::operator<<((LPSTR)val);

	return *this;
}

CMemoryStream& CMemoryStream::operator<<(const CStringW &val)
{
	CMemoryStream::operator<<((LPWSTR)val);

	return *this;
}
#endif

// 输出流
CMemoryStream& CMemoryStream::operator>>(bool &val)
{
	Read(&val, sizeof(bool));
	return *this;
}

CMemoryStream& CMemoryStream::operator>>(char &val)
{
	Read(&val, sizeof(char));
	return *this;
}

CMemoryStream& CMemoryStream::operator>>(unsigned char &val)
{
	Read(&val, sizeof(unsigned char));
	return *this;
}

CMemoryStream& CMemoryStream::operator>>(short &val)
{
	Read(&val, sizeof(short));
	return *this;
}

CMemoryStream& CMemoryStream::operator>>(unsigned short &val)
{
	Read(&val, sizeof(unsigned short));
	return *this;
}

CMemoryStream& CMemoryStream::operator>>(int &val)
{
	Read(&val, sizeof(int));
	return *this;
}

CMemoryStream& CMemoryStream::operator>>(unsigned int &val)
{
	Read(&val, sizeof(unsigned int));
	return *this;
}

CMemoryStream& CMemoryStream::operator>>(long &val)
{
	Read(&val, sizeof(long));
	return *this;
}

CMemoryStream& CMemoryStream::operator>>(unsigned long &val)
{
	Read(&val, sizeof(unsigned long));
	return *this;
}

CMemoryStream& CMemoryStream::operator>>(float &val)
{
	Read(&val, sizeof(float));
	return *this;
}

CMemoryStream& CMemoryStream::operator>>(double &val)
{
	Read(&val, sizeof(double));
	return *this;
}

CMemoryStream& CMemoryStream::operator>>(char *&val)
{
	DWORD len;
	Read(&len, sizeof(DWORD));
	if (len == -1)
	{
		len = 0;
		val = NULL;
	}
	else if (len == 0)
	{
		val = new char[8];
		memset(val, 0, 8);
	}
	else {
		val = new char[len +1];
		DWORD size = Read(val, len);
		val[len] = '\0';
		if (size != len)
		{
			delete[] val;
			val = NULL;
		}
	}

	return *this;
}

CMemoryStream& CMemoryStream::operator>>(wchar_t *&val) // BSTR 字符串
{
	DWORD len;

	Read(&len, sizeof(DWORD));	
	val = new wchar_t[len + sizeof(DWORD) +1];
	memcpy(val, &len, sizeof(DWORD));
	DWORD size = Read(val+sizeof(DWORD), len*sizeof(wchar_t));
	val[len] = NULL;
	if (size != len)
	{
		delete[] val;
		val = NULL;
	}

	return *this;
}

CMemoryStream& CMemoryStream::operator>>(string &val)
{
	char *str = NULL;

	CMemoryStream::operator>>(str);

	if (str)
	{
		val = str;
		delete[] str;
	}
	else
		val = "";

	return *this;	
}

CMemoryStream& CMemoryStream::operator>>(wstring &val)
{
	wchar_t *str = NULL;

	CMemoryStream::operator>>(str);

	if (str)
	{
		val = str;
		delete[] str;
	}
	else
		val = L"";

	return *this;	
}

#ifdef _KIMI_CORE_USE_MFC
CMemoryStream& CMemoryStream::operator>>(CStringA &val)
{
	char *str = NULL;

	CMemoryStream::operator>>(str);

	if (str)
	{
		val = str;
		delete[] str;
	}
	else
		val = "";

	return *this;	
}

CMemoryStream& CMemoryStream::operator>>(CStringW &val)
{
	wchar_t *str = NULL;

	CMemoryStream::operator>>(str);

	if (str)
	{
		val = str;
		delete[] str;
	}
	else
		val = L"";

	return *this;	
}
#endif

#pragma warning( pop )

_KIMI_NAMESPACE_END