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

void server::cmdNick(int fd, std::string buff){
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

void server::cmdTopic(int fd, std::string buff)
{
	std::string tmp;
	std::string channel;
	std::istringstream iss(buff);
	int index;
	(void)fd;

	iss >> tmp;
	iss >> channel;
	std::cout << "wtf = " << channel << std::endl;
	if (findChanbyName(channel) == -1){
		std::cout << findChanbyName(channel) << " test" << std::endl;
		send(fd, std::string("403 " + channel + ":no such channel\r\n").c_str(), std::string("403 " + channel + ":no such channel\r\n").length(), 0);
		return ;
	}
	index = findChanbyName(channel);
	iss >> tmp;
	iss >> tmp;
	std::cout << "topic = " << tmp << std::endl;
	if (vecChannel[index].getConnected(mapUser.find(fd)->second) == true){
		if (tmp.empty() == true && vecChannel[index].getTopic().empty() == true){
			send(fd, std::string("331 " + channel + " :No topic is set\r\n").c_str(), std::string("331 " + channel + " :No topic is set\r\n").length(), 0);
			std::cout << "ça devrait pas = " << vecChannel[index].getTopic() << std::endl;
			return ;
		}
		else if (tmp.empty() == true && vecChannel[index].getTopic().empty() == false){
			send(fd, std::string("332 " + channel + " :" + vecChannel[index].getTopic() + "\r\n").c_str(), std::string("332 " + channel + " :" + vecChannel[index].getTopic() + "\r\n").length(), 0);
			return ;
		}
		else if (tmp.empty() == false && vecChannel[index].isTopicRestricted() == true && vecChannel[index].getAdmin(mapUser.find(fd)->second) == false){
			send(fd, std::string("482 " + channel + " :You do not have permission to change the topic\r\n").c_str(), std::string("482 " + channel + " :You do not have permission to change the topic\r\n").length(), 0);
			return ;
		}
		else if (tmp.empty() == false && vecChannel[index].isTopicRestricted() == true && vecChannel[index].getAdmin(mapUser.find(fd)->second) == true){
			vecChannel[index].setTopic(tmp);
			send(fd, std::string("TOPIC " + channel + tmp + "\r\n").c_str(), std::string(channel + tmp + "\r\n").length(), 0);
			return ;
		}
		else if (tmp.empty() == false && vecChannel[index].isTopicRestricted() == false){
			vecChannel[index].setTopic(tmp);
			std::cout << "On est entré" << std::endl;
			send(fd, std::string("TOPIC " + channel + tmp + "\r\n").c_str(), std::string(channel + tmp + "\r\n").length(), 0);
			std::cout << "ça devrait pas = " << vecChannel[index].getTopic().empty() << std::endl;
			return ;
		}
	}
	else{
		send(fd, std::string("442 " + channel + " :You are not part of this channel\r\n").c_str(), std::string("442 " + channel + " :You are not part of this channel\r\n").length(), 0);
		return ;
	}
}

void server::cmdMode()
{

}





