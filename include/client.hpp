#pragma once

#include "irc.hpp"

class client
{
	bool _pwd;
	bool _ban;
	std::string Identity;
	std::string Username;
	std::string Nickname;
	std::string Password;

	std::string extract(const std::string& message, const std::string& start, const std::string& end);
	std::string rtrim(const std::string& str);
	std::string ltrim(const std::string& str);





	public:
	client();
	~client();


	void firstMessage(std::string message);

	bool getPWD() const;
	bool getBAN() const;
	std::string getPassword() const;
	std::string getIdentity() const;
	std::string getUsername() const;
	std::string getNickname() const;


	void setPWD();
	void setBAN();


};
	



