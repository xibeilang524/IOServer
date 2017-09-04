#ifndef __KIMI_KERNEL_STL_STRING_H__
#define __KIMI_KERNEL_STL_STRING_H__

#include <io.h>
#include <string>

_KIMI_NAMESPACE_BEGIN

#define _STL_STRING_FORMAT_MAX_BUFFER	1024

class KIMI_CORE_CLASS stl_string
{
public:
	/**
	 * 按指定长度截断字符串，如果字符串中包含有中文，则不允许出现半个汉字的情况
	 *
	 * @param[in/out] s     待截断的字符串
	 * @param[in]	  limit 保留字符串长度
	 * @return				返回截断字符串长度
	 */
	static string::size_type truncate(string &s, string::size_type limit);

	/**
	 * 将一个字符串以空格、'\r'、'\n'或'\t'为做分隔符进行分割，并返回分割后的字符串数组
	 *
	 * @param[in] limit     返回最大数组数
	 * @return				返回分割后的字符串数组
	 */
	 static vector<string> split(const string &s, string::size_type limit = string::npos);

	/**
	 * 将一个字符串s按指定的字符separator进行分割
	 *
	 * @param[in] s			待分割的字符串
	 * @param[in] separator 分隔字符
	 * @param[in] limit     返回最大数组数
	 * @return				返回分割后的字符串数组
	 *
	 * @使用例子
	 * @code
	 * std::string s("This|is|a|test.");
	 * std::vector<std::string> v = s.split('|');
	 * std::copy(v.begin(), v.end(), std::ostream_iterator<std::string>(std::cout, "\n"));
	 *
	 * This
	 * is
	 * a
	 * test.
	 * @endcode
	 */
	 static vector<string> split(const string &s, string::value_type separator, string::size_type limit = string::npos);

	/**
	 * 将一个字符串s按指定的字符串separator进行分割
	 *
	 * @param[in] s			待分割的字符串
	 * @param[in] separator 分隔字符串
	 * @param[in] limit     返回最大数组数
	 * @return				返回分割后的字符串数组
	 *
	 * @ref split_ex
	 */
	static vector<string> split(const string &s, const string &separator, string::size_type limit = string::npos);

	/**
	 * 转换字符串s为整形
	 *
	 */
	static long int integer(const string &s);

	/**
	 * 将字符串按指定的块大小进行分割，并返回字符串数组
	 *
	 * @使用例子
	 * @code
	 * string s("abcdefghijk");
	 * vector<string> v = s.chunk_split(3);
	 * std::copy(v.begin(), v.end(), ostream_iterator<string>(cout, " "));
	 *
	 * abc def ghi jk
	 * @endcode
	 */
	static vector<string> chunk_split(const string &s, string::size_type chunklen);

	/**
	 * 在字符串s中搜索所有为needle的子字符串，将其替换为rs
	 *
	 * @使用例子
	 * @code
	 * string s("This is a test.");
	 * s.replace("is", "ere");
	 * std::cout << s << std::endl;
	 *
	 * There ere a test.
	 * @endcode
	 */
	static void replace(string &s, const string &needle, const string &rs);

	/**
	 * 在字符串s中搜索所有为needle的字符，将其替换为c
	 *
	 */
	static void replace(string &s, string::value_type needle, string::value_type c);

	/**
	 * 将字符串s转换为小写
	 *
	 */
	static void tolower(string &s);

	/**
	 * 将字符串s转换为大写
	 *
	 */
	static void toupper(string &s);

	/**
	 * 统计字符串str在字符串s中出现的次数
	 *
	 */
	static string::size_type count(const string &s, const string &str);

	/**
	 * 字符串是否全是字母和数字a-z、A-Z和0-9
	 *
	 */
	static bool is_alnum(const string &s);

	/**
	 * 字符串是否全是字母a-z和A-Z
	 *
	 */
	static bool is_alpha(const string &s);

	/**
	 * 字符串是否全部为数字0-9
	 *
	 */
	static bool is_numeric(const string &s);

	/**
	 * 字符串是否都为小写
	 *
	 */
	static bool is_lower(const string &s);

	/**
	 * 字符串是否都为大写
	 *
	 */
	static bool is_upper(const string &s);

	/**
	 * 转换字符的大小写
	 *
	 * @post 将所有小写字符转换为大写，将所有大写字符转换为小写
	 * @param[in/out] s	传入一个字符串，并返回被转换后的结果
	 * @return void
	 */
	static void swapcase(string &s);

	/**
	* 
	*/
	static void ltrim(string &s);
	static void rtrim(string &s);
	static void trim(string &s);

	/**
	 * 格式化字符串
	 *
	 * @param[in] fmt		传入带格式化控制符的字符串
	 * @param[in] ...		传入与格式化控制符对应的变参
	 * @return string		返回格式化后的字符串，该返回的字符串带null不超过1024长度
	 */
	static string format(const char *fmt, ...);

	/**
	 * 格式化字符串
	 *
	 * @param[in] maxsize	传入最大格式化后的字符串长度
	 * @param[in] fmt		传入带格式化控制符的字符串
	 * @param[in] ...		传入与格式化控制符对应的变参
	 * @return string		返回格式化后的字符串，该返回的字符串带null不超过maxsize长度
	 */
	static string format(size_t maxsize, const char *fmt, ...);

	/**
	 * 格式化字符串
	 *
	 * @param[out] s		用于接收格式化后的字符串
	 * @param[in] fmt		传入带格式化控制符的字符串
	 * @param[in] ...		传入与格式化控制符对应的变参
	 * @return bool			是否格式化成功
	 */
	static bool format(string& s, const char *fmt, ...);

	/**
	 * 格式化字符串
	 *
	 * @param[out] s		用于接收格式化后的字符串，该返回的字符串带null不超过maxsize长度
	 * @param[in] maxsize	传入最大格式化后的字符串长度
	 * @param[in] fmt		传入带格式化控制符的字符串
	 * @param[in] ...		传入与格式化控制符对应的变参
	 * @return bool			是否格式化成功
	 */
	static bool format(string& s, size_t maxsize, const char *fmt, ...);

protected:
};

_KIMI_NAMESPACE_END

#endif // end of __KIMI_KERNEL_STL_STRING_H__