#include "client.hpp"

client::client(pollfd *fds): _level(0)
{
	(void)fds;
	_profil.insert(std::make_pair("Nickname", ""));
	_profil.insert(std::make_pair("Password", ""));


}

client::~client(void) {}


int client::getLevel(void) { return _level; }
std::string client::getNickname(void) { return _nickname; }

void client::setLevel(int level) { _level = level; }

void client::fillProfil(void)
{
}