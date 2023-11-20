#include "irc.hpp"

channel::~channel(void) {}
channel::channel(std::string name)
{
	chan.name = name;
	chan.i_Mode = false;
	chan.k_Mode = false;
	chan.l_Mode = false;
	chan.t_Mode = false;
	chan.o_Mode = false;
	chan.maxConnectedUser = 1000;
	chan.nbConnectedUser = 0;
	chan.needPass = false;
	chan.password = "";
	chan.topicMessage = "";
	serverChannel();

}

int channel::getNbConnectedUser(void) const { return chan.nbConnectedUser; }
int channel::getNeedPass(void) const { return chan.needPass; }
int channel::getMaxConnectedUser(void) const { return chan.maxConnectedUser; }
std::string channel::getChannelName() const { return chan.name; }
std::string channel::getTopicMessage() const { return chan.topicMessage; }
std::string channel::getPassword() const { return chan.password; }

bool channel::getIsChanOp(int id)
{
	for(it = chan.chanOp.begin(); it != chan.chanOp.end(); it++)
	{
		if (it->getID() == id)
			return true;
	}
	return false;
}

bool channel::getIsConnected(int id)
{
	for(it = chan.connected.begin(); it != chan.connected.end(); it++)
	{
		if (it->getID() == id)
			return true;
	}
	return false;
}

bool channel::getIsInvited(int id)
{
	for(it = chan.invited.begin(); it != chan.invited.end(); it++)
	{
		if (it->getID() == id)
			return true;
	}
	return false;
}

std::string channel::getAllChanOp(void)
{
	msg = "";

	for(it = chan.chanOp.begin(); it != chan.chanOp.end(); it++)
		msg += it->getNickname() + " ";
	msg += " \n";
	return msg;
}

std::string channel::getAllConnected(void)
{
	msg = "";
	for(it = chan.connected.begin(); it != chan.chanOp.end(); it++)
		msg += it->getNickname() + " ";
	msg += " \n";
	return msg;
}

std::string channel::getAllInvited(void)
{
	msg = "";
	for(it = chan.invited.begin(); it != chan.chanOp.end(); it++)
		msg += it->getNickname() + " ";
	msg += " \n";
	return msg;
}

bool channel::getMode(char c) const
{
	if (c == 'i')
		return chan.i_Mode;
	if (c == 't')
		return chan.t_Mode;
	if (c == 'k')
		return chan.k_Mode;
	if (c == 'o')
		return chan.o_Mode;
	if (c == 'l')
		return chan.l_Mode;
	return false;
}


void channel::setNeedPass(bool value) { chan.needPass = value; }
void channel::setMaxConnectedUser(int value) { chan.maxConnectedUser = value; }
void channel::setPassword(std::string password) { chan.password = password; }
void channel::setTopicMessage(std::string message) { chan.topicMessage = message; }

void channel::setUserConnect(client const &user)
{
	if (getIsConnected(user.getID()) == true)
	{
		msg = user.getNickname() + "is already connected to " + chan.name + " \n";
		return;
	}
	if (chan.nbConnectedUser >= chan.maxConnectedUser)
	{
		msg = chan.name + " is full. Try later\n";
		return;
	}
	chan.connected.push_back(user);
	chan.nbConnectedUser++;
	if (chan.nbConnectedUser == 1)
		setUserChanOp(user);
	welcomeMessage(user);
	sendToChannel(user, "is connected");
}

void channel::setUserDisconnect(client const &user)
{
	if (getIsConnected(user.getID()) == true)
	{
		chan.connected.erase(findUser(user, "connected"));
		chan.nbConnectedUser--;
		sendToChannel(user, "disconnected");
		msg = "You are disconnected to " + chan.name + " \n";
		send(user.getFD(), msg.c_str(), msg.size(), 0);
	}
}

void channel::setUserInvited(client const &user)
{
	if (getIsInvited(user.getID()) == false)
		chan.invited.push_back(user);
}

void channel::setUserChanOp(client const &user)
{
	if (getIsChanOp(user.getID()) == false)
		chan.invited.push_back(user);
}



void channel::welcomeMessage(client const &user)
{
	std::ostringstream oss;
	oss << chan.nbConnectedUser;
	std::string txt;
	msg = ":" + user.getNickname() + "!" + user.getUsername() + "@localhost JOIN " + chan.name +  "\n";
	send(user.getFD(), msg.c_str(), msg.size(), 0);
	txt = "Welcome to the channel " + chan.name + "\n";
	msg = ":" + user.getNickname() + "!" + user.getUsername() + "@localhost PRIVMSG " + chan.name + " :" + txt + " \r\n";
	send(user.getFD(), msg.c_str(), msg.size(), 0);
	txt = "Administrator : " + getAllChanOp();
	msg = ":" + user.getNickname() + "!" + user.getUsername() + "@localhost PRIVMSG " + chan.name + " :" + txt + " \r\n";
	send(user.getFD(), msg.c_str(), msg.size(), 0);
	txt = "User(s) connected : " + oss.str() + " \n";
	msg = ":" + user.getNickname() + "!" + user.getUsername() + "@localhost PRIVMSG " + chan.name + " :" + txt + " \r\n";
	send(user.getFD(), msg.c_str(), msg.size(), 0);
	msg = ":ircserv 332 " + user.getNickname() + " :" + chan.topicMessage + " \r\n";
	send(user.getFD(), msg.c_str(), msg.size(), 0);

}

void channel::sendToChannel(client const &user, std::string message)
{
	std::string txt;
	for(it = chan.connected.begin(); it != chan.connected.end(); it++)
	{
		if (it->getID() != user.getID())
		{
			// msg = "PRIVMSG " + chan.name + " :<" + user.getNickname() + "> " + message + " \r\n";
			msg = ":" + user.getNickname() + "!" + user.getUsername() + "@localhost PRIVMSG " + chan.name + " :" + message + " \r\n";
			send(it->getFD(), msg.c_str(), msg.size(), 0);
		}
	}
}

void channel::serverChannel()
{
	client admin(-1, -1);
	if (chan.name == "#Piscine")
	{
		chan.needPass = true;
		chan.topicMessage = "Exchange with the students of your promo";
		chan.maxConnectedUser = 1000;
		chan.password = "stud42";
		chan.chanOp.push_back(admin);
		chan.invited.push_back(admin);
	}
	else if (chan.name == "#Minilibix")
	{
		chan.needPass = false;
		chan.topicMessage = "Exchange around 'Solong' and 'Cub3D' projects";
		chan.maxConnectedUser = 1000;
		chan.chanOp.push_back(admin);
		chan.invited.push_back(admin);
	}
	else if (chan.name == "#Shell")
	{
		chan.needPass = false;
		chan.topicMessage = "Exchange around the shell commands";
		chan.maxConnectedUser = 1000;
		chan.chanOp.push_back(admin);
		chan.invited.push_back(admin);
	}
	else if (chan.name == "#Staff")
	{
		chan.needPass = true;
		chan.topicMessage = "Be evaluated by the staff";
		chan.maxConnectedUser = 2;
		chan.password = "stud42";
		chan.chanOp.push_back(admin);
		chan.invited.push_back(admin);
		chan.i_Mode = true;
	}

}

std::vector<client>::iterator channel::findUser(client const &user, std::string vec)
{
	std::vector<client>::iterator it1;
	std::vector<client>::iterator it2;
	if (vec == "chanOp")
	{
		it1 = chan.chanOp.begin();
		it2 = chan.chanOp.end();
	}
	else if (vec == "connected")
	{
		it1 = chan.connected.begin();
		it2 = chan.connected.end();
	}
	else if (vec == "invited")
	{
		it1 = chan.invited.begin();
		it2 = chan.invited.end();
	}
	for(it = it1; it != it2; it++)
	{
		if (it->getID() == user.getID())
			return it;
	}
	return it;
}

bool channel::userCanWrite(client const &user)
{
	if (getIsConnected(user.getID()) == false)
		return false;
	if (chan.i_Mode == true && getIsInvited(user.getID()) == false)
		return false;
	return true;
}

bool channel::userCanJoin(client const &user, std::string password)
{
	if (chan.i_Mode == true && getIsInvited(user.getID()) == false)
	{
		msg = "You are not on the guest list of " + chan.name + " \n";
		send(user.getFD(), msg.c_str(), msg.size(), 0);
		return false;
	}
	if (chan.needPass == true && chan.password != password)
	{
		msg = chan.name + " password invalid\n";
		send(user.getFD(), msg.c_str(), msg.size(), 0);
		return false;
	}
	return true;
}




