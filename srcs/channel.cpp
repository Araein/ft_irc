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

//**********************************//GETTER//**********************************//

int channel::getNbConnectedUser(void) const { return chan.nbConnectedUser; }
int channel::getNeedPass(void) const { return chan.needPass; }
int channel::getMaxConnectedUser(void) const { return chan.maxConnectedUser; }
std::string channel::getChannelName() const { return chan.name; }
std::string channel::getTopicMessage() const { return chan.topicMessage; }
std::string channel::getPassword() const { return chan.password; }

bool channel::getIsChanOp(int id)
{
	for (std::vector<client>::iterator it = chan.chanOp.begin(); it != chan.chanOp.end(); it++)
	{
		if (it->getID() == id)
			return true;
	}
	return false;
}

bool channel::getIsConnected(int id)
{
	for (std::vector<client>::iterator it = chan.connected.begin(); it != chan.connected.end(); it++)
	{
		if (it->getID() == id)
			return true;
	}
	return false;
}

bool channel::getIsInvited(int id)
{
	for (std::vector<client>::iterator it = chan.invited.begin(); it != chan.invited.end(); it++)
	{
		if (it->getID() == id)
			return true;
	}
	return false;
}

bool channel::getIsExcluded(int id)
{
	for (std::vector<client>::iterator it = chan.excluded.begin(); it != chan.excluded.end(); it++)
		if (it->getID() == id)
			return true;
	return false;
}

std::string channel::getAllChanOp(void)
{
	std::string msg = "";

	for (std::vector<client>::iterator it = chan.chanOp.begin(); it != chan.chanOp.end(); it++)
		msg += it->getNickname() + " ";
	msg += " \n";
	return msg;
}

std::string channel::getAllConnected(void)
{
	std::string msg = "";
	for (std::vector<client>::iterator it = chan.connected.begin(); it != chan.chanOp.end(); it++)
		msg += it->getNickname() + " ";
	msg += " \n";
	return msg;
}

std::string channel::getAllInvited(void)
{
	std::string msg = "";
	for (std::vector<client>::iterator it = chan.invited.begin(); it != chan.chanOp.end(); it++)
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

//**********************************//SETTER//**********************************//

void channel::setNeedPass(bool value) { chan.needPass = value; }
void channel::setMaxConnectedUser(int value) { chan.maxConnectedUser = value; }
void channel::setPassword(std::string password) { chan.password = password; }
void channel::setTopicMessage(std::string message) { chan.topicMessage = message; }

void channel::setUserConnect(client const &user)//********** CONNECTE UN CLIENT AU CANAL
{
	if (getIsConnected(user.getID()) == true)
		return;
	chan.connected.push_back(user);
	chan.nbConnectedUser++;
	if (getIsExcluded(user.getID()) == false)
	{
		if (chan.nbConnectedUser == 1)
			setUserChanOp(user);
		welcomeMessage(user);
		sendToChannel(user, "is connected");
	}
}

void channel::setUserDisconnect(client const &user)//********** DECONNECTE UN CLIENT DU CANAL
{
	if (getIsConnected(user.getID()) == true)
	{
		chan.connected.erase(findUser(user, "connected"));
		chan.nbConnectedUser--;
		sendToChannel(user, "disconnected");
		std::string msg = "" + chan.name + " \n";
		if (getIsExcluded(user.getID()) == false)
			send(user.getFD(), msg.c_str(), msg.size(), 0);
	}
	if (getIsExcluded(user.getID()) == true)
		setUserExcluded(user, false);
}

void channel::setUserInvited(client const &user)//********** INSCRIT UN CLIENT COMME INVITE
{
	if (getIsInvited(user.getID()) == false)
		chan.invited.push_back(user);
}

void channel::setUserChanOp(client const &user)//********** INSCRIT UN CLIENT COMME CHANNEL OPERATOR(CHANOP)
{
	if (getIsChanOp(user.getID()) == false)
		chan.chanOp.push_back(user);
}

void channel::setUserExcluded(client const &user, bool value)//********** INSCRIT/DESINSCRIT UN CLIENT COMME EXCLU(BANNI)
{
	if (value == true)
	{
		if (getIsExcluded(user.getID()) == false)
			chan.excluded.push_back(user);
	}
	else if (value == false)
	{
		if (getIsExcluded(user.getID()) == true)
		{
			for (std::vector<client>::iterator it = chan.excluded.begin(); it != chan.excluded.end(); it++)
			{
				if (it->getID() == user.getID())
				{
					chan.excluded.erase(it);
					break;
				}
			}
		}
	}
}


//**********************************//FUNCTION//**********************************//

void channel::welcomeMessage(client const &user)//********** MESSAGE D'ACCUEIL EN ENTRANT DANS UN CANAL(A REVOIR)
{
	std::ostringstream oss;
	oss << chan.nbConnectedUser;
	std::string txt;
	std::string msg = ":" + user.getNickname() + "!" + user.getUsername() + "@localhost JOIN " + chan.name +  "\n";
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
	// msg = ":ircserv 332 " + user.getNickname() + " :" + chan.topicMessage + " \r\n";
	// send(user.getFD(), msg.c_str(), msg.size(), 0);
}

void channel::sendToChannel(client const &user, std::string message)//********** COPIE LE MESSAGE D'UN CLIENT VERS TOUT LES CLIENT DANS LE CHANNEL
{
	if (getIsExcluded(user.getID()) == true)
		return;
	for (std::vector<client>::iterator it = chan.connected.begin(); it != chan.connected.end(); it++)
	{
		if (it->getFD() > 0 &&  it->getID() != user.getID())
		{
			std::string msg = ":" + user.getNickname() + "!" + user.getUsername() + "@localhost PRIVMSG " + chan.name + " :" + message + " \r\n";
			if (getIsExcluded(it->getID()) == false)
				send(it->getFD(), msg.c_str(), msg.size(), 0);
		}
	}
}



void channel::serverChannel()//********** CREE LES CHANNEL DE DEMARRAGE
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

std::vector<client>::iterator channel::findUser(client const &user, std::string vec)//********** TROUVE UN CLIENT DANS UNE DES BASES
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
	for (std::vector<client>::iterator it = it1; it != it2; it++)
	{
		if (it->getID() == user.getID())
			return it;
	}
	return it2;
}

bool channel::userCanWrite(client const &user)//********** INDIQUE SI LE CLIENT A LE DROIT D'ECRIRE
{
	if (getIsConnected(user.getID()) == false || getIsExcluded(user.getID()) == true)
		return false;
	if (chan.i_Mode == true && getIsInvited(user.getID()) == false)
		return false;
	return true;
}

void channel::userCanJoin(client const &user, std::string password)//********** EXCLU UN CLIENT QUI N A PAS LE DROIT DE REJOINDRE UN CANAL
{
	if (chan.i_Mode == true && getIsInvited(user.getID()) == false)
	{
		setUserExcluded(user, true);
		std::string msg = "You are not on the guest list of " + chan.name + " \n";
		send(user.getFD(), msg.c_str(), msg.size(), 0);
		return;
	}
	if (chan.needPass == true && chan.password != password)
	{
		setUserExcluded(user, true);
		std::string msg = chan.name + " invalid password\n";
		send(user.getFD(), msg.c_str(), msg.size(), 0);
		return;
	}
	if (chan.nbConnectedUser == chan.maxConnectedUser)
	{
		setUserExcluded(user, true);
		std::string msg = chan.name + " is full\n";
		send(user.getFD(), msg.c_str(), msg.size(), 0);
		return;
	}
}




