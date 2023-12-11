#include "irc.hpp"


//**********************************//GESTION DES MESSAGES ENTRANTS//**********************************//

void server::receivMessage(void)
{
	for (int i = 1; i < maxFD + 1; i++)
	{
		if (_fds[i].revents & (POLLIN))
		{
			inputMessage(_fds[i].fd);
			_fds[i].revents = 0;
		}
	}
}

void server::inputMessage(int fd)
{
	std::string msg;
	int size;
	char buff[bufferSize];
	mapUser.find(fd)->second.setNetcat(-2);
	memset(&buff, 0, bufferSize);
	size = recv(fd, buff, bufferSize - 1, MSG_DONTWAIT);
	if (size < 0)
	{
		if (errno != EAGAIN && errno != EWOULDBLOCK)
		{
			std::string CLIENT = ":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost ";
			msg = CLIENT + "400 " + mapUser.find(fd)->second.getNickname() + " :Your message could not be sent./nPlease try again\r\n";
			send(fd, msg.c_str(), msg.size(), 0);
		}
	}
	else if (size == 0)
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
	iss >> command;
	if (mapUser.find(fd)->second.getLog() >= 2 || command == "QUIT" || command == "quit")
		parseCommand(buff, fd);
	else
	{
		size_t i = buff.find("PASS");
		size_t j = buff.find("pass");
		std::string username;
		std::string password = extract(buff, "PASS ", "\n");
		if (password.size() == 0)
			password = extract(buff, "pass ", "\n");
		std::string nickname = extract(buff, "NICK ", "\n");
		std::string name = extract(buff, "USER ", "\n");
		iss.clear();
		iss.str(name);
		iss >> username;
		if (mapUser.find(fd)->second.getLog() == 0)
		{
			if (mapUser.find(fd)->second.getNetcat() == -2 && i == std::string::npos && j == std::string::npos)
				cmdPass(command, fd);
			else
				cmdPass(password, fd);
		}
		if (mapUser.find(fd)->second.getLog() == 1)
		{
			mapUser.find(fd)->second.setLog();
			std::string str = "NICK " + nickname + " " + username;
			cmdNick(fd, str);
			printHome(fd);
			std::cout << GREEN << BOLD << "[42_IRC:  USER LOGGED IN] "<< mapUser.find(fd)->second.getNickname() << NONE << std::endl;
		}
	}
}








