#include "irc.hpp"

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
