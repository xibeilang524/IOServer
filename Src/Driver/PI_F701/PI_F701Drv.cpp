// PI_F701Drv.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "PI_F701Drv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CPidrvApp

BEGIN_MESSAGE_MAP(CPidrvApp, CWinApp)
	//{{AFX_MSG_MAP(CPidrvApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPidrvApp construction

CPidrvApp::CPidrvApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPidrvApp object

CPidrvApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPidrvApp initialization

BOOL CPidrvApp::InitInstance()
{
	if (!AfxSocketInit())
	{
		// 注释掉该弹出对话框，避免由于SOCKET出错而弹出对话框堵塞代理服务器运行
		//AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	return TRUE;
}

int CPidrvApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	//_CrtDumpMemoryLeaks();

	return CWinApp::ExitInstance();
}
