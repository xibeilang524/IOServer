
// LicenseMgrDoc.cpp : CLicenseMgrDoc 类的实现
//

#include "stdafx.h"
#include "LicenseMgr.h"
#include "MainFrm.h"
#include "ViewTree.h"
#include "FileView.h"
#include "LicenseMgrDoc.h"
#include "LicenseMgrView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLicenseMgrDoc

IMPLEMENT_DYNCREATE(CLicenseMgrDoc, CDocument)

BEGIN_MESSAGE_MAP(CLicenseMgrDoc, CDocument)
#ifdef _LICENSE_VIEW_APP
	ON_COMMAND(ID_FILE_IMPORT, &CLicenseMgrDoc::OnFileImport)
	ON_UPDATE_COMMAND_UI(ID_FILE_IMPORT, &CLicenseMgrDoc::OnUpdateFileImport)
	ON_COMMAND(ID_FILE_EXPORT, &CLicenseMgrDoc::OnFileExport)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT, &CLicenseMgrDoc::OnUpdateFileExport)
#endif
END_MESSAGE_MAP()


// CLicenseMgrDoc 构造/析构

CLicenseMgrDoc::CLicenseMgrDoc()
{
#ifdef _LICENSE_VIEW_APP
	m_szPathName = Utility::GetModulePath( AfxGetInstanceHandle() ).c_str();

	// 首先在当前目录查找
	CString szPathName = m_szPathName + _T("TPRILicense.lic");
	if (Utility::DirectoryExists(szPathName))
		m_szPathName = szPathName;
	else {
		// 在当前目录的License子目录中查找
		szPathName = m_szPathName + _T("License\\TPRILicense.lic");

		if (Utility::DirectoryExists(szPathName))
			m_szPathName = szPathName;
		else {
			m_szPathName += _T("TPRILicense.lic");
		}
	}
#endif
}

CLicenseMgrDoc::~CLicenseMgrDoc()
{
}

CViewTree* CLicenseMgrDoc::GetTreeCtrl()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	if (!pMainFrame)
		return NULL;

	return pMainFrame->GetTreeCtrl();
}

BOOL CLicenseMgrDoc::LoadXmlFromResoure(DWORD dwResID, CString &szXML)
{
	szXML.Empty();

	HINSTANCE hi = AfxGetResourceHandle( );
	HRSRC hRsrc = FindResource(hi, MAKEINTRESOURCE(dwResID), TEXT("XML"));
	if (NULL == hRsrc)
		return FALSE;

	//获取资源的大小
	int nsize = (int)SizeofResource(hi, hRsrc); 
	if (0 == nsize)		
		return FALSE;

	//加载资源
	HGLOBAL hGlobal = LoadResource(hi, hRsrc); 
	if (NULL == hGlobal)
		return FALSE;

	//锁定资源
	LPVOID pBuffer = LockResource(hGlobal); 
	if (NULL == pBuffer)
	{
		FreeResource(hGlobal);
		return FALSE;
	}

	//_tcsncpy(szSQLScript.GetBuffer(nsize + 1), (LPCTSTR)pBuffer, nsize);
	//szSQLScript.ReleaseBuffer();

	char *ptr = new char[nsize + 1];
	memcpy(ptr, pBuffer, nsize);
	ptr[nsize] = '\0';
	szXML = ptr;
	delete[] ptr;

	FreeResource(hGlobal);
	return TRUE;
}

#ifndef _LICENSE_VIEW_APP
BOOL CLicenseMgrDoc::InitLicense(BOOL bSetValue/* = TRUE*/, DWORD dwXmlResoureID/* = 0*/)
{
	TiXmlDocument xmlDoc;

	if (dwXmlResoureID == 0)
	{
		string strFile = Utility::GetModulePath( AfxGetInstanceHandle() );
		strFile += "LicenseConfig.xml";
		xmlDoc.LoadFile(strFile);
	}
	else {
		CString szXML;
		if (!LoadXmlFromResoure(dwXmlResoureID, szXML))
			return FALSE;

		xmlDoc.Parse(szXML);
	}

	if (!xmlDoc.RootElement())
		return FALSE;

	TiXmlNode* pXmlNode = xmlDoc.FirstChild("license");
	if (!pXmlNode)
		return FALSE;

	pXmlNode = pXmlNode->FirstChild("software");
	while(pXmlNode)
	{
		TiXmlElement* pXmlElement = (TiXmlElement*)pXmlNode;

		CLicenseSoftware* pSoftware = theLicenseFile.GetLicenseSoftware( pXmlElement->Attribute("name"), true );

		if (pSoftware)
		{
			pSoftware->SetDescription( pXmlElement->Attribute("description") );
			InitSoftware(pXmlElement, pSoftware, bSetValue);

		}		

		pXmlNode = pXmlNode->NextSibling("software");
	}

	return TRUE;
}

BOOL CLicenseMgrDoc::InitSoftware(TiXmlElement* pXmlSoftware, CLicenseSoftware *pSoftware, BOOL bSetValue/* = TRUE*/)
{
	TiXmlNode* pPropertyNode = pXmlSoftware->FirstChild("property");
	while(pPropertyNode)
	{
		TiXmlElement* pPropertyElement = (TiXmlElement*)pPropertyNode;

		string strCategory = pPropertyElement->Attribute( "category" );
		string strName = pPropertyElement->Attribute( "name" );
		string strDisplayName = pPropertyElement->Attribute( "displayname" );
		string strDataType = pPropertyElement->Attribute( "datatype" );
		string strDefault = pPropertyElement->Attribute( "default" );
		string strDescr = pPropertyElement->Attribute( "description" );
		string strMin = pPropertyElement->Attribute( "min" );
		string strMax = pPropertyElement->Attribute( "max" );
		string strOptions = pPropertyElement->Attribute( "options" );

		if (strMin.empty() || strMax.empty())
		{
			strMin = "0";
			strMax = "0";
		}

		CLicenseProperty* pPty = pSoftware->AddProperty(strName.c_str(), strDisplayName.c_str(), strDataType.c_str(), bSetValue ? strDefault.c_str() : NULL, strDescr.c_str(), strCategory.c_str(),
			atoi(strMin.c_str()), atoi(strMax.c_str()), strOptions.c_str());

		pPropertyNode = pPropertyNode->NextSibling("property");
	}

	return TRUE;
}
#endif

void CLicenseMgrDoc::InitTreeView()
{
	CViewTree* pTreeCtrl = GetTreeCtrl();
	if (!pTreeCtrl)
		return;

	pTreeCtrl->DeleteAllItems();


#ifdef _LICENSE_VIEW_APP
	if (theLicenseFile.GetLicenseSoftwares().size() == 0)
		return;
#endif

	CString szPathName = GetPathName();
	if (szPathName.IsEmpty())
		szPathName = _T("<未命名授权文件>");
	else {
		int pos = szPathName.ReverseFind(_T('\\'));
		if (pos != -1)
			szPathName.Delete(0, pos + 1);
	}


	HTREEITEM hRoot = pTreeCtrl->InsertItem(szPathName, 2, 2);
	pTreeCtrl->SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);

	LPTREENODEDATA pNodeData = new TREENODEDATA;
	if (!pNodeData)
		return;
	pNodeData->nNodeType = NODE_LICENSE_FILE;
	pNodeData->szDescr[0] = '\0';

	pTreeCtrl->SetItemData(hRoot, reinterpret_cast<DWORD_PTR>(pNodeData));

	_LicenseSoftwaresArray &mapSoftwares = theLicenseFile.GetLicenseSoftwares();

	for (size_t i = 0; i < mapSoftwares.size(); i++)
	{
		HTREEITEM hTree = pTreeCtrl->InsertItem(mapSoftwares[i].GetSoftwareName(), 1, 1, hRoot);

		LPTREENODEDATA pNodeData = new TREENODEDATA;
		if (!pNodeData)
			continue;

		pNodeData->nNodeType = NODE_LICENSE_SOFTWARE;
		strcpy_s(pNodeData->szDescr, 80, mapSoftwares[i].GetDescription());
		pTreeCtrl->SetItemData(hTree, reinterpret_cast<DWORD_PTR>(pNodeData));
	}

	pTreeCtrl->Expand(hRoot, TVE_EXPAND);
}

BOOL CLicenseMgrDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	theLicenseFile.Clear();
#ifndef _LICENSE_VIEW_APP
	InitLicense();
#endif
	InitTreeView();
	SetModifiedFlag(FALSE);

	return TRUE;
}


BOOL CLicenseMgrDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
// 	if (!CDocument::OnOpenDocument(lpszPathName))
// 		return FALSE;
// 
// 	// TODO:  在此添加您专用的创建代码
// 
// 	return TRUE;

	BOOL bOK = theLicenseFile.Load(lpszPathName);
#ifndef _LICENSE_VIEW_APP
	InitLicense(FALSE); // 加载XML配置文件中有关属性的定义信息，比如min/max/options等
#endif

	POSITION pos = GetFirstViewPosition();
	CLicenseMgrView *pView = (CLicenseMgrView *)GetNextView(pos);
	pView->RemoveAllLicensePropList();

	GetTreeCtrl()->DeleteAllItems();

	SetPathName(lpszPathName);
	InitTreeView();

	GetTreeCtrl()->SelectItem(GetTreeCtrl()->GetRootItem());

	SetModifiedFlag(FALSE);     // back to unmodified	

	return bOK;
}

BOOL CLicenseMgrDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	ENSURE(lpszPathName);

#ifndef _LICENSE_VIEW_APP

	theLicenseFile.Save(lpszPathName);	

	SetPathName(lpszPathName);

	SetModifiedFlag(FALSE);     // back to unmodified

	POSITION pos = GetFirstViewPosition();
	CLicenseMgrView *pView = (CLicenseMgrView *)GetNextView(pos);
	pView->UpdateCurSelLicensePropList();
#endif

	return TRUE;        // success

	//return CDocument::OnSaveDocument(lpszPathName);
}

void CLicenseMgrDoc::OnCloseDocument()
{
	POSITION pos = GetFirstViewPosition();
	CLicenseMgrView *pView = (CLicenseMgrView *)GetNextView(pos);

	pView->RemoveAllLicensePropList();
	GetTreeCtrl()->DeleteAllItems();

	CDocument::OnCloseDocument();
}


// CLicenseMgrDoc 序列化

void CLicenseMgrDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CLicenseMgrDoc 诊断

#ifdef _DEBUG
void CLicenseMgrDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CLicenseMgrDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CLicenseMgrDoc 命令
#ifdef _LICENSE_VIEW_APP

void CLicenseMgrDoc::OnFileImport()
{
	CString szPathName;
	
	CFileDialog dlgFile(TRUE, _T(".lic"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_FILEMUSTEXIST, 
		_T("授权文件(*.lic)|*.lic||"), NULL, 0);

	dlgFile.m_ofn.lpstrTitle = _T("导入授权文件");
	dlgFile.m_ofn.lpstrFile = szPathName.GetBuffer(_MAX_PATH);

	INT_PTR nResult = dlgFile.DoModal();
	szPathName.ReleaseBuffer();
	
	if (nResult != IDOK)
		return;

	if (szPathName.CompareNoCase(GetPathName()) == 0)
	{
		AfxMessageBox(_T("不允许导入源授权文件，请重新选择导入文件！"), MB_OK | MB_ICONWARNING);
		return;
	}

	CString szMsg;
	CLicenseFile file;

	if ( !file.Load( szPathName ) )
	{
		szMsg.Format(_T("加载授权文件“%s”失败，可能不是有效的授权文件格式！"), szPathName);
		AfxMessageBox(szMsg, MB_OK | MB_ICONERROR);
		return;
	}

	if (!file.CheckMachineCode())
	{
		if (AfxMessageBox(_T("不该计算机的无效授权文件，是否确认要导入！"), MB_YESNO | MB_ICONQUESTION) == IDNO)
			return;
	}
	else if (file.CheckExpiredTime() <= 0)
	{
		if (AfxMessageBox(_T("该授权文件已经过期失效，是否确认要导入！"), MB_YESNO | MB_ICONQUESTION) == IDNO)
			return;
	}

	if (!file.Save( GetPathName() ))
	{
		szMsg.Format(_T("保存授权文件“%s”失败！"), GetPathName());
		AfxMessageBox(szMsg, MB_OK | MB_ICONERROR);
		return;
	}

	theLicenseFile = file;

	POSITION pos = GetFirstViewPosition();
	CLicenseMgrView *pView = (CLicenseMgrView *)GetNextView(pos);
	pView->RemoveAllLicensePropList();

	GetTreeCtrl()->DeleteAllItems();

	InitTreeView();

	GetTreeCtrl()->SelectItem(GetTreeCtrl()->GetRootItem());

	SetModifiedFlag(FALSE);

	szMsg.Format(_T("导入授权文件“%s”成功！"), szPathName);
	AfxMessageBox(szMsg, MB_OK | MB_ICONINFORMATION);
}

void CLicenseMgrDoc::OnUpdateFileImport(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
}

void CLicenseMgrDoc::OnFileExport()
{
	CString szPathName;

	CFileDialog dlgFile(FALSE, _T(".lic"), _T("TPRILicense"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		_T("授权文件(*.lic)|*.lic||"), NULL, 0);

	dlgFile.m_ofn.lpstrTitle = _T("导出授权文件");
	dlgFile.m_ofn.lpstrFile = szPathName.GetBuffer(_MAX_PATH);

	INT_PTR nResult = dlgFile.DoModal();
	szPathName.ReleaseBuffer();

	if (nResult != IDOK)
		return;

	if (szPathName.CompareNoCase(GetPathName()) == 0)
	{
		AfxMessageBox(_T("不允许导出到源授权文件，请重新选择导出文件！"), MB_OK | MB_ICONWARNING);
		return;
	}

	theLicenseFile.Save( szPathName );	

	CString szMsg;
	szMsg.Format(_T("导出授权文件“%s”成功！"), szPathName);
	AfxMessageBox(szMsg, MB_OK | MB_ICONINFORMATION);
}

void CLicenseMgrDoc::OnUpdateFileExport(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( (theLicenseFile.GetLicenseSoftwares().size() != 0) );
}

#endif