#include "irc.hpp"

channel::~channel(void) {}
channel::channel(void)
{
	chan.i_Mode = false;
	chan.k_Mode = false;
	chan.l_Mode = false;
	chan.t_Mode = false;
	chan.o_Mode = false;
	chan.maxConnectedUser = 1000;
	chan.nbConnectedUser = 0;
	chan.needPass = false;
	index = 1;
}

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
	index = 0;
}

//**********************************//GETTER//**********************************//

int channel::getNbConnectedUser(void) const { return chan.nbConnectedUser; }
int channel::getNeedPass(void) const { return chan.needPass; }
int channel::getMaxConnectedUser(void) const { return chan.maxConnectedUser; }
std::string channel::getChannelName() const { return chan.name; }
std::string channel::getTopic() const { return chan.topicMessage; }
std::string channel::getPassword() const { return chan.password; }
std::vector<client> channel::getConnectedVector()const { return chan.connected; }

bool channel::getIsChanOp(int id) const
{
	for (std::vector<client>::const_iterator it = chan.chanOp.begin(); it != chan.chanOp.end(); it++)
	{
		if (it->getID() == id)
			return true;
	}
	return false;
}

bool channel::getIsConnected(int id) const
{
	for (std::vector<client>::const_iterator it = chan.connected.begin(); it != chan.connected.end(); it++)
	{
		if (it->getID() == id)
			return true;
	}
	return false;
}

bool channel::getIsInvited(int id) const
{
	for (std::vector<client>::const_iterator it = chan.invited.begin(); it != chan.invited.end(); it++)
	{
		if (it->getID() == id)
			return true;
	}
	return false;
}

std::string channel::getAllConnected(void) const
{
	std::string msg = "";
	for (std::vector<client>::const_iterator it = chan.connected.begin(); it != chan.chanOp.end(); it++)
		msg += it->getNickname() + " ";
	msg += " \n";
	return msg;
}


std::string channel::getAllInvited(void) const
{
	std::string msg = "";
	for (std::vector<client>::const_iterator it = chan.invited.begin(); it != chan.chanOp.end(); it++)
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

std::string channel::getAllMode(void) const // removed the o mode return as it never appears when listing modes even after setting a new op, adding password if k mode is set
{
	std::string txt= "";
	if (chan.i_Mode == true)
		txt += 'i';
	if (chan.t_Mode == true)
		txt += 't';
	if (chan.k_Mode == true)
		txt += 'k';
	if (chan.l_Mode == true)
		txt += 'l';
	if (chan.k_Mode == true)
		txt += getPassword();
	return txt;
}

std::string channel::getAllChanOp(void) const
{
	std::string msg = "";

	for (std::vector<client>::const_iterator it = chan.chanOp.begin(); it != chan.chanOp.end(); it++)
		msg += it->getNickname() + " ";
	msg += " \n";
	return msg;
}

bool channel::getConnectedFromString(std::string const &user) const
{
	for (std::vector<client>::const_iterator it = chan.connected.begin(); it != chan.connected.end(); it++)
	{
		if (it->getNickname() == user)
			return true;
	}
	return false;
}

client* channel::getClient(const std::string& user)
{
	for (size_t i = 0; i < chan.connected.size(); i++)
	{
		if (chan.connected[i].getNickname() == user)
			return &chan.connected[i];
	}
	return NULL;
}


//**********************************//SETTER//**********************************//

void channel::setNeedPass(bool value) { chan.needPass = value; }
void channel::setMaxConnectedUser(int value) { chan.maxConnectedUser = value; }
void channel::setPassword(std::string const &password) { chan.password = password; }
void channel::setTopic(std::string const &message) { chan.topicMessage = message; }
void channel::setMode(char c, bool value)
{
	if (c == 'i')
		chan.i_Mode = value;
	if (c == 't')
		chan.t_Mode = value;
	if (c == 'k')
		chan.k_Mode = value;
	if (c == 'o')
		chan.o_Mode = value;
	if (c == 'l')
		chan.l_Mode = value;
}

void channel::setUserConnect(client &user)
{
	if (getIsConnected(user.getID()) == true)
		return;
	chan.connected.push_back(user);
	if (user.getNickname() != "MrRobot")
		chan.nbConnectedUser++;
	if (chan.connected.size() == 1)
		chan.chanOp.push_back(user);
	user.addChannel(*this);
	welcomeMessage(user);
	sendInfoToChannel(user, " has logged in");
}

void channel::setUserDisconnect(client &user)
{
	for (std::vector<client>::iterator it = chan.connected.begin(); it != chan.connected.end(); it++)
	{
		if (it->getID() == user.getID())
		{
			user.deleteChannel(*this);
			if (getIsChanOp(user.getID()) == true)
			{
				for (std::vector<client>::iterator it1 = chan.chanOp.begin(); it1 != chan.chanOp.end(); it1++)
				{
					if (it1->getID() == user.getID())
					{
						chan.chanOp.erase(it1);
						break;
					}
				}
			}
			chan.connected.erase(it);
			if (user.getNickname() != "MrRobot")
				chan.nbConnectedUser--;
			sendToChannel(user, "has left");
			break;
		}
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
		chan.chanOp.push_back(user);
}

void channel::setChannelName(std::string const &name) { chan.name = name; }

void channel::setAllInvited(void){
	for (std::vector<client>::iterator it = chan.connected.begin(); it != chan.connected.end(); it++){
		chan.invited.push_back(*it);
	}
}

void channel::setUserShutdown(client const &user)
{
	for (std::vector<client>::iterator it = chan.invited.begin(); it != chan.invited.end(); it++)
	{
		if (it->getNickname() == user.getNickname())
		{
			chan.invited.erase(it);
			break;
		}
	}
	for (std::vector<client>::iterator it = chan.chanOp.begin(); it != chan.chanOp.end(); it++)
	{
		if (it->getNickname() == user.getNickname())
		{
			chan.chanOp.erase(it);
			break;
		}
	}
	for (std::vector<client>::iterator it = chan.connected.begin(); it != chan.connected.end(); it++)
	{
		if (it->getNickname() == user.getNickname())
		{
			chan.connected.erase(it);
			break;
		}
	}
}

//**********************************//FUNCTION//**********************************//

void channel::welcomeMessage(client const &user) const
{
	std::time_t currentTime = std::time(0);
	std::ostringstream oss;
	oss << currentTime;
	std::string msg;
	std::string CLIENT = ":" + user.getNickname() + "!" + user.getUsername() + "@localhost ";
	msg = CLIENT + "JOIN " + chan.name + "\r\n";
	send(user.getFD(), msg.c_str(), msg.size(), 0);
	if (chan.topicMessage.size() > 0)
	{
		msg = CLIENT + "332 " + user.getNickname() + " " + chan.name + " :" + chan.topicMessage + "\r\n";
		send(user.getFD(), msg.c_str(), msg.size(), 0);
	}
	msg = CLIENT + "353 " + user.getNickname() + " = " + chan.name + " :" + userList() + "\r\n";
	send(user.getFD(), msg.c_str(), msg.size(), 0);
	msg = CLIENT + "366 " + user.getNickname() + " " + chan.name + " :End of NAMES list\r\n";
	send(user.getFD(), msg.c_str(), msg.size(), 0);
	msg = CLIENT + "329 " + user.getNickname() + " " + chan.name + " " + oss.str() +"\r\n";
	send(user.getFD(), msg.c_str(), msg.size(), 0);
}

void channel::sendToChannel(client const &user, std::string const &message)
{
	for (std::vector<client>::iterator it = chan.connected.begin(); it != chan.connected.end(); it++)
	{
		if (it->getFD() > 0 &&  it->getID() != user.getID())
		{
			std::string msg = ":" + user.getNickname() + "!" + user.getUsername() + "@localhost PRIVMSG " + chan.name + " :" + message + "\r\n";
			send(it->getFD(), msg.c_str(), msg.size(), 0);
		}
	}
}

void channel::sendToChannelNotice(client const &user, std::string const &message)
{
	for (std::vector<client>::iterator it = chan.connected.begin(); it != chan.connected.end(); it++)
	{
		if (it->getFD() > 0)
		{
			std::string CLIENT = ":" + user.getNickname() + "!" + user.getUsername() + "@localhost NOTICE ";
			std::string  msg = CLIENT + chan.name + " :" + message + "\r\n";
			send(it->getFD(), msg.c_str(), msg.size(), 0);
		}
	}
}

void channel::sendInfoToChannel(client const &user, std::string const &message)
{
	std::string msg;
	std::string CLIENT;
	for (std::vector<client>::iterator it = chan.connected.begin(); it != chan.connected.end(); it++)
	{
		if (it->getFD() > 0 &&  it->getID() != user.getID())
		{
			CLIENT = ":" + user.getNickname() + "!" + user.getUsername() + "@localhost 372 ";
			msg = CLIENT + chan.name + " :" + message + "\n";
			send(it->getFD(), msg.c_str(), msg.size(), 0);
		}
	}
}

bool channel::userCanWrite(client const &user)
{
	if (getIsConnected(user.getID()) == false)
	{

		std::string CLIENT = ":" + user.getNickname() + "!" + user.getUsername() + "@localhost ";
		std::string msg = CLIENT + "404 " + user.getNickname() + " " + chan.name +  ":You have not joined the channel\r\n";
		send(user.getFD(), msg.c_str(), msg.size(), 0);
		return false;
	}
	return true;
}

bool channel::userCanJoin(client const &user, std::string const &password) const
{
	std::string CLIENT = ":" + user.getNickname() + "!" + user.getUsername() + "@localhost ";
	if (chan.i_Mode == true && getIsInvited(user.getID()) == false)
	{
		std::string msg = CLIENT + "473 " + user.getNickname() + " " + chan.name +  " :" + chan.name + "\r\n";
		send(user.getFD(), msg.c_str(), msg.size(), 0);
		return false;
	}
	if (chan.needPass == true && chan.password != password)
	{
		std::string msg = CLIENT + "475 " + user.getNickname() + " " + chan.name +  " :" + chan.name + "\r\n";
		send(user.getFD(), msg.c_str(), msg.size(), 0);
		return false;
	}
	if (chan.nbConnectedUser == chan.maxConnectedUser)
	{
		std::string msg = CLIENT + "471 " + user.getNickname() + " " + chan.name +  " :" + chan.name + "\r\n";
		send(user.getFD(), msg.c_str(), msg.size(), 0);
		return false;
	}
	return true;
}

std::string channel::userList(void) const
{
	std::string txt = "";
	for (std::vector<client>::const_iterator it = chan.connected.begin(); it != chan.connected.end(); it++)
	{
		if (getIsChanOp(it->getID()) == false)
			txt += it->getNickname() + " ";
	}
	for (std::vector<client>::const_iterator it = chan.chanOp.begin(); it != chan.chanOp.end(); it++)
		txt += "@" + it->getNickname() + " ";
	return txt;
}

void channel::switchUser(client const &user)
{
	for (std::vector<client>::iterator it = chan.chanOp.begin(); it != chan.chanOp.end(); it++)
	{
		if (it->getID() == user.getID())
		{
			chan.chanOp.erase(it);
			chan.chanOp.push_back(user);
		}
	}
	for (std::vector<client>::iterator it = chan.connected.begin(); it != chan.connected.end(); it++)
	{
		if (it->getID() == user.getID())
		{
			chan.connected.erase(it);
			chan.connected.push_back(user);
		}
	}
	for (std::vector<client>::iterator it = chan.invited.begin(); it != chan.invited.end(); it++)
	{
		if (it->getID() == user.getID())
		{
			chan.invited.erase(it);
			chan.invited.push_back(user);
		}
	}
}

void channel::sendToChannelnoPRIVMSG(client const &user, std::string const &message)
{
	for (int i = 0; i < chan.nbConnectedUser; i++)
	{
		if (chan.connected[i].getID() != user.getID())
		{
			if (send(chan.connected[i].getFD(), message.c_str(), message.size(), 0) == -1)
				std::cerr << RED << "erreur send" << NONE << std::endl;
		}
	}
}

void channel::sendToOne(client const &user, std::string const &message) const
{
	for (std::vector<client>::const_iterator it = chan.connected.begin(); it != chan.connected.end(); it++)
	{
		if (it->getFD() > 0 &&  it->getID() == user.getID())
		{
			std::string msg = ":" + user.getNickname() + "!" + user.getUsername() + "@localhost PRIVMSG " + chan.name + " :" + message + "\r\n";
			send(it->getFD(), msg.c_str(), msg.size(), 0);
			break;
		}
	}
}

void channel::undoUserChanOp(client const &user){
	if (getIsChanOp(user.getID()) == true){
		for (std::vector<client>::iterator it = chan.chanOp.begin(); it != chan.chanOp.end(); it++){
			if (it->getID() == user.getID()){
				chan.chanOp.erase(it);
				return ;
			}
		}
	}
}

void channel::unsetUserInvited(client const &user){
	for (std::vector<client>::iterator it = chan.invited.begin(); it != chan.invited.end(); it++){
		if (it->getID() == user.getID())
		{
			chan.invited.erase(it);
			break;
		}
	}
}
