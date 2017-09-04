#ifndef __REGEX_H__
#define __REGEX_H__

#pragma once
#pragma warning(push)
#pragma warning(disable : 4018)
#pragma warning(disable : 4389)
#include <atlrx.h>			// ATL regex
#pragma warning(pop)
#include <vector>				// STL vector
using namespace std;

TRUNK_NAMESPACE_BEGIN

//////////////////
// CRegex encapsulates CAtlRegExp<> to make ATL regular expressions a little
// more usable. CRegex hides CAtlRegExp and CAtlREMatchContext and also
// implements useful functions like Replace and Split. Assumes you are using
// default CAtlRECharTraits (ie, TCHARs).
//
class _GENERAL_CLASS CRegex : public CAtlRegExp<> {
protected:
 	CString m_re;					// the regular expression
	BOOL m_bCaseSensitive;		// case sensitive?
	CAtlREMatchContext<> m_mc; // internal ATL match context
	LPCTSTR m_szIn;				// original input string
	LPCTSTR m_szNext;				// next character to search

	// helper to extract string from ATL MatchGroup
	CString GetMGString(const CAtlREMatchContext<>::MatchGroup& mg) const
	{
		CString match = mg.szStart;
		return match.Left((int)(mg.szEnd-mg.szStart));
	}

public:
	REParseError m_err;			// current ATL parse error, if any

	// helper functions to get error name from error code
	static LPCTSTR GetErrorName(REParseError err);

	// default constructor
	CRegex() : m_err(REPARSE_ERROR_OK) { }

	// construct from RE and case-sensitive flag
	CRegex(LPCTSTR szRE, BOOL bCaseSensitive=TRUE)
	{
		m_err = Parse(szRE, bCaseSensitive);
	}

	// copy ctor
	CRegex(const CRegex& r)
	{
		*this = r; // do assignment to parse
	}

	~CRegex() { }

	// assignment: only copy regex and case flag, not dynamic state!
	// Reparse expression.
	const CRegex& operator= (const CRegex& r)
	{
		m_re = r.m_re;
		m_bCaseSensitive = r.m_bCaseSensitive;
		if (!m_re.IsEmpty())
			m_err = Parse(m_re, m_bCaseSensitive);
		return *this;
	}

	// convert to string: return RE
	operator LPCTSTR() const { return m_re; }

	// Parse RE: reset internal state and return error code.
	REParseError Parse(LPCTSTR szRE, BOOL bCaseSensitive=TRUE)
	{
		m_re = szRE;
		m_szIn = m_szNext = NULL;
		return CAtlRegExp<>::Parse(szRE, bCaseSensitive);
	}

	// Set input string. Use this with NextMatch to find all matches.
	void SetInput(LPCTSTR szIn) 
	{
		m_szIn = m_szNext = szIn;
	}

	// Find a single match in input string.
	// Optional 2nd arg returns pointer to next input char to search after
	// match. Used internally to find successive matches.
	BOOL Match(LPCTSTR szIn, LPCTSTR* ppszEnd=NULL)
	{
		if (szIn==NULL || *szIn==0)
			return FALSE;
		SetInput(szIn);
		return CAtlRegExp<>::Match(szIn, &m_mc, ppszEnd);
	}

	// Find next match after calling SetInput
	BOOL NextMatch()
	{
		return CAtlRegExp<>::Match(m_szNext, &m_mc, &m_szNext);
	}

	// Get current match; optional arg returns offset in input string.
	CString GetMatch(int* pOffset=NULL) const
	{
		if (pOffset)
			*pOffset = (int)(m_mc.m_Match.szStart - m_szIn);
		return GetMGString(m_mc.m_Match);
	}

	// Get number of groups. In ATL syntax, groups are marked with {}.
	UINT GetNumGroups() const
	{
		return m_mc.m_uNumGroups;
	}

	// Get nth match group. Optional arg returns offset into input string.
	CString GetGroup(int nIndex, int* pOffset=NULL) const
	{
		CAtlREMatchContext<>::MatchGroup mg;
		const_cast<CRegex*>(this)->m_mc.GetMatch(nIndex, &mg);
		if (pOffset)
			*pOffset = (int)(mg.szStart - m_szIn);			
		return GetMGString(mg);
	}

	// replace callback for dyanmic Replace
	typedef CString (CALLBACK* REREPLACEPROC)(const CRegex& re,
		const CString& match, void* param);

	// Global replace. This is not implemented by ATL, but easy to do. Does
	// not support $1 $2 variables (replace with subgroup match).
	CString Replace(LPCTSTR szIn, LPCTSTR val, BOOL bAll=TRUE);
	static CString Replace(LPCTSTR szIn, LPCTSTR regex, LPCTSTR val, BOOL bAll=TRUE,
		BOOL bIgnoreCase=FALSE);

	CString Replace(LPCTSTR szIn, REREPLACEPROC replfn, void* param);
	static CString Replace(LPCTSTR szIn, LPCTSTR regex, REREPLACEPROC replfn,
		void* param, BOOL bIgnoreCase=FALSE);

	// Split function implements Perl-style split. Splits input string into
	// array (STL vector) of CStrings serparated by regex. For example, if you
	// call Split("one,two, three",",\b*"), you'll get an array of three
	// strings: ["one","two","three"]. Optional arg says maximum number to
	// split.
	vector<CString> Split(LPCTSTR szIn, int nMax=0);
	static vector<CString> Split(LPCTSTR szIn, LPCTSTR szSep, BOOL bCaseSensitive=TRUE,
		int nMax=0);
};

TRUNK_NAMESPACE_END

#endif // end of __REGEX_H__