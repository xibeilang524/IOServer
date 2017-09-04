
// LicenseMgrView.cpp : CLicenseMgrView 类的实现
//

#include "stdafx.h"
#include "LicenseMgr.h"

#include "LicenseMgrDoc.h"
#include "LicenseMgrView.h"
#include "CustomGridProperty.h"
#include "ViewTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLicenseMgrView

IMPLEMENT_DYNCREATE(CLicenseMgrView, CView)

BEGIN_MESSAGE_MAP(CLicenseMgrView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()

// CLicenseMgrView 构造/析构

CLicenseMgrView::CLicenseMgrView()
{
#ifdef _LICENSE_VIEW_APP
	m_bFirstOpenView = TRUE;
#endif
}

CLicenseMgrView::~CLicenseMgrView()
{
}

void CLicenseMgrView::UpdateCurSelLicensePropList()
{
	CViewTree *pTreeCtrl = GetDocument()->GetTreeCtrl();

	if (pTreeCtrl->GetRootItem() == NULL)
	{
		m_wndPropList.RemoveAll();
		return;
	}

	CString szPathName = GetDocument()->GetPathName();
	if (szPathName.IsEmpty())
		szPathName = _T("<未命名授权文件>");
	else {
		int pos = szPathName.ReverseFind(_T('\\'));
		if (pos != -1)
			szPathName.Delete(0, pos + 1);
	}

	pTreeCtrl->SetItemText(pTreeCtrl->GetRootItem(), szPathName);

	if (!pTreeCtrl->GetSelectedItem())
	{
		pTreeCtrl->SelectItem( pTreeCtrl->GetRootItem() );
	}
	else {
		if (m_szSoftwareName.IsEmpty())
			UpdateLicensePropList(NULL);
		else
			UpdateLicensePropList(m_szSoftwareName);
	}
}

BOOL CLicenseMgrView::UpdateLicensePropList(LPCTSTR lpszSoftwareName/* = NULL*/)
{
	if (!m_wndPropList.GetSafeHwnd())
		return TRUE;

	m_wndPropList.RemoveAll();

	if (!lpszSoftwareName)
	{
		m_szSoftwareName.Empty();

		CTPRIPropertyGridProperty *pGroup = new CTPRIPropertyGridProperty(_T("基本信息"));
		// 文件名
		CString szPathName = GetDocument()->GetPathName();
		if (szPathName.IsEmpty())
			szPathName = _T("<未命名授权文件>");

		CTPRIPropertyGridProperty* pProp = new CTPRIPropertyGridProperty(_T("授权文件名"), (_variant_t)szPathName, _T("当前授权文件保存的文件名"));
		pProp->AllowEdit(FALSE);
		pGroup->AddSubItem(pProp);

#ifdef _LICENSE_VIEW_APP
		CString szAuthMsg;
		BOOL bMC = theLicenseFile.CheckMachineCode();
		BOOL bTime = (theLicenseFile.CheckExpiredTime() > 0);
		if (bMC && bTime)
		{
			szAuthMsg = _T("授权有效");
		}
		else {
			if (!bMC)
				szAuthMsg = _T("非本计算机授权");
			else
				szAuthMsg = _T("授权过期");
		}

		pProp = new CTPRIPropertyGridProperty(_T("授权状态"), (_variant_t)szAuthMsg, _T("当前授权文件对本计算机的授权状态，分为授权有效、非本计算机授权、授权过期三种情况"));
		pProp->AllowEdit(FALSE);
		pGroup->AddSubItem(pProp);
#endif

		m_wndPropList.AddProperty(pGroup);

		pGroup = new CTPRIPropertyGridProperty(_T("授权信息"));

		// 授权厂家
		pProp = new CTPRIPropertyGridProperty(_T("授权厂家"), (_variant_t)theLicenseFile.GetUserName(), _T("授权厂家名称"));
#ifdef _LICENSE_VIEW_APP
		pProp->AllowEdit(FALSE);
#endif
		pGroup->AddSubItem( pProp );

		// 机器码
		pProp = new CTPRIPropertyGridMachineCodeProperty(_T("授权机器码"), (_variant_t)theLicenseFile.GetMachineCode(), _T("授权计算机的机器码"));
#ifdef _LICENSE_VIEW_APP
		pProp->AllowEdit(FALSE);
#endif
		pGroup->AddSubItem( pProp );

		// 是否授权过期时间
#ifdef _LICENSE_VIEW_APP
		CString szBoolText = theLicenseFile.IsExpiredTime() ? _T("True") : _T("False");
		pProp = new CTPRIPropertyGridProperty(_T("允许授权过期"), (_variant_t)szBoolText, 
			_T("是否允许授权过期日期生效，如果为TRUE，则到达指定授权过期日期后授权文件将失效"));
		pProp->AllowEdit(FALSE);
#else
		pProp = new CTPRIPropertyGridProperty(_T("允许授权过期"), (_variant_t)theLicenseFile.IsExpiredTime(), 
			_T("是否允许授权过期日期生效，如果为TRUE，则到达指定授权过期日期后授权文件将失效"));
#endif
		pGroup->AddSubItem( pProp );

		// 授权过期时间
#ifdef _LICENSE_VIEW_APP
		CTime tt(theLicenseFile.GetExpiredTime());
		pProp = new CTPRIPropertyGridProperty(_T("授权过期日期"), (_variant_t)tt.Format(_T("%Y-%m-%d")), _T("授权计算机的机器码"), 0);
		pProp->AllowEdit(FALSE);
#else
		pProp = new CTPRIPropertyGridDateProperty(_T("授权过期日期"), theLicenseFile.GetExpiredTime(), _T("授权计算机的机器码"), 0);
#endif
		pGroup->AddSubItem( pProp );

		m_wndPropList.AddProperty(pGroup);
	}
	else {  // 软件项
		m_szSoftwareName = lpszSoftwareName;

		CLicenseSoftware *pSoftware = theLicenseFile.GetLicenseSoftware(lpszSoftwareName);
		if (!pSoftware)
			return FALSE;

		CTPRIPropertyGridProperty *pGroup;
		CTPRIPropertyGridProperty* pProp;
		vector<CTPRIPropertyGridProperty *> arrGroup;
		string strOptions;

		_LicPropertiesArray	&arrProperties = pSoftware->GetProperties();
		for (size_t i = 0; i < arrProperties.size(); i++)
		{
			CLicenseProperty &pty = arrProperties[i];

			switch(pty.GetDataType())
			{
			case LIC_DT_BOOL:
#ifdef _LICENSE_VIEW_APP
				pProp = new CTPRIPropertyGridProperty(pty.GetDisplayName(), (_variant_t)(pty.AsBool()?_T("True"):_T("False")), pty.GetDescription());
#else
				pProp = new CTPRIPropertyGridProperty(pty.GetDisplayName(), (_variant_t)pty.AsBool(), pty.GetDescription());
#endif
				break;
			case LIC_DT_NUMBER:
				pProp = new CTPRIPropertyGridProperty(pty.GetDisplayName(), (_variant_t)pty.AsLong(), pty.GetDescription());
#ifndef _LICENSE_VIEW_APP
				if (pty.GetMinValue() < pty.GetMaxValue())
				{
					pProp->EnableSpinControl(TRUE, pty.GetMinValue(), pty.GetMaxValue());
				}
#endif
				break;
			case LIC_DT_FLOAT:
				pProp = new CTPRIPropertyGridProperty(pty.GetDisplayName(), (_variant_t)pty.AsDouble(), pty.GetDescription());
				break;
			case LIC_DT_DATE:
#ifdef _LICENSE_VIEW_APP
				{
					CTime tt( pty.AsLong() );
					pProp = new CTPRIPropertyGridProperty(pty.GetDisplayName(), tt.Format(_T("%Y-%m-%d")), pty.GetDescription());
				}
#else
				pProp = new CTPRIPropertyGridDateProperty(pty.GetDisplayName(), pty.AsLong(), pty.GetDescription());
#endif
				break;
			//case LIC_DT_STRING:
			default:
				pProp = new CTPRIPropertyGridProperty(pty.GetDisplayName(), (_variant_t)pty.AsString(), pty.GetDescription());
#ifndef _LICENSE_VIEW_APP
				strOptions = pty.GetOptions();
				if (!strOptions.empty())
				{
					vector<string> arrOptions = stl_string::split(strOptions, ';');
					for (size_t i = 0; i < arrOptions.size(); i++)
					{
						pProp->AddOption( arrOptions[i].c_str() );						
					}
					pProp->AllowEdit(FALSE);
				}
#endif
				break;
			}
#ifdef _LICENSE_VIEW_APP
			pProp->AllowEdit(FALSE);
#endif
			pProp->setPropertyName( pty.GetName() );

			CString strCategory = pty.GetCategory();
			if (strCategory.IsEmpty())
				arrGroup.push_back( pProp );
			else {
				BOOL bFind = FALSE;
				for (size_t i = 0; i < arrGroup.size(); i++)
				{
					if (strCategory.Compare(arrGroup[i]->GetName()) == 0)
					{
						arrGroup[i]->AddSubItem(pProp);
						bFind = TRUE;
						break;
					}
				}

				if (!bFind) {
					pGroup = new CTPRIPropertyGridProperty( strCategory );
					pGroup->AddSubItem(pProp);
					arrGroup.push_back(pGroup);
				}
			}
		}

		for (size_t i = 0; i < arrGroup.size(); i++)
		{
			m_wndPropList.AddProperty(arrGroup[i]);
		}
	}

	m_wndPropList.RedrawWindow();

	return TRUE;
}

void CLicenseMgrView::RemoveAllLicensePropList()
{
	m_wndPropList.RemoveAll();
	m_wndPropList.RedrawWindow();
}

BOOL CLicenseMgrView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CLicenseMgrView 绘制

void CLicenseMgrView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

#ifdef _LICENSE_VIEW_APP
	if (m_bFirstOpenView)
	{
		if (Utility::DirectoryExists(GetDocument()->m_szPathName))
			GetDocument()->OnOpenDocument(GetDocument()->m_szPathName);
		else
			GetDocument()->SetPathName(GetDocument()->m_szPathName);

		m_bFirstOpenView = FALSE;
	}
#endif

	UpdateCurSelLicensePropList();
}

void CLicenseMgrView::OnDraw(CDC* /*pDC*/)
{
	CLicenseMgrDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}

void CLicenseMgrView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CLicenseMgrView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	//theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CLicenseMgrView 诊断

#ifdef _DEBUG
void CLicenseMgrView::AssertValid() const
{
	CView::AssertValid();
}

void CLicenseMgrView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLicenseMgrDoc* CLicenseMgrView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLicenseMgrDoc)));
	return (CLicenseMgrDoc*)m_pDocument;
}
#endif //_DEBUG


// CLicenseMgrView 消息处理程序

void CLicenseMgrView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient,rectCombo;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top + cyTlb, rectClient.Width(), rectClient.Height() - cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
}

int CLicenseMgrView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("未能创建属性网格\n");
		return -1;      // 未能创建
	}

	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* 已锁定*/);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* 锁定*/);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// 所有命令将通过此控件路由，而不是通过主框架路由:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();
	return 0;
}

void CLicenseMgrView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CLicenseMgrView::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
}

void CLicenseMgrView::OnUpdateExpandAllProperties(CCmdUI* pCmdUI)
{
}

void CLicenseMgrView::OnSortProperties()
{
	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

void CLicenseMgrView::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}

void CLicenseMgrView::OnSetFocus(CWnd* pOldWnd)
{
	CView::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CLicenseMgrView::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CView::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CLicenseMgrView::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

  	LOGFONT lf;
  	afxGlobalData.fontRegular.GetLogFont(&lf);
 
 	NONCLIENTMETRICS info;
 	info.cbSize = sizeof(info);
 
 	afxGlobalData.GetNonClientMetrics(info);
 
 	lf.lfHeight = info.lfMenuFont.lfHeight;
 	lf.lfWeight = info.lfMenuFont.lfWeight;
 	lf.lfItalic = info.lfMenuFont.lfItalic;
 
	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
}


LRESULT CLicenseMgrView::OnPropertyChanged(WPARAM, LPARAM lParam)
{
#ifndef _LICENSE_VIEW_APP
	CTPRIPropertyGridProperty* pProp = reinterpret_cast<CTPRIPropertyGridProperty*>(lParam);
	
	COleVariant varValue = pProp->GetValue(); // 改变之后的值
	// pProp->GetOriginalValue();  // 改变之前的值

	CString strValue;

	if (m_szSoftwareName.IsEmpty()) // 当前编辑的是授权文件属性
	{
		CString strName = pProp->GetName();  // 被改变的参数名

		if (strName == _T("授权厂家"))
		{
			strValue = varValue.bstrVal;

			theLicenseFile.SetUserName( strValue );
		}
		else if (strName == _T("授权机器码"))
		{
			strValue = varValue.bstrVal;

			theLicenseFile.SetMachineCode( strValue );
		}
		else if (strName == _T("允许授权过期"))
		{
			theLicenseFile.EnabledExpiredTime( varValue.boolVal != VARIANT_FALSE );
		}
		else if (strName == _T("授权过期日期"))
		{
			theLicenseFile.SetExpiredTime( varValue.lVal );
		}
	}
	else { // 当前编辑的是软件授权项属性
		CLicenseSoftware *pSoftware = theLicenseFile.GetLicenseSoftware(m_szSoftwareName);
		if (!pSoftware)
			return 0;

		CString strName = pProp->GetPropertyName();  // 被改变的参数名

		CLicenseProperty *pPty = pSoftware->GetProperty(strName);
		if (!pPty)
			return 0;

		switch(pPty->GetDataType())
		{
		case LIC_DT_BOOL:
			pPty->SetValue( (bool)(varValue.boolVal != VARIANT_FALSE) );
			break;
		case LIC_DT_NUMBER:
		case LIC_DT_DATE:
			pPty->SetValue( varValue.lVal );
			break;
		case LIC_DT_FLOAT:
			pPty->SetValue( varValue.dblVal );
			break;
			//case LIC_DT_STRING:
		default:
			strValue = varValue.bstrVal;
			pPty->SetValue( strValue );
			break;
		}
	}

	if (pProp->IsModified())
		GetDocument()->SetModifiedFlag(TRUE);
#endif

	return 0;
}




