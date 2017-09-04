// 102Drv.h : main header file for the 102Drv DLL
//

#if !defined(AFX_102Drv_H__7B1E2BF8_CF88_4BEF_B4F8_50166608C10B__INCLUDED_)
#define AFX_102Drv_H__7B1E2BF8_CF88_4BEF_B4F8_50166608C10B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMy102DrvApp
// See 102Drv.cpp for the implementation of this class
//

class CMy102DrvApp : public CWinApp
{
public:
	CMy102DrvApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMy102DrvApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMy102DrvApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_102Drv_H__7B1E2BF8_CF88_4BEF_B4F8_50166608C10B__INCLUDED_)
