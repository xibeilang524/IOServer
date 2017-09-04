// DlgAcceCfg.cpp : 实现文件
//

#include "stdafx.h"
#include "SimdrvEx.h"
#include "DlgAcceCfg.h"


// DlgAcceCfg 对话框

IMPLEMENT_DYNAMIC(CDlgAcceCfg, CDialog)

CDlgAcceCfg::CDlgAcceCfg(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAcceCfg::IDD, pParent)
	, m_span(1000)
	, m_timeincr(1)
	//, m_starttime(COleDateTime::GetCurrentTime())
	//, m_endtime(COleDateTime::GetCurrentTime())
	, m_bAllowAcce(FALSE)
{
	m_starttime = COleDateTime::GetCurrentTime();
	COleDateTime et(m_starttime.GetYear() , 
		            m_starttime.GetMonth(),
					m_starttime.GetDay(), 
					m_starttime.GetHour(), 
					m_starttime.GetMinute(), 
					m_starttime.GetSecond() -1);
	m_endtime   = et;
}

CDlgAcceCfg::~CDlgAcceCfg()
{
}

void CDlgAcceCfg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SPAN, m_span);
	DDV_MinMaxInt(pDX, m_span, 0, 30000);
	DDX_Text(pDX, IDC_EDIT_INCRE, m_timeincr);
	DDV_MinMaxInt(pDX, m_timeincr, 0, 86400);
	DDX_Text(pDX, IDC_EDIT_ST, m_starttime);
	DDX_Text(pDX, IDC_EDIT_ET, m_endtime);
	DDX_Check(pDX, IDC_CHECK_ALLOW, m_bAllowAcce);
}


BEGIN_MESSAGE_MAP(CDlgAcceCfg, CDialog)
END_MESSAGE_MAP()


// DlgAcceCfg 消息处理程序
