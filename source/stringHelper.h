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


inline std::vector<std::string> split_string( std::string_view s, char delim=',')
{
	if (s.empty())
		return {};

	std::vector<std::string> elems;
	std::stringstream ss;
	ss.str(std::string(s));
	std::string item;
	while (std::getline(ss, item, delim))
		elems.push_back(item);
	return elems;
}