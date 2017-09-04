
// LicenseMgrDoc.h : CLicenseMgrDoc 类的接口
//


#pragma once

class CViewTree;

class CLicenseMgrDoc : public CDocument
{
protected: // 仅从序列化创建
	CLicenseMgrDoc();
	DECLARE_DYNCREATE(CLicenseMgrDoc)

// 属性
public:
#ifdef _LICENSE_VIEW_APP
	CString	m_szPathName;
#endif
	CViewTree* GetTreeCtrl();

// 操作
public:

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 实现
public:
	virtual ~CLicenseMgrDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL LoadXmlFromResoure(DWORD dwResID, CString &szXML);
#ifndef _LICENSE_VIEW_APP
	BOOL InitLicense(BOOL bSetValue = TRUE, DWORD dwXmlResoureID = 0);
	BOOL InitSoftware(TiXmlElement* pXmlSoftware, CLicenseSoftware *pSoftware, BOOL bSetValue = TRUE);
#endif
	void InitTreeView();

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
#ifdef _LICENSE_VIEW_APP
	afx_msg void OnFileImport();
	afx_msg void OnUpdateFileImport(CCmdUI *pCmdUI);
	afx_msg void OnFileExport();
	afx_msg void OnUpdateFileExport(CCmdUI *pCmdUI);
#endif
};


