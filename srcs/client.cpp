#include "irc.hpp"

client::~client(void) {}
client::client(int id, int fd): _id(id), _fd(fd), _pwd(false), _ban(false) { }

void client::firstMessage(std::string message)
{
	Password = extract(message, "PASS ", "\n");
	Nickname = extract(message, "NICK ", "\n");
	Username = extract(message, "USER ", " ");
	Identity = extract(message, ":", "\n");
}

bool client::getPWD(void) const { return _pwd; }

int client::getID(void) const { return _id; }
int client::getFD(void) const { return _fd; }
std::string client::getPassword(void) const { return Password;}
std::string client::getIdentity(void) const { return Identity;}
std::string client::getNickname(void) const { return Nickname;}
std::string client::getUsername(void) const { return Username;}

void client::setPWD(void) { _pwd = true; }

void client::setBAN(void) { _ban = true; }
void client::setNickname(std::string nick) { Nickname = nick; }




