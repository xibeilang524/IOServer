// Readini.h
#ifndef __READINI_H__
#define __READINI_H__

TRUNK_NAMESPACE_BEGIN

class _GENERAL_CLASS CReadIni
{
public:
	CReadIni();
	~CReadIni();
public:
	BOOL ReadIniFile(LPCTSTR lpszFile);

protected:
	CString m_szLine;	//ÐÐ
	CString m_szBlkName; //¿éÃû

protected:
	void	DoLine();

	virtual void OnDoKeyVal(LPCTSTR lpszBlkName,LPCTSTR lpszKeyName,LPCTSTR lpszKeyVal){};
};

TRUNK_NAMESPACE_END

#endif // end of __READINI_H__