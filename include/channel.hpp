#pragma once

#include "irc.hpp"

typedef struct chanData{
	bool i_Mode;
	bool t_Mode;
	bool k_Mode;
	bool o_Mode;
	bool l_Mode;
	bool needPass;
	int maxConnectedUser;
	int nbConnectedUser;
	std::string name;
	std::string password;
	std::string topicMessage;
	std::vector<client> chanOp;
	std::vector<client> invited;
	std::vector<client> connected;
	std::vector<client> excluded;
}channelData;

class channel
{

	channelData chan;

	void serverChannel();
	std::vector<client>::iterator findUser(client const &user, std::string vec);

	public:
	channel(std::string name);
	~channel();


	int getNbConnectedUser() const;
	int getNeedPass() const;
	int getMaxConnectedUser() const;
	bool getIsChanOp(int id);
	bool getIsConnected(int id);
	bool getIsInvited(int id);
	bool getIsExcluded(int id);
	bool getMode(char c) const;
	std::string getPassword() const;
	std::string getChannelName() const;
	std::string getTopicMessage() const;
	std::string getAllChanOp();
	std::string getAllConnected();
	std::string getAllInvited();

	void setNeedPass(bool value);
	void setMaxConnectedUser(int value);
	void setPassword(std::string password);
	void setTopicMessage(std::string message);
	void setUserConnect(client const &user);
	void setUserDisconnect(client const &user);
	void setUserInvited(client const &user);
	void setUserExcluded(client const &user, bool value);
	void setUserChanOp(client const &user);


	void sendToChannel(client const &user, std::string message);
	void welcomeMessage(client const &user);
	bool userCanWrite(client const &user);
	void userCanJoin(client const &user, std::string password);


};