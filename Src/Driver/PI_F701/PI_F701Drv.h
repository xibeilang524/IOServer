// PI_F701Drv.h : main header file for the PIDRV DLL
//

#if !defined(AFX_PIDRV_H__7B8529D2_79C8_45C5_BECE_B360DC3F73FA__INCLUDED_)
#define AFX_PIDRV_H__7B8529D2_79C8_45C5_BECE_B360DC3F73FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPidrvApp
// See PI_F701Drv.cpp for the implementation of this class
//

class CPidrvApp : public CWinApp
{
public:
	CPidrvApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPidrvApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CPidrvApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PIDRV_H__7B8529D2_79C8_45C5_BECE_B360DC3F73FA__INCLUDED_)
