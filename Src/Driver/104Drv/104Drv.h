// 104Drv.h : main header file for the 104Drv DLL
//

#if !defined(AFX_104Drv_H__7B1E2BF8_CF88_4BEF_B4F8_50166608C10B__INCLUDED_)
#define AFX_104Drv_H__7B1E2BF8_CF88_4BEF_B4F8_50166608C10B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMy104DrvApp
// See 104Drv.cpp for the implementation of this class
//

class CMy104DrvApp : public CWinApp
{
public:
	CMy104DrvApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMy104DrvApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMy104DrvApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_104Drv_H__7B1E2BF8_CF88_4BEF_B4F8_50166608C10B__INCLUDED_)
