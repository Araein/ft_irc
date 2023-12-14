#include "irc.hpp"

//**********************************//INCOMING MESSAGE//**********************************//

void server::receivMessage(void)
{
	for (int i = 1; i < maxFD + 1; i++)
	{
		if (_fds[i].revents & POLLIN)
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
	char left[bufferSize];
	mapUser.find(fd)->second.setNetcat(-2);
	memset(&buff, 0, bufferSize);
	memset(&left, 0, bufferSize);
	size = recv(fd, buff, bufferSize - 1, MSG_DONTWAIT);
	if ((std::string(buff).find("USER") == std::string::npos || std::string(buff).find("NICK") == std::string::npos) && mapUser.find(fd)->second.getLog() < 2){
		size += recv(fd, left, bufferSize - 1, MSG_DONTWAIT);
	}
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
			configureNewUser(std::string(buff).append(left), fd);
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
	std::cout << "debug = " << buff << std::endl;
	if (command == "QUIT" || command == "quit")
		parseCommand(buff, fd);
	else if (command == "/QUIT" || command == "/quit")
		parseCommand(&buff[1], fd);
	else
	{
		size_t i = buff.find("PASS");
		size_t j = buff.find("pass");
		std::string password = extract(buff, "PASS ", "\n");
		if (password.size() == 0)
			password = extract(buff, "pass ", "\n");
		std::string nickname = extract(buff, "NICK ", "\n");
		if (mapUser.find(fd)->second.getLog() == 0)
		{
			std::string username = extract(buff, "USER ", " ");
			mapUser.find(fd)->second.setUsername(username);
			std::string str = "NICK " + nickname;
			cmdNick(fd, str);
			mapUser.find(fd)->second.setLog();
			std::cout << GREEN << BOLD << "[42_IRC:  USER LOGGED IN] "<< mapUser.find(fd)->second.getNickname() << NONE << std::endl;
		}
		if (mapUser.find(fd)->second.getLog() == 1)
		{
			if (mapUser.find(fd)->second.getNetcat() == -2 && i == std::string::npos && j == std::string::npos)
				cmdPass(command, fd);
			else
				cmdPass(password, fd);
		}
	}
}
