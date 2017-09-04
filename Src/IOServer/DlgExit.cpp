// DlgExit.cpp : 实现文件
//

#include "stdafx.h"
#include "IOServer.h"
#include "DlgExit.h"
#include "afxdialogex.h"


// CDlgExit 对话框

IMPLEMENT_DYNAMIC(CDlgExit, CDialogEx)

CDlgExit::CDlgExit(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgExit::IDD, pParent)
{

}

CDlgExit::~CDlgExit()
{
}

void CDlgExit::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgExit, CDialogEx)
END_MESSAGE_MAP()


// CDlgExit 消息处理程序


BOOL CDlgExit::Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
{
	// TODO: 在此添加专用代码和/或调用基类
	BOOL bRet = CDialogEx::Create(CDlgExit::IDD, pParentWnd);
	return bRet;
	//return CDialogEx::Create(lpszTemplateName, pParentWnd);
}


BOOL CDlgExit::CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, LPVOID lpParam)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CDialogEx::CreateEx(dwExStyle, lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, lpParam);
}
