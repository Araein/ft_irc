#include "client.hpp"

client::client(infoConnect *user): _level(0)
{
	(void)user;
	profil.insert(std::make_pair("Nickname", ""));
	profil.insert(std::make_pair("Password", ""));


}

client::~client(void) {}


int client::getLevel(void) { return _level; }

void client::setLevel(int level) { _level = level; }

void client::fillProfil(void)
{
}