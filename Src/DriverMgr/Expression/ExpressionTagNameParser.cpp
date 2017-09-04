// ScriptEngine.cpp: implementation of the CScriptEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <assert.h>  // assert()
#include "ExpressionTagNameParser.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CExpressionTagNameParser::CExpressionTagNameParser(LPCTSTR lpszScript/* = NULL*/)
{
	if (lpszScript) {
		Parse(lpszScript);
	}
}

CExpressionTagNameParser::~CExpressionTagNameParser()
{
	m_arrTagNames.RemoveAll();
}

LPCTSTR CExpressionTagNameParser::GetAt(int nIndex) const
{
	return m_arrTagNames[nIndex];
}

void CExpressionTagNameParser::_AdjustTagName(CString &szTagName)
{
	szTagName.TrimLeft();
	szTagName.TrimRight();
	if (szTagName.IsEmpty() || szTagName.GetLength() == 1)
		return;

	if (szTagName.GetAt(0) == _T('\"') && szTagName.GetAt(szTagName.GetLength() - 1) == _T('\"'))
		szTagName = szTagName.Mid(1, szTagName.GetLength() - 2);
	else if (szTagName.GetAt(0) == _T('\'') && szTagName.GetAt(szTagName.GetLength() - 1) == _T('\''))
		szTagName = szTagName.Mid(1, szTagName.GetLength() - 2);

	szTagName.TrimLeft();
	szTagName.TrimRight();
}

BOOL CExpressionTagNameParser::_AddTagName(LPCTSTR lpszTagName)
{
	for(int i = 0; i < m_arrTagNames.GetSize(); i++)
		if (m_arrTagNames[i] == lpszTagName)
			return FALSE;

	m_arrTagNames.Add(lpszTagName);
	return TRUE;
}

BOOL CExpressionTagNameParser::_Match_X(LPCTSTR lpszRegexExpr, LPCTSTR lpszStr, UINT nResultIndex, BOOL bCaseSensitive/* = FALSE*/)
{	
	CRegex regex(lpszRegexExpr, bCaseSensitive);
	if (regex.m_err != REPARSE_ERROR_OK)
		return FALSE;

	int nCount = 0;
	regex.SetInput(lpszStr);
	while (regex.NextMatch())
	{
		if (regex.GetNumGroups() <= nResultIndex)
			continue;

		int offset = 0;
		CString szTagName = regex.GetGroup(nResultIndex, &offset);
		_AdjustTagName(szTagName);
		if (!szTagName.IsEmpty())
		{
			_AddTagName(szTagName);
			nCount++;
		}
	}
	
	return (nCount > 0);
}

void CExpressionTagNameParser::Parse(LPCTSTR lpszScript)
{
	m_arrTagNames.RemoveAll();

	// 前后增加一个空格，用于正确提取this变量
	CString szCode = _T(" ");
	szCode += lpszScript;
	szCode += _T(" ");

	_Match_X( REGEX_EXPR, szCode, 0 );
	//_Match_X( REGEX_THIS_EXPR, szCode, 0 );

	CRegex regex(REGEX_THIS_EXPR, TRUE);
	if (regex.m_err == REPARSE_ERROR_OK)
	{
		int nCount = 0;
		regex.SetInput(szCode);
		while (regex.NextMatch())
		{
			int offset = 0;
			CString szTagName = regex.GetGroup(0, &offset);
			if (szTagName == _T("this")) {
				_AddTagName(_T("__this__"));
				break;
			}
		}
	}
}

BOOL CExpressionTagNameParser::IsExists(LPCTSTR lpszTagName)
{
	for (int i = 0; i < m_arrTagNames.GetSize(); i++)
	{
		if (m_arrTagNames[i] == lpszTagName)
			return TRUE;
	}

	return FALSE;
}