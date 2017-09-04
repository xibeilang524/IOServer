#include "stdafx.h"
#include "GeneralAPI.h"
#include "General\EditNumber.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

TRUNK_NAMESPACE_BEGIN

/////////////////////////////////////////////////////////////////////////////
// CNumberBox

CNumberBox::CNumberBox()
{
	m_pWin=NULL;
}

CNumberBox::~CNumberBox()
{
}


BEGIN_MESSAGE_MAP(CNumberBox, CComboBox)
	//{{AFX_MSG_MAP(CNumberBox)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNumberBox message handlers

void CNumberBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CString strText;
	GetWindowText(strText);
	if( (strText.GetLength() >=4 || nChar > '9' || nChar < '0')
		&& nChar != VK_BACK)
		return;
	
	CComboBox::OnChar(nChar, nRepCnt, nFlags);
	
}

BOOL CNumberBox::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message ==WM_KEYDOWN )
	{
		CString strText;
		GetWindowText(strText);
		strText.TrimLeft ();
		strText.TrimRight ();
		if(IsNumber((TCHAR)pMsg->wParam) && strText.GetLength() < 5  ||
		   pMsg->wParam ==VK_BACK	||
		   pMsg->wParam ==VK_DELETE ||
		   pMsg->wParam ==VK_TAB	||
		   pMsg->wParam ==VK_RETURN 
		   )
		{
			return CComboBox::PreTranslateMessage(pMsg);
			
		}
		else if(pMsg->wParam == VK_LEFT)
		{
			int e;
			e=GetEditSel();
			SetEditSel(e-1,e-1);
			return TRUE;			
		}
		else if(pMsg->wParam == VK_RIGHT)
		{
			int e;
			e=GetEditSel();		
			SetEditSel(e+1,e+1);
			return TRUE;
		}	
		else
		{
			return TRUE;
		}
	}
	return CComboBox::PreTranslateMessage(pMsg);
	
}

BOOL CNumberBox::IsNumber(TCHAR cCode)
{
	if(cCode >'9' || cCode <'0')
	{
		return FALSE;
	}
	return TRUE;
}

TRUNK_NAMESPACE_END