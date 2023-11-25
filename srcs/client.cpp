#include "irc.hpp"

client::~client(void) {}
client::client(int id, int fd): _fd(fd), _id(id), _log(0), _netcat(0)
{
	std::ostringstream oss;
	oss << _id;
	_nickname = "user_" + oss.str();
	_username = "user" + oss.str();
	if (_id == -1)
		_nickname = "chanOp_42stud";//********** ADMIN DES CHANNEL DE DEMARRAGE
}


int client::getID(void) const { return _id; }
int client::getFD(void) const { return _fd; }
int client::getLog(void) const { return _log; }
int client::getNetcat(void) const { return _netcat; }
std::string client::getPassword(void) const { return _password;}
std::string client::getNickname(void) const { return _nickname;}
std::string client::getUsername(void) const { return _username;}


void client::setPassword(std::string pass) { _password = pass; }
void client::setUsername(std::string username) { _username = username; }
void client::setLog(void) { _log++; }
void client::setNetcat(int value) { _netcat = value; }
void client::setNickname(std::string nickname) { _nickname = nickname; }
