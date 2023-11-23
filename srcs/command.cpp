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


void server::cmdNick(int fd, std::string buff){
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

void server::cmdKick(int fd, std::string buff)
{
	std::istringstream iss1(buff);
	std::string buff2;
	std::map<int, client>::iterator it = mapUser.find(fd);
	unsigned int countchannel = 0;
	if (it != mapUser.end())
	{
		client &kicker = it->second;
		while (std::getline(iss1, buff2))
		{
			buff2 = buff2 + "\n";
			std::istringstream iss(buff2);
			std::string message, kickCommand, mychannel, nicks, reason;
			std::vector<channel>::iterator channelIt;
			iss >> kickCommand;
			iss >> mychannel;
			std::vector<std::string> channels;
			std::istringstream iss3(mychannel);
			std::string channel;
			while (std::getline(iss3, channel, ','))
				channels.push_back(channel);
			iss >> nicks;
			std::getline(iss, reason);
			if (reason == " :\r")
				reason = " :" + kicker.getNickname() + "\r";
			std::vector<std::string>::iterator actualChannel;
			for (actualChannel = channels.begin(); actualChannel != channels.end(); ++actualChannel)
			{
				bool notadmin = false;
				bool nochannel = false;
				bool notmember = false;	
				mychannel = *actualChannel;

				for (channelIt = channelList.begin(); channelIt != channelList.end(); ++channelIt)
				{
					if (channelIt->getChannelName() == mychannel)
					{
						nochannel = true;
						if (channelIt->getConnected(kicker))
						{
							notmember = true;
							if (channelIt->getAdmin(kicker))
								notadmin = true;
						}
						break;
					}
				}
				if (!nochannel) // si channel n'existe pas
				{
					if  (countchannel < channels.size())
					{
						message = ":" + kicker.getNickname() + "!" + kicker.getUsername() + "@localhost 403 " + kicker.getUsername() + " " + mychannel + " :No such channel\r\n";
						send(kicker.getFD(), message.c_str(), message.size(), 0);
					}
					countchannel++;
					continue ;
				}
				else if (!notmember) //si kicker pas membre
				{
					message = ":" + kicker.getNickname() + "!" + kicker.getUsername() + "@localhost 442 " + kicker.getUsername() + " " + mychannel + " :You're not on that channel\r\n";
					send(kicker.getFD(), message.c_str(), message.size(), 0);
					countchannel++;
					continue ;
				}
				else if (!notadmin) //si kicker pas admin
				{
					message = ":" + kicker.getNickname() + "!" + kicker.getUsername() + "@localhost 482 " + kicker.getUsername() + " " + mychannel + " :You're not channel operator\r\n";
					send(kicker.getFD(), message.c_str(), message.size(), 0);
					countchannel++;
					continue ;
				}
				if (channelIt->getConnectedFromString(nicks))
				{
					client *targetClient = channelIt->getClient(nicks);
					if (!targetClient)
					{
						countchannel++;
						continue;
					}
					channelIt->setDisconnect(*targetClient);
					message = ":" + kicker.getNickname() + "!" + kicker.getUsername() + "@localhost KICK " + mychannel + " " + nicks + reason + "\n";
					send(targetClient->getFD(), message.c_str(), message.size(), 0);
					message = ":" + kicker.getNickname() + "!" + kicker.getUsername() + "@localhost KICK " + mychannel + " " + nicks + reason + "\n";
					channelIt->sendToChannel(*targetClient, message);
				}
				else //si le membre n'est pas sur le channel
				{
					message = ":" + kicker.getNickname() + "!" + kicker.getUsername() + "@localhost 401 " + kicker.getUsername() + " " + nicks + " :No such nick/channel\r\n";
					send(kicker.getFD(), message.c_str(), message.size(), 0);
				}
				countchannel++;
			}
		}
	}
}

// void server::cmdJoin(std::string buff, int fd)
// {
// 	std::istringstream iss(buff);
// 	std::string cmd;
// 	std::string name;
// 	iss >> cmd >> name;
// 	int join = 0;
	
// 	std::map<int, client>::iterator it = mapUser.find(fd);
//     client& myclient = it->second;

// 	for (size_t i = 0; i < channelList.size(); i++)
// 	{
// 		if (channelList[i].getChannelName().compare(name) == 0) // verifie si le channel existe deja
// 		{
// 			join = 1;
// 			channelList[i].setNbUserUp();//incremente le nombre de connectes
// 			channelList[i].setConnect((mapUser.find(fd))->second);//insert le nouveau clien client dans un vector dans channel
// 			if (channelList[i].getNbUser() == 1)//si c est le 1er connecte il devient admin
// 				channelList[i].setAdminTrue((mapUser.find(fd))->second);
// 			std::string message = ":" + myclient.getNickname() + "!" + myclient.getUsername() + "@localhost JOIN " + channelList[i].getChannelName() + "\r\n";		
// 			send(fd, message.c_str(), message.size(), 0);
// 		}
// 	}
// 	if (join == 0)//si le channel n existe pas on le cree
// 	{
// 		channel temp(name);
// 		temp.setNbUserUp();
// 		temp.setConnect((mapUser.find(fd))->second);
// 		temp.setAdminTrue((mapUser.find(fd))->second);
// 		channelList.push_back(temp);
// 		std::string message = ":" + myclient.getNickname() + "!" + myclient.getUsername() + "@localhost JOIN " + temp.getChannelName() + "\r\n";		
// 		send(fd, message.c_str(), message.size(), 0);
// 	}
// }
	
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


//    Parameters: <channel> *( "," <channel> ) [ <Part Message> ]

//pas reussi a trigger ERR_NEEDMOREPARAMS
void server::cmdPart(int fd, std::string buff)
{
	std::istringstream iss(buff);
	std::map<int, client>::iterator it = mapUser.find(fd);
	if (it != mapUser.end())
	{
		client &parter = it->second;
		std::string message, partCommand, mychannel, reason;
		std::vector<channel>::iterator channelIt;
		iss >> partCommand;
		iss >> mychannel;
		std::vector<std::string> channels;
		std::istringstream iss3(mychannel);
		std::string channel;
		while (std::getline(iss3, channel, ','))
			channels.push_back(channel);
		std::getline(iss, reason);
		if (reason == " :\r")
			reason = " :" + parter.getNickname() + "\r"; //voir ce qu'il y a dans ce cas
		std::vector<std::string>::iterator actualChannel;
		for (actualChannel = channels.begin(); actualChannel != channels.end(); ++actualChannel)
		{
			bool nochannel = false;
			bool notmember = false;	
			mychannel = *actualChannel;

			for (channelIt = channelList.begin(); channelIt != channelList.end(); ++channelIt)
			{
				if (channelIt->getChannelName() == mychannel)
				{
					nochannel = true;
					if (channelIt->getConnected(parter))
						notmember = true;
					break;
				}
			}
			if (!nochannel) // si channel n'existe pas
			{
				message = ":" + parter.getNickname() + "!" + parter.getUsername() + "@localhost 403 " + parter.getUsername() + " " + mychannel + " :No such channel\r\n";
				send(parter.getFD(), message.c_str(), message.size(), 0);
				continue ;
			}
			else if (!notmember) //si  pas membre
			{
				message = ":" + parter.getNickname() + "!" + parter.getUsername() + "@localhost 442 " + parter.getUsername() + " " + mychannel + " :You're not on that channel\r\n";
				send(parter.getFD(), message.c_str(), message.size(), 0);
				continue ;
			}
			channelIt->setDisconnect(parter);
			message = ":" + parter.getNickname() + "!" + parter.getUsername() + "@localhost PART " + mychannel + reason + "\n";
			channelIt->sendToChannel(parter, message);
			send(fd, message.c_str(), message.size(), 0);
		}
	}
}
