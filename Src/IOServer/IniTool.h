#pragma once
#include "iniconfig.h"

class CIniTool :
	public CIniConfig
{
public:
	CIniTool();
	CIniTool(LPCTSTR sFile);
	~CIniTool();
protected:
	CString m_szFile;
public:
	BOOL Read()
	{
		return ReadIniFile(m_szFile);
	};
	BOOL Write();//–¥»Î
};

