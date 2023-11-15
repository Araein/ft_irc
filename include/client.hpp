#pragma once

#include "irc.hpp"

class client
{
	int _id;
	int _fd;
	bool _pwd;
	bool _ban;
	std::string Identity;
	std::string Username;
	std::string Nickname;
	std::string Password;

	public:
	client(int id, int fd);
	~client();

	void firstMessage(std::string message);

	bool getPWD() const;
	int getID() const;
	int getFD() const;
	std::string getPassword() const;
	std::string getIdentity() const;
	std::string getUsername() const;
	std::string getNickname() const;

	void setPWD();
	void setBAN(void);
	void setNickname(std::string nick);

};
	



