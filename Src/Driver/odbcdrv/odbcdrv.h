// odbcdrv.h : main header file for the ODBCDRV DLL
//

#if !defined(AFX_ODBCDRV_H__4CB552B0_D660_410C_B1B7_13BCB03230B5__INCLUDED_)
#define AFX_ODBCDRV_H__4CB552B0_D660_410C_B1B7_13BCB03230B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// COdbcdrvApp
// See odbcdrv.cpp for the implementation of this class
//

class COdbcdrvApp : public CWinApp
{
public:
	COdbcdrvApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COdbcdrvApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(COdbcdrvApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ODBCDRV_H__4CB552B0_D660_410C_B1B7_13BCB03230B5__INCLUDED_)
