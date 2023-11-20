#pragma once

#include "irc.hpp"

class client
{

	int  _fd;
	int  _id;
	int _status;
	std::string _nickname;
	std::string _username;
	std::string _password;

	public:
	client(int id, int fd);
	~client();



	int getID() const;
	int getFD() const;
	int getStatus() const;
	std::string getPassword() const;
	std::string getUsername() const;
	std::string getNickname() const;

	void setPassword(std::string pass);
	void setUsername(std::string username);
	void setNickname(std::string nickname);
	void setStatus();

};