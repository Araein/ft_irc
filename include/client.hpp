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

	// std::string extract(const std::string& message, const std::string& start, const std::string& end);
	// std::string rtrim(const std::string& str);
	// std::string ltrim(const std::string& str);

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


};
	



