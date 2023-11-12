#include "client.hpp"

client::client(pollfd *fds): _level(0)
{
	(void)fds;
	_profil.insert(std::make_pair("Nickname", ""));
	_profil.insert(std::make_pair("Password", ""));


}

client::~client(void) {}




void client::fillProfil(void)
{
}

/*****setters*****/
void client::setPassword(const std::string& password) { _password = password; }
void client::setNickname(const std::string& nickname) { _nickname = nickname; }
void client::setUsername(const std::string& username) { _username = username; }
void client::setIdentity(const std::string& identity) { _identity = identity; }
void client::setLevel(int level) { _level = level; }

/********getters*******/

int client::getLevel(void) { return _level; }
std::string client::getPassword() { return _password; }
std::string client::getNickname() { return _nickname; }
std::string client::getUsername() { return _username; }
std::string client::getIdentity() { return _identity; }
