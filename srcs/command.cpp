#include "irc.hpp"

// KICK [<channel>] <nicks> [<reason>]

// pas reussi a trigger :
// ERR_NEEDMOREPARAMS
// ERR_BADCHANMASK

void server::cmdKick(int fd, std::string buff)
{
	//std::cout << "buff = " << buff << std::endl;
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

				for (channelIt = vecChannel.begin(); channelIt != vecChannel.end(); ++channelIt)
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
						std::cout << "countchannel = " << countchannel << " size = " << channels.size() << std::endl;
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

void server::cmdJoin(std::string buff, int fd)
{
	std::istringstream iss(buff);
	std::string cmd;
	std::string name;
	iss >> cmd >> name;
	int join = 0;
	
	std::map<int, client>::iterator it = mapUser.find(fd);
    client& myclient = it->second;

	for (size_t i = 0; i < vecChannel.size(); i++)
	{
		if (vecChannel[i].getChannelName().compare(name) == 0) // verifie si le channel existe deja
		{
			join = 1;
			vecChannel[i].setNbUserUp();//incremente le nombre de connectes
			vecChannel[i].setConnect((mapUser.find(fd))->second);//insert le nouveau clien client dans un vector dans channel
			if (vecChannel[i].getNbUser() == 1)//si c est le 1er connecte il devient admin
				vecChannel[i].setAdminTrue((mapUser.find(fd))->second);
			std::string message = ":" + myclient.getNickname() + "!" + myclient.getUsername() + "@localhost JOIN " + vecChannel[i].getChannelName() + "\r\n";		
			send(fd, message.c_str(), message.size(), 0);
		}
	}
	if (join == 0)//si le channel n existe pas on le cree
	{
		channel temp(name);
		temp.setNbUserUp();
		temp.setConnect((mapUser.find(fd))->second);
		temp.setAdminTrue((mapUser.find(fd))->second);
		vecChannel.push_back(temp);
		std::string message = ":" + myclient.getNickname() + "!" + myclient.getUsername() + "@localhost JOIN " + temp.getChannelName() + "\r\n";		
		send(fd, message.c_str(), message.size(), 0);
	}
}

void server::cmdPrivmsg(int fd, std::string buff)
{
	std::istringstream iss(buff);
	std::string command;
	iss >> command;
	std::string s1 = buff.substr(command.size(), buff.size());
	iss.clear();
	iss.str(s1);
	std::string channelName;
	iss >> channelName;
	std::string s2 = s1.substr(channelName.size() + 1, s1.size());
	std::string message = ":" + (mapUser.find(fd))->second.getNickname() + " PRIVMSG " + channelName + " :" + s2 + "\r\n";
	std::cout << channelName << " <" << mapUser.find(fd)->second.getNickname() << "> " << s2 << std::endl;
	
	size_t i = 0;
	while (i < vecChannel.size() && vecChannel[i].getChannelName().compare(channelName) != 0)
		i++;
	vecChannel[i].sendToChannel(mapUser.find(fd)->second, message);
}

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

void server::cmdInvite()
{

}

void server::cmdTopic()
{

}

void server::cmdMode()
{

}

// gerer les noms de channel qui contiennent ":" ou des "'" ?? est ce possible?


//quand /PART sansa rg -> on leave le current channel 
// si PART avec juste message -> on leave avec le message

// PART [<channels>] [<message>]

// PART renvois au precedent channel non quitté

void server::cmdPart(int fd, std::string buff)
{
	size_t Pos = buff.find(' ');
    if (Pos != std::string::npos)
		buff = buff.substr(Pos + 1);

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
			for (it = vecChannel.begin(); it != vecChannel.end(); ++it) 
			{
				if (it->getChannelName() == mychannel) 
				{			
					if (it != vecChannel.end())
					{
						if(it->getConnected(client))
						{
							it->setDisconnect(client);
							std::cout << "Client " << client.getNickname() << " left channel " << mychannel << ": " << message << std::endl;
							message = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost PART " + mychannel + " :" + message;
							it->sendToChannel(client, message);
							send(fd, message.c_str(), message.size(), 0);
						}
					}
					break;
				}
			}
    	}
	}
}
