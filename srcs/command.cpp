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

void server::cmdPrivmsg(std::string buff, int fd)
{
	std::string m1;
	std::string name;
	std::string str;
	std::string message;
	std::istringstream iss;
	iss.str(buff);
	std::getline(iss, m1, '#');
	str = buff.substr(m1.size(), buff.size());
	iss.clear();
	iss.str(str);
	std::getline(iss, name, ' ');
	message = str.substr(name.size(), str.size());

	std::cout << name << " [" << mapUser.find(fd)->second.getNickname() << "] " << message << std::endl;
	//jusque la je recupere les info mais je n ai pas encore reussi a a m en servir efficacement
	
	size_t i = 0;
	while (i < vecChannel.size() && vecChannel[i].getChannelName().compare(name) != 0)
		i++;
	vecChannel[i].sendToChannel(mapUser.find(fd)->second, message);
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





