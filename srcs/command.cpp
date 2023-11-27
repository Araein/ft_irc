#include "irc.hpp"

void server::parseCommand(std::string buff, int fd)
{
	std::istringstream iss(buff);
	std::string command;
	std::string param;
	std::string msg;
	iss >> command >> param;
	std::cout << CYAN << "[42_IRC: <<<< " << mapUser.find(fd)->second.getNickname() << ":" << command << NONE << std::endl;
	if (command == "kick" || command == "KICK")
	{
		cmdKick(fd, buff);
	}
	else if (command == "nick" || command == "NICK")
	{
		cmdNick(fd, buff);
	}
	else if (command == "privmsg" || command == "PRIVMSG")
	{
		cmdPrivmsg(fd, buff);
	}
	else if (command == "join" || command == "JOIN")
	{
		cmdJoin(buff, fd);
	}
	else if (command == "invite" || command == "INVITE")
	{
		std::cout << "DEBUG commande recu a traiter: INVITE" << std::endl;//********** A SUPPRIMER
	}
	else if (command == "topic" || command == "TOPIC")
	{
		cmdTopic(fd, buff);
	}
	else if (command == "mode" || command == "MODE")
	{
		// cmdMode();
	}
	else if (command == "part" || command == "PART")
	{
		cmdPart(fd, buff);
	}
	else if (command == "ping" || command == "PING")
	{
		cmdPing(buff, fd);
	}
	else if (command == "pass" || command == "PASS")
	{
		cmdPass(param, fd);
	}
	else if (command == "quit" || command == "QUIT")
	{
		std::cout << GREEN << "[42_IRC:  USER LOGGED OUT] " << mapUser.find(fd)->second.getNickname() << NONE << std::endl;
		closeOne(fd);
	}
	else
	{
		msg = ":ircserv 421 :" + mapUser.find(fd)->second.getNickname() + " :42IRC does not support " + command + "\r\n";
		send(fd, msg.c_str(), msg.size(), 0);
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
						if (channelIt->getIsConnected(kicker.getID()))
						{
							notmember = true;
							if (channelIt->getIsChanOp(kicker.getID()))
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
					channelIt->setUserDisconnect(targetClient);
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

void server::cmdNick(int fd, std::string buff)
{
std::cout << "BUFF:" << std::endl;
	std::vector<std::string> vec = splitCommandNick(buff);
	std::string msg;
	std::string CLIENT = ":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() +  "@localhost ";
	if (vec.size() == 1)
	{
		msg = CLIENT + "431 " + mapUser.find(fd)->second.getNickname() + " :" + mapUser.find(fd)->second.getNickname() + "\r\n";
		send(fd, msg.c_str(), msg.size(), 0);
		return;
	}
	else if (nameUserCheck(vec[1]) == false)
	{
		msg = CLIENT + "432 " + mapUser.find(fd)->second.getNickname() + " :" + mapUser.find(fd)->second.getNickname() + "\r\n";
		send(fd, msg.c_str(), msg.size(), 0);
		return;
	}
	else if (nameExist(vec[1]) == false)
	{
		msg = CLIENT + "433 " + mapUser.find(fd)->second.getNickname() + " :" + mapUser.find(fd)->second.getNickname() + "\r\n";
		send(fd, msg.c_str(), msg.size(), 0);
		return;
	}
	if (vec[1].size() > 30)
		mapUser.find(fd)->second.setNickname(vec[1].substr(0, 29));
	else
		mapUser.find(fd)->second.setNickname(vec[1]);
	if (vec.size() < 3 || (nameUserCheck(vec[2]) == false))
		mapUser.find(fd)->second.setUsername(mapUser.find(fd)->second.getNickname());
	else
		mapUser.find(fd)->second.setUsername(vec[2]);
	mapUser.find(fd)->second.chanUpDate();
	userUpDate(&mapUser.find(fd)->second);
	CLIENT = ":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() +  "@localhost ";
	msg = CLIENT + "001 " + mapUser.find(fd)->second.getNickname() + " :Your new nickname is " + mapUser.find(fd)->second.getNickname() + "\r\n";
	send(fd, msg.c_str(), msg.size(), 0);
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
	std::string name;
	iss >> name;
	std::string commandText = commandParam.substr(name.size() + 1, commandParam.size());
	if ((it = selectChannel(name)) != channelList.end())
	{
		if (it->userCanWrite(&mapUser.find(fd)->second) == true)
		{
			it->sendToChannel(mapUser.find(fd)->second, commandText);
			std::cout << YELLOW << name << " <" << mapUser.find(fd)->second.getNickname() << "> "<< commandText  << NONE;
		}
		else
		{
			std::string msg = ":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost PRIVMSG " + name + " :You are not allowed to write in this channel \r\n";
			send(fd, msg.c_str(), msg.size(), 0);
		}
	}
	else
	{
		std::string msg = ":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost 403 " + " :" + name + " does not exist\r\n";
		send(fd, msg.c_str(), msg.size(), 0);
	}	
}

void server::cmdJoin(std::string buff, int fd)
{
	std::map<std::string, std::string> chanPass = splitCommandJoin(buff);
	std::map<std::string, std::string>::iterator it_chanPass = chanPass.begin();
	std::vector<channel>::iterator it_channelList;
	std::string CLIENT = ":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost ";

	while (it_chanPass != chanPass.end())
	{
		if ((it_channelList = selectChannel(it_chanPass->first)) != channelList.end())//channel existant
		{
			if (it_channelList->userCanJoin(&mapUser.find(fd)->second, it_chanPass->second) == true)
				it_channelList->setUserConnect(&mapUser.find(fd)->second);
		}
		else
		{
			std::string channelName; 
			if (it_chanPass->first.size() == 1)
			{
				std::string msg = CLIENT + "403 " + mapUser.find(fd)->second.getUsername() + " " + it_chanPass->first +  " :Invalid name\r\n";
				send(fd, msg.c_str(), msg.size(), 0);
			}
			else
			{
				if (it_chanPass->first.size() > 50)
					channelName = it_chanPass->first.substr(0, 49);
				else
					channelName = it_chanPass->first;
				channel temp(channelName);
				temp.setUserConnect(&mapUser.find(fd)->second);
				channelList.push_back(temp);
			}
		}
		it_chanPass++;
	}
}

void server::cmdInvite(int fd, std::string buff)
{
	std::istringstream iss(buff);
	std::map<int, client>::iterator it = mapUser.find(fd);
	if (it != mapUser.end())
	{
		client &inviter = it->second;
		std::string message, inviteCommand, mychannel, user;
		std::vector<channel>::iterator channelIt;
		std::map<int, client>::iterator it2;
		iss >> inviteCommand;
		iss >> user;
		iss >> mychannel;
		bool nochannel = false;
		bool notmember = false;
		bool alreadymember = false;
		for (it2 = mapUser.begin(); it2 != mapUser.end(); ++it2) 
		{
			if (it2->second.getNickname() == user) 
				break;
		}
		if (it2 == mapUser.end())
		{
			message = ":" + inviter.getNickname() + "!" + inviter.getUsername() + "@localhost 401 " + inviter.getUsername() + " " + user + " :No such nick/channel\r\n";
			send(inviter.getFD(), message.c_str(), message.size(), 0);
			return ;
		}
		for (channelIt = channelList.begin(); channelIt != channelList.end(); ++channelIt)
		{
			if (channelIt->getChannelName() == mychannel)
			{
				nochannel = true;
				if (channelIt->getIsConnected(inviter.getID()))
					notmember = true;
				if (channelIt->getConnectedFromString(user))
					alreadymember = true;	
				break;
			}
		}
		if (!nochannel)
		{
			message = ":" + inviter.getNickname() + "!" + inviter.getUsername() + "@localhost 403 " + inviter.getUsername() + " " + mychannel + " :No such channel\r\n";
			send(inviter.getFD(), message.c_str(), message.size(), 0);
			return ;
		}
		else if (!notmember)
		{
			message = ":" + inviter.getNickname() + "!" + inviter.getUsername() + "@localhost 442 " + inviter.getUsername() + " " + mychannel + " :You're not on that channel\r\n";
			send(inviter.getFD(), message.c_str(), message.size(), 0);
			return ;
		}
		else if (alreadymember)
		{
			message = ":" + inviter.getNickname() + "!" + inviter.getUsername() + "@localhost 443 " + inviter.getUsername() + " " + user + " " + mychannel + " :is already on channel\r\n";
			send(inviter.getFD(), message.c_str(), message.size(), 0);
			return ;	
		}
		// if (channelIt->private() && !(channelIt->getAdmin(inviter))) //-> si channel privé et user pas operator de ce channel
		// {
		// 	message = ":" + inviter.getNickname() + "!" + inviter.getUsername() + "@localhost 482 " + inviter.getUsername() + " " + mychannel + " :You're not channel operator\r\n";
		// 	channelIt->sendToChannel(inviter, message);
		// 	return ;
		// }

		// ajouter le user a la liste des invités/admis du channel ici
		message = ":" + inviter.getNickname() + "!" + inviter.getUsername() + "@localhost 341 " + inviter.getUsername() + " " + user + " " + mychannel + "\r\n";
		send(inviter.getFD(), message.c_str(), message.size(), 0);
		std::cout << message << std::endl;
		message = ":" + inviter.getNickname() + "!" + inviter.getUsername() + "@localhost NOTICE @" + mychannel + " :" + inviter.getUsername() + " invited " + user + " into channel  " + mychannel + "\r\n";
		send(inviter.getFD(), message.c_str(), message.size(), 0);
		message = ":" + it2->second.getNickname() + "!" + it2->second.getUsername() + "@localhost NOTICE @" + mychannel + " :" + inviter.getUsername() + " invited " + user + " into channel  " + mychannel + "\r\n";
		send(it2->second.getFD() , message.c_str(), message.size(), 0);
	}
}

void server::cmdTopic(int fd, std::string buff)
{
	std::string tmp;
<<<<<<< HEAD
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
=======
	std::string channelName;
	std::istringstream iss(buff);
	// int index;

	iss >> tmp;
	iss >> channelName;
	std::vector<channel>::iterator it_channelList = selectChannel(channelName);
	if (it_channelList == channelList.end()){
		send(fd, std::string("403" + channelName + ":no such channel\r\n").c_str(), std::string("403 " + channelName + ":no such channel\r\n").length(), 0);
		return ;
	}
	// index = findChanbyName(channel);
	tmp = iss.str().substr(6 + channelName.length() + 1, buff.length() - (6 + channelName.length() + 1));
	if (it_channelList->getIsConnected(mapUser.find(fd)->second.getID()) == true){
		if (tmp.empty() == true && it_channelList->getTopic().empty() == true){
			send(fd, std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost 331 " + mapUser.find(fd)->second.getNickname() + " " + channelName + " :No topic is set\r\n").c_str(), std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost 331 " + mapUser.find(fd)->second.getNickname() + " " + channelName + " :No topic is set\r\n").length(), 0);
			return ;
		}
		else if (tmp.empty() == true && it_channelList->getTopic().empty() == false){
			send(fd, std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost 332 " + mapUser.find(fd)->second.getNickname() + " " + channelName + " :" + it_channelList->getTopic() + "\r\n").c_str(), std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost 332 " + mapUser.find(fd)->second.getNickname() + " " + channelName + " :" + it_channelList->getTopic() + "\r\n").length(), 0);
			return ;
		}
		else if (tmp.empty() == false && it_channelList->getMode('t') == true && it_channelList->getIsChanOp(mapUser.find(fd)->second.getID()) == false){
			send(fd, std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost 482 " + mapUser.find(fd)->second.getNickname() + " " + channelName + ":You do not have permission to change the topic\r\n").c_str(), std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost 482 " + mapUser.find(fd)->second.getNickname() + " " + channelName + ":You do not have permission to change the topic\r\n").length(), 0);
			return ;
		}
		else if (tmp.empty() == false && it_channelList->getMode('t') == true && it_channelList->getIsChanOp(mapUser.find(fd)->second.getID()) == true){
			it_channelList->setTopic(tmp);
			send(fd, std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost TOPIC " + channelName + " " + tmp + "\r\n").c_str(), std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost TOPIC " + channelName + " " + tmp + "\r\n").length(), 0);
			it_channelList->sendToChannel(mapUser.find(fd)->second, std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost TOPIC " + channelName + " " + tmp + "\r\n").c_str());
			return ;
		}
		else if (tmp.empty() == false && it_channelList->getMode('t') == false){
			it_channelList->setTopic(tmp);
			send(fd, std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost TOPIC " + channelName + " " + tmp + "\r\n").c_str(), std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost TOPIC " + channelName + " " + tmp + "\r\n").length(), 0);
			it_channelList->sendToChannel(mapUser.find(fd)->second, std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost TOPIC " + channelName + " " + tmp + "\r\n").c_str());
>>>>>>> 3e7d6980c69169e8d0fc35d40044e4ed50616e22
			return ;
		}
	}
	else{
<<<<<<< HEAD
		send(fd, std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost 442 " + channel + ":You are not part of this channel\n").c_str(), std::string("442 " + channel + ":You are not part of this channel\n").length(), 0);
=======
		send(fd, std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost 442 " + channelName + ":You are not part of this channel\n").c_str(), std::string("442 " + channelName + ":You are not part of this channel\n").length(), 0);
>>>>>>> 3e7d6980c69169e8d0fc35d40044e4ed50616e22
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
<<<<<<< HEAD
			reason = " :" + parter.getNickname() + "\r"; //voir ce qu'il y a dans ce cas
=======
			reason = " :" + parter.getNickname() + "\r";
>>>>>>> 3e7d6980c69169e8d0fc35d40044e4ed50616e22
		std::vector<std::string>::iterator actualChannel;
		for (actualChannel = channels.begin(); actualChannel != channels.end(); ++actualChannel)
		{
			bool nochannel = false;
			bool notmember = false;	
			mychannel = *actualChannel;
<<<<<<< HEAD

=======
>>>>>>> 3e7d6980c69169e8d0fc35d40044e4ed50616e22
			for (channelIt = channelList.begin(); channelIt != channelList.end(); ++channelIt)
			{
				if (channelIt->getChannelName() == mychannel)
				{
					nochannel = true;
<<<<<<< HEAD
					if (channelIt->getConnected(parter))
=======
					if (channelIt->getIsConnected(parter.getID()))
>>>>>>> 3e7d6980c69169e8d0fc35d40044e4ed50616e22
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
<<<<<<< HEAD
			channelIt->setDisconnect(parter);
=======
			channelIt->setUserDisconnect(&parter);
			// enlever le user des invités du channel
>>>>>>> 3e7d6980c69169e8d0fc35d40044e4ed50616e22
			message = ":" + parter.getNickname() + "!" + parter.getUsername() + "@localhost PART " + mychannel + reason + "\n";
			channelIt->sendToChannel(parter, message);
			send(fd, message.c_str(), message.size(), 0);
		}
	}
}
<<<<<<< HEAD
=======

void server::cmdPass(std::string password, int fd)
{
	std::string msg;
	std::string CLIENT = ":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost ";
	if (mapUser.find(fd)->second.getLog() > 0)
	{
		msg = CLIENT + " 100 You have already logged in\r\n";
		send(fd, msg.c_str(), msg.size(), 0);
		return;
	}
	if (_password.compare(password) == 0)
	{
		mapUser.find(fd)->second.setLog();
		msg = ":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost " + GREEN + BOLD + "You have successfully logged in" + NONE + "\r\n";
		send(fd, msg.c_str(), msg.size(), 0);
		sendWelcomMsgs(fd);
		return;
	}
	msg = ":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost 464 " + RED + BOLD + "Invalid password, you will be disconnected" + NONE + "\r\n";
	send(fd, msg.c_str(), msg.size(), 0);
	closeOne(fd);
}




>>>>>>> 3e7d6980c69169e8d0fc35d40044e4ed50616e22
