////////////////////////////////////////////////////////////////
// MSDN Magazine -- August 2005
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual Studio .NET 2003 (V7.1) on Windows XP. Tab size=3.
//
#include "stdafx.h"
#include "GeneralAPI.h"
#include "General\Regex.h"

TRUNK_NAMESPACE_BEGIN

//////////////////
// Replace 1st or all occurrences of regex in input string with szVal. Returns
// new string; doesn't change original.
//
CString CRegex::Replace(LPCTSTR szIn, LPCTSTR szVal, BOOL bAll)
{
	CString s;
	LPCTSTR szRest = szIn;
	while (Match(szIn, &szRest)) {
		int offset=0;
		GetMatch(&offset);
		CString temp = szIn;
		s += temp.Left(offset);
		s += szVal;
		szIn = szRest;
		if (!bAll)
			break;
	}
	s += szIn;
	return s;
}

//////////////////
// Static overload for Replace doesn't require a CRegex object.
//
CString CRegex::Replace(LPCTSTR szIn, LPCTSTR regex, LPCTSTR szVal, BOOL bAll,
	BOOL bIgnoreCase)
{
	CRegex re(regex, !bIgnoreCase);
	return re.Replace(szIn, szVal, bAll);
}

//////////////////
// Dyanmic version uses callback fn for replacement text.
//
CString CRegex::Replace(LPCTSTR szIn, REREPLACEPROC replfn, void* param)
{
	CString s;
	LPCTSTR szRest = szIn;
	while (Match(szIn, &szRest)) {
		int offset=0;
		CString match = GetMatch(&offset);
		CString temp = szIn;
		s += temp.Left(offset);
		CString szVal = (*replfn)(*this, match, param);
		s += szVal;
		szIn = szRest;
	}
	s += szIn;
	return s;
}

//////////////////
// Static overload of dyanmic Replace.
//
CString CRegex::Replace(LPCTSTR szIn, LPCTSTR regex, REREPLACEPROC replfn, void* param,
	BOOL bIgnoreCase)
{
	CRegex re(regex, !bIgnoreCase);
	return re.Replace(szIn, replfn, param);
}

//////////////////
// Implement Perl-style split function. Breaks szIn into an array of strings
// separated by regex.
//
vector<CString> CRegex::Split(LPCTSTR szIn, int nMax)
{
	vector<CString> ar;
	ar.reserve(nMax>0 ? nMax : 10);		 // reserve for efficiency
	if (nMax==0)
		nMax = 0xFFFF;							 // should be enough!

	int count = 0;								 // number of strings split
	LPCTSTR szRest = szIn;					 // next character to search
	while (Match(szIn, &szRest)) {		 // while separator found:
		int offset=0;							 //   offset of match into input string
		GetMatch(&offset);					 //   ..get it
		CString str = szIn;					 //   start of input string
		str = str.Left(offset);				 //   left of input string not including match
		ar.push_back(str);					 //   add it to array
		szIn = szRest;							 //   continue searching after match
		if (++count >= nMax)					 //   if enough matches found:
			break;								 //     ..stop
	}
	if (*szIn)									 // if anything left:
		ar.push_back(szIn);					 //   add last string
	return ar;
}

//////////////////
// Static overload for Split doesn't require CRegex object.
//
vector<CString> 
CRegex::Split(LPCTSTR szIn, LPCTSTR szSep, BOOL bCaseSensitive, int nMax)
{
	CRegex re(szSep, bCaseSensitive);
	return re.Split(szIn, nMax);
}

//////////////////
// Get string name of ATL REParseError error code.
//
LPCTSTR CRegex::GetErrorName(REParseError err)
{
	static struct {
		REParseError err;
		LPCTSTR name;
	} MyErrs[] = {
		{ REPARSE_ERROR_OK,					_T("REPARSE_ERROR_OK") },
		{ REPARSE_ERROR_OUTOFMEMORY,		_T("REPARSE_ERROR_OUTOFMEMORY") },
		{ REPARSE_ERROR_BRACE_EXPECTED,	_T("REPARSE_ERROR_BRACE_EXPECTED") },
		{ REPARSE_ERROR_PAREN_EXPECTED,	_T("REPARSE_ERROR_PAREN_EXPECTED") },
		{ REPARSE_ERROR_BRACKET_EXPECTED,_T("REPARSE_ERROR_BRACKET_EXPECTED") },
		{ REPARSE_ERROR_UNEXPECTED,		_T("REPARSE_ERROR_UNEXPECTED") },
		{ REPARSE_ERROR_EMPTY_RANGE,		_T("REPARSE_ERROR_EMPTY_RANGE") },
		{ REPARSE_ERROR_INVALID_GROUP,	_T("REPARSE_ERROR_INVALID_GROUP") },
		{ REPARSE_ERROR_INVALID_RANGE,	_T("REPARSE_ERROR_INVALID_RANGE") },
		{ REPARSE_ERROR_EMPTY_REPEATOP,	_T("REPARSE_ERROR_EMPTY_REPEATOP") },
		{ REPARSE_ERROR_INVALID_INPUT,	_T("REPARSE_ERROR_INVALID_INPUT") },
		{ REPARSE_ERROR_OK, NULL }
	};
	for (int i=0; MyErrs[i].name; i++) {
		if (err==MyErrs[i].err)
			return MyErrs[i].name;
	}
	return _T("Unknown error");
}

TRUNK_NAMESPACE_END