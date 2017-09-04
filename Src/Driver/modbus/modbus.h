// modbus.h : main header file for the MODBUS DLL
//

#if !defined(AFX_MODBUS_H__C5C70A11_3D3F_4F1E_B507_CAE4EAED1EF8__INCLUDED_)
#define AFX_MODBUS_H__C5C70A11_3D3F_4F1E_B507_CAE4EAED1EF8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CModbusApp
// See modbus.cpp for the implementation of this class
//

class CModbusApp : public CWinApp
{
public:
	CModbusApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModbusApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CModbusApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CString g_LoadString(UINT UID);
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODBUS_H__C5C70A11_3D3F_4F1E_B507_CAE4EAED1EF8__INCLUDED_)
