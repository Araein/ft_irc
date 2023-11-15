#pragma once

#include "irc.hpp"

class server
{
	int _id;
	int _port;
	pollfd _fds[maxFD + 1];
	std::string _password;
	int _curPlace;
	int _totalPlace;
	std::map<int, client> mapUser;
	std::vector<channel> vecChannel;


	void acceptNewUser();
	void userMessage(int fd);
	void errMessage(int fd);
	void sendWelcomeMsgs(int fd);
	int findPlace();
	void printFullUser(int fd);
	void printNewUser(int fd);
	void closeOne(int fd);
	void parseMessage(std::string buff, int fd);

	void cmdKick();
	void cmdJoin(std::string buff, int fd);
	void cmdInvite();
	void cmdTopic();
	void cmdMode();
	void cmdPrivmsg(int fd, std::string buff);

	public:
	server(int fd, int port, std::string password);
	~server();

	int getFD(int i) const;
	int getPort() const;
	std::string getPassword() const;

	void setFD(int i, int val);

	void closeAll();
	bool initSocket();
	void mainLoop();
	void createChannel();



};
