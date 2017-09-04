#ifndef __CCLIPBOARD_H__
#define __CCLIPBOARD_H__

TRUNK_NAMESPACE_BEGIN

class _GENERAL_CLASS CClipboard
{
public:
	static CString GetText();
	static BOOL GetText(LPSTR lpszBuffer, int nBufSize);
	static DWORD GetTextLength();
	static BOOL SetText(LPCTSTR lpszBuffer);
};

TRUNK_NAMESPACE_END

#endif // end of __CCLIPBOARD_H__