#include "irc.hpp"

void server::parseCommand(std::string buff, int fd)
{
	std::istringstream iss(buff);
	std::string command;
	std::string param;
	std::string msg;
	iss >> command >> param;
	if (command == "CAP" || command == "WHOIS")
		return;
	if (command == "KICK")
	{
		std::cout << "DEBUG commande recu a traiter: KICK" << std::endl; //********** A SUPPRIMER
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
		std::cout << "DEBUG commande recu a traiter: INVITE" << std::endl;//********** A SUPPRIMER
	}
	else if (command == "TOPIC")
	{
		cmdTopic(fd, buff);
	}
	else if (command == "MODE")
	{
		// cmdMode();
	}
	else if (command == "PART")
	{
		cmdPart(fd, buff);
	}
	else if (command == "PING")
	{
		cmdPing(buff, fd);
	}
	else if (command == "QUIT")
	{
		std::cout << "[SERVER: USER DISCONNECTED]: " << mapUser.find(fd)->second.getNickname() << std::endl;
		closeOne(fd);
	}
	else
		std::cout << "DEBUGMessage en attente de parsing: " << buff;//********** A SUPPRIMER
}

void server::cmdKick(void) {}

void server::cmdNick(int fd, std::string buff)
{
	std::istringstream iss(buff);
	std::string command;
	iss >> command;
	iss >> command;
	if (checkNickname(command, fd) == true)
	{
		mapUser.find(fd)->second.setNickname(command);
		send(fd, std::string("001 " + mapUser.find(fd)->second.getNickname() + "\r\n").c_str(), std::string("001 " + mapUser.find(fd)->second.getNickname() + "\r\n").length(), 0);
		send(fd, std::string("Your new nickname is " + command + "\r\n").c_str(), std::string("Your new nickname is " + command + "\r\n").length(), 0);
	}
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
		else
		{
			std::string msg = ":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost PRIVMSG " + channelName + " :You are not allowed to write in this channel \r\n";
			send(fd, msg.c_str(), msg.size(), 0);
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
			it->userCanJoin(mapUser.find(fd)->second, password);
			it->setUserConnect(mapUser.find(fd)->second);
			return;
	}
	// if (checkChannelName(channelName) == false)//voir comment faire
	// {
	// 	msg = channelName + " is invalid and cannot be create\n";
	// 	return;
	// }
		channel temp(channelName);//si nouveau channel
		temp.setUserConnect((mapUser.find(fd))->second);
		if (password.size() > 0)
	{
		temp.setNeedPass(true);
		temp.setPassword(password);// voir si query
	}
		channelList.push_back(temp);
	}


void server::cmdTopic(int fd, std::string buff)
{
	std::string tmp;
	std::string channel;
	std::istringstream iss(buff);
	int index;

	iss >> tmp;
	iss >> channel;
	if (findChanbyName(channel) == -1){
		send(fd, std::string("403" + channel + ":no such channel\r\n").c_str(), std::string("403 " + channel + ":no such channel\r\n").length(), 0);
		return ;
	}
	index = findChanbyName(channel);
	tmp = iss.str().substr(6 + channel.length() + 1, buff.length() - (6 + channel.length() + 1));
	if (channelList[index].getIsConnected(mapUser.find(fd)->second.getID()) == true){
		if (tmp.empty() == true && channelList[index].getTopic().empty() == true){
			send(fd, std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost 331 " + mapUser.find(fd)->second.getNickname() + " " + channel + " :No topic is set\r\n").c_str(), std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost 331 " + mapUser.find(fd)->second.getNickname() + " " + channel + " :No topic is set\r\n").length(), 0);
			return ;
		}
		else if (tmp.empty() == true && channelList[index].getTopic().empty() == false){
			send(fd, std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost 332 " + mapUser.find(fd)->second.getNickname() + " " + channel + " :" + channelList[index].getTopic() + "\r\n").c_str(), std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost 332 " + mapUser.find(fd)->second.getNickname() + " " + channel + " :" + channelList[index].getTopic() + "\r\n").length(), 0);
			return ;
		}
		else if (tmp.empty() == false && channelList[index].isTopicRestricted() == true && channelList[index].getIsChanOp(mapUser.find(fd)->second.getID()) == false){
			send(fd, std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost 482 " + mapUser.find(fd)->second.getNickname() + " " + channel + ":You do not have permission to change the topic\r\n").c_str(), std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost 482 " + mapUser.find(fd)->second.getNickname() + " " + channel + ":You do not have permission to change the topic\r\n").length(), 0);
			return ;
		}
		else if (tmp.empty() == false && channelList[index].isTopicRestricted() == true && channelList[index].getIsChanOp(mapUser.find(fd)->second.getID()) == true){
			channelList[index].setTopic(tmp);
			send(fd, std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost TOPIC " + channel + " " + tmp + "\r\n").c_str(), std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost TOPIC " + channel + " " + tmp + "\r\n").length(), 0);
			channelList[index].sendToChannel(mapUser.find(fd)->second, std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost TOPIC " + channel + " " + tmp + "\r\n").c_str());
			return ;
		}
		else if (tmp.empty() == false && channelList[index].isTopicRestricted() == false){
			channelList[index].setTopic(tmp);
			send(fd, std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost TOPIC " + channel + " " + tmp + "\r\n").c_str(), std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost TOPIC " + channel + " " + tmp + "\r\n").length(), 0);
			channelList[index].sendToChannel(mapUser.find(fd)->second, std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost TOPIC " + channel + " " + tmp + "\r\n").c_str());
			return ;
		}
	}
	else{
		send(fd, std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost 442 " + channel + ":You are not part of this channel\n").c_str(), std::string("442 " + channel + ":You are not part of this channel\n").length(), 0);
		return ;
	}
}

// void server::cmdMode(int fd, std::string buff)
// {
// 	std::istringstream iss(buff);
// }

void server::cmdPing(std::string buff, int fd)
{
	std::istringstream iss(buff);
	std::string pingMsg;
	iss >> pingMsg >> pingMsg; 
	std::string pongMsg = "PONG " + pingMsg + "\r\n";
	send(fd,pongMsg.c_str(), pongMsg.size(), 0);
	std::cout << "DEBUG PONG" << std::endl;//********** A SUPPRIMER
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
							if (it->getIsExcluded(client.getID() == false))
								send(fd, message.c_str(), message.size(), 0);
						}
					}
					break;
				}
			}
    	}
	}
}



