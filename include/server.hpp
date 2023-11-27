#pragma once

#include "irc.hpp"

class server
{ 
	int _id;
	int _port;
	int _curPlace;
	int _totalPlace;

	client *admin;
	channel *chan;
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
	std::string printChannel();
	void printHome(int fd);


//**********************************/COMMANDE//**********************************/
	void parseCommand(std::string buff, int fd);
	void cmdKick(int fd, std::string buff);
	void cmdNick(int fd, std::string nickname);
	void cmdJoin(std::string buff, int fd);
	void cmdInvite(int fd, std::string buff);
	void cmdTopic(int fd, std::string buff);
	void cmdMode();
	void cmdPass(std::string password, int fd);
	void cmdPart(int fd, std::string buff);
	void cmdPing(std::string buff, int fd);
	void cmdPrivmsg(int fd, std::string buff);


	int getFD(int i) const;
	int getPort() const;
	std::string getPassword() const;
//**********************************/SERVER//**********************************/
	void acceptNewUser();
	void userNetcat();
	void createChannel();


//**********************************/INPUT//**********************************/
	void receivMessage();
	void configureNewUser(std::string const &buff, int fd);
	void inputMessage(int);
	void inputError(int);


//**********************************/UTILS//**********************************/
	std::string startServer() const;
	void closeOne(int);
	int findPlace() const;
	bool nameUserCheck(std::string name) const;
	bool nameExist(std::string name);
	bool checkChannelName(std::string name);
	std::vector<channel>::iterator selectChannel(std::string name);
	int findChanbyName(std::string chan);
	std::map<std::string, std::string> splitCommandJoin(std::string buff);
	std::vector<std::string> splitCommandNick(std::string buff);
	void userUpDate(client *user);

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