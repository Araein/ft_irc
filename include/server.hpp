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
	std::string _partCommand;
	std::map<int, client> mapUser;
	std::vector<channel> channelList;
	std::vector<privChannel> privateList;

//*************FICHIERS DANS LESQUELS SONT IMPLEMENTER LES FONCTIONS*************//
//**********************************//THEME//**********************************//
	void sendWelcomMsgs(int) const;
	void printServerHeader() const;
	std::string printChannel();
	void printHome(int fd);
	std::string printBonus(void);


//**********************************/COMMANDE//**********************************/
	void parseCommand(std::string buff, int fd);
	void cmdKick(int fd, std::string buff);
	void cmdNick(int fd, std::string nickname);
	void cmdJoin(std::string buff, int fd);
	void cmdInvite(int fd, std::string buff);
	void cmdTopic(int fd, std::string buff);
	void cmdMode(int fd, std::string buff);
	void cmdPass(std::string password, int fd);
	void cmdPart(int fd, std::string buff);
	void cmdPing(std::string buff, int fd);
	void cmdPrivmsg(int fd, std::string buff);
	void cmdPrivateMsg(int fd, std::vector<std::string> vec);

//**********************************/SERVER//**********************************/
	void acceptNewUser();
	void userNetcat();
	void createChannel();


//**********************************/INPUT//**********************************/
	void receivMessage();
	void configureNewUser(std::string const &buff, int fd);
	void inputMessage(int);


//**********************************/UTILS//**********************************/
	std::string startServer() const;
	void closeOne(int);
	int findPlace() const;
	bool nameUserCheck(std::string name) const;
	bool nameExist(std::string name);
	std::vector<privChannel>::iterator selectPrivChan(std::string name1, std::string name2);
	std::vector<channel>::iterator selectChannel(std::string name);
	std::map<int, client>::iterator selectUser(std::string name);
	std::map<std::string, std::string> splitCommandJoin(std::string buff);
	std::vector<std::string> splitCommand(std::string buff);
	std::vector<std::string> splitCommandPrivmsg(std::string buff);
	void userUpDate(client *user, std::string oldNick);
	std::string deleteCRLF(std::string str);
	int jumpToNextMode(std::string::iterator it);
	bool findKey(std::vector<std::string> vec, std::string key);


//**********************************/BOT//**********************************/
	void mybot(int fd, const std::string& command, std::string channelstr);
	std::string getCurrentTime();
	std::string flipCoin();
	std::string getTrivia();


//**********************************/TRANSFER//**********************************/
	void trfSend(int fd, std::string txt, std::string channelName);
	void trfGet(int fd, std::string txt, std::string channelName);
	void trfDel(int fd, std::string txt, std::string channelName);
	void trfHelp(int fd, std::string channelName);
	std::string extractFilename(std::string filename);

public:
	server(int, std::string);
	~server();

//**********************************/SERVER//**********************************/
	bool initSocket();
	void mainLoop();

//**********************************/UTILS//**********************************/
	void closeAll();

};