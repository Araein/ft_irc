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
}

//**********************************//GETTER//**********************************//

int channel::getNbConnectedUser(void) const { return chan.nbConnectedUser; }
int channel::getNeedPass(void) const { return chan.needPass; }
int channel::getMaxConnectedUser(void) const { return chan.maxConnectedUser; }
std::string channel::getChannelName() const { return chan.name; }
std::string channel::getTopic() const { return chan.topicMessage; }
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

std::string channel::getAllMode(void) const
{
	std::string txt= "";
	if (chan.i_Mode == true)
		txt += 'i';
	if (chan.t_Mode == true)
		txt += 't';
	if (chan.k_Mode == true)
		txt += 'k';
	if (chan.o_Mode == true)
		txt += 'o';
	if (chan.l_Mode == true)
		txt += 'l';
	return txt;
}

bool channel::getConnectedFromString(std::string const &user) const
{
	for (size_t i = 0; i < chan.connected.size(); i++)
	{
		if (chan.connected[i].getNickname() == user)
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
void channel::setPassword(std::string password) { chan.password = password; }
void channel::setTopic(std::string message) { chan.topicMessage = message; }
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

void channel::setUserConnect(client const &user)//********** CONNECTE UN CLIENT AU CANAL
{
	if (getIsConnected(user.getID()) == true)
		return;
	chan.connected.push_back(user);
	chan.nbConnectedUser++;
	if (chan.nbConnectedUser == 1)
		chan.chanOp.push_back(user);;
	welcomeMessage(user);
	sendToChannel(user, "is connected");
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
	std::time_t currentTime = std::time(0);
	std::ostringstream oss;
	oss << currentTime;
	std::string msg;
	std::string CLIENT = ":" + user.getNickname() + "!" + user.getUsername() + "@localhost ";
	msg = CLIENT + "JOIN " + chan.name + "\n";
	send(user.getFD(), msg.c_str(), msg.size(), 0);
	if (chan.topicMessage.size() > 0)
	{
		msg = CLIENT + "332 " + user.getNickname()+ " " + chan.name + " :" + chan.topicMessage + "\r\n";
		send(user.getFD(), msg.c_str(), msg.size(), 0);
	}
	msg = CLIENT + "353 " + user.getNickname() + " = " + chan.name + " :" + userList() + "\r\n";
	send(user.getFD(), msg.c_str(), msg.size(), 0);
	msg = CLIENT + "366 " + user.getNickname() + " " + chan.name + " :End of NAMES list\r\n";
	send(user.getFD(), msg.c_str(), msg.size(), 0);
	msg = CLIENT + "329 " + user.getNickname() + " " + chan.name + " " + oss.str() +"\r\n";//a revoir
	send(user.getFD(), msg.c_str(), msg.size(), 0);
	msg = CLIENT + "324 " + user.getNickname() + " " + chan.name + " " + getAllMode() +"\r\n";//a revoir
	send(user.getFD(), msg.c_str(), msg.size(), 0);



}

void channel::sendToChannel(client const &user, std::string message)//********** COPIE LE MESSAGE D'UN CLIENT VERS TOUT LES CLIENT DANS LE CHANNEL
{
	for (std::vector<client>::iterator it = chan.connected.begin(); it != chan.connected.end(); it++)
	{
		if (it->getFD() > 0 &&  it->getID() != user.getID())
		{
			std::string msg = ":" + user.getNickname() + "!" + user.getUsername() + "@localhost PRIVMSG " + chan.name + " :" + message + " \r\n";
			send(it->getFD(), msg.c_str(), msg.size(), 0);
		}
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
	if (getIsConnected(user.getID()) == false)
		return false;
	if (chan.i_Mode == true && getIsInvited(user.getID()) == false)
		return false;
	return true;
}

bool channel::userCanJoin(client const &user, std::string password)//********** EMPECHE UN CLIENT NON AUTORISE A ENTRER DANS UN CHANNEL
{
	if (chan.i_Mode == true && getIsInvited(user.getID()) == false)
	{
		std::string msg = ":" + user.getNickname() + "!" + user.getUsername() + "@localhost 473 " + user.getUsername() + chan.name +  " :" + chan.name + "\r\n";
		send(user.getFD(), msg.c_str(), msg.size(), 0);
		return false;
	}
	if (chan.needPass == true && chan.password != password)
	{
		std::string msg = ":" + user.getNickname() + "!" + user.getUsername() + "@localhost 475 " + user.getUsername() + chan.name +  " :" + chan.name + "\r\n";
		send(user.getFD(), msg.c_str(), msg.size(), 0);
		return false;
	}
	if (chan.nbConnectedUser == chan.maxConnectedUser)
	{
		std::string msg = ":" + user.getNickname() + "!" + user.getUsername() + "@localhost 471 " + user.getUsername() + chan.name +  " :" + chan.name + "\r\n";
		send(user.getFD(), msg.c_str(), msg.size(), 0);
		return false;
	}
	return true;
}

std::string channel::userList(void)
{
	std::string txt = "";
	for (std::vector<client>::iterator it = chan.connected.begin(); it != chan.connected.end(); it++)
	{
		if (getIsChanOp(it->getID()) == false)
			txt += it->getNickname() + " ";
	}
	for (std::vector<client>::iterator it = chan.chanOp.begin(); it != chan.chanOp.end(); it++)
		txt += "@" + it->getNickname() + " ";
	return txt;
}


