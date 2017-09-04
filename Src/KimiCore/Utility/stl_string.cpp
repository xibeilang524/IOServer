// Utility.cpp : ¸¨Öúº¯Êý
//

#include "stdafx.h"
#include "KimiCore.h"
#include "KimiCore\stl_string.h"

_KIMI_NAMESPACE_BEGIN

string::size_type stl_string::truncate(string &s, string::size_type limit)
{
	size_t len = s.length();
	if (len <= limit)
		return 0;

	char ch;
	while(len > limit)
	{
		ch = s[len - 1];

		if (ch >= 0 && ch <= 127)
			--len;
		else {
			--len;
			if ((len - 2 >= 0) && (s[len - 2] < 0 || s[len - 2] > 127))
				--len;
		}
	}

	string::size_type erase = s.length() - len;
	s.erase(len, erase);

	return erase;
}

vector<string> stl_string::split(const string &s, string::size_type limit/* = string::npos*/)
{
	vector<string> v;

	string::const_iterator i = s.begin(), last = i;
	for (; i != s.end(); i++)
	{
		if (*i == ' ' || *i == '\n' || *i == '\t' || *i == '\r')
		{
			if (i + 1 != s.end() && (i[1] == ' ' || i[1] == '\n' || i[1] == '\t' || i[1] == '\r'))
				continue;

			v.push_back(string(last, i));
			last = i + 1;
			if (v.size() >= limit - 1)
			{
				v.push_back(string(last, s.end()));
				return v;
			}
		}
	}

	if (last != i)
		v.push_back(string(last, i));
	return v;
}

vector<string> stl_string::split(const string &s, string::value_type separator, string::size_type limit/* = string::npos*/)
{
	vector<string> v;

	string::const_iterator i = s.begin(), last = i;
	for (; i != s.end(); i++)
	{
		if (*i == separator)
		{
			v.push_back(string(last, i));
			last = i + 1;
			if (v.size() >= limit - 1)
			{
				v.push_back(string(last, s.end()));
				return v;
			}
		}
	}

	if (last != i)
		v.push_back(string(last, i));

	return v;
}

vector<string> stl_string::split(const string &s, const string &separator, string::size_type limit/* = string::npos*/)
{
	vector<string> v;

	string::const_iterator	i = s.begin(), last = i;
	for (; i != s.end(); i++)
	{
		if (string(i, i + separator.length()) == separator)
		{
			v.push_back(string(last, i));
			last = i + separator.length();

			if (v.size() >= limit - 1)
			{
				v.push_back(string(last, s.end()));
				return v;
			}
		}
	}

	if (last != i)
		v.push_back(string(last, i));

	return v;
}

long int stl_string::integer(const string &s)
{
	long int retval = 0;
	bool neg = false;

	for (string::const_iterator i = s.begin(); i != s.end(); i++)
	{
		if (i == s.begin())
		{
			if (*i == '-')
			{
				neg = true;
				continue;
			}
			else if (*i == '+')
				continue;
		}
		if (*i >= '0' && *i <= '9')
		{
			retval *= 10;
			retval += *i - '0';
		}
		else
			break;
	}

	if (neg)
		retval *= -1;

	return retval;
}

vector<string> stl_string::chunk_split(const string &s, string::size_type chunklen)
{
	vector<string> retval;
	retval.reserve(s.size() / chunklen + 1);

	string::size_type count = 0;
	string::const_iterator i = s.begin(), last = i;
	for (; i != s.end(); i++, count++)
	{
		if (count == chunklen)
		{
			count = 0;
			retval.push_back(string(last, i));
			last = i;
		}
	}

	if (last != i)
		retval.push_back(string(last, i));

	return retval;
}

void stl_string::replace(string &s, const string &needle, const string &rs)
{
	string::size_type
		lastpos = 0,
		thispos;

	while ((thispos = s.find(needle, lastpos)) != string::npos)
	{
		s.replace(thispos, needle.length(), rs);
		lastpos = thispos + 1;
	}
}

void stl_string::replace(string &s, string::value_type needle, string::value_type c)
{
	for (string::iterator i = s.begin(); i != s.end(); i++)
		if (*i == needle)
			*i = c;
}

void stl_string::tolower(string &s)
{
	for (string::iterator i = s.begin(); i != s.end(); i++)
		if (*i >= 'A' && *i <= 'Z')
			*i = (*i) + ('a' - 'A');
}

void stl_string::toupper(string &s)
{
	for (string::iterator i = s.begin(); i != s.end(); i++)
		if (*i >= 'a' && *i <= 'z')
			*i = (*i) - ('a' - 'A');
}

string::size_type stl_string::count(const string &s, const string &str)
{
	string::size_type
		count = 0,
		last = 0,
		cur = 0;

	while ((cur = s.find(str, last + 1)) != string::npos)
	{
		count++;
		last = cur;
	}

	return count;
}

bool stl_string::is_alnum(const string &s)
{
	if (s.length() == 0)
		return false;

	for (string::const_iterator i = s.begin(); i != s.end(); i++)
	{
		if (*i < 'A' || *i > 'Z')
			if (*i < '0' || *i > '9')
				if (*i < 'a' || *i > 'z')
					return false;
	}

	return true;
}

bool stl_string::is_alpha(const string &s)
{
	if (s.length() == 0)
		return false;

	for (string::const_iterator i = s.begin(); i != s.end(); i++)
		if (*i < 'A' || (*i > 'Z' && (*i < 'a' || *i > 'z')))
			return false;

	return true;
}

bool stl_string::is_numeric(const string &s)
{
	if (s.length() == 0)
		return false;

	for (string::const_iterator i = s.begin(); i != s.end(); i++)
		if (*i < '0' || *i > '9')
			return false;

	return true;
}

bool stl_string::is_lower(const string &s)
{
	if (s.length() == 0)
		return false;

	for (string::const_iterator i = s.begin(); i != s.end(); i++)
		if (*i < 'a' || *i < 'z')
			return false;

	return true;
}

bool stl_string::is_upper(const string &s)
{
	if (s.length() == 0)
		return false;

	for (string::const_iterator i = s.begin(); i != s.end(); i++)
		if (*i < 'A' || *i > 'Z')
			return false;

	return true;
}

void stl_string::swapcase(string &s)
{
	for (string::iterator i = s.begin(); i != s.end(); i++)
		if (*i >= 'A' && *i <= 'Z')
			*i += ('a' - 'A');
		else if (*i >= 'a' && *i <= 'z')
			*i -= ('a' - 'A');
}

void stl_string::ltrim(string &s)
{
	if (s.empty())
		return;

	string::iterator c; 

	// Erase whitespace before the string 
	for (c = s.begin(); c != s.end() && iswspace(*c++);); s.erase(s.begin(), --c); 
}

void stl_string::rtrim(string &s)
{
	if (s.empty())
		return;

	string::iterator c; 

	// Erase whitespace after the string 
	for (c = s.end(); c != s.begin() && iswspace(*--c);); s.erase(++c, s.end()); 
}

void stl_string::trim(string &s)
{
	if (s.empty())
		return;

	string::iterator c; 

	// Erase whitespace before the string 
	for (c = s.begin(); c != s.end() && iswspace(*c++);); s.erase(s.begin(), --c); 

	// Erase whitespace after the string 
	for (c = s.end(); c != s.begin() && iswspace(*--c);); s.erase(++c, s.end()); 
}

string stl_string::format(const char *fmt, ...)
{
	char str[_STL_STRING_FORMAT_MAX_BUFFER];

	memset(str, 0, _STL_STRING_FORMAT_MAX_BUFFER);

	va_list args;
	va_start ( args, fmt );
	vsnprintf_s(str, _STL_STRING_FORMAT_MAX_BUFFER, _TRUNCATE, fmt, args );
	va_end ( args );

	return string(str);
}

string stl_string::format(size_t maxsize, const char *fmt, ...)
{
	char *str = (char *)malloc(maxsize);
	if (!str)
		return "";
	
	memset(str, 0, maxsize);

	va_list args;
	va_start ( args, fmt );
	vsnprintf_s(str, maxsize, _TRUNCATE, fmt, args );
	va_end ( args );

	string s = str;
	free(str);

	return s;
}

bool stl_string::format(string& s, const char *fmt, ...)
{
	char str[_STL_STRING_FORMAT_MAX_BUFFER];

	memset(str, 0, _STL_STRING_FORMAT_MAX_BUFFER);

	va_list args;
	va_start ( args, fmt );
	vsnprintf_s (str, _STL_STRING_FORMAT_MAX_BUFFER, _TRUNCATE, fmt, args );
	va_end ( args );

	s = str;

	return true;
}

bool stl_string::format(string& s, size_t maxsize, const char *fmt, ...)
{
	char *str = (char *)malloc(maxsize);
	if (!str)
		return false;

	memset(str, 0, maxsize);

	va_list args;
	va_start ( args, fmt );
	vsnprintf_s(str, maxsize, _TRUNCATE, fmt, args );
	va_end ( args );

	s = str;

	free(str);

	return true;
}

_KIMI_NAMESPACE_END