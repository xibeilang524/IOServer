// Base64.h: interface for the CBase64 class.
#ifndef __KIMI_KERNEL_BASE64_H__
#define __KIMI_KERNEL_BASE64_H__

_KIMI_NAMESPACE_BEGIN

#ifndef _LPCBYTE_DEFINED
#define _LPCBYTE_DEFINED
typedef const BYTE *LPCBYTE;
#endif


class KIMI_CORE_CLASS CBase64  
{
private:

	int DecodeChars(LPCSTR in, char *out);
	void EncodeChars(LPCSTR in, char *out, int count);

	int DecodeBytes(LPCSTR in, LPBYTE out);
	void EncodeBytes(LPCBYTE in, LPSTR out, int count);

	UCHAR FindBase64Val(const char character);

public:
	CBase64();
	virtual ~CBase64();

	BOOL DecodeString(LPCSTR szIn, LPSTR szOut, int nOutLen);
	BOOL EncodeString(LPCSTR szIn, LPSTR szOut, int nOutLen);

	BOOL DecodeData(LPCSTR szIn, LPBYTE szOut, int* nOutLen);
	BOOL EncodeData(LPCBYTE szIn, int nInLen, LPSTR szOut, int nOutLen);

};

_KIMI_NAMESPACE_END

#endif // end of __KIMI_KERNEL_BASE64_H__
