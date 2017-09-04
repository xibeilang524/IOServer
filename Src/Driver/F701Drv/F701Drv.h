// F701Drv.h : main header file for the 101DRV DLL
//

#if !defined(AFX_101DRV_H__4ADBC0DD_5612_4590_9478_F580761A45B0__INCLUDED_)
#define AFX_101DRV_H__4ADBC0DD_5612_4590_9478_F580761A45B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CF701DrvApp
// See F701Drv.cpp for the implementation of this class
//

class CF701DrvApp : public CWinApp
{
public:
	CF701DrvApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CF701DrvApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CF701DrvApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_101DRV_H__4ADBC0DD_5612_4590_9478_F580761A45B0__INCLUDED_)
