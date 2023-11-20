#include "irc.hpp"

void server::parseCommand(std::string buff, int fd)
{
	std::istringstream iss(buff);
	std::string command;
	std::string msg;
	iss >> command;
	if (command == "KICK")
	{
		std::cout << "commande recu a traiter: KICK" << std::endl; 
	}
	else if (command == "NICK")
	{
		cmdNick(fd, buff);
	}
	else if (command == "PRIVMSG")
	{
		cmdPrivmsg(fd, buff);
	}
	else if (command == "JOIN")
	{
		cmdJoin(buff, fd);
	}
	else if (command == "INVITE")
	{
		std::cout << "commande recu a traiter: INVITE" << std::endl;
	}
	else if (command == "TOPIC")
	{
		std::cout << "commande recu a traiter: TOPIC" << std::endl;
	}
	else if (command == "MODE")
	{
		std::cout << "commande recu a traiter: " << buff << std::endl;
	}
	else if (command == "PART")
	{
		cmdPart(fd, buff);
	}
	else if (command == "PING")
	{
		cmdPing(buff, fd);
	}
	else if (command == "WHOIS")
	{
		cmdWHOIS(fd);
	}
	else if (command == "QUIT")
	{
		std::cout << "[SERVER: USER DISCONNECTED]: " << mapUser.find(fd)->second.getNickname() << std::endl;
		closeOne(fd);
	}
	else
		std::cout << "Message en attente de parsing: " << buff;
}

void server::cmdKick(void) {}

void server::cmdNick(int fd, std::string buff)
{
	std::istringstream iss(buff);
	std::string command;
	iss >> command;
	iss >> command;
	mapUser.find(fd)->second.setNickname(command);
	send(fd, std::string("001 " + mapUser.find(fd)->second.getNickname() + "\r\n").c_str(), std::string("001 " + mapUser.find(fd)->second.getNickname() + "\r\n").length(), 0);
	send(fd, std::string("Your new nickname is " + command + "\r\n").c_str(), std::string("Your new nickname is " + command + "\r\n").length(), 0);
}

void server::cmdPrivmsg(int fd, std::string buff)
{
	std::vector<channel>::iterator it;
	std::istringstream iss(buff);
	std::string command;
	iss >> command;
	std::string commandParam = buff.substr(command.size(), buff.size());
	iss.clear();
	iss.str(commandParam);
	std::string channelName;
	iss >> channelName;
	std::string commandText = commandParam.substr(channelName.size() + 1, commandParam.size());
	if ((it = selectChannel(channelName)) != channelList.end())
	{
		if (it->userCanWrite(mapUser.find(fd)->second) == true)
		{
			it->sendToChannel(mapUser.find(fd)->second, commandText);
			std::cout << channelName << " <" << mapUser.find(fd)->second.getNickname() << "> " << commandText << std::endl;
		}
	}
	else
	{
		std::string msg = channelName + " is invalid\n";
		send(fd, msg.c_str(), msg.size(), 0);
	}	
}

void server::cmdJoin(std::string buff, int fd)
{
	std::vector<channel>::iterator it;
	std::istringstream iss(buff);
	std::string msg;
	std::string command;
	std::string channelName;
	std::string password;
	iss >> command >> channelName >> std::ws >> password;
	if ((it = selectChannel(channelName)) != channelList.end())//channel existant
	{
		if (it->userCanJoin(mapUser.find(fd)->second, password) == true)
			it->setUserConnect(mapUser.find(fd)->second);
		return;
	}
	if (checkChannelName(channelName) == false)//nom de channel a checker
	{
		msg = channelName + " is invalid and cannot be create\n";
		send(fd, msg.c_str(), msg.size(), 0);
		return;
	}
	channel temp(channelName);//si nouveau channel
	temp.setUserConnect((mapUser.find(fd))->second);
	if (password.size() > 0)
	{
		temp.setNeedPass(true);
		temp.setPassword(password);
	}
	channelList.push_back(temp);
}


void server::cmdInvite(void) {}

void server::cmdTopic(void) {}

void server::cmdMode(void) {}

void server::cmdPing(std::string buff, int fd)
{
	std::istringstream iss(buff);
	std::string pingMsg;
	iss >> pingMsg >> pingMsg; 
	std::string pongMsg = "PONG " + pingMsg + "\r\n";
	send(fd,pongMsg.c_str(), pongMsg.size(), 0);
}

void server::cmdWHOIS(int fd)
{
	std::string msg;
	msg = ":ircserv 318 mapUser.find(fd)->second.getNickname() :End of WHOIS list\n";
	send(fd, msg.c_str(), msg.size(), 0);
}

void server::cmdPart(int fd, std::string buff)
{
	size_t Pos = buff.find(' ');
    if (Pos != std::string::npos)
		buff = buff.substr(Pos + 1);
	
	// gerer si absence de raison? (no ':')
    std::map<int, client>::iterator it = mapUser.find(fd);

    if (it != mapUser.end())
	{
        client& client = it->second;
        std::istringstream iss(buff);
        std::string mychannel;
		size_t mypos = buff.find_first_of(":");
		std::string message = buff.substr(mypos + 1, buff.length());
		buff = buff.substr(0, mypos);

        while (1)
		{
			size_t mypos = buff.find_first_of(",");
			if (mypos != std::string::npos)
			{
				mychannel = buff.substr(0, mypos);
				buff = buff.substr(mypos + 1, buff.length());
			}
			else
			{
				size_t mypos = buff.find_first_of("#");
				if (mypos != std::string::npos)
					mychannel = buff.substr(mypos, buff.length() - 1);
				else
					break;
				buff = "";
			}
			std::vector<channel>::iterator it;
			for (it = channelList.begin(); it != channelList.end(); ++it) 
			{
				if (it->getChannelName() == mychannel) 
				{			
					if (it != channelList.end())  //on verifie que le channel existe a chaque fois
					{
						if(it->getIsConnected(client.getID()))
						{
							it->setUserDisconnect(client);
							std::cout << "<" << client.getNickname() << "> left channel : " << mychannel << std::endl;
							message = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost PART " + mychannel + " :" + message;
							it->sendToChannel(mapUser.find(fd)->second, message);
							send(fd, message.c_str(), message.size(), 0);
						}
					}
					break;
				}
			}
    	}
	}
}


