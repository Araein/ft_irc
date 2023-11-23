#include "client.hpp"

client::~client(void) {}
client::client(int id, int fd): _fd(fd), _id(id), _status(0)
{
	std::ostringstream oss;
	oss << _id;
	_nickname = "user" + oss.str();
	_username = _nickname;
	if (_id == -1)
		_nickname = "SuperAdmin";//********** ADMIN DES CHANNEL DE DEMARRAGE
std::cout << "DEBUG new client FD: "<< _fd << "nick: " << _nickname << std::endl;//********** A SUPPRIMER
}

void client::fillProfil(std::string message)
{
	_password = extract(message, "PASS ", "\n");
	_nickname = extract(message, "NICK ", "\n");
	_username = extract(message, "USER ", " ");
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

int client::getID(void) const { return _id; }
int client::getFD(void) const { return _fd; }
int client::getStatus(void) const { return _status; }
std::string client::getPassword(void) const { return _password;}
std::string client::getNickname(void) const { return _nickname;}
std::string client::getUsername(void) const { return _username;}


void client::setPassword(std::string pass) { _password = pass; }
void client::setUsername(std::string username) { _username = username; }
void client::setStatus(void) { _status++; }
void client::setNickname(std::string nickname) { _nickname = nickname; }
