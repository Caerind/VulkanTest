#include "String.hpp"

#include <algorithm> // std::transform

#if defined(NU_PLATFORM_WINDOWS)
	#include <Windows.h> // TODO : Include only if not already included
#endif

namespace nu
{

std::map<U32, std::string> StringId::gStrings;

void toLower(std::string& string)
{
	std::transform(string.begin(), string.end(), string.begin(), ::tolower);
}

void toUpper(std::string& string)
{
	std::transform(string.begin(), string.end(), string.begin(), ::toupper);
}

void toLower(const std::string& string, std::string& result)
{
	result = string;
	toLower(result);
}

void toUpper(const std::string& string, std::string& result)
{
	result = string;
	toUpper(result);
}

void toWideString(const std::string& string, std::wstring& wideString)
{
	// TODO : Improve this ?
	// https://stackoverflow.com/questions/27220/how-to-convert-stdstring-to-lpcwstr-in-c-unicode
	// -> ws(s.begin(), s.end()) is elegant but wrong

	#if defined(NU_PLATFORM_WINDOWS)
		int len;
		int slength = (int)string.length() + 1;
		len = MultiByteToWideChar(CP_ACP, 0, string.c_str(), slength, 0, 0);
		wchar_t* buf = new wchar_t[len];
		MultiByteToWideChar(CP_ACP, 0, string.c_str(), slength, buf, len);
		wideString = std::wstring(buf);
		delete[] buf;
	#else
		wideString = std::wstring(string.begin(), string.end());
	#endif
}

bool split(std::string& base, std::string& result, char separator)
{
	U32 found = base.find(separator);
	if (found != std::string::npos)
	{
		result = base.substr(0, found);
		base = base.substr(found + 1);
		return true;
	}
	return false;
}

bool split(std::string& base, std::string& result, const std::string& separator)
{
	U32 found = base.find(separator);
	if (found != std::string::npos)
	{
		result = base.substr(0, found);
		base = base.substr(found + separator.size());
		return true;
	}
	return false;
}

bool contains(const std::string& string, char c)
{
	return string.find(c) != std::string::npos;
}

bool contains(const std::string& string, const std::string& c)
{
	return string.find(c) != std::string::npos;
}

bool limitSize(std::string& string, U32 size)
{
	if (string.size() > size)
	{
		string = string.substr(0, size);
		return true;
	}
	return false;
}

bool limitSize(const std::string& string, std::string& result, U32 size)
{
	if (string.size() > size)
	{
		result = string.substr(0, size);
		return true;
	}
	else
	{
		result = string;
		return false;
	}
}

void trimWhitespace(std::string& string)
{
	string = string.substr(0, string.find_last_not_of(" \t") + 1).substr(string.find_first_not_of(" \t"));
}

bool StringId::getString(std::string& string) const
{
	auto itr = gStrings.find(mStringId);
	if (itr != gStrings.end())
	{
		string = itr->second;
		return true;
	}
	return false;
}

U32 StringId::hash(const std::string& string, bool store)
{
	U32 h = StringId::hashRuntime(string, 0);
	if (store && gStrings.find(h) == gStrings.end())
	{
		gStrings[h] = string;
	}
	return h;
}

} // namespace nu
