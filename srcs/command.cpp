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
	for (size_t i = 0; i < vecChannel.size(); i++)
	{
		if (vecChannel[i].getChannelName().compare(name) == 0) // verifie si le channel existe deja
		{
			join = 1;
			vecChannel[i].setNbUserUp();//incremente le nombre de connectes
			vecChannel[i].setConnect((mapUser.find(fd))->second);//insert le nouveau clien client dans un vector dans channel
			if (vecChannel[i].getNbUser() == 1)//si c est le 1er connecte il devient admin
				vecChannel[i].setAdminTrue((mapUser.find(fd))->second);
		}
	}
	if (join == 0)//si le channel n existe pas on le cree
	{
		channel temp(name);
		temp.setNbUserUp();
		temp.setConnect((mapUser.find(fd))->second);
		temp.setAdminTrue((mapUser.find(fd))->second);
		vecChannel.push_back(temp);
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





