#if !defined(AFX_PROGRESSTHREAD_H__84E5A683_1317_4300_80D9_89EC2F9C6C47__INCLUDED_)
#define AFX_PROGRESSTHREAD_H__84E5A683_1317_4300_80D9_89EC2F9C6C47__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProgressThread.h : header file
//

#include "ProgressDlg.h"


/////////////////////////////////////////////////////////////////////////////
// CProgressThread thread

class CProgressThread : public CWinThread
{
	DECLARE_DYNCREATE(CProgressThread)
protected:
	CProgressThread();           // protected constructor used by dynamic creation

// Attributes
public:
	CProgressDlg *m_pDlg;
	BOOL m_bHighPrivilege;

// Operations
public:
	void SetText(LPCTSTR lpszText);
	void SetRange(int nMin, int nMax, int nStep = 0);
	void SetPos(int nPos);
	int GetPos();
	void StepIt();
	void BeginUpdate();
	void EndUpdate();
	void Close();
	static CProgressThread *StartThread();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgressThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CProgressThread();

	// Generated message map functions
	//{{AFX_MSG(CProgressThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGRESSTHREAD_H__84E5A683_1317_4300_80D9_89EC2F9C6C47__INCLUDED_)
