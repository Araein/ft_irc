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

	/*********communication intra channel**********/
	
    std::map<std::string, std::vector<client*> > channels; // string = nom du channel  // vector = membres du channel
	//il faudra ajouter la suppression des channels lorsque personne est dedans ?

	
	/**********************************************/

	void acceptNewUser();
	void userMessage(int fd);
	void errMessage(int fd);
	void sendWelcomeMsgs(int fd);
	int findPlace();
	int findChanbyName(std::string channel) const;
	void printFullUser(int fd);
	void printNewUser(int fd);
	void closeOne(int fd);
	void parseMessage(std::string buff, int fd);

	void cmdKick();
	void cmdNick(int fd, std::string buff);
	void cmdJoin(std::string buff, int fd);
	void cmdInvite();
	void cmdTopic(int fd, std::string buff);
	void cmdMode();
	void cmdPrivmsg(int fd, std::string buff);

	public:
	server(int fd, int port, std::string password);
	~server();

	/*********communication intra channel**********/
	
    void joinChannel(int fd, const std::string& channel);
    void sendMessage(int fd, const std::string& channel, std::string& message);

	
	/**********************************************/

	int getFD(int i) const;
	int getPort() const;
	std::string getPassword() const;

	void setFD(int i, int val);

	void closeAll();
	bool initSocket();
	void mainLoop();
	void createChannel();



};
