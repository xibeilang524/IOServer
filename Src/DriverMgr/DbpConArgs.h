#pragma once
#include "Define.h"


class CDbpConArgs
{
public:
	CDbpConArgs(void);
	~CDbpConArgs(void);

	TDBPARG m_args[MAX_DBP_CONARGS]; //空间
	int     m_nsize; //参数个数

	CString m_szfile;

	BOOL LoadConArgs();//读入配置参数
	BOOL SaveConArgs();//保存配置参数
};

