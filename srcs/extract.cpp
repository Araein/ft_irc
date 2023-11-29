#include "irc.hpp"

bool IsNotSpace(int ch)
{
	return !std::isspace(ch);
}

std::string ltrim(const std::string& str)
{
	std::string::const_iterator it = std::find_if(str.begin(), str.end(), IsNotSpace);
	return std::string(it, str.end());
}

std::string rtrim(const std::string& str)
{
	std::string::const_reverse_iterator it = std::find_if(str.rbegin(), str.rend(), IsNotSpace);
	return std::string(str.begin(), it.base());
}

std::string extract(const std::string& message, const std::string& start, const std::string& end)
{
	size_t startPos = message.find(start);
	size_t endPos = message.find(end, startPos + start.length());

	if (startPos != std::string::npos && endPos != std::string::npos)
	{
		return rtrim(ltrim(message.substr(startPos + start.length(), endPos - startPos - start.length())));
	}

	return "";
}
