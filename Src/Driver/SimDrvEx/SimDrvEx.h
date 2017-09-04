// SimdrvEx.h : main header file for the SIMDRV DLL
//

#if !defined(AFX_SIMDRV_H__B1A64995_DDED_4FA8_94C8_E947F331D4E2__INCLUDED_)
#define AFX_SIMDRV_H__B1A64995_DDED_4FA8_94C8_E947F331D4E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSimdrvApp
// See SimdrvEx.cpp for the implementation of this class
//

class CSimdrvApp : public CWinApp
{
public:
	CSimdrvApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimdrvApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSimdrvApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMDRV_H__B1A64995_DDED_4FA8_94C8_E947F331D4E2__INCLUDED_)
