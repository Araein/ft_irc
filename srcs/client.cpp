#include "irc.hpp"

client::~client(void) {}
client::client(void): _pwd(false), _ban(false)
{
	Identity = "";
	Username = "";
	Nickname = "";
	Password = "";
}

void client::firstMessage(std::string message)
{
	Password = extract(message, "PASS ", "\n");
	Nickname = extract(message, "NICK ", "\n");
	Username = extract(message, "USER ", " ");
	Identity = extract(message, ":", "\n");
}

std::string client::ltrim(const std::string& str)
{
	std::string::const_iterator it = std::find_if(str.begin(), str.end(), IsNotSpace);
	return std::string(it, str.end());
}

std::string client::rtrim(const std::string& str)
{
	std::string::const_reverse_iterator it = std::find_if(str.rbegin(), str.rend(), IsNotSpace);
	return std::string(str.begin(), it.base());
}

std::string client::extract(const std::string& message, const std::string& start, const std::string& end)
{
	size_t startPos = message.find(start);
	size_t endPos = message.find(end, startPos + start.length());

	if (startPos != std::string::npos && endPos != std::string::npos)
	{
		return rtrim(ltrim(message.substr(startPos + start.length(), endPos - startPos - start.length())));
	}

	return "";
}



bool client::getPWD(void) const { return _pwd; }
bool client::getBAN(void) const { return _ban; }
std::string client::getPassword(void) const { return Password;}
std::string client::getIdentity(void) const { return Identity;}
std::string client::getNickname(void) const { return Nickname;}
std::string client::getUsername(void) const { return Username;}


void client::setPWD(void) { _pwd = true; }
void client::setBAN(void) { _ban = true; }





