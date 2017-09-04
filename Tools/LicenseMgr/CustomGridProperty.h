#pragma once

//////////////////////////////////////////////////////////////////////////
/// CTPRIPropertyGridProperty - TPRI属性基础类，主要增加m_szPropertyName变量用于存在属性项的原始名
//////////////////////////////////////////////////////////////////////////
class CTPRIPropertyGridProperty : public CMFCPropertyGridProperty
{
public:
	CTPRIPropertyGridProperty(const CString& strGroupName, DWORD_PTR dwData = 0, BOOL bIsValueList = FALSE) 
		: CMFCPropertyGridProperty(strGroupName, dwData, bIsValueList) {}

	// Simple property
	CTPRIPropertyGridProperty(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL, LPCTSTR lpszValidChars = NULL)
		: CMFCPropertyGridProperty(strName, varValue, lpszDescr, dwData, lpszEditMask, lpszEditTemplate, lpszValidChars) {}

public:
	LPCTSTR GetPropertyName() const {
		return m_szPropertyName;
	}
	void setPropertyName(LPCTSTR lpszPropertyName) {
		m_szPropertyName = lpszPropertyName;
	}

private:
	CString m_szPropertyName;
};

//////////////////////////////////////////////////////////////////////////
/// CTPRIPropertyGridDateProperty - 日期属性编辑器
//////////////////////////////////////////////////////////////////////////
class CTPRIPropertyGridDateProperty : public CTPRIPropertyGridProperty
{
public:
	CTPRIPropertyGridDateProperty(const CString& strName, long lTime, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);

	virtual BOOL OnUpdateValue();
	virtual CString FormatProperty();

protected:
	virtual CWnd* CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat);
	virtual BOOL OnEdit(LPPOINT /*lptClick*/);
};

//////////////////////////////////////////////////////////////////////////
/// CTPRIPropertyGridMultipleLineTextProperty - 多行文本属性编辑器
//////////////////////////////////////////////////////////////////////////
class CTPRIPropertyGridMultipleLineTextProperty : public CTPRIPropertyGridProperty
{
public:
	CTPRIPropertyGridMultipleLineTextProperty(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);

	int GetEditHeight() const {
		return m_nEditHeight;
	}
	void SetEditHeight(int nEditHeight) {
		m_nEditHeight = nEditHeight;
	}
protected:
	virtual CWnd* CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat);

private:
	int m_nEditHeight;
};

//////////////////////////////////////////////////////////////////////////
/// CTPRIPropertyGridMachineCodeProperty - 机器码编辑器
//////////////////////////////////////////////////////////////////////////
class CTPRIPropertyGridMachineCodeProperty : public CTPRIPropertyGridMultipleLineTextProperty
{
public:
	CTPRIPropertyGridMachineCodeProperty(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);

	virtual BOOL OnUpdateValue();
};