// MyFrmParse.h: interface for the CMyFrmParse class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYFRMPARSE_H__1AD52466_5898_4762_B5F7_DA665B9B6484__INCLUDED_)
#define AFX_MYFRMPARSE_H__1AD52466_5898_4762_B5F7_DA665B9B6484__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\..\SDK\ddclib.h"
class CMyFrmParse : public CFrmParse  
{
public:
	CMyFrmParse();
	virtual ~CMyFrmParse();
	virtual int	Parse();//·Ö°ü,·µ»Øm_nStatus
};

#endif // !defined(AFX_MYFRMPARSE_H__1AD52466_5898_4762_B5F7_DA665B9B6484__INCLUDED_)
