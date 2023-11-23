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
	std::map<int, client> mapUser;
	std::vector<channel> channelList;


	void acceptNewUser();
	void userMessage(int fd);
	void errMessage(int fd);
	void sendWelcomeMsgs(int fd);
	int findChanbyName(std::string channel) const;
	void printNewUser(int fd);
	void closeOne(int fd);
	void parseMessage(std::string buff, int fd);
//*************FICHIERS DANS LESQUELS SONT IMPLEMENTER LES FONCTIONS*************//
//**********************************//THEME//**********************************//
	void sendWelcomMsgs(int) const;
	void printServerHeader() const;
	void listChannel(int fd);
	void printFullUser(int fd) const;
	void printHome(int fd);


//**********************************/COMMANDE//**********************************/
	void parseCommand(std::string buff, int fd);
	void cmdKick(int fd, std::string buff);
	void cmdNick(int fd, std::string buff);
	void cmdJoin(std::string buff, int fd);
	void cmdInvite();
	void cmdTopic(int fd, std::string buff);
	void cmdMode();
	void cmdPart(int fd, std::string buff);
	void cmdPing(std::string buff, int fd);
	void cmdPrivmsg(int fd, std::string buff);


	int getFD(int i) const;
	int getPort() const;
	std::string getPassword() const;
//**********************************/SERVER//**********************************/


//**********************************/INPUT//**********************************/
	void receivMessage();
	void inputMessage(int);
	void inputError(int);
	void checkPassword(std::string pass, int fd);


//**********************************/UTILS//**********************************/
	std::string startServer() const;
	void clearFDStemp(void);
	int findPlace(void) const;
	bool nameChar(std::string name, int index) const;
	bool nameExist(std::string name);
	bool checkChannelName(std::string name);
	bool checkNickname(std::string nick, int fd);
	std::vector<channel>::iterator selectChannel(std::string name);


public:
	server(int, std::string);
	~server();

//**********************************/SERVER//**********************************/
	bool initSocket();
	void mainLoop();
	void createChannel();


//**********************************/UTILS//**********************************/
	void closeAll();

};