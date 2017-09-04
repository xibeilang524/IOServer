// TagItemObject.h: interface for the CTagItemObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXPRESSIONTAGNAMEPARSER_H__5168BBA8_0765_43F2_A2D1_624FD9B6DF64__INCLUDED_)
#define AFX_EXPRESSIONTAGNAMEPARSER_H__5168BBA8_0765_43F2_A2D1_624FD9B6DF64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*
 * 从脚本中使用正则表达式来解析出其中包含的标签名列表，有以下几种情况来确定
 * 标签字符串：

 *  TV("Tag Name")
 */

/*
 * 用于处理第7和第8种情况，标签名匹配结果在第三项中
 * GRETA::Regex :  [^a-zA-Z0-9_]*TV[' ']*\([' ']*"(("\\".|[^\\])*?)"
 * CATLRegExp   :  [^a-zA-Z0-9_]*TV\b*\(\b*{(\"[^\"]*\")|(\'[^\']*\')}
 */
#define REGEX_EXPR _T("[^a-zA-Z0-9_]*TV\b*\\(\\b*{(\\\"[^\\\"]*\\\")|(\\'[^\\']*\\')}")

/*
 * 解析this变量
 * [^a-zA-Z0-9_]{this}[^a-zA-Z0-9_]
 */
#define REGEX_THIS_EXPR _T("[^a-zA-Z0-9_]{this}[^a-zA-Z0-9_]")

// 该方法还未考虑注释块的问题，可以解决的方式时先将字符串中的注释代码去除，再进行正则表达式解析
class CExpressionTagNameParser
{
public:
	CExpressionTagNameParser(LPCTSTR lpszScript = NULL);
	~CExpressionTagNameParser();

	int GetCount() { return m_arrTagNames.GetSize(); }
	LPCTSTR GetAt(int nIndex) const;
	void Parse(LPCTSTR lpszScript);
	BOOL IsExists(LPCTSTR lpszTagName);

protected:
	void _AdjustTagName(CString &szTagName);
	BOOL _AddTagName(LPCTSTR lpszTagName);

	BOOL _Match_X(LPCTSTR lpszRegexExpr, LPCTSTR lpszStr, UINT nResultIndex, BOOL bCaseSensitive = FALSE);

private:
	CStringArray m_arrTagNames;
};

#endif // !defined(AFX_EXPRESSIONTAGNAMEPARSER_H__5168BBA8_0765_43F2_A2D1_624FD9B6DF64__INCLUDED_)
