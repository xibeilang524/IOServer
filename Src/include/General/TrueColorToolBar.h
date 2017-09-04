#ifndef __TRUECOLOR_TOOLBAR_H__
#define __TRUECOLOR_TOOLBAR_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrueColorToolBar.h : header file
//

TRUNK_NAMESPACE_BEGIN
/////////////////////////////////////////////////////////////////////////////
// CTrueColorToolBar window

class _GENERAL_CLASS CTrueColorToolBar : public CToolBar
{
public:
	BOOL LoadTrueColorToolBar(int nBtnWidth, UINT uToolBar,	UINT uToolBarHot, UINT uToolBarDisabled);
	BOOL LoadTrueColorToolBar(int nBtnWidth, UINT uToolBar,	UINT uToolBarHot, UINT uToolBarDisabled, COLORREF clsMaskColor);
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	virtual CSize CalcDynamicLayout(int nLength, DWORD dwMode);

private:
	CSize	m_sizeGripper;

protected:
	BOOL SetTrueColorToolBar(UINT uToolBarType,	UINT uToolBar, int nBtnWidth, BOOL bMaskColor = FALSE, COLORREF clsMaskColor = 0);
	void	DrawGripper( CDC & ) ;
	struct _AFX_CONTROLPOS
	{
		int nIndex, nID;
		CRect rectOldPos;
	};
	void _GetButton(int nIndex, TBBUTTON* pButton) const;
	void _SetButton(int nIndex, TBBUTTON* pButton);
	void SizeToolBar(TBBUTTON* pData, int nCount, int nLength, BOOL bVert = FALSE);
	CSize CalcLayout(DWORD dwMode, int nLength = -1);

	afx_msg void OnNcPaint();
	DECLARE_MESSAGE_MAP()
};

TRUNK_NAMESPACE_END
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __TRUECOLOR_TOOLBAR_H__
