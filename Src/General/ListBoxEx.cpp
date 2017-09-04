// ListBoxEx.cpp : implementation file
//

#include "stdafx.h"
#include "GeneralAPI.h"
#include "General\ListBoxEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

TRUNK_NAMESPACE_BEGIN
/////////////////////////////////////////////////////////////////////////////
// CListBoxEx

CListBoxEx::CListBoxEx()
{
	m_nMaxWidth = 0;
}

CListBoxEx::~CListBoxEx()
{
}

void CListBoxEx::UpdateScrollBar(LPCTSTR lpszItem)
{	
	SCROLLINFO scrollInfo;
    memset(&scrollInfo, 0, sizeof(SCROLLINFO));
    scrollInfo.cbSize = sizeof(SCROLLINFO);
    scrollInfo.fMask  = SIF_ALL;
    GetScrollInfo(SB_VERT, &scrollInfo, SIF_ALL);
	
	SIZE      sSize;
    CClientDC myDC(this);
    CFont* pListBoxFont = GetFont();
    if(pListBoxFont != NULL)
    {
        CFont* pOldFont = myDC.SelectObject(pListBoxFont);
        GetTextExtentPoint32(myDC.m_hDC, lpszItem, (int)strlen(lpszItem), &sSize);
        m_nMaxWidth = max(m_nMaxWidth, (int)sSize.cx);
        SetHorizontalExtent(m_nMaxWidth + 3);
        myDC.SelectObject(pOldFont);
	}
}

BEGIN_MESSAGE_MAP(CListBoxEx, CListBox)
	//{{AFX_MSG_MAP(CListBoxEx)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CListBoxEx::AddString( LPCTSTR lpszItem )
{
	int nRet = CListBox::AddString(lpszItem);
	UpdateScrollBar(lpszItem);

	return nRet;
}

int CListBoxEx::InsertString(int nIndex, LPCTSTR lpszItem)
{
	int nRet = CListBox::InsertString(nIndex, lpszItem);
	UpdateScrollBar(lpszItem);
	
	return nRet;
}

int CListBoxEx::DeleteString(UINT nIndex)
{
	RECT lRect;
	GetWindowRect(&lRect);

	int nRet = CListBox::DeleteString(nIndex);

	int nBoxWidth = lRect.right - lRect.left;
	m_nMaxWidth = nBoxWidth;

	SIZE sSize;
	CClientDC myDC(this);

	int I;
	char szEntry[257];

	for (I = 0; I < GetCount(); I++)
	{
		GetText(I, szEntry);
		GetTextExtentPoint32(myDC.m_hDC, szEntry, (int)strlen(szEntry), &sSize);
		m_nMaxWidth = max(m_nMaxWidth, (int)sSize.cx);
	}

	if (m_nMaxWidth > nBoxWidth) // 显示水平滚动条
	{
		ShowScrollBar(SB_HORZ, TRUE);
		SetHorizontalExtent(m_nMaxWidth);
	}
	else
	{
		ShowScrollBar(SB_HORZ, FALSE);
	}
	
	return nRet;
}

void CListBoxEx::ResetContent()
{
	CListBox::ResetContent();
	
	m_nMaxWidth = 0;
	SetHorizontalExtent(0);
}

/////////////////////////////////////////////////////////////////////////////
// CListBoxEx message handlers

TRUNK_NAMESPACE_END
