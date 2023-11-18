#include "irc.hpp"

void server::cmdKick()
{

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
			for (it = vecChannel.begin(); it != vecChannel.end(); ++it) 
			{
				if (it->getChannelName() == mychannel) 
				{			
					if (it != vecChannel.end())  //on verifie que le channel existe a chaque fois
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
