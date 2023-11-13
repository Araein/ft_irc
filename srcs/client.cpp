#include "client.hpp"

client::client(pollfd *fds): _level(0)
{
	(void)fds;
<<<<<<< HEAD
	_profil.insert(std::make_pair("Nickname", ""));
	_profil.insert(std::make_pair("Password", ""));
=======
	// _profil.insert(std::make_pair("Nickname", ""));
	// _profil.insert(std::make_pair("Password", ""));
>>>>>>> 1bf8c0a73195a7ebcdbc30524917c3eb388e1c59


}

client::~client(void) {}


<<<<<<< HEAD
int client::getLevel(void) { return _level; }
std::string client::getNickname(void) { return _nickname; }

void client::setLevel(int level) { _level = level; }

void client::fillProfil(void)
{
}
=======


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
>>>>>>> 1bf8c0a73195a7ebcdbc30524917c3eb388e1c59
