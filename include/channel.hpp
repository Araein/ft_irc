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
}channelData;

class channel
{

	channelData chan;

	public:
	channel();
	channel(std::string name);
	~channel();

	int index;

//**********************************/GETTER//**********************************/
	int getNbConnectedUser() const;
	int getNeedPass() const;
	int getMaxConnectedUser() const;
	bool getIsChanOp(int id) const;
	bool getIsConnected(int id) const;
	bool getIsInvited(int id) const;
	bool getMode(char c) const;
	std::string getAllMode(void) const;
	std::string getPassword() const;
	std::string getChannelName() const;
	std::vector<client> getConnectedVector() const;
	std::string getTopic() const;
	std::string getAllChanOp() const;
	std::string getAllConnected() const;
	std::string getAllInvited() const;
	bool userCanWrite(client *user, std::string channelName);
	bool getConnectedFromString(std::string const &user) const;
	client* getClient(const std::string& user);



//**********************************/SETTER//**********************************/
	void setNeedPass(bool value);
	void setMaxConnectedUser(int value);
	void setPassword(std::string const &password);
	void setTopic(std::string const &message);
	void setUserConnect(client &user);
	void setUserDisconnect(client &user);
	void setUserInvited(client const &user);
	void setUserChanOp(client const &user);
	void setMode(char c, bool value);
	void setChannelName(std::string const &name);
	void setAllInvited(void);
	void setUserShutdown(client const &user);
	void undoUserChanOp(client const &user);
	void unsetUserInvited(client const &user);


//**********************************/FUNCTION//**********************************/
	void sendToChannel(client const &user, std::string const &message);
	void sendToChannelNotice(client const &user, std::string const &message);
	void sendInfoToChannel(client const &user, std::string const &message);
	void welcomeMessage(client const &user) const;
	bool userCanWrite(client const &user);
	bool userCanJoin(client const &user, std::string const &password) const;
	void switchUser(client const &user);
	std::string userList() const;
	void sendToChannelnoPRIVMSG(client const &user, std::string const &message);
	void sendToOne(client const &user, std::string const &message) const;

};