#pragma once

#include "irc.hpp"

class server
{
	int _id;
	int _port;
	int _curPlace;
	int _totalPlace;

	pollfd _fds[maxFD + 1];
	std::string _password;
	// std::vector<int> clientList;
	std::map<int, client> mapUser;
	std::vector<channel> channelList;

	//THEME
	void sendWelcomMsgs(int) const;
	void printServerHeader() const;
	void listChannel(int fd);
	void printFullUser(int fd) const;
	void printHome(int fd);
	void printInfo(int fd) const;

	//COMMANDE
	void parseCommand(std::string buff, int fd);
	void cmdKick();
	void cmdNick(int fd, std::string buff);
	void cmdJoin(std::string buff, int fd);
	void cmdInvite();
	void cmdTopic();
	void cmdMode();
	void cmdNotice(int fd);
	void cmdPart(int fd, std::string buff);
	void cmdWHOIS(int fd);
	void cmdPing(std::string buff, int fd);
	void cmdPrivmsg(int fd, std::string buff);

	//SERVER
	void acceptNewUser();
	void createChannel();
	void test();

	//INPUT
	void receivMessage();
	void inputMessage(int);
	void inputError(int);
	void checkPassword(std::string pass, int fd);
	void checkNickname(std::string nick, int fd);


	//UTILS
	std::string startServer() const;
	void closeOne(int);
	void clearFDStemp(void);
	int findPlace() const;
	bool nameChar(std::string name, int index) const;
	bool nameExist(std::string name);
	bool checkChannelName(std::string name);
	std::vector<channel>::iterator selectChannel(std::string name);


public:
	server(int, std::string);
	~server();

	//SERVER
	bool initSocket();
	void mainLoop();

	//UTILS
	void closeAll();
};