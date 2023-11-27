#include "irc.hpp"

void server::receivMessage(void)
{
	for (int i = 1; i < maxFD + 1; i++)
	{
		if (_fds[i].revents & (POLLIN | POLLERR))
		{
			if (_fds[i].revents & POLLIN)
				inputMessage(_fds[i].fd);
			else if (_fds[i].revents & POLLERR)
				inputError(_fds[i].fd);
			_fds[i].revents = 0;
		}
	}
}

void server::inputError(int fd)
{
	(void)fd;
	std::cout << "DEBUG errMessage en cours" << std::endl;
}

void server::inputMessage(int fd)
{
	std::string msg;
	int size;
	char buff[bufferSize];
	mapUser.find(fd)->second.setNetcat(-1);
	memset(&buff, 0, bufferSize);
	size = recv(fd, buff, bufferSize - 1, MSG_DONTWAIT);
	if (size < 0)//********** MESSAGE VIDE OU MAL RECEPTIONNE
	{
		if (errno != EAGAIN && errno != EWOULDBLOCK)
		{
			msg = ":400 " + mapUser.find(fd)->second.getNickname() + ":Your message could not be sent./nPlease try again\r\n";
			send(fd, msg.c_str(), msg.size(), 0);
		}
	}
	else if (size == 0)//********** MESSAGE INDIQUANT LA DECONNEXION D'UN CLIENT
	{
		std::cout << GREEN << "[42_IRC:  USER DISCONNECTED] " << mapUser.find(fd)->second.getNickname() << NONE << std::endl;
		closeOne(fd);
	}
	else 
	{
		if (mapUser.find(fd)->second.getLog() < 2)
			configureNewUser(buff, fd);
		else
		{
			if (buff[0] == '/')
				parseCommand(&buff[1], fd);
			else
				parseCommand(buff, fd);
		}
	}
}

void server::configureNewUser(std::string const &buff, int fd)
{
	std::istringstream iss(buff);
	std::string command;
	std::string msg;
	iss >> command;
	if (command == "QUIT" || command == "quit" || command == "PING")// || command == "PASS" || command == "pass")
		parseCommand(buff, fd);
	else
	{
		std::string username;
		std::string password = extract(buff, "PASS ", "\n");
		std::string nickname = extract(buff, "NICK ", "\n");
		std::string name = extract(buff, "USER ", "\n");
		iss.clear();
		iss.str(name);
		iss >> username;
		if (mapUser.find(fd)->second.getLog() == 0)
			cmdPass(password, fd);
		if (mapUser.find(fd)->second.getLog() == 1)
		{
			mapUser.find(fd)->second.setLog();
			std::string str = "NICK " + nickname + " " + username;
			cmdNick(fd, str);
			// if (username.size() == 0 || nameUserCheck(username) == false || nameExist(username) == false)
			// 	mapUser.find(fd)->second.setUsername(mapUser.find(fd)->second.getNickname());
			// else 
			// 	mapUser.find(fd)->second.setUsername(username);
			printHome(fd);
			std::cout << GREEN << BOLD << "[42_IRC:  USER LOGGED IN] "<< mapUser.find(fd)->second.getNickname() << NONE << std::endl;
		}
	}
}








