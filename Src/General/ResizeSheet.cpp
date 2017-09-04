// ResizeSheet.cpp : implementation file
//

#include "stdafx.h"
#include "GeneralAPI.h"
#include "General\ResizeSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

TRUNK_NAMESPACE_BEGIN

/////////////////////////////////////////////////////////////////////////////
// CResizeSheet

IMPLEMENT_DYNAMIC(CResizeSheet, CPropertySheet)

CResizeSheet::CResizeSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CResizeSheet::CResizeSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CResizeSheet::~CResizeSheet()
{
}


BEGIN_MESSAGE_MAP(CResizeSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CResizeSheet)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResizeSheet message handlers

BOOL CResizeSheet::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style |=WS_MAXIMIZEBOX;
	return CPropertySheet::PreCreateWindow(cs);
}

int CResizeSheet::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	lpCreateStruct->style =WS_MAXIMIZEBOX | WS_CAPTION | WS_SYSMENU | WS_SIZEBOX;
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}

BOOL CResizeSheet::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CWnd::Create(lpszClassName, lpszWindowName, WS_MAXIMIZEBOX | WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_SIZEBOX, rect, pParentWnd, nID, pContext);
}

TRUNK_NAMESPACE_END