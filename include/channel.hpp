#pragma once

#include "irc.hpp"

class channel
{

	int _nbUser;
	std::string _name;
	std::string _topic;
	int _topicRestricted;
	std::vector<client> connected;
	std::vector<client> admin;
	std::vector<client> banned;

	public:
	channel(std::string name);
	~channel();

	void sendToChannel(client const &, std::string);

	std::string getChannelName() const;
	bool getConnected(client const &) const;
	bool getAdmin(client const &) const;
	bool getIsBanned(client const &) const;
	bool isTopicRestricted(void) const;
	std::string getTopic(void) const;
	int getNbUser() const;

	void setConnect(client const &);
	void setDisconnect(client const &);
	void setAdminTrue(client const &);
	void setAdminFalse(client const &);
	void setBannedTrue(client const &);
	void setBannedFalse(client const &);
	void setTopicRestriction();
	void setTopic(std::string topic);
	void setNbUserUp();
	void setNbUserDown();

};
