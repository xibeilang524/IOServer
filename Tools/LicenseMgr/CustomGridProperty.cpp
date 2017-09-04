#include "StdAfx.h"
#include "CustomGridProperty.h"

#define AFX_PROP_HAS_LIST 0x0001
#define AFX_PROP_HAS_BUTTON 0x0002
#define AFX_PROP_HAS_SPIN 0x0004

//////////////////////////////////////////////////////////////////////////
/// CMFCPropertyGridDateProperty

CTPRIPropertyGridDateProperty::CTPRIPropertyGridDateProperty(const CString& strName, long lTime, LPCTSTR lpszDescr, DWORD_PTR dwData)
	: CTPRIPropertyGridProperty(strName, (_variant_t)lTime, lpszDescr,dwData, NULL, NULL, NULL)
{
}

BOOL CTPRIPropertyGridDateProperty::OnUpdateValue()
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndInPlace);
	ASSERT_VALID(m_pWndList);
	ASSERT(::IsWindow(m_pWndInPlace->GetSafeHwnd()));

	CDateTimeCtrl *pWndDateCtrl = (CDateTimeCtrl *)m_pWndInPlace;
	CTime t;
	pWndDateCtrl->GetTime(t);

	BOOL bIsChanged = m_varValue.lVal != t.GetTime();

	if (bIsChanged)
	{
		m_varValue.lVal = (long)t.GetTime();

		m_pWndList->OnPropertyChanged(this);
	}

	return TRUE;
}

CString CTPRIPropertyGridDateProperty::FormatProperty()
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndList);

	COleVariant& var = m_varValue;

	CString strVal;

	if (m_bIsValueList)
	{
		for (POSITION pos = m_lstSubItems.GetHeadPosition(); pos != NULL;)
		{
			CMFCPropertyGridProperty* pProp = m_lstSubItems.GetNext(pos);
			ASSERT_VALID(pProp);

			strVal += pProp->FormatProperty();

			if (pos != NULL)
			{
				strVal += _T(", ");
			}
		}

		return strVal;
	}

	switch (var.vt)
	{
	case VT_BSTR:
		strVal = var.bstrVal;
		break;

	case VT_I4:
	case VT_INT:
		{
			CTime t(var.lVal);
			strVal = t.Format(_T("%Y-%m-%d"));
			break;
		}

	default:
		// Unsupported type
		strVal = _T("*** error ***");
	}

	return strVal;
}

CWnd* CTPRIPropertyGridDateProperty::CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat)
{
	switch (m_varValue.vt)
	{
	case VT_BSTR:
	case VT_R4:
	case VT_R8:
	case VT_UI1:
	case VT_I2:
	case VT_INT:
	case VT_UINT:
	case VT_I4:
	case VT_UI2:
	case VT_UI4:
	case VT_BOOL:
		break;

	default:
		if (!m_bIsValueList)
		{
			return NULL;
		}
	}

	CDateTimeCtrl *pWndDateCtrl = new CDateTimeCtrl();
	DWORD dwStyle = WS_VISIBLE | WS_CHILD | WS_TABSTOP | DTS_LONGDATEFORMAT;

	if (!m_bEnabled || !m_bAllowEdit)
	{
		dwStyle |= ES_READONLY;
	}
	pWndDateCtrl->Create(dwStyle, rectEdit, m_pWndList, AFX_PROPLIST_ID_INPLACE);
	pWndDateCtrl->SetFormat(_T("yyyy-MM-dd"));

	bDefaultFormat = TRUE;
	return pWndDateCtrl;
}

BOOL CTPRIPropertyGridDateProperty::OnEdit(LPPOINT /*lptClick*/)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndList);

	if (!HasValueField())
	{
		return FALSE;
	}

	m_pWndInPlace = NULL;

	CRect rectEdit;
	CRect rectSpin;

	AdjustInPlaceEditRect(rectEdit, rectSpin);

	BOOL bDefaultFormat = FALSE;
	m_pWndInPlace = CreateInPlaceEdit(rectEdit, bDefaultFormat);

	if (m_pWndInPlace != NULL)
	{
		CTime t(m_varValue.lVal);
		
		CDateTimeCtrl *pWndDateCtrl = (CDateTimeCtrl*)m_pWndInPlace;
		pWndDateCtrl->SetTime(&t);

		m_pWndInPlace->SetFont(IsModified() ? &m_pWndList->GetBoldFont() : m_pWndList->GetFont());
		m_pWndInPlace->SetFocus();

		if (!m_bAllowEdit)
		{
			m_pWndInPlace->HideCaret();
		}

		m_bInPlaceEdit = TRUE;
		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
/// CTPRIPropertyGridMultipleLineTextProperty
//////////////////////////////////////////////////////////////////////////
CTPRIPropertyGridMultipleLineTextProperty::CTPRIPropertyGridMultipleLineTextProperty(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr, DWORD_PTR dwData)
	: CTPRIPropertyGridProperty(strName,varValue, lpszDescr,dwData, NULL, NULL, NULL)
{
	m_nEditHeight = 100;
}

CWnd* CTPRIPropertyGridMultipleLineTextProperty::CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat)
{
	switch (m_varValue.vt)
	{
	case VT_BSTR:
	case VT_R4:
	case VT_R8:
	case VT_UI1:
	case VT_I2:
	case VT_INT:
	case VT_UINT:
	case VT_I4:
	case VT_UI2:
	case VT_UI4:
	case VT_BOOL:
		break;

	default:
		if (!m_bIsValueList)
		{
			return NULL;
		}
	}

	CEdit* pWndEdit = NULL;

	pWndEdit = new CEdit;

	DWORD dwStyle = WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_MULTILINE|ES_WANTRETURN|ES_AUTOVSCROLL |WS_HSCROLL| WS_VSCROLL;

	if (!m_bEnabled || !m_bAllowEdit)
	{
		dwStyle |= ES_READONLY;
	}

	rectEdit.bottom = rectEdit.top + m_nEditHeight;

	pWndEdit->Create(dwStyle, rectEdit, m_pWndList, AFX_PROPLIST_ID_INPLACE);

	bDefaultFormat = TRUE;
	return pWndEdit;
}

//////////////////////////////////////////////////////////////////////////
/// CTPRIPropertyGridMachineCodeProperty
//////////////////////////////////////////////////////////////////////////
CTPRIPropertyGridMachineCodeProperty::CTPRIPropertyGridMachineCodeProperty(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr/* = NULL*/, DWORD_PTR dwData/* = 0*/)
	: CTPRIPropertyGridMultipleLineTextProperty(strName,varValue, lpszDescr,dwData)
{

}

BOOL CTPRIPropertyGridMachineCodeProperty::OnUpdateValue()
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndInPlace);
	ASSERT_VALID(m_pWndList);
	ASSERT(::IsWindow(m_pWndInPlace->GetSafeHwnd()));

	CString strText;
	m_pWndInPlace->GetWindowText(strText);
	strText.Trim();

	BOOL bRes = FALSE;
	BOOL bIsChanged = FormatProperty() != strText;

	if (!strText.IsEmpty())
	{
		CMachineCodeHelper mch;
		if (!mch.SetMachineBase64Encode(strText))
		{
			strText = m_varValueOrig;
			m_pWndInPlace->SetWindowText(strText);

			AfxMessageBox(_T("机器码校验失败，请检查您输入的机器码信息是否完整！"), MB_OK | MB_ICONERROR);
			return FALSE;
		}
	}
	bRes = TextToVar(strText);

	if (bRes && bIsChanged)
	{
		m_pWndList->OnPropertyChanged(this);
	}

	return bRes;
}