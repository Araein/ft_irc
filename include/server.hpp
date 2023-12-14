#pragma once

#include "irc.hpp"

struct transferFile{
	int fdExp;
	int fdDest;
	std::string id;
	std::string filename;
	std::string pathFile;
	std::string textFile;
};

struct privChannel{
	std::string name1;
	std::string name2;
};

class server
{ 
	int _id;
	int _port;
	int _curPlace;
	int _totalPlace;

	client *admin;
	client *MrRobot;
	channel *chan;

	pollfd _fds[maxFD + 1];
	std::string _password;
	std::map<int, std::string> truncCmd;
	std::map<int, client> mapUser;
	std::vector<channel> channelList;
	std::vector<privChannel> privateList;

//**********************************//THEME//**********************************//
//*************FICHIERS DANS LESQUELS SONT IMPLEMENTER LES FONCTIONS*************//
//**********************************//THEME//**********************************//
	void sendWelcomMsgs(int) const;
	void printServerHeader() const;
	std::string printChannel() const;
	void printHome(int fd) const;
	std::string printBonus(void) const;


//**********************************/COMMAND//**********************************/
	void parseCommand(std::string buff, int fd);
	void cmdKick(int fd, std::string buff);
	void cmdNick(int fd, std::string buff);
	void cmdJoin(std::string buff, int fd);
	void cmdInvite(int fd, std::string buff);
	void cmdTopic(int fd, std::string buff);
	void cmdMode(int fd, std::string buff);
	void cmdPass(std::string password, int fd);
	void cmdPart(int fd, std::string buff);
	void cmdPing(std::string buff, int fd);
	void cmdPrivmsg(int fd, std::string buff);
	void cmdPrivateMsg(int fd, std::vector<std::string> vec);
	void cmdNotice(int fd, std::string buff);


	int getFD(int i) const;
	int getPort() const;
	std::string getPassword() const;
//**********************************/SERVER//**********************************/
	void acceptNewUser();
	void userNetcat();
	void createChannel(void);

//**********************************/INPUT//**********************************/
	void receivMessage();
	void configureNewUser(std::string const &buff, int fd);
	void inputMessage(int);


//**********************************/UTILS//**********************************/
	std::string startServer() const;
	void closeOne(int);
	int findPlace() const;
	int findChanbyName(std::string chan) const;
	bool nameUserCheck(std::string const &name) const;
	bool nameExist(std::string const &name) const;
	std::vector<privChannel>::const_iterator selectPrivChan(std::string const &name1, std::string const &name2) const;
	std::vector<channel>::iterator selectChannel(std::string const &name);
	std::map<int, client>::iterator selectUser(std::string const &name);
	std::map<std::string, std::string> splitCommandJoin(std::string const &buff);
	std::vector<std::string> splitCommand(std::string const &buff);
	std::vector<std::string> splitCommandPrivmsg(std::string const &buff);
	void userUpDate(client &user, std::string const &oldNick);
	std::string deleteCRLF(std::string str);
	int jumpToNextMode(std::string::iterator it);
	bool findKey(std::vector<std::string> vec, std::string key);
	std::map<int, std::string>::iterator selectTrunc(int fd);


//**********************************/BOT//**********************************/
	void mybot(int fd, const std::string& command, std::string channelstr);
	std::string getCurrentTime();
	std::string flipCoin();
	std::string getTrivia();


//**********************************/TRANSFER//**********************************/
	void trfSend(int fd, std::string const &txt, std::string const &channelName);
	void trfGet(int fd, std::string const &txt, std::string const &channelName);
	void trfDel(int fd, std::string const &txt, std::string const &channelName);
	void trfHelp(int fd) const;
	std::string extractFilename(std::string const &filename);

public:
	server(int, std::string const &password);
	~server();

//**********************************/SERVER//**********************************/
	bool initSocket();
	void mainLoop();

//**********************************/UTILS//**********************************/
	void closeAll();

};