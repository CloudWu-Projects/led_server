#pragma once


#include <string>
#include <locale>
#include <codecvt>

#ifdef UNICODE
// convert string to wstring
inline std::wstring to_wide_string(const std::string& input)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.from_bytes(input);
}
// convert wstring to string 
inline std::string to_byte_string(const std::wstring& input)
{
	//std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.to_bytes(input);
}

#else
#define to_wide_string(x) x
#define to_byte_string(x) x


#endif
namespace UTF8_ASCI {
	inline std::wstring Utf82Unicode(const std::string& utf8string)
	{
		int widesize = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, NULL, 0);
		if (widesize == ERROR_NO_UNICODE_TRANSLATION)
		{
			throw std::exception("Invalid UTF-8 sequence.");
		}
		if (widesize == 0)
		{
			throw std::exception("Error in conversion.");
		}
		std::vector<wchar_t> resultstring(widesize);
		int convresult = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, &resultstring[0], widesize);
		if (convresult != widesize)
		{
			throw std::exception("La falla!");
		}
		return std::wstring(&resultstring[0]);
	}
	inline std::string WideByte2Acsi(std::wstring& wstrcode)
	{
		int asciisize = ::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, NULL, 0, NULL, NULL);
		if (asciisize == ERROR_NO_UNICODE_TRANSLATION)
		{
			throw std::exception("Invalid UTF-8 sequence.");
		}
		if (asciisize == 0)
		{
			throw std::exception("Error in conversion.");
		}
		std::vector<char> resultstring(asciisize);
		int convresult = ::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, &resultstring[0], asciisize, NULL, NULL);
		if (convresult != asciisize)
		{
			throw std::exception("La falla!");
		}
		return std::string(&resultstring[0]);
	}

	inline std::string UTF_82ASCII(std::string& strUtf8Code)
	{
		std::string strRet("");
		//先把 utf8 转为 unicode 
		std::wstring wstr = Utf82Unicode(strUtf8Code);
		//最后把 unicode 转为 ascii 
		strRet = WideByte2Acsi(wstr);
		return strRet;
	}
}
inline std::string getAString(std::wstring_view fmt)
{
	char   buffer[5 * 1024] = { 0 };
	size_t size = 0;
	errno_t ret = wcstombs_s(&size, buffer, fmt.data(), fmt.size());
	return buffer;
}

inline uint32_t unicode_to_byte_array(std::wstring_view str, char* buf, int32_t bufSize)
{
	if (buf == nullptr || str.empty() || bufSize <= 0)
		return 0;
	char const* p = reinterpret_cast<char const*>(&str[0]);
	std::size_t size = str.size() * sizeof(str.front());
	memcpy_s(buf, bufSize, p, size);
	return size;
}

inline std::wstring utf8_to_unicode(std::string_view strUTF8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.data(), -1, nullptr, 0);
	if (len == 0)
		return {};
	--len;
	std::wstring result;
	result.resize(len, 0);
	MultiByteToWideChar(CP_UTF8, 0, strUTF8.data(), -1, &result[0], len);
	return result;
}

inline std::string urf8_toAString(std::string_view strUTF8)
{
	std::wstring result = utf8_to_unicode(strUTF8);
	return getAString(result);
}
inline std::vector<std::string> split_string(const std::string& s, char delim = ',')
{
	if (s.empty())
		return {};

	std::vector<std::string> elems;
	std::stringstream ss;
	ss.str(s);
	std::string item;
	while (std::getline(ss, item, delim))
		elems.push_back(item);
	return elems;
}