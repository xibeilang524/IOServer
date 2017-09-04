///////////////////////////////////////////////////////////////////
//   参数字符串解析器
//
//   该单元封装了参数化字符串的解析与构造功能。
//   我们对参数化字符串格式定义如下：
//
//       Param1=Value1;Param2=Value2;....
//
//       参数字符串的参数由一系列的Param和Value组成，Param和Value
//   之间用等号分隔，两组参数之前用分号分隔，参数名Param只允许使用汉
//   字、数字或英文字母，参数值Value无特别约定。
//       Param和Value约定不得大于255个字符
//
//
//   作者：何清
//   tech
// 
///////////////////////////////////////////////////////////////////
//   更新历史:
// 
//   @2010-11-28
//      创建文档
///////////////////////////////////////////////////////////////////
#ifndef __PARAMETER_STRING_PARSER_H__
#define __PARAMETER_STRING_PARSER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


TRUNK_NAMESPACE_BEGIN

class _GENERAL_CLASS CParameterStringParser
{
public:
	CParameterStringParser(LPCTSTR lpszParameterString = NULL);
	virtual ~CParameterStringParser();

public:
	BOOL SetParameterString(LPCTSTR lpszParameterString);
	CString GetParameterString();
	
	int GetCount() const {
		return (int)m_mapValues.GetCount();
	}
	BOOL IsEmpty() const {
		return (GetCount() == 0);
	}
	
	BOOL Lookup(LPCTSTR lpszParam, CString& szValue) const;
	void RemoveAll();
	void Remove(LPCTSTR lpszParam);

	CString& operator []( LPCTSTR lpszParam );
	int GetValueByInt(LPCTSTR lpszParam, int nDefault = 0);
	long GetValueByLong(LPCTSTR lpszParam, long nDefault = 0);
	float GetValueByFloat(LPCTSTR lpszParam, float fDefault = 0.0);
	double GetValueByDouble(LPCTSTR lpszParam, double dbDefault = 0.0);
	CString GetValueByString(LPCTSTR lpszParam, LPCTSTR lpszDefault = _T(""));
	void SetValue(LPCTSTR lpszParam, int nValue);
	void SetValue(LPCTSTR lpszParam, long nValue);
	void SetValue(LPCTSTR lpszParam, float nValue);
	void SetValue(LPCTSTR lpszParam, double nValue);
	void SetValue(LPCTSTR lpszParam, LPCTSTR lpszValue);

protected:
	BOOL DoParse();
	BOOL DoParseLine(LPCTSTR lpszLine, int nSeparator);
	void EncodeString(LPCTSTR lpszStr, CString &szEncodeStr); // 编码字符串

private:
	CString				m_szParameterString;
	CMapStringToString	m_mapValues;
};


TRUNK_NAMESPACE_END

#endif // end of __PARAMETER_STRING_PARSER_H__
