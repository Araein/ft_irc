#pragma once

#include "irc.hpp"

class client
{

	std::string rtrim(const std::string& str);
	std::string ltrim(const std::string& str);
	int  _fd;
	int  _id;
	int _log;
	clock_t _netcat;
	std::string _nickname;
	std::string _username;
	std::string _password;
	std::vector<channel> channelConnected;

	public:
	client(int id, int fd);
	~client();

	void firstMessage(std::string message);
	std::string extract(const std::string& message, const std::string& start, const std::string& end);
	void fillProfil(std::string message);

	bool getPWD() const;
	int getID() const;
	int getFD() const;
	int getLog() const;
	int getNetcat() const;
	size_t getHowManyChannel() const;
	std::string getPassword() const;
	std::string getUsername() const;
	std::string getNickname() const;
	std::vector<channel>::iterator getConnectBegin();
	std::vector<channel>::iterator getConnectEnd();

	void setPassword(std::string pass);
	void setUsername(std::string username);
	void setNickname(std::string nickname);
	void setLog();
	void setNetcat(int value);
	void setNetcat();
	void addChannel(channel *chan);
	void deleteChannel(channel const &chan);
	void chanUpDate();


};