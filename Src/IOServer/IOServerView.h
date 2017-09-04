
// IOServerView.h : CIOServerView 类的接口
//

#pragma once
#include "WinTagInfo.h"


class CIOServerView : public CView
{
protected: // 仅从序列化创建
	CIOServerView();
	DECLARE_DYNCREATE(CIOServerView)

// 特性
public:
	CIOServerDoc* GetDocument() const;
	CString		m_szQuery;//查询字符串,name\n 代理标签名 \n 驱动标签名\n 描述\n 类型\n
	CFont		m_fntText;
	CButton		m_btnQuery;//查询按钮
	CButton		m_btnRefresh;//刷新按钮
	UINT		m_uTimer;
	CWinTagInfo	m_wndTagTable; //窗口
// 操作
public:
	CString		GetQueryInfo();
	void		Query();
	int			DelSelTags(CString szDecName);//删除选择的标签
// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CIOServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetDeviceTag();
	afx_msg void OnQueryTag();
	afx_msg void OnExportTag();
	afx_msg void OnExportSelectTag();
	afx_msg void OnImportTag();
	afx_msg void OnDelTag();
	afx_msg void OnAddTag();
	afx_msg void OnReFresh();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

#ifndef _DEBUG  // IOServerView.cpp 中的调试版本
inline CIOServerDoc* CIOServerView::GetDocument() const
   { return reinterpret_cast<CIOServerDoc*>(m_pDocument); }
#endif

