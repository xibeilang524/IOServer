// ProgressThread.cpp : implementation file
//

#include "stdafx.h"
#include "ProgressThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgressThread

IMPLEMENT_DYNCREATE(CProgressThread, CWinThread)

CProgressThread::CProgressThread()
{
	m_pDlg = NULL;
	m_bHighPrivilege = FALSE;
}

CProgressThread::~CProgressThread()
{
}

BOOL CProgressThread::InitInstance()
{
	// TODO:  perform and per-thread initialization here

	// 通常系统内的每个线程都有自己的输入队列。本函数允许线程和进程共享输入队列。
	// 连接了线程后，输入焦点、窗口激活、鼠标捕获、键盘状态以及输入队列状态都会进入共享状态
	//::AttachThreadInput(m_nThreadID, AfxGetApp()->m_nThreadID, TRUE ); 
	
	m_pDlg = new CProgressDlg();    
		
	m_pDlg->Create(IDD_DIALOG_PROGRESS);	
	m_pDlg->ShowWindow(SW_SHOW);	

	return TRUE;
}

int CProgressThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	if (m_pDlg)
	{
		m_pDlg->DestroyWindow();
		delete m_pDlg;
		m_pDlg = NULL;
	}	

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CProgressThread, CWinThread)
	//{{AFX_MSG_MAP(CProgressThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CProgressThread::SetText(LPCTSTR lpszText)
{
	if (!m_pDlg || !m_pDlg->GetSafeHwnd())
		return;

	m_pDlg->SetInfo(lpszText);
}

void CProgressThread::SetRange(int nMin, int nMax, int nStep)
{
	if (!m_pDlg || !m_pDlg->GetSafeHwnd())
		return;
	
	m_pDlg->GetProgressCtrl()->SetRange32(nMin, nMax);
	if (nStep > 0)
		m_pDlg->GetProgressCtrl()->SetStep(nStep);
}

void CProgressThread::SetPos(int nPos)
{
	if (!m_pDlg || !m_pDlg->GetSafeHwnd())
		return;
	
	m_pDlg->GetProgressCtrl()->SetPos(nPos);
}

int CProgressThread::GetPos()
{
	if (!m_pDlg || !m_pDlg->GetSafeHwnd())
		return 0;
	
	return m_pDlg->GetProgressCtrl()->GetPos();
}

void CProgressThread::StepIt()
{
	if (!m_pDlg || !m_pDlg->GetSafeHwnd())
		return;
	
	m_pDlg->GetProgressCtrl()->StepIt();
}

void CProgressThread::BeginUpdate()
{
	if (!m_pDlg || !m_pDlg->GetSafeHwnd())
		return;

	if(m_bHighPrivilege)
		LockWindowUpdate(m_pDlg->GetSafeHwnd());
	else
		SendMessage(m_pDlg->GetSafeHwnd(), WM_SETREDRAW, FALSE, 0); 
}

void CProgressThread::EndUpdate()
{
	if (!m_pDlg || !m_pDlg->GetSafeHwnd())
		return;

	if(m_bHighPrivilege)
		LockWindowUpdate(NULL);
	else
		SendMessage(m_pDlg->GetSafeHwnd(), WM_SETREDRAW, TRUE, 0);
	m_pDlg->RedrawWindow();
}

void CProgressThread::Close()
{
	PostThreadMessage(WM_QUIT, 0, 0);
}

CProgressThread *CProgressThread::StartThread()
{
	CProgressThread *pThread = (CProgressThread*) AfxBeginThread(
		RUNTIME_CLASS(CProgressThread),		
		THREAD_PRIORITY_NORMAL, 		
		0, 
		CREATE_SUSPENDED);  
	
	ASSERT(pThread->IsKindOf(RUNTIME_CLASS(CProgressThread)));
	pThread->m_bAutoDelete = TRUE;
	
	pThread->ResumeThread();  
	Sleep(100); // 要保证InitInstance()已经被执行完成
	
	return pThread;
}
/////////////////////////////////////////////////////////////////////////////
// CProgressThread message handlers
