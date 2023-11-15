#pragma once

#include "irc.hpp"

class client
{
	bool _pwd;
	bool _ban;
	int			_fd;
	std::string _perm;
	std::string Identity;
	std::string Username;
	std::string Nickname;
	std::string Password;

	std::string rtrim(const std::string& str);
	std::string ltrim(const std::string& str);

	public:
	client();
	~client();

	void firstMessage(std::string message);
	std::string extract(const std::string& message, const std::string& start, const std::string& end);

	bool getPWD() const;
	bool getBAN() const;
	std::string getPassword() const;
	std::string getIdentity() const;
	std::string getUsername() const;
	std::string getNickname() const;
	int			getFD() const;
	void setFD(int myfd);
	void setPWD();
	void setBAN();
	void setNickname(std::string nick);


};
	



